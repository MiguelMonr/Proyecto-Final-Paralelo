#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iterator>
#include <cctype>
using namespace std;

void readVocabulary(const string& filename, vector<string>& vocabulary) {
    ifstream vocabStream(filename);
    string line;
    while (getline(vocabStream, line)) {
        istringstream ss(line);
        string word;
        while (getline(ss, word, ',')) {
            vocabulary.push_back(word);
        }
    }
}

void countWords(const vector<string>& files, const vector<string>& vocabulary, vector<vector<string>>& matriz) {
    for (int i = 0; i < files.size(); i++) {
        map<string, int> dictionary;
        for (const string& word : vocabulary) {
            dictionary[word] = 0;
        }
        ifstream fileStream(files[i]);
        string line;
        while (getline(fileStream, line)) {
            istringstream ss(line);
            string word;
            while (getline(ss, word, ',')) {
                if (dictionary.find(word) != dictionary.end()) {
                    dictionary[word]++;
                }
            }
        }
        for (int j = 0; j < vocabulary.size(); j++) {
            matriz[1 + i][j] = to_string(dictionary[vocabulary[j]]);
        }
    }
}

void writeMatrixToCSV(const std::string& filename, std::vector<std::vector<std::string>> matriz, int rows, int cols) {
    std::ofstream csvFile(filename);
    if (!csvFile.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            csvFile << matriz[i][j];
            if (j != cols - 1) {
                csvFile << ",";
            }
        }
        csvFile << std::endl;
    }
    csvFile.close();
}

void calculateAverageTime(double total_time, int ejecuciones) {
    double avg_time = (total_time / ejecuciones) / 1000;
    cout << "Promedio de tiempo de ejecucion: " << avg_time << " segundos" << endl;
    ofstream csv_file("tiempo_serial.csv");
    if (csv_file.is_open()) {
        csv_file << avg_time;
        csv_file.close();
    }
}

std::vector<std::string> readFileNames(const std::string& filename) {
    std::vector<std::string> lines;
    std::ifstream fileStream(filename);
    if (!fileStream.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return lines;
    }
    std::string line;
    while (getline(fileStream, line)) {
        lines.push_back(line);
    }
    fileStream.close();
    return lines;
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        cerr << "Usage: " << argv[0] << "<listado_nombres_archivos.txt> <archivo_vocabulario.txt> <tamanio_vocabulario> <output_file.csv> \n";
        return 1;
    }

    const string files_names = argv[1];
    const string arch_vocabulario = argv[2];
    const int tamanio_voc = atoi(argv[3]);
    const string output_file_name = argv[4];
    const int ejecuciones = 10;
    double total_time = 0.0;
    vector<string> vocabulary;

    readVocabulary(arch_vocabulario, vocabulary);

    vector<vector<string>> matriz(7, vector<string>(tamanio_voc));
    copy(vocabulary.begin(), vocabulary.end(), matriz[0].begin());
    std::vector<std::string> files = readFileNames(files_names);

    std::ofstream csvFile("iteration_times_serial.csv");
    if (!csvFile.is_open()) {
        cerr << "Failed to open iteration times file." << endl;
        return 1;
    }

    csvFile << "Iteration Time Serial (ms)";
    for (int i = 0; i < ejecuciones; i++) {
        auto start_time = chrono::high_resolution_clock::now();
        countWords(files, vocabulary, matriz);
        writeMatrixToCSV(output_file_name, matriz, 7, tamanio_voc);
        auto end_time = chrono::high_resolution_clock::now();
        double iteration_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
        total_time += iteration_time;
        cout << "Tiempo de ejecucion: " << iteration_time / 1000 << " segundos \n";
        csvFile << "," << iteration_time;
    }
    csvFile.close();
    calculateAverageTime(total_time, ejecuciones);

    return 0;
}

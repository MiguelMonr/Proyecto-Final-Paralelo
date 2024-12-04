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
#include <mpi.h>

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
    ofstream csv_file("tiempo_paralelo.csv");
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

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int num_processes = 6;
    int process_id;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);

    if (argc < 5) {
        if (process_id == 0) {
            cerr << "Usage: " << argv[0] << " <listado_nombres_archivos.txt> <archivo_vocabulario.txt> <tamanio_vocabulario> <output_file.csv>\n";
        }
        MPI_Finalize();
        return 1;
    }

    const string files_names = argv[1];
    const string arch_vocabulario = argv[2];
    const int tamanio_voc = atoi(argv[3]);
    const string output_file_name = argv[4];
    const int ejecuciones = 10;
    double total_time = 0.0;

    vector<string> vocabulary;
    vector<string> files;

    if (process_id == 0) {
        readVocabulary(arch_vocabulario, vocabulary);
        files = readFileNames(files_names);
    }

    int vocab_size = vocabulary.size();
    MPI_Bcast(&vocab_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (process_id != 0) {
        vocabulary.resize(vocab_size);
    }

    for (int i = 0; i < vocab_size; i++) {
        int len;
        if (process_id == 0) {
            len = vocabulary[i].length();
        }
        MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (process_id != 0) {
            vocabulary[i].resize(len);
        }
        MPI_Bcast(const_cast<char*>(vocabulary[i].data()), len, MPI_CHAR, 0, MPI_COMM_WORLD);
    }

    vector<vector<string>> matriz(7, vector<string>(tamanio_voc));
    if (process_id == 0) {
        copy(vocabulary.begin(), vocabulary.end(), matriz[0].begin());
    }

    vector<vector<string>> all(matriz);
    MPI_Gather(matriz.data() + 1, 6, MPI_DOUBLE, all.data() + 1, 6, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double start_time;
    double end_time;

    for (int i = 0; i < ejecuciones; i++) {
        start_time = MPI_Wtime();
        countWords(files, vocabulary, matriz);
        if (process_id == 0) {
            writeMatrixToCSV(output_file_name, all, 7, tamanio_voc);
        }
        end_time = MPI_Wtime();
        double iteration_time = end_time - start_time;
        double total_iteration_time = 0.0;
        cout << "Tiempo de ejecucion: " << iteration_time / 1000 << " segundos \n";
        MPI_Reduce(&iteration_time, &total_iteration_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        if (process_id == 0) {
            total_time += iteration_time;
        }
    }

    calculateAverageTime(total_time, ejecuciones);
    MPI_Finalize();
    return 0;
}

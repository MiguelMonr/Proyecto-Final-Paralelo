/*
    SECTION A): LIBRARIES
*/
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

/*
    SECTION B) FUNCTIONS
*/

/*
    FUNCTION: Read Vocabulary
*/
void readVocabulary(const string &filename, vector<string> &vocabulary)
{

    // Creating an input file stream object used to read data
    ifstream vocabStream(filename);

    // Declaring an string variable called line
    string line;

    // While-loop to read lines from a File
    while (getline(vocabStream, line))
    {

        // Creating input string stream from line variable
        istringstream ss(line);

        // Declaring an string variable called word
        string word;

        // While-loop to extract words
        while (getline(ss, word, ','))
        {

            // Storing words in the vector
            vocabulary.push_back(word);
        }
    }
}

/*
    FUNCTION: Count words
*/
void countWords(const vector<string> &files, const vector<string> &vocabulary, vector<vector<string>> &matriz)
{

    // For-loop to iterate over all files names vector
    for (int i = 0; i < files.size(); i++)
    {

        // Declaring a dictionary that will hold the word counts
        map<string, int> dictionary;

        // For-loop that initializes the dictionary of each word count to zero
        for (const string &word : vocabulary)
        {

            // Setting each word count to zero
            dictionary[word] = 0;
        }

        // Opening a file used to read data from the file
        ifstream fileStream(files[i]);

        // Declaring a string auxiliar variable to read
        string line;

        // While-loop that iterates over each line from the file
        while (getline(fileStream, line))
        {

            // Initializing an string stream with the content of line
            istringstream ss(line);

            // Declaring a string auxiliar variable for word
            string word;

            // While-loop to read characters until a comma is encountered
            while (getline(ss, word, ','))
            {

                // Checking if the word extracted from the line is found
                if (dictionary.find(word) != dictionary.end())
                {

                    // If the word is found in the dictonary, increment its associaed
                    dictionary[word]++;
                }
            }
        }

        // For-loop to iterate over each element in the vocabulary vector
        for (int j = 0; j < vocabulary.size(); j++)
        {

            // Storing the counts in the matrix
            matriz[1 + i][j] = to_string(dictionary[vocabulary[j]]);
        }
    }
}

/*
    FUNCTION: Write Matrix to a CSV
*/
void writeMatrixToCSV(const std::string &filename, std::vector<std::vector<std::string>> matriz, int rows, int cols)
{

    // Opening an output file stream for writing the CSV file
    std::ofstream csvFile(filename);

    // Checking if the file stream was successfully opened
    if (!csvFile.is_open())
    {

        // Printing error message and closing
        std::cerr << "Failed to open file: " << filename << std::endl;

        // Exiting
        return;
    }

    // For-Loop to use through each row of the matrix
    for (int i = 0; i < rows; i++)
    {

        // For-Loop to use through each column of the matrix
        for (int j = 0; j < cols; j++)
        {

            // Writing the current element to the file
            csvFile << matriz[i][j];

            // Adding a comma unless it's the last element in the row
            if (j != cols - 1)
            {

                // Adding comma
                csvFile << ",";
            }
        }

        // End the current row
        csvFile << std::endl;
    }

    // Closing the file stream
    csvFile.close();
}

/*
    FUNCTION: Calculate average time
*/
void calculateAverageTime(double total_time, int ejecuciones)
{

    // Calculating average time
    double avg_time = (total_time / ejecuciones) / 1000;

    // Displaying the average time on console
    cout << "Promedio de tiempo de ejecucion: " << avg_time << " segundos" << endl;

    // Writing the average time on a CSV for plotting purposes
    ofstream csv_file("tiempo_serial.csv");

    // Checking that the file stream is open
    if (csv_file.is_open())
    {

        // Writing on the file
        csv_file << avg_time;

        // Closing the file stream
        csv_file.close();
    }
}

/*
    FUNCTION: Read files names list
*/
std::vector<std::string> readFileNames(const std::string &filename)
{

    // Initializing variable lines
    std::vector<std::string> lines;

    // Opening the files names
    std::ifstream fileStream(filename);

    // Check if the file is open
    if (!fileStream.is_open())
    {

        // Printing  opening error
        std::cerr << "Failed to open file: " << filename << std::endl;

        // Return an empty vector if file cannot be opened
        return lines;
    }

    // Initializing string auxiliar variable
    std::string line;

    // Read file line by line
    while (getline(fileStream, line))
    {

        // Appending on the vector
        lines.push_back(line);
    }

    // Close the file after reading
    fileStream.close();

    // Returning vector
    return lines;
}

/*
    SECTION C) MAIN
*/

int main(int argc, char *argv[])
{

    // Command line argument validation: if fewer than 5 arguments are provided
    if (argc < 5)
    {

        // Displaying usage message
        cerr << "Usage: " << argv[0] << "<listado_nombres_archivos.txt> <archivo_vocabulario.txt> <tamanio_vocabulario> <output_file.csv> \n";

        // Program exit
        return 1;
    }

    // Storing the first command-line argument (argv[1]) as files_names
    const string files_names = argv[1];

    // Storing the second command-line argument (argv[2]) as arch_vocabulario
    const string arch_vocabulario = argv[2];

    // Storing the third command-line argument (argv[3]) as tamanio_voc
    const int tamanio_voc = atoi(argv[3]);

    // Storing the fourth command-line argument (argv[4]) as output_file
    const string output_file_name = argv[4];

    // Initialiazing number of iterations for testing process
    const int ejecuciones = 6;

    // Initialiazing acumulate time value variable
    double total_time = 0.0;

    // Declaring vector for vocabulary
    vector<string> vocabulary;

    // Using our read vocabulary function
    readVocabulary(arch_vocabulario, vocabulary);
    /*
        // Checking that is not empty
        if (vocabulary.empty()) {

            // Displaying error
            cerr << "El archivo de palabras esta vacio" << endl;

            //Ending
            return 1;

        }
    */
    // Initializing a matrix to store word counts
    vector<vector<string>> matriz(7, vector<string>(tamanio_voc));

    // Copy vocabulary to the first row of the matrix
    copy(vocabulary.begin(), vocabulary.end(), matriz[0].begin());

    // Getting files names with our function
    std::vector<std::string> files = readFileNames(files_names);

    // Opening a file to write iteration times
    std::ofstream csvFile("iteration_times_serial.csv");

    // Check if the file is open
    if (!csvFile.is_open())
    {

        // Displaying error
        cerr << "Failed to open iteration times file." << endl;

        // Exit if file cannot be opened
        return 1;
    }

    // Writing header
    csvFile << "Iteration Time Serial (ms)";

    // For-loop to iterate as many times we established on ejecuciones int variable
    for (int i = 0; i < ejecuciones; i++)
    {

        // Recording starting time of the word counting operation
        auto start_time = chrono::high_resolution_clock::now();

        // Using our  count words function implementation
        countWords(files, vocabulary, matriz);

        // Using our write matyrix to csv functions implementation
        writeMatrixToCSV(output_file_name, matriz, 7, tamanio_voc);

        // Gettting the end time
        auto end_time = chrono::high_resolution_clock::now();

        // Calculating the duration of each iteration
        double iteration_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

        // Accumulating total time
        total_time += iteration_time;

        // Displaying the time execution on console
        cout << "Tiempo de ejecucion: " << iteration_time / 1000 << " segundos \n";

        // Writing iteration execution time on CSV
        csvFile << "," << iteration_time;
    }

    // Closing the file stream
    csvFile.close();

    // Using ou average time execution function
    calculateAverageTime(total_time, ejecuciones);

    // Finishing
    return 0;
}
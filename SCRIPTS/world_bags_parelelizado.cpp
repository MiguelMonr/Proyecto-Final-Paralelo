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
#include <mpi.h>

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
void countWords(const vector<string> &files, const vector<string> &vocabulary, vector<vector<string> > &matriz)
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
void writeMatrixToCSV(const std::string &filename, std::vector<std::vector<std::string> > matriz, int rows, int cols)
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
    ofstream csv_file("tiempo_paralelo.csv");

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
    FUNCTION: Read files names list function
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

    // Initializing MPI
    MPI_Init(&argc, &argv);

    // Initializing the total number of processes and each process_id
    int num_processes = 6;

    // Initializing each process_id
    int process_id;

    /*
        MPI_Comm_size retrieves the total number of processes in the communicator specified
        by the user. This is crucial for determining how many processes are available for
        distributing work and for designing the division of tasks in parallel algorithms.
    */
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    /*
        MPI_Comm_rank determines the rank (or unique identifier) of the calling process within
        the specified communicator. Each process in a communicator is assigned a unique rank
        starting from 0 up to the number of processes minus one. This rank is used to address
        processes individually, often for sending and receiving messages, and for conditionally
        executing parts of the code based on the process rank.
    */
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);

    // Checking if the necessary arguments are passed
    if (argc < 5)
    {

        if (process_id == 0)
        {

            cerr << "Usage: " << argv[0] << " <listado_nombres_archivos.txt> <archivo_vocabulario.txt> <tamanio_vocabulario> <output_file.csv>\n";
        }

        /*
            MPI_Finalize() is a function from the Message Passing Interface (MPI) library,
            which is used for programming parallel applications, particularly on distributed
            memory systems. This function plays a crucial role in the lifecycle of an MPI
            application by serving as the termination point for MPI functionality in your program.
        */
        MPI_Finalize();

        // Finishing program
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

    //
    vector<string> files;

    // Master reads the vocabulary and the file names
    if (process_id == 0)
    {

        // Using our read vocabulary function
        readVocabulary(arch_vocabulario, vocabulary);

        //
        files = readFileNames(files_names);
    }

    //
    int vocab_size = vocabulary.size();

    /*
        MPI_Bcast is a collective communication function provided by MPI, which is
        extensively used in parallel programming, especially within distributed memory
        systems. This function broadcasts a message from the root process to all other
        processes in a communicator.
    */
    MPI_Bcast(&vocab_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (process_id != 0)
    {

        // All processes resize the vocabulary vector to match the size
        vocabulary.resize(vocab_size);
    }

    // For loop to broadcast vocabulary words
    for (int i = 0; i < vocab_size; i++)
    {
        int len;

        if (process_id == 0)
        {
            len = vocabulary[i].length();
        }

        /*
            MPI_Bcast is a collective communication function provided by MPI, which is
            extensively used in parallel programming, especially within distributed memory
            systems. This function broadcasts a message from the root process to all other
            processes in a communicator.
        */
        MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD);

        if (process_id != 0)
        {
            vocabulary[i].resize(len);
        }

        /*
            MPI_Bcast is a collective communication function provided by MPI, which is
            extensively used in parallel programming, especially within distributed memory
            systems. This function broadcasts a message from the root process to all other
            processes in a communicator.
        */
        MPI_Bcast(const_cast<char *>(vocabulary[i].data()), len, MPI_CHAR, 0, MPI_COMM_WORLD);
    }

    // Here the word counts are stored: 7 rows for 7 books,
    // columns contain the word and the count of how many times it appears
    vector<vector<string> > matriz(7, vector<string>(tamanio_voc));

    if (process_id == 0)
    {

        // copying the vocabulary
        copy(vocabulary.begin(), vocabulary.end(), matriz[0].begin());
    }

    // MPI_Gather to collect the counted words from each process
    vector<vector<string> > all(matriz);

    /*
        MPI_Gather is a collective communication function in the MPI used in parallel
        programming, particularly effective in distributed memory systems. This function
        gathers data from all processes in a communicator and collects it into a single data
        array in a designated root process.
    */
    MPI_Gather(matriz.data() + 1, 6, MPI_DOUBLE, all.data() + 1, 6, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    /*
    if (process_id == 0) {

        // Using our write matyrix to csv functions implementation
        writeMatrixToCSV(output_file_name, all, 7, tamanio_voc);

    }
    */

    // Defining auxiliar variables to track time execution
    double start_time;
    double end_time;

    // Calculating the execution time for each iteration
    for (int i = 0; i < ejecuciones; i++)
    {

        /*
            MPI_Wtime() is a function provided by the MPI used in parallel programming,
            particularly on distributed memory systems. This function is
            part of MPI's timing interface, which is designed to offer a portable way to
            measure elapsed time. It is especially useful in performance evaluations of parallel
            applications, where understanding the timing of operations across different
            processes is crucial.
        */
        start_time = MPI_Wtime();

        // Using our  count words function implementation
        countWords(files, vocabulary, matriz);

        // Writing the matrix to a .csv
        if (process_id == 0)
        {

            // Using our write matyrix to csv functions implementation
            writeMatrixToCSV(output_file_name, all, 7, tamanio_voc);
        }

        /*
            MPI_Wtime() is a function provided by MPI used in parallel programming,
            particularly on distributed memory systems. This function is
            part of MPI's timing interface, which is designed to offer a portable way to
            measure elapsed time. It is especially useful in performance evaluations of parallel
            applications, where understanding the timing of operations across different
            processes is crucial.
        */
        end_time = MPI_Wtime();

        // Getting total iteration time
        double iteration_time = end_time - start_time;
        double total_iteration_time = 0.0;

        // Displaying the time execution
        cout << "Tiempo de ejecucion: " << iteration_time / 1000 << " segundos \n";

        /*
            MPI_Reduce is a collective communication function in the Message Passing Interface (MPI)
            used in parallel programming, particularly effective in distributed memory systems. This
            function is part of a family of operations known as reduction operations, which help
            perform a specific operation across values held by all processes in a group and then
            send the result to one process in the group.
        */
        MPI_Reduce(&iteration_time, &total_iteration_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        //
        if (process_id == 0)
        {

            //
            total_time += iteration_time;
        }
    }

    // Calculating the avarge time
    calculateAverageTime(total_time, ejecuciones);

    /*
        MPI_Finalize() is a function from the Message Passing Interface (MPI) library,
        which is used for programming parallel applications, particularly on distributed
        memory systems. This function plays a crucial role in the lifecycle of an MPI
        application by serving as the termination point for MPI functionality in your program.
    */
    MPI_Finalize();

    // Finishing program
    return 0;
}
/**
 * FIT VUT
 * 
 * @author Jakub Sadílek
 * @date 3.5.2021
 * @file mm.cpp
 * 
 * @brief Mesh Multiplication - parallel algorithm
 * 
*/

#include <iostream>
#include <mpi.h>
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <sstream>

#define MATRIX1_FILENAME "mat1"
#define MATRIX2_FILENAME "mat2"

#define TAG_M1_HEIGHT 1
#define TAG_M2_HEIGHT 2
#define TAG_M1_WIDTH 3
#define TAG_M2_WIDTH 4
#define TAG_ST_LEFT 5
#define TAG_ST_TOP 6
#define TAG_VERT_CELL 7
#define TAG_HORIZ_CELL 8
#define TAG_RES 9

/**
 * Class which represents matrix.
 * 
 * @class Matrix
*/
class Matrix {
    public:
        unsigned height = 0;
        unsigned width = 0;

        std::vector<int> data;
};

/**
 * Class which contains important values for each processor.
 * 
 * @class Proc
*/
class Proc {
    public:
        int id;                 // Processor ID
        unsigned i;             // Row index
        unsigned j;             // Col index

        int cellValue = 0;      // Value of cell in final matrix

        unsigned M1_height;     // Matrix 1 height
        unsigned M2_height;     // Matrix 2 height
        unsigned M1_width;      // Matrix 1 width
        unsigned M2_width;      // Matrix 2 width
};

MPI_Status status;

Proc procConfig;

// Queues for initial processors.
std::queue<int> vertQueue;      // Queue for top processors
std::queue<int> horizQueue;     // Queue for left processors

/**
 * Function prints message to stderr and terminates program on error.
 * 
 * @param message Message to print.
*/
void error(const char *message) {
    fprintf(stderr, "%s\n", message);
    MPI_Abort(MPI_COMM_WORLD, 1);
    MPI_Finalize();
    exit(0);
}

/**
 * Function loads matrix from file and returns it as a class.
 * 
 * @param filname Name of file which contains the matrix.
 * @param ord Matrix order.
 * @returns Class Matrix.
*/
Matrix loadMatrix(std::string filename, int ord) {
    std::ifstream file (filename.c_str());

    if (!file.is_open())                    // Can program open a file?
        error("Cannot open file.");

    Matrix matrix;
    std::string line;

    getline(file, line);                    // Value on first row

    unsigned n = std::stoi(line);

    int number;
    while (std::getline(file, line)) {      // For each row of matrix
        std::stringstream lineStream(line);

        // Inspired: https://stackoverflow.com/questions/20659066/parse-string-to-vector-of-int
        while (lineStream >> number)        // Store each number as int to the vector
            matrix.data.push_back(number);

        matrix.height++;                    // Increase row counter
    }

    file.close();

    matrix.width = matrix.data.size() / matrix.height;  // Get width of matrix

    if (ord == 1 && n != matrix.height)             // Does the 1st matrix correct height?
        error("Error: Matrix 1 has different heigth than gived number.");
    if (ord != 1 && n != matrix.width)              // Does the 2nd matrix correct width?
        error("Error: Matrix 2 has different width than gived number.");
    if (matrix.data.size() % matrix.height != 0)    // Does the matrix correct shape?
        error("Error: Wrong matrix shape.");
        
    return matrix;
}

/**
 * Funcion sets position of the processor in the final matrix according to its ID.
 * 
 * @param width Width of the final matrix.
*/
void setCoordinates(unsigned width) {
    procConfig.i = procConfig.id / width;   // Row
    procConfig.j = procConfig.id % width;   // Column
}

/**
 * Function calculates processors ID according to its coordinates.
 * 
 * @param i Row index of processor.
 * @param j Column index of processor.
*/
int getProcID(unsigned i, unsigned j) {
    return i * procConfig.M2_width + j;
}

/**
 * Preparation process of the 1st processor before starting calculation.
 * 
 * @param procCount Number of processors.
*/
void firstProcPreparation(int procCount) {
    Matrix matrix_1 = loadMatrix(MATRIX1_FILENAME, 1);
    Matrix matrix_2 = loadMatrix(MATRIX2_FILENAME, 2);
    
    if (procCount != (matrix_1.height * matrix_2.width))    // Check correct number of processors
        error("Bad number of processors to start the algorithm calculation.");

    if (matrix_1.width != matrix_2.height)
        error("Cannot multiply these matrices.");

    for (int i = 1; i < procCount; i++) {           // Send matrix infos to other processors
        MPI_Send(&matrix_1.height, 1, MPI_UNSIGNED, i, TAG_M1_HEIGHT, MPI_COMM_WORLD);
        MPI_Send(&matrix_2.height, 1, MPI_UNSIGNED, i, TAG_M2_HEIGHT, MPI_COMM_WORLD);
        MPI_Send(&matrix_1.width, 1, MPI_UNSIGNED, i, TAG_M1_WIDTH, MPI_COMM_WORLD);
        MPI_Send(&matrix_2.width, 1, MPI_UNSIGNED, i, TAG_M2_WIDTH, MPI_COMM_WORLD);
    }

    procConfig.M1_height = matrix_1.height;         // Save matrix infos to the 1st processor
    procConfig.M2_height = matrix_2.height;
    procConfig.M1_width = matrix_1.width;
    procConfig.M2_width = matrix_2.width;

    setCoordinates(matrix_2.width);                 // Set position of processor in matrix

    for (int i = 1; i < matrix_1.height; i++) {     // Send rows of 1st matrix to initial processors on the left
        for (int j = 0; j < matrix_1.width; j++) {
            MPI_Send(&matrix_1.data[i*matrix_1.width + j], 1, MPI_INT, getProcID(i, 0), TAG_ST_LEFT, MPI_COMM_WORLD);
        }
    }

    for (int i = 0; i < matrix_2.height; i++) {     // Send columns of 2nd matrix to initial processors on the top
        for (int j = 1; j < matrix_2.width; j++) {
            MPI_Send(&matrix_2.data[i*matrix_2.width + j], 1, MPI_INT, getProcID(0, j), TAG_ST_TOP, MPI_COMM_WORLD);
        }
    }

    for (int i = 0; i < matrix_1.width; i++) {      // Save 1st row of 1st matrix to the 1st processor.
        horizQueue.push(matrix_1.data[i]);
    }
    for (int i = 0; i < matrix_2.height; i++) {     // Save 1st column of 2nd matrix to the 1st processor.
        vertQueue.push(matrix_2.data[i*matrix_2.width]);
    }
}

/**
 * Preparation process of all processors except the 1st one before starting calculation.
*/
void procPreparation() {
    // Receive and store matrix informations from the 1st processor.
    MPI_Recv(&procConfig.M1_height, 1, MPI_UNSIGNED, 0, TAG_M1_HEIGHT, MPI_COMM_WORLD, &status);
    MPI_Recv(&procConfig.M2_height, 1, MPI_UNSIGNED, 0, TAG_M2_HEIGHT, MPI_COMM_WORLD, &status);
    MPI_Recv(&procConfig.M1_width, 1, MPI_UNSIGNED, 0, TAG_M1_WIDTH, MPI_COMM_WORLD, &status);
    MPI_Recv(&procConfig.M2_width, 1, MPI_UNSIGNED, 0, TAG_M2_WIDTH, MPI_COMM_WORLD, &status);

    setCoordinates(procConfig.M2_width);    // Set position of processor in the matrix

    int number;
    
    if (procConfig.i == 0) {        // If its 1st vertical processor, then receive column
        for (int i = 0; i < procConfig.M2_height; i++) {
            MPI_Recv(&number, 1, MPI_INT, 0, TAG_ST_TOP, MPI_COMM_WORLD, &status);
            vertQueue.push(number);
        }
    }
    else if (procConfig.j == 0) {   // If its 1st horizontal processor, then receive row
        for (int i = 0; i < procConfig.M1_width; i++) {
            MPI_Recv(&number, 1, MPI_INT, 0, TAG_ST_LEFT, MPI_COMM_WORLD, &status);
            horizQueue.push(number);
        }
    }
}

/**
 * Start of algorithm. Function calculates for each processor its cell value.
*/
void meshMult() {
    int horizCell, vertCell;

    for (int i = 0; i < procConfig.M1_width; i++) {     // Or M2_height - its same, both matrix must have same atleast one side
        if (procConfig.i == 0) {                        // Vertical initial processor - get value from queue
            vertCell = vertQueue.front();
            vertQueue.pop();
        }
        else {                                          // Vertical non initial processor - get value from previous processor
            MPI_Recv(&vertCell, 1, MPI_INT, getProcID(procConfig.i-1, procConfig.j), TAG_VERT_CELL, MPI_COMM_WORLD, &status);
        }

        if (procConfig.j == 0) {                        // Horizontal initial processor - get value from queue
            horizCell = horizQueue.front();
            horizQueue.pop();
        }
        else {                                          // Horizontal non initial processor - get value from previous processor
            MPI_Recv(&horizCell, 1, MPI_INT, getProcID(procConfig.i, procConfig.j-1), TAG_HORIZ_CELL, MPI_COMM_WORLD, &status);
        }

        procConfig.cellValue += horizCell * vertCell;   // Value calculation

        if (procConfig.i < procConfig.M1_height - 1) {  // If its not the last processor in column, send value to the next processor
            MPI_Send(&vertCell, 1, MPI_INT, getProcID(procConfig.i+1, procConfig.j), TAG_VERT_CELL, MPI_COMM_WORLD);
        }
        if (procConfig.j < procConfig.M2_width - 1) {   // If its not the last processor in row, send value to the next processor
            MPI_Send(&horizCell, 1, MPI_INT, getProcID(procConfig.i, procConfig.j+1), TAG_HORIZ_CELL, MPI_COMM_WORLD);
        }
    }
}

/**
 * Function sends processors cell value to the 1st processor.
*/
void sendResults() {
    MPI_Send(&procConfig.cellValue, 1, MPI_INT, 0, TAG_RES, MPI_COMM_WORLD);
}

/**
 * 1st processor receives cell values from all other processors and stores them into the matrix class.
 * 
 * @param procCount Number of processors.
 * @returns Class of final matrix.
*/
Matrix receiveResults(int procCount) {
    Matrix matrix;
    int cell;

    matrix.data.push_back(procConfig.cellValue);    // Store value from the 1st processor

    for (int i = 1; i < procCount; i++) {           // Receive and store value from other processors
        MPI_Recv(&cell, 1, MPI_INT, i, TAG_RES, MPI_COMM_WORLD, &status);
        matrix.data.push_back(cell);
    }

    matrix.height = procConfig.M1_height;
    matrix.width = procConfig.M2_width;

    return matrix;
}

/**
 * Function prints matrix on stdout.
 * 
 * @param matrix Matrix to print.
*/
void printMatrix(Matrix matrix) {
    std::cout << matrix.height << ":" << matrix.width << std::endl;     // Print matrix dimensions

    for (unsigned i = 0; i < matrix.height; i++) {      // Print matrix cells
        for (unsigned j = 0; j < matrix.width; j++) {
            std::cout << matrix.data.front();
            matrix.data.erase(matrix.data.begin());

            if (j+1 == matrix.width)
                std::cout << std::endl;
            else
                std::cout << " ";
        }
    }
}

int main(int argc, char **argv) {
    int procID, procCount;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &procID);
    MPI_Comm_size(MPI_COMM_WORLD, &procCount);

    procConfig.id = procID;

    if (procID == 0)                    // First processor preparation
        firstProcPreparation(procCount);
    else                                // Other processors preparation
        procPreparation();

    meshMult();     // Mesh multiplication algorithm calculation

    if (procID == 0) {
        Matrix m = receiveResults(procCount);   // 1st processor gets results from other processors
        printMatrix(m);                         // 1st processor prints results
    }
    else {
        sendResults();                          // Other processors sends results
    }

    MPI_Finalize();
    return 0;
}

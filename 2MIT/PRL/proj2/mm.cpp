/**
 * FIT VUT
 * 
 * @author Jakub Sadílek
 * @date 3.5.2021
 * @file pms.cpp
 * 
 * @brief Mesh Multiplication
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

class Matrix {
    public:
        unsigned height = 0;
        unsigned width = 0;
        std::vector<int> data;
};

class Proc {
    public:
        int id;
        unsigned i;
        unsigned j;

        unsigned M1_height;
        unsigned M2_height;
        unsigned M1_width;
        unsigned M2_width;
};

MPI_Status status;

Proc procConfig;

std::queue<int> topQueue;
std::queue<int> leftQueue;

void error(const char *message) {
    fprintf(stderr, "%s\n", message);
    MPI_Abort(MPI_COMM_WORLD, 1);
    MPI_Finalize();
    exit(0);
}

Matrix loadMatrix(std::string filename, int ord) {
    std::ifstream file (filename.c_str());

    if (!file.is_open())                // Can program open a file?
        error("Cannot open file.");

    Matrix matrix;
    std::string line;
    unsigned n;

    getline(file, line);

    if (ord == 1)
        n = std::stoi(line);
    else 
        n = std::stoi(line);

    int number;
    while (std::getline(file, line)) {
        std::stringstream lineStream(line);

        // Inspired: https://stackoverflow.com/questions/20659066/parse-string-to-vector-of-int
        while (lineStream >> number)
            matrix.data.push_back(number);

        matrix.height++;
    }

    file.close();

    matrix.width = matrix.data.size() / matrix.height;

    if (ord == 1 && n != matrix.height)
        error("Error: Matrix 1 has different heigth than gived number.");
    if (ord != 1 && n != matrix.width)
        error("Error: Matrix 2 has different width than gived number.");
    if (matrix.data.size() % matrix.height != 0)
        error("Error: Wrong matrix shape.");
        
    return matrix;
}

void printMatrix(Matrix matrix) {
    std::cout << matrix.height << ":" << matrix.width << std::endl;

    for (unsigned i = 0; i < matrix.height; i++) {
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

int getProcID(unsigned i, unsigned j) {
    return i * procConfig.M2_width + j;
}

void setCoordinates(unsigned width) {
    procConfig.i = procConfig.id / width;   // Row
    procConfig.j = procConfig.id % width;   // Col
}

void printQueue(std::queue<int> queue) {
    if (!queue.empty()) {
        std::cout << queue.front();
        queue.pop();
    }
    while (!queue.empty()) {
        std::cout << " " << queue.front();
        queue.pop();
    }
    std::cout << std::endl;
}

void firstProcPreparation(int procCount) {
    Matrix matrix_1 = loadMatrix(MATRIX1_FILENAME, 1);
    Matrix matrix_2 = loadMatrix(MATRIX2_FILENAME, 2);

    for (int i = 1; i < procCount; i++) {
        MPI_Send(&matrix_1.height, 1, MPI_UNSIGNED, i, TAG_M1_HEIGHT, MPI_COMM_WORLD);
        MPI_Send(&matrix_2.height, 1, MPI_UNSIGNED, i, TAG_M2_HEIGHT, MPI_COMM_WORLD);
        MPI_Send(&matrix_1.width, 1, MPI_UNSIGNED, i, TAG_M1_WIDTH, MPI_COMM_WORLD);
        MPI_Send(&matrix_2.width, 1, MPI_UNSIGNED, i, TAG_M2_WIDTH, MPI_COMM_WORLD);
    }

    procConfig.M1_height = matrix_1.height;
    procConfig.M2_height = matrix_2.height;
    procConfig.M1_width = matrix_1.width;
    procConfig.M2_width = matrix_2.width;

    setCoordinates(matrix_2.width);

    for (int i = 1; i < matrix_1.height; i++) {
        for (int j = 0; j < matrix_1.width; j++) {
            MPI_Send(&matrix_1.data[i*matrix_1.width + j], 1, MPI_INT, getProcID(i, 0), TAG_ST_LEFT, MPI_COMM_WORLD);
        }
    }

    for (int i = 0; i < matrix_2.height; i++) {
        for (int j = 1; j < matrix_2.width; j++) {
            MPI_Send(&matrix_2.data[i*matrix_2.width + j], 1, MPI_INT, getProcID(0, j), TAG_ST_TOP, MPI_COMM_WORLD);
        }
    }

    for (int i = 0; i < matrix_1.width; i++) {
        leftQueue.push(matrix_1.data[i]);
    }
    for (int i = 0; i < matrix_2.height; i++) {
        topQueue.push(matrix_2.data[i*matrix_2.width]);
    }

}

void procPreparation() {
    MPI_Recv(&procConfig.M1_height, 1, MPI_UNSIGNED, 0, TAG_M1_HEIGHT, MPI_COMM_WORLD, &status);
    MPI_Recv(&procConfig.M2_height, 1, MPI_UNSIGNED, 0, TAG_M2_HEIGHT, MPI_COMM_WORLD, &status);
    MPI_Recv(&procConfig.M1_width, 1, MPI_UNSIGNED, 0, TAG_M1_WIDTH, MPI_COMM_WORLD, &status);
    MPI_Recv(&procConfig.M2_width, 1, MPI_UNSIGNED, 0, TAG_M2_WIDTH, MPI_COMM_WORLD, &status);

    setCoordinates(procConfig.M2_width);

    int number;
    
    if (procConfig.i == 0) {        // Top processors
        for (int i = 0; i < procConfig.M2_height; i++) {
            MPI_Recv(&number, 1, MPI_INT, 0, TAG_ST_TOP, MPI_COMM_WORLD, &status);
            topQueue.push(number);
        }
    }
    else if (procConfig.j == 0) {   // Left processors
        for (int i = 0; i < procConfig.M1_width; i++) {
            MPI_Recv(&number, 1, MPI_INT, 0, TAG_ST_LEFT, MPI_COMM_WORLD, &status);
            leftQueue.push(number);
        }
    }
    /*if (procConfig.id == 3) {
    printQueue(topQueue);
    std::cout << "L" << std::endl;
    printQueue(leftQueue);
    }*/
}

int main(int argc, char **argv) {
    int procID, procCount;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &procID);
    MPI_Comm_size(MPI_COMM_WORLD, &procCount);

    procConfig.id = procID;

    if (procID == 0) {                  // First processor
        firstProcPreparation(procCount);
    }
    else {                              // Other processors
        procPreparation();
    }

    MPI_Finalize();
    return 0;
}

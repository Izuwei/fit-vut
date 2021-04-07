#include <iostream>
#include <string.h>
#include <mpi.h>
#include <queue>
#include <fstream>

#define NUMBER_OF_NUMBERS 16
#define INPUT_FILENAME "numbers"

#define TAG 0

MPI_Status status;

std::queue<unsigned> queue1;
std::queue<unsigned> queue2;

unsigned procCycle = 0;
unsigned sequenceLength = 0;
bool storeFirstQ = true;
unsigned tookQ1 = 0, tookQ2 = 0;
unsigned startRecv, endRecv;
unsigned startSend, endSend;

void error(const char *message) {
    fprintf(stderr, "%s\n", message);
    MPI_Finalize();
    exit(1);
}

std::queue<unsigned> getNumbers() {
    std::ifstream file (INPUT_FILENAME);

    if (!file.is_open())
        error("Cannot open file.");

    unsigned number;
    std::queue<unsigned> numbers;

    for (int i = 0; i < NUMBER_OF_NUMBERS; i++) {
        number = file.get();

        if (!file.good())
            error("Enter at least 16 numbers.");

        numbers.push(number);
    }
    
    file.close();
    return numbers;
}

/**
 * Function sets boundaries of how the processor will work.
 * All formulas are given on slides in the lecture.
*/
void procConfig(int id) {
    // Input sequence at one queue: 2^(i-1)
    sequenceLength = 1 << (id - 1);

    // Processor starts to run in a cycle: 2^i + i
    // 'i' is index of processor in the pipeline, starting from 0.
    startSend = (1 << (id)) + id;

    // Processor ends the cycle: (n-1) + 2^i + i
    // 'n' is input length.
    endSend = (NUMBER_OF_NUMBERS-1)+(1 << (id)) + id;

    // Same as before, but for processor "i-1".
    startRecv = (1 << (id-1)) + id - 1;
    endRecv = (NUMBER_OF_NUMBERS-1)+(1 << (id-1)) + id-1;
}

void printQueue(std::queue<unsigned> queue) {
    while (!queue.empty()) {
        std::cout << queue.front() << " ";
        queue.pop();
    }
    std::cout << std::endl;
}

void storeNumber(unsigned number) {
    if (procCycle == sequenceLength) {
        procCycle = 0;
        storeFirstQ = !storeFirstQ;
    }

    if (storeFirstQ) {
        queue1.push(number);
    }
    else {
        queue2.push(number);
    }

    procCycle++;
}

unsigned loadNumber(int id) {
    unsigned number;

    if (queue1.empty() && queue2.empty()) {
        return -1;
    }
    else if (queue1.empty() || tookQ1 == sequenceLength) {
        number = queue2.front();
        queue2.pop();
        tookQ2++;
    }
    else if (queue2.empty() || tookQ2 == sequenceLength) {
        number = queue1.front();
        queue1.pop();
        tookQ1++;
    }
    else if (queue1.front() <= queue2.front()) {
        number = queue1.front();
        queue1.pop();
        tookQ1++;
    } 
    else {
        number = queue2.front();
        queue2.pop();
        tookQ2++;
    }

    return number;
}

void firstProcTask(int cycle, int id, std::queue<unsigned> &queue) {
    if (queue.empty())
        return;

    unsigned number = queue.front();
    queue.pop();

    MPI_Send(&number, 1, MPI_UNSIGNED, id+1 , TAG, MPI_COMM_WORLD);
}

void procTask(int cycle, int id) {
    unsigned number;

    if (cycle >= startRecv && cycle < endRecv) {
        MPI_Recv(&number, 1, MPI_UNSIGNED, id-1, TAG, MPI_COMM_WORLD, &status);

        storeNumber(number);
    }
    //if (id == 3) {
    //std::cout << "ID: " << id << " CYCLE: " << cycle << " DOSLO: " << number << std::endl;
    //printQueue(queue1);
    //printQueue(queue2);
    //}

    if (cycle >= startSend && cycle < endSend) {
        number = loadNumber(id);

        if (number != -1) {
            MPI_Send(&number, 1, MPI_UNSIGNED, id+1, TAG, MPI_COMM_WORLD);
        }

        if (tookQ1 == sequenceLength && tookQ2 == sequenceLength) {
            tookQ1 = 0;
            tookQ2 = 0;
        }
    }
}

void lastProcTask(int cycle, int id) {
    unsigned number;

    if (cycle >= startRecv && cycle < endRecv) {
        MPI_Recv(&number, 1, MPI_UNSIGNED, id-1, TAG, MPI_COMM_WORLD, &status);

        storeNumber(number);
    }

    if ((cycle >= startSend && cycle < endSend)){
        number = loadNumber(id);

        if (number != -1) {
            std::cout << number << std::endl;
        }
    }
}

int main(int argc, char **argv) {
    int procID, procCount;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &procID);
    MPI_Comm_size(MPI_COMM_WORLD, &procCount);

    procConfig(procID);

    if (procID == 0) {
        queue1 = getNumbers();
        printQueue(queue1);
    }

    // Algorithm ends in: n + 2^r + r - 1 cycles. It's same as 2*n + log n - 1.
    const int limit = NUMBER_OF_NUMBERS + (1 << (procCount - 1)) + (procCount - 1) - 1;
    for (int i = 0; i < limit; i++) {
        if (procID == 0) {                  // First processor
            firstProcTask(i, procID, queue1);
        }
        else if (procID == procCount -1) {  // Last processor
            lastProcTask(i, procID);
        }
        else {                              // Others processors between
            procTask(i, procID);
        }
    }

    MPI_Finalize(); 
    return 0;
}
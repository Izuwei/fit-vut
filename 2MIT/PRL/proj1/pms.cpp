/**
 * FIT VUT
 * 
 * @author Jakub Sadílek
 * @date 9.4.2021
 * @file pms.cpp
 * 
 * @brief Pipeline Merge Sort - parallel algorithm
 * 
*/

#include <iostream>
#include <mpi.h>
#include <queue>
#include <fstream>

#define NUMBER_OF_NUMBERS 16      // Number of numbers on input (n).
#define NUMBER_OF_PROCS 5         // Number of processors = log2(n) + 1
#define INPUT_FILENAME "numbers"  // Input filename.

#define TAG 0

MPI_Status status;

std::queue<unsigned> queue1;      // Queue 1 (top)
std::queue<unsigned> queue2;      // Queue 2 (bottom)

unsigned procCycle = 0;           // Internal counter for each processor.
unsigned sequenceLength = 0;      // Length of sequence on each queue.
bool storeFirstQ = true;          // Store in first queue? Or second.
unsigned tookQ1 = 0, tookQ2 = 0;  // Counter of taken numbers from queues.
unsigned startRecv, endRecv;      // Interval for processor, when to start receiving.
unsigned startSend, endSend;      // Interval for processor, when to start sending.

/**
 * Function prints error message to stderr and terminates program with 1.
*/
void error(const char *message) {
    fprintf(stderr, "%s\n", message);
    MPI_Abort(MPI_COMM_WORLD, 1);
    MPI_Finalize();
    exit(0);
}

/**
 * Function binary load numbers from file and return them in queue.
*/
std::queue<unsigned> getNumbers() {
    std::ifstream file (INPUT_FILENAME);

    if (!file.is_open())                // Can program open a file?
        error("Cannot open file.");

    unsigned number;
    std::queue<unsigned> numbers;

    for (int i = 0; i < NUMBER_OF_NUMBERS; i++) {
        number = file.get();            // Get 1 byte of data and save it as number.

        if (!file.good())               // No error? EoF?
            error("Enter at least 16 numbers.");

        numbers.push(number);
    }
    
    file.close();
    return numbers;
}

/**
 * Function sets boundaries of how the processor will work.
 * All formulas are given on slides in the lecture.
 * 
 * @param id ID of processor.
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

/**
 * Function prints given queue.
 * 
 * @param queue Queue to print.
*/
void printQueue(std::queue<unsigned> queue) {
    while (!queue.empty()) {
        std::cout << queue.front() << " ";
        queue.pop();
    }
    std::cout << std::endl;
}

/**
 * Function stores number into one of queues of processor.
 * 
 * @param number Number to store.
*/
void storeNumber(unsigned number) {
    if (procCycle == sequenceLength) {  // Number of inserted numbers into queue corresponds to the sequence.
        procCycle = 0;                  // So, swap queue.
        storeFirstQ = !storeFirstQ;
    }

    if (storeFirstQ) {                  // Store into first queue (top).
        queue1.push(number);
    }
    else {                              // Store into second queue (bottom).
        queue2.push(number);
    }

    procCycle++;
}

/**
 * Function loads number from queue while adhering the sequence.
 * 
 * @param id ID of processor.
*/
unsigned loadNumber(int id) {
    unsigned number;

    if (queue1.empty() && queue2.empty()) {     // Both queues are empty, can't do nothing.
        return -1;
    }
    else if (queue1.empty() || tookQ1 == sequenceLength) {  // First is empty or over taken.
        number = queue2.front();
        queue2.pop();
        tookQ2++;
    }
    else if (queue2.empty() || tookQ2 == sequenceLength) {  // Second is empty or over taken.
        number = queue1.front();
        queue1.pop();
        tookQ1++;
    }
    else if (queue1.front() <= queue2.front()) {    // We can take from both, so take smaller number.
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

/**
 * Task of the first processor.
 * Each cycle send number from input queue to second processor.
 * 
 * @param cycle Cycle of algorithm.
 * @param id Id of processor. First processor ID = 0.
 * @param queue Input queue.
*/
void firstProcTask(int cycle, int id, std::queue<unsigned> &queue) {
    if (queue.empty())      // If input is empty, do nothing.
        return;

    unsigned number = queue.front();
    queue.pop();

    MPI_Send(&number, 1, MPI_UNSIGNED, id+1 , TAG, MPI_COMM_WORLD);  // Send to 2nd processor.
}

/**
 * Task for all processors except the first one and the last one.
 * 
 * @param cycle Cycle of algorithm.
 * @param id ID of processor.
*/
void procTask(int cycle, int id) {
    unsigned number;

    // Processor before me started working, so I should receive data.
    if (cycle >= startRecv && cycle <= endRecv) {
        MPI_Recv(&number, 1, MPI_UNSIGNED, id-1, TAG, MPI_COMM_WORLD, &status);  // Receive data.

        storeNumber(number);            // Store number to queue.
    }

    // Processor starts working.
    if (cycle >= startSend && cycle <= endSend) {
        number = loadNumber(id);        // Load number to send.

        if (number != -1) {             // If it's correct, send it to next processor.
            MPI_Send(&number, 1, MPI_UNSIGNED, id+1, TAG, MPI_COMM_WORLD);
        }

        // Sequence from both queues are compared and sended, so start new sequence.
        if (tookQ1 == sequenceLength && tookQ2 == sequenceLength) {
            tookQ1 = 0;
            tookQ2 = 0;
        }
    }
}

/**
 * Task of the first processor.
 * Last processor does not send numbers to next processor, but prints them.
 * 
 * @param cycle Cycle of algorithm.
 * @param id ID of processor.
*/
void lastProcTask(int cycle, int id) {
    unsigned number;

    if (cycle >= startRecv && cycle <= endRecv) {    // Receive.
        MPI_Recv(&number, 1, MPI_UNSIGNED, id-1, TAG, MPI_COMM_WORLD, &status);

        storeNumber(number);
    }

    if ((cycle >= startSend && cycle <= endSend)){   // Print
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

    // Program cannot run with a different number of processors.
    if (procCount != NUMBER_OF_PROCS) {
        error("This number of processors is not allowed.");
    }

    // Configure each processor.
    procConfig(procID);

    if (procID == 0) {      // Load n print input sequence from 1st processor.
        queue1 = getNumbers();
        printQueue(queue1);
    }

    // Algorithm ends in: n + 2^r + r - 1 cycles. It's same as 2*n + log n - 1.
    const int limit = NUMBER_OF_NUMBERS + (1 << (procCount - 1)) + (procCount - 1) - 1;

    for (int i = 0; i <= limit; i++) {
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

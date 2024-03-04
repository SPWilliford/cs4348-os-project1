#ifndef PROJECT1_COMMON_H
#define PROJECT1_COMMON_H

#include <iostream>
#include <fstream>

enum OperationCode
{
    READ,
    WRITE,
    END
};

struct Message
{
        OperationCode opCode;
        int address;
        int data; // only needed for write operation
};

int setup(int argc, char* argv[], std::ifstream& file, int& timerValue, int pipeCPUToMemory[2], int pipeMemoryToCPU[2]);

#endif

#include "Common.h"
#include "CPU.h"
#include "Memory.h"
#include <iostream>
#include <fstream>
#include <unistd.h>

int setup(int argc, char* argv[], std::ifstream& file, int& timerValue, int pipeCPUToMemory[2], int pipeMemoryToCPU[2])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <sample1.txt> <timerValue>" << std::endl;
        return 1;
    }

    file.open(argv[1]);
    if (!file)
    {
        std::cerr << "Could not open file: " << argv[1] << std::endl;
        return 1;
    }

    try
    {
        timerValue = std::atoi(argv[2]);
        if (timerValue <= 0)
        {
            std::cerr << "Invalid timer value: " << argv[2] << ". The timer value must be greater than 0." << std::endl;
            return 1;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error parsing timer value: " << e.what() << std::endl;
        return 1;
    }

    if (pipe(pipeCPUToMemory) == -1 || pipe(pipeMemoryToCPU) == -1)
    {
        std::cerr << "Failure to create pipes" << std::endl;
        return 1;
    }

    return 0; // Success
}

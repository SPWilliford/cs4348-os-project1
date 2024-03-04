#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sys/wait.h>

#include "CPU.h"
#include "Memory.h"
#include "Common.h"

int main(int argc, char* argv[])
{
   std::ifstream file;
   int timerValue;
   int pipeCPUToMemory[2];
   int pipeMemoryToCPU[2];

   if (setup(argc, argv, file, timerValue, pipeCPUToMemory, pipeMemoryToCPU) != 0)
   {
       return 1;
   }

    pid_t pid = fork();
    if (pid == -1)
    {
        std::cerr << "Fork failed" << std::endl;
    }
    else if (pid > 0) // parent process - CPU
    {
        close(pipeCPUToMemory[0]);
        close(pipeMemoryToCPU[1]);

        simulateCPU(pipeMemoryToCPU[0], pipeCPUToMemory[1], timerValue);
        wait(NULL);
    }
    else // child process - Memory
    {
        close(pipeMemoryToCPU[0]);
        close(pipeCPUToMemory[1]);

        simulateMemory(pipeCPUToMemory[0], pipeMemoryToCPU[1], file);
    }

    return 0;
}

#include "Common.cpp"
#include "Memory.cpp"
#include "CPU.cpp"

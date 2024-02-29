#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <program.txt> <timerValue>" <<std::endl;
        return 1;
    }

    const char* filename = argv[1];
    int timerValue = std::atoi(argv[2]);

    if (timerValue <= 0)
    {
        std::cerr << "Invalid timer value: " << argv[2] << ". The timer value must be greater than 0.";
    }

    if (fork())
    {
        // parent process
        std::cout << "hello from parent" << std::endl;
    }
    else
    {
        // child process
        std::cout << "hello from child" << std::endl;
    }

}

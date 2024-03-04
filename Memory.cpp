#include <string>
#include <sstream>
#include <unistd.h>
#include <iostream>
#include "Common.h"

void simulateMemory(int read_fd, int write_fd, std::ifstream& file)
{
    int memory[2000] = {0};
    loadMemoryFromFile(file, memory, 2000);

    while (true)
    {
        Message msg{};

        ssize_t bytesRead = read(read_fd, &msg, sizeof(msg));
        if (bytesRead == -1)
        {
            std::cerr << "Read error occurred." << std::endl;
            break;
        }
        switch (msg.opCode)
        {
            case READ:
            {
                int value = memory[msg.address];
                write(write_fd, &value, sizeof(value));
                break;
            }
            case WRITE:
            {
                memory[msg.address] = msg.data;
                break;
            }
            case END:
            {
                return;

            }
            default:
                std::cerr << "Unknown operation requested." << std::endl;
        }
    }
}

void loadMemoryFromFile(std::ifstream& file, int memory[], int memorySize)
{
    std::string line;
    int address = 0;
    while (std::getline(file, line) && address < memorySize)
    {
        if (line.empty()) continue;
        std::istringstream lineStream(line);
        char firstChar = lineStream.peek();
        if (firstChar == '.')
        {
            lineStream.ignore();
            int newAddress;
            if (lineStream >> newAddress)
            {
                if (newAddress >= 0 && newAddress < memorySize)
                {
                    address = newAddress;
                }
                else
                {
                    std::cerr << "Error: Address out of bounds (" << newAddress << ")." << std::endl;
                }
            }
            continue;
        }
        int value;
        if (lineStream >> value)
        {
            if (address < memorySize)
            {
                memory[address++] = value;
            }
            else
            {
                std::cerr << "Error: Memory overflow at address " << address << "." << std::endl;
                break;
            }
        }
    }
}

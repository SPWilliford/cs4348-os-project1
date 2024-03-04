#ifndef PROJECT1_MEMORY_H
#define PROJECT1_MEMORY_H

#include "Common.h"

void simulateMemory(int read_fd, int write_fd, std::ifstream& file);

void loadMemoryFromFile(std::ifstream& file, int memory[], int memorySize);

#endif //PROJECT1_MEMORY_H

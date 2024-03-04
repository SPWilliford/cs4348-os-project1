#ifndef PROJECT1_CPU_H
#define PROJECT1_CPU_H

#include "Common.h"


const int TIMER_INTERRUPT_HANDLER_ADDRESS = 1000;
const int SOFTWARE_INTERRUPT_HANDLER_ADDRESS = 1500;
const int SYSTEM_STACK_START_ADDRESS = 1999;

enum CPUMode { USER_MODE, KERNEL_MODE };

enum InterruptType { TIMER, SOFTWARE };

struct CPUState
{
    int PC = 0; // Program Counter
    int SP = 999; // Stack Pointer
    int IR = 0; // Instruction Register
    int AC = 0; // Accumulator
    int X = 0, Y = 0; // General-purpose registers
    int operand = 0; // Operand for current instruction
    int instructionCount = 0;
    int timerThreshold;
    CPUMode mode = USER_MODE;
    bool running = true; // CPU running state
};

void simulateCPU(int read_fd, int write_fd, int timerThreshold);

void fetchInstruction(int read_fd, int write_fd, CPUState& cpu);

void executeInstruction(int read_fd, int write_fd, CPUState& cpu);

void handleInterrupt(int read_fd, int write_fd, CPUState& cpu, InterruptType type);

void restoreStateAfterInterrupt(int read_fd, int write_fd, CPUState& cpu);

void saveStateForInterrupt(int write_fd, CPUState& cpu);

bool instructionRequiresOperand(int IR);

int readFromMemory(int read_fd, int write_fd, int address, CPUState& cpu);

void writeToMemory(int write_fd, int address, int value, CPUState& cpu);

int getRandomNumber();

void putPort(int AC,int operand);

void pushToStack(int write_fd, CPUState& cpu, int value);

int popFromStack(int read_fd, int write_fd, CPUState& cpu);


#endif //PROJECT1_CPU_H

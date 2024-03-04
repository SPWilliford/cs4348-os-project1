#ifndef PROJECT1_CPU_H
#define PROJECT1_CPU_H

#include "Common.h"


const int TIMER_INTERRUPT_HANDLER_ADDRESS = 1000;
const int SOFTWARE_INTERRUPT_HANDLER_ADDRESS = 1500;
const int SYSTEM_STACK_START_ADDRESS = 1999;

enum CPUMode { USER_MODE, KERNEL_MODE };

enum InterruptType { TIMER, SOFTWARE };

struct CPUState {
    int PC; // Program Counter
    int SP; // Stack Pointer
    int IR; // Instruction Register
    int AC; // Accumulator
    int X, Y; // General-purpose registers
    int operand; // Operand for current instruction
    int instructionCount;
    int timerThreshold;
    CPUMode mode;
    bool running; // CPU running state

    CPUState()
            : PC(0), SP(999), IR(0), AC(0), X(0), Y(0), operand(0), instructionCount(0), mode(USER_MODE),
              running(true) {}
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

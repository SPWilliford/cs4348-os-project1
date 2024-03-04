#include "Common.h"
#include <set>
#include <ctime>

void simulateCPU(int read_fd, int write_fd, int timerThreshold)
{
    CPUState cpu;
    cpu.timerThreshold = timerThreshold;

    while (cpu.running)
    {
        fetchInstruction(read_fd, write_fd, cpu);
        executeInstruction(read_fd, write_fd, cpu);

        cpu.instructionCount++;
        if (cpu.instructionCount >= cpu.timerThreshold)
        {
            handleInterrupt(read_fd, write_fd, cpu, TIMER);
        }
    }
}

void fetchInstruction(int read_fd, int write_fd, CPUState& cpu)
{
    Message fetchMsg = {READ, cpu.PC, 0};

    //fetch instruction and increment PC
    write(write_fd, &fetchMsg, sizeof(fetchMsg));
    read(read_fd, &cpu.IR, sizeof(cpu.IR));
    cpu.PC++;

    // check if instruction requires operand and fetch if necessary
    if (instructionRequiresOperand(cpu.IR))
    {
        fetchMsg.address = cpu.PC;
        write(write_fd, &fetchMsg, sizeof(fetchMsg));
        read(read_fd, &cpu.operand, sizeof(cpu.operand));
        cpu.PC++;
    }
}

void handleInterrupt(int read_fd, int write_fd, CPUState& cpu, InterruptType type)
{
    if (cpu.mode == KERNEL_MODE)
    {
        // Interrupts are not allowed in Kernel Mode
        return;
    }

    cpu.mode = KERNEL_MODE;
    saveStateForInterrupt(write_fd, cpu);

    if (type == TIMER)
    {
        cpu.instructionCount = 0;
        cpu.PC = TIMER_INTERRUPT_HANDLER_ADDRESS;
    }
    else if (type == SOFTWARE)
    {
        cpu.PC = SOFTWARE_INTERRUPT_HANDLER_ADDRESS;
    }

}

void saveStateForInterrupt(int write_fd, CPUState& cpu)
{
    int systemSP = SYSTEM_STACK_START_ADDRESS;

    // push the SP to the system stack
    systemSP--;
    writeToMemory(write_fd, systemSP, cpu.SP, cpu);

    // push the PC to system stack
    systemSP--;
    writeToMemory(write_fd, systemSP, cpu.PC, cpu);

    // update SP register
    cpu.SP = systemSP;
}

void restoreStateAfterInterrupt(int read_fd, int write_fd, CPUState& cpu)
{
    // Pop PC from system stack
    cpu.PC = readFromMemory(read_fd, write_fd, cpu.SP, cpu);
    cpu.SP++;
    cpu.SP = readFromMemory(read_fd, write_fd, cpu.SP, cpu);
    cpu.mode = USER_MODE;
}

bool instructionRequiresOperand(int IR)
{
    static std::set<int> instructionsRequiringOperands;
    instructionsRequiringOperands.insert(1);
    instructionsRequiringOperands.insert(2);
    instructionsRequiringOperands.insert(3);
    instructionsRequiringOperands.insert(4);
    instructionsRequiringOperands.insert(5);
    instructionsRequiringOperands.insert(7);
    instructionsRequiringOperands.insert(9);
    instructionsRequiringOperands.insert(20);
    instructionsRequiringOperands.insert(21);
    instructionsRequiringOperands.insert(22);
    instructionsRequiringOperands.insert(23);
    return instructionsRequiringOperands.find(IR) != instructionsRequiringOperands.end();
}

int readFromMemory(int read_fd, int write_fd, int address, CPUState& cpu)
{
    if (cpu.mode == USER_MODE && address >= TIMER_INTERRUPT_HANDLER_ADDRESS && address <= SYSTEM_STACK_START_ADDRESS)
    {
        std::cerr << "Memory access violation: Attempt to access system memory in user mode" << std::endl;
        Message msg = {END, 0, 0};
        write(write_fd, &msg, sizeof(msg));
        cpu.running = false;
        return 1;
    }
    Message msg = {READ, address, 0};
    write(write_fd, &msg, sizeof(msg));
    int value;
    read(read_fd, &value, sizeof(value));
    return value;
}

void writeToMemory(int write_fd, int address, int value, CPUState& cpu)
{
    if (cpu.mode == USER_MODE && (address >= 1000 && address <= 1999))
    {
        Message msg = {END, 0, 0};
        write(write_fd, &msg, sizeof(msg));
        std::cerr << "Memory access violation: Attempt to write to system memory in user mode" << std::endl;
        cpu.running = false;
        return;
    }
    Message msg = {WRITE, address, value};
    write(write_fd, &msg, sizeof(msg));
}

int getRandomNumber()
{
    static bool initialized = false;
    if (!initialized)
    {
        srand(std::time(NULL));
        initialized = true;
    }

    // Generate a random number between 1 and 100
    return rand() % 100 + 1;
}

void putPort(int AC,int operand)
{
    if (operand == 1)
    {
        std::cout << AC << std::flush; // Print AC as an integer
    }
    else if (operand == 2)
    {
        std::cout << static_cast<char>(AC) << std::flush; // Print AC as a character
    }
}

void pushToStack(int write_fd, CPUState& cpu, int value)
{
    cpu.SP--;
    writeToMemory(write_fd, cpu.SP, value, cpu);
}

int popFromStack(int read_fd, int write_fd, CPUState& cpu)
{
    int value = readFromMemory(read_fd, write_fd, cpu.SP, cpu);
    cpu.SP++;
    return value;
}

void executeInstruction(int read_fd, int write_fd, CPUState& cpu)
{
    switch (cpu.IR)
    {
        case 1: // Load value - Load the value into the AC
            cpu.AC = cpu.operand;
            break;
        case 2: // Load addr - Load the value at the address into the AC
            cpu.AC = readFromMemory(read_fd, write_fd, cpu.operand, cpu);
            break;
        case 3: // LoadInd addr - Load the value from the address found in the given address into the AC
        {
            int indirectAddress = readFromMemory(read_fd, write_fd, cpu.operand, cpu);
            cpu.AC = readFromMemory(read_fd, write_fd, indirectAddress, cpu);
            break;
        }
        case 4: // LoadIdxX addr - Load the value at (address+X) into the AC
            cpu.AC = readFromMemory(read_fd, write_fd, cpu.operand + cpu.X, cpu);
            break;
        case 5: // LoadIdxY addr - Load the value at (address+Y) into the AC
            cpu.AC = readFromMemory(read_fd, write_fd, cpu.operand + cpu.Y, cpu);
            break;
        case 6: // LoadSpX - Load from (SP+X) into the AC
            cpu.AC = readFromMemory(read_fd, write_fd, cpu.SP + cpu.X, cpu);
            break;
        case 7: // Store addr - Store the value in the AC into the address
            writeToMemory(write_fd, cpu.operand, cpu.AC, cpu);
            break;
        case 8: // Get - Gets a random int from 1 to 100 into the AC
            cpu.AC = getRandomNumber();
            break;
        case 9: // Put port - If port=1, writes AC as an int to the screen. If port=2, writes AC as char to the screen.
            putPort(cpu.AC, cpu.operand);
            break;
        case 10: // AddX - Add the value in X to the AC
            cpu.AC += cpu.X;
            break;
        case 11: // AddY - Add the value in Y to the AC
            cpu.AC += cpu.Y;
            break;
        case 12: // SubX - Subtract the value in X from the AC
            cpu.AC -= cpu.X;
            break;
        case 13: // subY - Subtract the value in Y from the AC
            cpu.AC -= cpu.Y;
            break;
        case 14: // CopyToX - Copy the value in the AC to X
            cpu.X = cpu.AC;
            break;
        case 15: // CopyFromX - Copy the value in X to the AC
            cpu.AC = cpu.X;
            break;
        case 16: // CopyToY - Copy the value in the AC to Y
            cpu.Y = cpu.AC;
            break;
        case 17: // CopyFromY - Copy the value in Y to the AC
            cpu.AC = cpu.Y;
            break;
        case 18: // CopyToSp - Copy the value in AC to the SP
            cpu.SP = cpu.AC;
            break;
        case 19: // CopyFromSp - Copy the value in SP to the AC
            cpu.AC = cpu.SP;
            break;
        case 20: // Jump addr - Jump to the address
            cpu.PC = cpu.operand;
            break;
        case 21: // JumpIfEqual addr - Jump to the address only if the value in the AC is zero
            if (cpu.AC == 0) { cpu.PC = cpu.operand; }
            break;
        case 22: // JumpIfNotEqual addr - Jump to the address only if the value in the AC is not zero
            if (cpu.AC != 0) { cpu.PC = cpu.operand; }
            break;
        case 23: // Call addr - Push return address onto stack, jump to the address
        {
            pushToStack(write_fd, cpu, cpu.PC + 1);
            cpu.PC = cpu.operand;
            break;
        }
        case 24: // Return - Pop return address from the stack, jump to the address
        {
            cpu.PC = popFromStack(read_fd, write_fd, cpu) - 1;
            break;
        }
        case 25: // IncX - Increment the value in X
            cpu.X++;
            break;
        case 26: // DecX - Decrement the value in X
            cpu.X--;
            break;
        case 27: // Push - Push AC onto stack
            pushToStack(write_fd, cpu, cpu.AC);
            break;
        case 28: // Pop - Pop from stack into AC
            cpu.AC = popFromStack(read_fd, write_fd, cpu);
            break;
        case 29: // Int - Perform system call
            handleInterrupt(read_fd, write_fd, cpu, SOFTWARE);
            break;
        case 30: // IRet - Return from system call
            restoreStateAfterInterrupt(read_fd, write_fd, cpu);
            break;
        case 50: // End - End execution
        {
            Message msg = {END, 0, 0};
            write(write_fd, &msg, sizeof(msg));
            cpu.running = false;
            break;
        }
        default:
            std::cerr << "Unknown instruction: " << cpu.IR << std::endl;
            break;
    }
}




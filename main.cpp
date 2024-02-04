#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <program file> <timer interval>" << std::endl;
        return 1;
    }

    std::string programFile = argv[1];
    int timerInterval = std::stoi(argv[2]);

    // Initialize CPU and Memory
    // You might use fork() and exec() here if you were separating into multiple processes
    // For a single-file project, you could simulate this behavior within a unified program flow

    std::cout << "Starting simulation with program: " << programFile
              << " and timer interval: " << timerInterval << std::endl;

    return 0;
}

#include <iostream>
#include <thread>

#include "Untils.hpp"
#include "UniqueWordsCounterManager.hpp"

int main(int argc, char** argv) {
    if (!isInputValid(argc, argv)) {
        printUsage();
        return 1;
    }

    const size_t workerCount = std::thread::hardware_concurrency();
    UniqueWordsCounterManager counterManager(argv[1], workerCount);

    std::cout << "Counting ..." << std::endl;
    std::cout << counterManager.CountUniqueWords() << std::endl;

    return 0;
}
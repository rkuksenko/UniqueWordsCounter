#ifndef UNIQUEWORDSCOUNTER_UTILS
#define UNIQUEWORDSCOUNTER_UTILS

#include <fstream>

bool isInputValid(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Invalid arguments count" << std::endl;
        return false;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: \"" << argv[1] << "\"" << std::endl;
        return false;
    }

    return true;
}

void printUsage() {
    std::cout << "Usage:" << std::endl;
    std::cout << "\tuiqueWordsCounter [filepath]" << std::endl;
    std::cout << "Example:" << std::endl;
    std::cout << "\t./uiqueWordsCounter ~/Effective_C++_55_Specific.txt" << std::endl;
}

#endif // UNIQUEWORDSCOUNTER_UTILS
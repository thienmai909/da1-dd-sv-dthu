#include <iostream>
#include <vector>
#include <filesystem>
#include "utility.hpp"

int main() {
    try {
        auto a = utility_input::readIntRangeRetry("Enter value", 10, 20, 3);

        if (a) std::cout << a.value();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}
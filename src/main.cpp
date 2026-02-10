#include <iostream>
#include <vector>
#include <optional>
#include <filesystem>
#include <typeinfo>
#include "utility.hpp"

int main() {
    try {
        auto a = utility_input::readInt<int>("Enter value: ", 1, 10, utility_input::Options(Option::None, Option::Ok));
        
        if (a) std::cout << "Has value: " << a.value() << " - type: " << typeid(a.value()).name() << std::endl;
        else std::cout << "Not value" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}
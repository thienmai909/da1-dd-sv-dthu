#include <iostream>
#include <vector>
#include <filesystem>
#include "utility.hpp"

int main() {
    try {
        const auto rows = utility_csv::read_csv("../data/sample.csv");

        for (const auto& h : rows.headers) 
            std::cout << h << " ";
        std::cout << std::endl;

        for (const auto& row : rows.rows) { 
            for (const auto& cell : row) 
                std::cout << cell << " ";
            std::cout << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}
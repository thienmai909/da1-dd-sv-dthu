#pragma once
#include <concepts>
#include <vector>
#include <string>
#include <span>

namespace School {
    template<typename T>
    concept Numeric = std::integral<T> || std::floating_point<T>;

    double calculate_average(std::span<const double> scores);

    struct Student {
        std::string name;
        std::vector<double> scores;
    };
}
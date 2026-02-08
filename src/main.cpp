#include <iostream>
#include <vector>
#include "stats.hpp"

int main() {
    std::vector<School::Student> classroom = {
        {"Nguyen Van A", {8.5, 9.0, 7.5}},
        {"Tran Thi B", {10.0, 9.5, 9.0}},
        {"Le Van C", {5.0, 6.5}}
    };

    std::cout << "=== BANG DIEM HOC SINH ===\n";

    for (const auto& student : classroom) {
        double avg = School::calculate_average(student.scores);
        std::cout << "Hoc sinh: " << student.name << " | Diem TB: " << avg << std::endl;
    }

    return 0;
}
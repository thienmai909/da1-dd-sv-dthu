#include <iostream>
#include "models.hpp"

int main() {
    DateTime now = DateTime::now();
    std::cout << "Now: " << now.toString() << "\n";

    DateTime future = now.addDays(30);
    std::cout << "After 30 days: "
              << future.toString() << "\n";

    std::cout << "Days between: "
              << now.daysBetween(future) << "\n";
}

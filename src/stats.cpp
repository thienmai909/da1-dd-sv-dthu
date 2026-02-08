#include "stats.hpp"
#include <numeric>

namespace School {
    double calculate_average(std::span<const double> scores) {
        if (scores.empty()) {
            return 0.0;
        }

        double sum = std::accumulate(scores.begin(), scores.end(), 0.0);

        return sum / scores.size();
    }
}
//
// Created by cmcfarlen on 12/15/21.
//

#include "util.h"

int dist(int a, int b) {
    if (b > a) {
        return b - a;
    }
    return a - b;
}

TEST_CASE("day7", "[aoc2021]") {
    auto input = split_char(slurp("day7.txt")[0], ',');
    auto positions = input | std::views::transform(to_int);

    auto best = std::make_pair(0, std::numeric_limits<uint64_t>::max());

    for (int p = 0; p < positions.size(); p++) {
        uint64_t cost = 0;
        for (int loc: positions) {
            uint64_t d = dist(loc, p);
            cost += ((d * (d+1)) / 2);
        }
        if (cost < best.second) {
            best = std::make_pair(p, cost);
        }
    }

    std::cout << "best position is: " << best.first << " cost " << best.second << "\n";
    REQUIRE(best.second == 101618069);
}



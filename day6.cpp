//
// Created by cmcfarlen on 12/15/21.
//

#include "util.h"


// day 6

TEST_CASE("day6", "[aoc2021]") {
    auto input = split_char(slurp("day6.txt")[0], ',');
    auto ints = input | std::views::transform(to_u64);
    int days = 256;

    std::array<uint64_t ,9> counts{};

    for (int idx: ints) {
        counts[idx]++;
    }

    for (int d = 0; d < days; d++) {
        uint64_t new_fish = counts[0];
        for (int i = 0; i < 8; i++) {
            counts[i] = counts[i+1];
        }
        counts[6] += new_fish;
        counts[8] = new_fish;
    }

    uint64_t totalfish = 0;
    for (uint64_t f: counts) {
        totalfish += f;
    }
    REQUIRE(totalfish == 1743335992042);

    std::cout << "lionfish: " << totalfish << "\n";
}



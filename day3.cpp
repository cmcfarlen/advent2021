#include "util.h"

std::pair<int,int> count_bit(const std::vector<int>& ints, int bit) {
    std::pair<int,int> result {};
    int v = 1 << bit;
    for (int i: ints) {
        if (i & v) {
            result.second++;
        } else {
            result.first++;
        }
    }
    return result;
}

int most_common(const std::vector<int>& ints, int bit) {
    auto [zeros, ones] = count_bit(ints, bit);
    if (ones >= zeros) {
        return 1;
    }
    return 0;
}
int least_common(const std::vector<int>& ints, int bit) {
    auto [zeros, ones] = count_bit(ints, bit);
    if (zeros <= ones) {
        return 0;
    }
    return 1;
}

bool bit_is(int v, int bit, int value) {
    return (v & (1 << bit)) == (value << bit);
}

unsigned int calculate_gamma(const std::vector<int>& ints, int bits_count) {
    unsigned int result = 0;
    for (int bit = 0; bit < bits_count; bit++) {
        auto [zeros, ones] = count_bit(ints, bit);
        if (ones > zeros) {
            result |= 1 << bit;
        }
    }
    return result;
}

unsigned int calculate_oxygen_generator_rating(std::vector<int> ints, int bits_count) {
    while (ints.size() > 1 && --bits_count >= 0) {
        int bit = most_common(ints, bits_count);

        ints = filter(ints, [=](auto x) { return bit_is(x, bits_count, bit); });

        std::cout << std::format("bit {}: {}\n", bits_count, bit);
        //dump(ints);
    }

    return ints[0];
}

unsigned int calculate_co2_scrubber_rating(std::vector<int> ints, int bits_count) {
    while (ints.size() > 1 && --bits_count >= 0) {
        int bit = least_common(ints, bits_count);

        ints = filter(ints, [=](auto x) { return bit_is(x, bits_count, bit); });

        std::cout << std::format("bit {}: {}\n", bits_count, bit);
        //dump(ints);
    }
    return ints[0];
}

TEST_CASE("day3", "[aoc2021]") {
    auto lines = slurp("day3.txt");
    int bits_count = lines[0].size();
    auto ints = lines | std::views::transform(to_int_binary);

    //std::cout << "bits count = " << bits_count << "\n";
    //dump(ints);

    std::vector<int> v = std::vector<int>(std::ranges::begin(ints), std::ranges::end(ints));

    unsigned int gamma = calculate_gamma(v, bits_count);
    unsigned int epsilon = (~gamma & 0b111111111111);
    //std::cout << " Gamma = " << gamma << " Epsilon = " << epsilon << " - (" << gamma*epsilon << ")\n";
    auto power = gamma*epsilon;
    REQUIRE(power == gamma*epsilon);

    unsigned int o2rating = calculate_oxygen_generator_rating(v, bits_count);
    unsigned int co2rating = calculate_co2_scrubber_rating(v, bits_count);

    //std::cout << "o2rating: " << o2rating << " co2: " << co2rating << "\n";
    //std::cout << "  life support rating: " << o2rating * co2rating << "\n";
    auto rating = o2rating * co2rating;
    REQUIRE(rating == 4406844);
}

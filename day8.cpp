#include "util.h"

bool is_unique_seq(const std::string& p) {
    switch (p.size()) {
        case 2:
        case 4:
        case 3:
        case 7:
            return true;
    }
    return false;
}

std::string sorted(const std::string& s) {
    std::string result = s;
    std::sort(result.begin(), result.end());
    return result;
}

std::string difference(const std::string& a, const std::string& b) {
    std::string result;
    std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(result));
    std::sort(result.begin(), result.end());
    return result;
}
std::string differences(const std::vector<std::string>& v) {
    return std::accumulate(std::next(v.begin()), v.end(), v.front(), difference);
}

std::string differences(const auto&... args) {
    std::vector<std::string> result;
    (result.push_back(args), ...);
    return differences(result);
}

std::string intersection(const std::string& a, const std::string& b) {
    std::string result;
    std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(result));
    std::sort(result.begin(), result.end());
    return result;
}

std::string intersections(const std::string& a, const auto&... args) {
    return (intersection(a, args), ...);
}

std::string intersections(const std::vector<std::string>& v) {
    return std::accumulate(std::next(v.begin()), v.end(), v.front(), intersection);
}

struct DecodeKey {
    std::string key;

    char decode(char x) {
        auto it = std::find(key.begin(), key.end(), x);
        return 'a' + (it - key.begin());
    }

    std::string decode(const std::string& x) {
        std::string result;
        std::transform(x.begin(), x.end(), std::back_inserter(result), [&](char x){return decode(x);});
        return sorted(result);
    }
};

struct DisplayInfo {
    std::array<std::string, 10> patterns;
    std::array<std::string, 4>  output;

    std::string find_size(size_t len) const {
        auto f = std::find_if(patterns.begin(), patterns.end(), [len](const std::string& x) { return x.size() == len; });
        if (f == patterns.end()) {
            fail(std::format("failed to find with size {} ", len));
        }
        return *f;
    }

    std::vector<std::string> copy_size(size_t len) const {
        std::vector<std::string> result;
        std::copy_if(patterns.begin(), patterns.end(), std::back_inserter(result), [len](const std::string& x) { return x.size() == len; });
        return result;
    }

    DecodeKey crack() const {
        auto one = find_size(2);
        auto four = find_size(4);
        auto seven = find_size(3);
        auto eight = find_size(7);

        //std::cout << std::format("1: {} 4: {} 7: {} 8: {}\n", one, four, seven, eight);

        // a - seven - one
        auto a = difference(seven, one);
        //std::cout << "A: " << a << "\n";

        auto fivesize = copy_size(5);
        auto midthree = intersections(fivesize);
        //std::cout << "middle three: " << midthree << "\n";

        // d - intersect (2, 3, 4, 5) (5 lengths)
        auto d = intersection(midthree, four);
        //std::cout << "D: " << d << "\n";

        // b - four - a - one - d
        auto b = differences(four, a, one, d);
        //std::cout << "B: " << b << "\n";

        // g - intersect 5 lengths - a - d
        auto g = differences(midthree, a, d);
        //std::cout << "G: " << g << "\n";

        // e - eight - seven - four - g
        auto e = differences(eight, seven, four, g);
        //std::cout << "E: " << e << "\n";

        // f - intersect 6 lengths with 1
        auto f = intersection(intersections(copy_size(6)), one);
        //std::cout << "F: " << f << "\n";

        // c - one - f
        auto c = difference(one, f);
        //std::cout << "C: " << c << "\n";

        return {a + b + c + d + e + f + g};
    }
};

int to_digit(const std::string& s) {
    static std::vector<std::string> digits = {
            "abcefg",   // 0
            "cf",       // 1
            "acdeg",    // 2
            "acdfg",    // 3
            "bcdf",     // 4
            "abdfg",    // 5
            "abdefg",   // 6
            "acf",      // 7
            "abcdefg",  // 8
            "abcdfg"    // 9
    };
    auto it = std::find(digits.begin(), digits.end(), s);
    if (it == digits.end()) {
        return -1;
    }
    return it - digits.begin();
}

TEST_CASE("day8", "[aoc2021]") {
    std::vector<DisplayInfo> input;
    auto lines = slurp("day8.txt");
    for (const std::string& line: lines) {
        auto parts = split_space(line);
        DisplayInfo info{};
        for (int i = 0; i < 10; i++) {
            info.patterns[i] = sorted(parts[i]);
        }
        for (int i = 0; i < 4; i++) {
            info.output[i] = sorted(parts[i+11]);
        }
        input.push_back(info);
    }

    int count = 0;
    for (const auto& i: input) {
        count += std::count_if(i.output.begin(), i.output.end(), is_unique_seq);
    }

    REQUIRE(count == 473);

    int total = 0;
    for (const auto& i: input) {
        auto key = i.crack();

        int val = 0;
        for (const auto& s: i.output) {
            auto v = key.decode(s);
            auto digit = to_digit(v);
            val = (val * 10) + digit;
        }
        total += val;
    }
    REQUIRE(total == 1097568);
}
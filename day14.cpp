#include "util.h"
#include <compare>

namespace day14 {

    struct Rule {
        std::string pattern;
        std::string insert;

        bool operator==(const Rule &) const = default;

        std::strong_ordering operator<=>(const Rule &) const = default;
    };

    struct RuleIndex {
        std::vector<char> buffer;

        RuleIndex(const std::vector<Rule> &rules) : buffer(26 * 26) {
            for (const Rule &r: rules) {
                add_rule(r);
            }
        }

        void add_rule(const Rule &rule) {
            buffer[(rule.pattern[0] - 'A') * 26 + (rule.pattern[1] - 'A')] = rule.insert[0];
        }

        char lookup(char a, char b) const {
            return buffer[(a - 'A') * 26 + (b - 'A')];
        }
    };

    struct Input {
        std::string templ;
        RuleIndex rules;
    };

    auto parse_input(const std::string &path) {
        auto lines = slurp(path);
        auto it = lines.begin();
        std::string t = *it++;
        std::vector<Rule> rules;

        while (it != lines.end()) {
            auto parts = split_char(*it++, ' ');
            rules.push_back({parts[0], parts[2]});
        }

        std::sort(rules.begin(), rules.end());

        return Input{t, RuleIndex(rules)};
    }

    std::vector<std::string_view> sliding(const std::string &s) {
        auto it = s.begin();
        std::vector<std::string_view> result;

        result.reserve(s.size());

        do {
            result.emplace_back(it, it + 2);
            ++it;
        } while ((it + 2 - 1) != s.end());

        return result;
    }

    std::string iterate(const std::string &s, const RuleIndex &rules) {
        std::string result(s.size() * 2 + 1, 0);
        std::size_t idx = 0;
        auto it = s.begin();

        do {
            result[idx++] = *it;
            result[idx++] = rules.lookup(*it, *(it + 1));
            ++it;
        } while ((it + 2 - 1) != s.end());

        result[idx++] = s.back();

        result.resize(idx);

        return result;
    }

    using PairsCounter = std::map<std::pair<char,char>, std::size_t>;

    std::size_t iterate2(const std::string &s, const RuleIndex &rules, int steps) {
        PairsCounter current_counts{};
        std::map<char, std::size_t> letter_counts{};

        for (auto p: sliding(s)) {
            current_counts[std::make_pair(p[0], p[1])]++;
        }
        for (char c: s) {
            letter_counts[c]++;
        }

        for (int step = 0; step < steps; step++) {
            PairsCounter new_counts{};

            for (auto [p, cnt]: current_counts) {
                char add = rules.lookup(p.first, p.second);
                auto p1 = std::make_pair(p.first, add);
                auto p2 = std::make_pair(add, p.second);
                new_counts[p1] += cnt;
                new_counts[p2] += cnt;
                letter_counts[add] += cnt;
            }

            current_counts = std::move(new_counts);
        }

        std::size_t max = std::numeric_limits<std::size_t>::min();
        std::size_t min = std::numeric_limits<std::size_t>::max();

        for (auto [k, v]: letter_counts) {
            if (v > 0) {
                max = std::max(max, v);
                min = std::min(min, v);
            }
        }

        return max - min;
    }

    std::array<std::size_t,26> frequencies(const std::string& s) {
        std::array<std::size_t,26> result {};
        for (char c: s) {
            result[c-'A']++;
        }
        return result;
    }

    int score(const std::string& s) {
        std::size_t max = std::numeric_limits<std::size_t>::min();
        std::size_t min = std::numeric_limits<std::size_t>::max();

        for (auto v: frequencies(s)) {
            if (v > 0) {
                max = std::max(max, v);
                min = std::min(min, v);
            }
        }

        return max - min;
    }
}

TEST_CASE("day14e", "[aoc2021]") {
    auto input = day14::parse_input("day14example.txt");

    for (auto& s: day14::sliding(input.templ)) {
        std::cout << s << "\n";
    }

    std::string t = input.templ;
    REQUIRE(t == "NNCB");
    t = day14::iterate(t, input.rules);
    REQUIRE(t == "NCNBCHB");
    t = day14::iterate(t, input.rules);
    REQUIRE(t == "NBCCNBBBCBHCB");
    t = day14::iterate(t, input.rules);
    REQUIRE(t == "NBBBCNCCNBBNBNBBCHBHHBCHB");
    t = day14::iterate(t, input.rules);
    REQUIRE(t == "NBBNBNBBCCNBCNCCNBBNBBNBBBNBBNBBCBHCBHHNHCBBCBHCB");

    t = day14::iterate(t, input.rules);
    REQUIRE(t.size() == 97);

    t = day14::iterate(t, input.rules);
    t = day14::iterate(t, input.rules);
    t = day14::iterate(t, input.rules);
    t = day14::iterate(t, input.rules);
    t = day14::iterate(t, input.rules);
    REQUIRE(t.size() == 3073);

    auto freq = frequencies(t);

    REQUIRE(freq['B'] == 1749);
    REQUIRE(freq['C'] == 298);
    REQUIRE(freq['H'] == 161);
    REQUIRE(freq['N'] == 865);

    REQUIRE(day14::score(t) == 1588);
}

TEST_CASE("day14ep2", "[aoc2021]") {
    auto input = day14::parse_input("day14example.txt");

    auto score = iterate2(input.templ, input.rules, 10);
    REQUIRE(score == 1588);
}

TEST_CASE("day14", "[aoc2021]") {
    auto input = day14::parse_input("day14.txt");

    auto t = input.templ;
    for (int i = 0; i < 10; i++) {
        t = day14::iterate(t, input.rules);
        std::cout << "step " << i << " length: " << t.size() << "\n";
    }

    REQUIRE(day14::score(t) == 2509);

    auto score = iterate2(input.templ, input.rules, 10);

    REQUIRE(score == 2509);

    score = iterate2(input.templ, input.rules, 40);

    REQUIRE(score == 2827627697643);
}
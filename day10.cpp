#include "util.h"

bool is_open(char x) {
    switch (x) {
        case '(':
        case '[':
        case '{':
        case '<':
            return true;
    }
    return false;
}

char expected(char x) {
    switch (x) {
        case '(':
            return ')';
        case '[':
            return ']';
        case '{':
            return '}';
        case '<':
            return '>';
    }
    return false;
}

int score_illegal_char(char x) {
    switch (x) {
        case ')':
            return 3;
        case ']':
            return 57;
        case '}':
            return 1197;
        case '>':
            return 25137;
    }
}

int score_complete_char(char x) {
    switch (x) {
        case ')':
            return 1;
        case ']':
            return 2;
        case '}':
            return 3;
        case '>':
            return 4;
    }
}

uint64_t score_complete_string(const std::string& s) {
    uint64_t total = 0;
    for (char x: s) {
        total = total * 5 + score_complete_char(x);
    }
    return total;
}

std::optional<char> find_corrupting(const std::string& s) {
    std::vector<char> stack;

    for (char c: s) {
        if (is_open(c)) {
            stack.push_back(c);
        } else {
            char e = expected(stack.back());
            if (e != c) {
                //std::cout << "'" << s <<  "' - Expected '" << e << "' but got '" << c << "'\n";
                return c;
            } else {
                stack.pop_back();
            }
        }
    }
    return std::nullopt;
}

std::optional<std::string> make_closing_string(const std::string& s) {
    std::vector<char> stack;

    for (char c: s) {
        if (is_open(c)) {
            stack.push_back(c);
        } else {
            char e = expected(stack.back());
            if (e != c) {
                // no closing strings for corrupted
                return std::nullopt;
            } else {
                stack.pop_back();
            }
        }
    }

    std::stringstream ss;
    while (!stack.empty()) {
        ss << expected(stack.back());
        stack.pop_back();
    }

    return ss.str();
}

int score_corrupt(const std::vector<std::string>& lines) {
    int total_score = 0;
    for (auto line: lines) {
        auto corr = find_corrupting(line);
        if (corr) {
            total_score += score_illegal_char(*corr);
        }
    }
    return total_score;
}

uint64_t score_complete(const std::vector<std::string>& lines) {
    std::vector<uint64_t> scores;
    for (auto line: lines) {
        auto complete = make_closing_string(line);
        if (complete) {
            scores.push_back(score_complete_string(*complete));
        }
    }
    std::sort(scores.begin(), scores.end());

    return scores[scores.size() / 2];
}

TEST_CASE("day10e", "[aoc2021]") {
    auto lines = slurp("day10example.txt");

    REQUIRE(score_corrupt(lines) == 26397);

    REQUIRE(score_complete(lines) == 288957);
}

TEST_CASE("day10", "[aoc2021]") {
    auto lines = slurp("day10.txt");

    REQUIRE(score_corrupt(lines) == 318081);

    REQUIRE(score_complete(lines) == 4361305341);
}

#include <string>
#include <vector>
#include <fstream>
#include <cerrno>
#include <cstring>
#include <format>
#include <iostream>
#include <ranges>
#include <functional>
#include <numeric>
#include <algorithm>
#include <sstream>

void dump(std::ranges::viewable_range auto&& r, std::ostream& out = std::cout, char* delim = "\n") {
    using R = decltype(r);
    using T = typename std::ranges::iterator_t<R>::value_type;
    std::copy(std::ranges::begin(r), std::ranges::end(r), std::ostream_iterator<T>(out, delim));
}

std::string pr_str(std::ranges::viewable_range auto&& r) {
    std::stringstream ss;
    ss << "[";
    dump(r, ss, ",");
    ss << "]";
    return ss.str();
}

void fail(const std::string& msg){
    if (errno != -1) {
        auto err = std::format("{}: {}", msg, ::strerror(errno));
        throw std::runtime_error(err.c_str());
    }
    throw std::runtime_error(msg.c_str());
}

std::vector<std::string> slurp(const std::string& name) {
    std::string path = "../input/" + name;

    std::ifstream f(path);
    std::vector<std::string> result{};
    if (f.is_open()) {
        std::string line;
        while (std::getline(f, line)) {
            if (!line.empty()) {
                result.push_back(line);
            }
        }
    } else {
        fail("open(" + path + ")");
    }
    return result;
}

std::vector<std::string> split_space(const std::string& l) {
    std::vector<std::string> result{};
    std::istringstream ss(l);
    std::string w;

    while (std::getline(ss, w, ' ')) {
        result.push_back(w);
    }

    return result;
}

int to_int(const std::string& s) {
    int i;
    std::from_chars(s.data(), s.data()+s.size(), i, 10);
    return i;
}

int to_int_binary(const std::string& s) {
    int i;
    std::from_chars(s.data(), s.data()+s.size(), i, 2);
    return i;
}

int sum(const std::vector<int>& v) {
    return std::accumulate(std::begin(v), std::end(v), 0);
}

auto partition(std::ranges::viewable_range auto&& r, int n) {
    using R = decltype(r);
    using T = typename std::ranges::iterator_t<R>::value_type;
    std::vector<std::vector<T>> result{};
    auto begin = std::ranges::begin(r);
    auto end = std::ranges::end(r);
    while ((begin + (n-1)) != end) {
        result.emplace_back(begin, begin+n);
        begin++;
    }
    return result;
}

auto count_incrementing(auto&& r) {
    auto count = std::accumulate(std::next(std::ranges::begin(r)), std::ranges::end(r), 0, [state=*std::ranges::begin(r)](int v, const auto& i) mutable {
        if (i > state) {
            v++;
        }
        state = i;
        return v;
    });

    return count;
}

int day1() {
    auto lines = slurp("day1.txt");

    auto ints = lines | std::views::transform(to_int);

    auto count = count_incrementing(ints);

    std::cout << "count is " << count << "\n";

    auto parts = partition(ints, 3);

    dump(parts | std::views::transform([](auto& r) { return pr_str(r); }));

    auto windowCount = count_incrementing(parts | std::views::transform(sum) );

    std::cout << "count is " << windowCount << "\n";

    return 0;
}

struct Position {
    int position = 0;
    int depth = 0;
};

Position operator+(const Position& a, const Position& b) {
    return Position{a.position + b.position, a.depth + b.depth};
}

std::ostream& operator<<(std::ostream& s, const Position& p) {
    return s << "[" << p.position << "," << p.depth << "]";
}

Position parseCommand(const std::string& l) {
    auto parts = split_space(l);

    auto direction = parts[0];
    int amount = to_int(parts[1]);

    if (direction == "forward") {
        return {amount, 0};
    } else if (direction == "up") {
        return {0, -amount};
    } else if (direction == "down") {
        return {0, amount};
    } else {
        fail(std::format("bad command {}:{}", direction, amount));
    }

    return {};
}

struct SubState {
    int aim = 0;
    Position pos = {};
};

int day2() {
    auto lines = slurp("day2.txt");
    auto commands = lines | std::views::transform(parseCommand);

    Position final = std::accumulate(std::ranges::begin(commands), std::ranges::end(commands), Position{}, [](const Position& p, const Position& cmd) {
        return p + cmd;
    });

    std::cout << " final position: " << final << " = " << (final.position * final.depth) << "\n";

    SubState sstate = std::accumulate(std::ranges::begin(commands), std::ranges::end(commands), SubState{}, [](const SubState& p, const Position& cmd) {
        if (cmd.position == 0) {
            return SubState{p.aim + cmd.depth, p.pos};
        } else {
            return SubState{p.aim, p.pos + Position{cmd.position, cmd.position * p.aim}};
        }
    });

    std::cout << " final sub position: " << sstate.pos << " = " << (sstate.pos.position * sstate.pos.depth) << "\n";
    return 0;
}

std::pair<int,int> count_bit(std::vector<int> ints, int bit) {
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

unsigned int calculate_gamma(const std::vector<int>& ints) {
    unsigned int result = 0;
    for (int bit = 0; bit < 12; bit++) {
        auto [zeros, ones] = count_bit(ints, bit);
        if (ones > zeros) {
            result |= 1 << bit;
        }
    }
    return result;
}

int day3() {
    auto lines = slurp("day3.txt");
    auto ints = lines | std::views::transform(to_int_binary);

    dump(ints);

    std::vector<int> v = std::vector<int>(std::ranges::begin(ints), std::ranges::end(ints));

    unsigned int gamma = calculate_gamma(v);
    unsigned int epsilon = (~gamma & 0b111111111111);
    std::cout << " Gamma = " << gamma << " Epsilon = " << epsilon << " - (" << gamma*epsilon << ")\n";
    return 0;
}

int main() {
    try {
        day1();
    } catch (std::exception& e) {
        std::cout << "fail: " << e.what() <<"\n";
    }

    return 0;
}

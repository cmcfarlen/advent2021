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
#include <optional>

void dump(std::ranges::viewable_range auto&& r, std::ostream& out = std::cout, const char* delim = "\n") {
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

std::vector<std::string> split_char(const std::string& l, char delim) {
    std::vector<std::string> result{};
    std::istringstream ss(l);
    std::string w;

    while (std::getline(ss, w, delim)) {
        result.push_back(w);
    }

    return result;
}

std::vector<std::string> split_space(const std::string& l) {
    return split_char(l, ' ');
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

auto filter(std::ranges::viewable_range auto&& r, auto pred) {
    using R = decltype(r);
    using T = typename std::ranges::iterator_t<R>::value_type;
    std::vector<T> result{};

    std::copy_if(std::ranges::begin(r), std::ranges::end(r), std::back_inserter(result), pred);

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
        dump(ints);
    }

    return ints[0];
}

unsigned int calculate_co2_scrubber_rating(std::vector<int> ints, int bits_count) {
    while (ints.size() > 1 && --bits_count >= 0) {
        int bit = least_common(ints, bits_count);

        ints = filter(ints, [=](auto x) { return bit_is(x, bits_count, bit); });

        std::cout << std::format("bit {}: {}\n", bits_count, bit);
        dump(ints);
    }
    return ints[0];
}

int day3() {
    auto lines = slurp("day3.txt");
    int bits_count = lines[0].size();
    auto ints = lines | std::views::transform(to_int_binary);

    std::cout << "bits count = " << bits_count << "\n";
    dump(ints);

    std::vector<int> v = std::vector<int>(std::ranges::begin(ints), std::ranges::end(ints));

    unsigned int gamma = calculate_gamma(v, bits_count);
    unsigned int epsilon = (~gamma & 0b111111111111);
    std::cout << " Gamma = " << gamma << " Epsilon = " << epsilon << " - (" << gamma*epsilon << ")\n";

    unsigned int o2rating = calculate_oxygen_generator_rating(v, bits_count);
    unsigned int co2rating = calculate_co2_scrubber_rating(v, bits_count);

    std::cout << "o2rating: " << o2rating << " co2: " << co2rating << "\n";

    std::cout << "  life support rating: " << o2rating * co2rating << "\n";

    return 0;
}

// Day 4: Giant Squid Bingo

struct BingoBoard {
    int id;
    std::vector<std::pair<int,bool>> spots;

    BingoBoard(int id, std::vector<std::pair<int,bool>> spots) : id(id), spots(std::move(spots)) {}

    void mark(int v) {
        for (auto& spot: spots) {
            if (spot.first == v) {
                spot.second = true;
                return;
            }
        }
    }

    bool complete(int a, int b, int c, int d, int e) {
        return spots[a].second &&
               spots[b].second &&
               spots[c].second &&
               spots[d].second &&
               spots[e].second;
    }

    bool winning() {
        // diagonals don't count
        return complete(0, 1, 2, 3, 4) ||
               complete(5, 6, 7, 8, 9) ||
               complete(10, 11, 12, 13, 14) ||
               complete(15, 16, 17, 18, 19) ||
               complete(20, 21, 22, 23, 24) ||

               complete(0, 5, 10, 15, 20) ||
               complete(1, 6, 11, 16, 21) ||
               complete(2, 7, 12, 17, 22) ||
               complete(3, 8, 13, 18, 23) ||
               complete(4, 9, 14, 19, 24);
    }

    void print() {
        for (int i = 0; i < 25; i++) {
            auto s = spots[i];
            if (s.second) {
                std::cout << "*" << " ";
            } else {
                std::cout << s.first << " ";
            }
            if ((i % 5) == 0) {
                std::cout << "\n";
            }
        }
        std::cout << "\n";
    }

    int score() {
        int sum = 0;
        for (auto spot: spots) {
            if (!spot.second) {
                sum += spot.first;
            }
        }
        return sum;
    }
};

struct BingoGame {
    std::vector<int> moves;
    int move_idx;
    std::vector<BingoBoard> boards;

    std::optional<BingoBoard> update() {
        int move = moves[move_idx++];
        std::cout << "move: " << move << "\n";
        for (auto& b: boards) {
            b.mark(move);

            if (b.winning()) {
                return b;
            }
        }

        return std::nullopt;
    }

    int last_move() {
        if (move_idx > 0) {
            return moves[move_idx - 1];
        }
        return 0;
    }

    bool done() const { return move_idx == moves.size(); }

    void play() {
        while (!done()) {
            auto maybe = update();
            if (maybe) {
                std::cout << "board " << maybe->id << " wins!\n";
                std::cout << " score: " << maybe->score() * last_move() << "\n";
                return;
            }
            boards[0].print();
        }

        std::cout << " no board wins" << "\n";
    }
};

BingoGame parse_bingo_game(const std::string& name) {
    std::string path = "../input/" + name;

    std::ifstream f(path);

    if (f.is_open()) {
        int board = 0;
        std::string moves_line;
        std::getline(f, moves_line);
        std::vector<std::string> moves = split_char(moves_line, ',');
        std::vector<BingoBoard> boards;

        auto ints = moves | std::views::transform(to_int);


        std::vector<std::pair<int,bool>> spots;
        spots.reserve(25);
        while (f.good()) {
            for (int i = 0; i < 25; i++) {
                int mv;
                f >> mv;

                std::cout << "spot: " << mv << "\n";

                spots.emplace_back(mv, false);
            }

            boards.emplace_back(board++, spots);
            spots.clear();
        }

        return BingoGame{std::vector<int>(std::ranges::begin(ints), std::ranges::end(ints)), 0, std::move(boards)};
    }
    fail("open(" + path + ")");
    return {};
}

void day4() {
    BingoGame game = parse_bingo_game("day4.txt");

    std::cout << "Board count: " << game.boards.size() << "\n";
    std::cout << "Move count: " << game.moves.size() << "\n";

    game.play();
}

int main() {
    try {
        day4();
    } catch (std::exception& e) {
        std::cout << "fail: " << e.what() <<"\n";
    }

    return 0;
}

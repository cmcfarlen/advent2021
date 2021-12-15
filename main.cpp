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

template <std::ranges::viewable_range R, typename T = typename std::ranges::iterator_t<R>::value_type>
std::vector<T> to_vec(const R& r) {
    return std::vector<T>(std::ranges::begin(r), std::ranges::end(r));
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

uint64_t to_u64(const std::string& s) {
    uint64_t i;
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
    bool done = false;

    BingoBoard(int id, std::vector<std::pair<int,bool>> spots) : id(id), spots(std::move(spots)) {}

    void mark(int v) {
        if (!done) {
            for (auto& spot: spots) {
                if (spot.first == v) {
                    spot.second = true;
                    return;
                }
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
    std::vector<std::pair<int,int>> winners;

    std::optional<BingoBoard> update() {
        std::optional<BingoBoard> result{};
        int move = moves[move_idx++];
        std::cout << "move: " << move << "\n";
        for (auto& b: boards) {
            if (!b.done) {
                b.mark(move);

                if (b.winning()) {
                    b.done = true;
                    if (!result) {
                        result = b;
                    }
                }
            }
        }

        return result;
    }

    int last_move() {
        if (move_idx > 0) {
            return moves[move_idx - 1];
        }
        return 0;
    }

    bool done() const { return move_idx == moves.size() || winners.size() == boards.size(); }

    void play() {
        while (!done()) {
            auto maybe = update();
            if (maybe) {
                std::cout << "board " << maybe->id << " wins after moving " << last_move() << "\n";
                std::cout << " score: " << maybe->score() * last_move() << "\n";
                winners.emplace_back(maybe->id, maybe->score() * last_move());
            }
        }

        std::cout << "First win: " << winners[0].first << " score: " << winners[0].second << "\n";
        std::cout << "Last win: " << winners.back().first << " score: " << winners.back().second << "\n";

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

// day 5

struct Point {
    int x = 0;
    int y = 0;
};

std::ostream& operator<<(std::ostream& o, const Point& p) {
    return o << "[" << p.x << "," << p.y << "]";
}

struct Line {
    Point a{}, b{};

    bool is_horizontal() const {
        return a.y == b.y;
    }
    bool is_vertical() const {
        return a.x == b.x;
    }

    int min_x() const {
        return std::min(a.x, b.x);
    }
    int max_x() const {
        return std::max(a.x, b.x);
    }
    int min_y() const {
        return std::min(a.y, b.y);
    }
    int max_y() const {
        return std::max(a.y, b.y);
    }
    int x_step() const {
        if (a.x < b.x) {
            return 1;
        } else {
            return -1;
        }
    }
    int y_step() const {
        if (a.y < b.y) {
            return 1;
        } else {
            return -1;
        }
    }
};

struct Diagram {
    std::vector<int> coords;
    int width;
    int height;

    Diagram(int w, int h) : coords(w * h, 0), width(w), height(h) {}

    int& at(int x, int y) { return coords[y*width + x]; }
    const int& at(int x, int y) const { return coords[y*width + x]; }

    void mark(const Line& l) {
        if (l.is_horizontal()) {
            int y = l.a.y;
            for (const auto& x: std::views::iota(l.min_x(), l.max_x()+1)) {
                ++at(x,y);
            }
        } else if (l.is_vertical()) {
            int x = l.a.x;
            for (const auto& y: std::views::iota(l.min_y(), l.max_y()+1)) {
                ++at(x,y);
            }
        } else {
            int y = l.a.y;
            for (int x = l.a.x; x != (l.b.x + l.x_step()); x += l.x_step()) {
                ++at(x, y);
                y += l.y_step();
            }
        }
    }

    void print() const {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int v = at(x, y);
                if (v > 0) {
                    std::cout << at(x,y);
                } else {
                    std::cout << '.';
                }
            }
            std::cout << "\n";
        }
    }

};

Point calculate_diagram_size(const std::vector<Line>& lines) {
    Point result {};

    for (const auto& line: lines) {
        result.x = std::max({result.x, line.a.x, line.b.x});
        result.y = std::max({result.y, line.a.y, line.b.y});
    }

    result.x++;
    result.y++;

    return result;
}

std::ostream& operator<<(std::ostream& o, const Line& l) {
    return o << '<' << l.a << " " << l.b << ">";
}

std::vector<Line> parse_day5(const std::string& name) {
    std::vector<std::string> lines = slurp(name);
    std::vector<Line> result{};
    result.reserve(lines.size());

    for (const auto& line: lines) {
        auto parts = split_char(line, ' ');
        auto left = split_char(parts[0], ',');
        auto right = split_char(parts[2], ',');


        result.push_back(Line{{to_int(left[0]), to_int(left[1])}, {to_int(right[0]), to_int(right[1])}});
    }

    return result;
}

void day5() {
    auto lines = parse_day5("day5.txt");
    auto max = calculate_diagram_size(lines);
    std::cout << "diagram size: " << max << "\n";

    Diagram d(max.y, max.y);

    for (const auto&l: lines) {
        d.mark(l);
    }
    d.print();

    int count = 0;
    for (int a: d.coords) {
        if (a > 1) {
            ++count;
        }
    }
    std::cout << "score is " << count << "\n";
}

// day 6

void day6() {
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

    std::cout << "lionfish: " << totalfish << "\n";
}

int dist(int a, int b) {
    if (b > a) {
        return b - a;
    }
    return a - b;
}

void day7() {
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
}

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

void day8() {
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

    std::cout << "count of unique seq: " << count << "\n";


    int total = 0;
    for (const auto& i: input) {
        auto key = i.crack();
        std::cout << "key: " << key.key;

        int val = 0;
        for (const auto& s: i.output) {
            auto v = key.decode(s);
            auto digit = to_digit(v);
            val = (val * 10) + digit;
        }
        total += val;
        std::cout << " Value = " << val << "\n";
    }
    std::cout << "Total = " << total << "\n";
}

int main() {
    try {
        day8();
    } catch (std::exception& e) {
        std::cout << "fail: " << e.what() <<"\n";
    }

    return 0;
}

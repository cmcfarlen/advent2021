//
// Created by cmcfarlen on 12/15/21.
//

#include "util.h"

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

TEST_CASE("day5", "[aoc2021]") {
    auto lines = parse_day5("day5.txt");
    auto max = calculate_diagram_size(lines);
    //std::cout << "diagram size: " << max << "\n";

    Diagram d(max.y, max.y);

    for (const auto&l: lines) {
        d.mark(l);
    }
    //d.print();

    int count = 0;
    for (int a: d.coords) {
        if (a > 1) {
            ++count;
        }
    }
    std::cout << "score is " << count << "\n";

    REQUIRE(count == 19164);
}
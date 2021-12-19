//
// Created by cmcfarlen on 12/19/21.
//

#include "util.h"

namespace day13 {

    struct Paper {
        std::size_t width;
        std::size_t height;
        std::vector<bool> marks;

        Paper(std::size_t w, std::size_t h) : marks(w*h), width(w), height(h) {}

        bool marked(std::size_t x, std::size_t y) const {
            return marks[y * width + x];
        }

        void mark(std::size_t x, std::size_t y) {
            marks[y * width + x] = true;
        }

        int mark_count() const {
            return std::count(marks.begin(), marks.end(), true);
        }

        Paper hfold(std::size_t at) const {
            Paper result(width, at);

            for (std::size_t y = 0; y < at; y++) {
                for (std::size_t x = 0; x < width; x++) {
                    if (marked(x, y) || marked(x, 2 * at - y)) {
                        result.mark(x, y);
                    }
                }
            }

            return result;
        }

        Paper vfold(std::size_t at) const {
            Paper result(at, height);

            for (std::size_t x = 0; x < at; x++) {
                for (std::size_t y = 0; y < height; y++) {
                    if (marked(x, y) || marked(2 * at - x, y)) {
                        result.mark(x, y);
                    }
                }
            }

            return result;
        }

        Paper fold(const std::pair<int,int>& at) const {
            auto [x, y] = at;
            if (x == 0) {
                return hfold(y);
            } else {
                return vfold(x);
            }
        }

        void print() {
            for (std::size_t y = 0; y < height; y++) {
                for (std::size_t x = 0; x < width; x++) {
                    std::cout << (marked(x, y) ? "#" : ".");
                }
                std::cout << "\n";
            }
        }
    };

    auto parse_paper(const std::string& path) {
        std::vector<std::pair<int, int>> marks;
        auto lines = slurp(path, true);
        std::size_t width = 0;
        std::size_t height = 0;
        bool parse_paper = true;
        std::vector<std::pair<int,int>> commands;
        for (const auto& line: lines) {
            if (parse_paper) {
                if (line.size() > 0) {
                    auto parts = split_char(line, ',');
                    if (parts.size() == 2) {
                        std::size_t x = to_int(parts[0]);
                        std::size_t y = to_int(parts[1]);
                        width = std::max(x, width);
                        height = std::max(y, height);
                        marks.push_back(std::make_pair(x, y));
                    } else {
                        fail(line);
                    }
                } else {
                    parse_paper = false;
                }
            } else {
                auto parts = split_char(line, ' ');
                auto xy = split_char(parts.back(), '=');
                int v = to_int(xy.back());
                if (xy.front() == "x") {
                    commands.push_back(std::make_pair(v, 0));
                } else {
                    commands.push_back(std::make_pair(0, v));
                }
            }
        }

        Paper result(width+1, height+1);
        for (auto [x, y]: marks) {
            result.mark(x, y);
        }
        return std::make_pair(result, std::move(commands));
    }

    Paper fold(const Paper& p, std::vector<std::pair<int,int>> commands) {
        return std::accumulate(commands.begin(), commands.end(), p, std::mem_fn(&Paper::fold));
    }

}

TEST_CASE("day13ep1", "[aoc2021]") {
    auto [p, commands] = day13::parse_paper("day13example.txt");
    REQUIRE(p.fold(commands.front()).mark_count() == 17);

    fold(p, commands).print();
}

TEST_CASE("day13p1", "[aoc2021]") {
    auto [p, commands] = day13::parse_paper("day13.txt");
    REQUIRE(p.fold(commands.front()).mark_count() == 661);
}

TEST_CASE("day13p2", "[aoc2021]") {
    auto [p, commands] = day13::parse_paper("day13.txt");
    fold(p, commands).print();
}

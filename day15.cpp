#include "util.h"

namespace day15 {
    using Point = std::pair<int,int>;

    struct PathNode {
        Point pos;
        int f;
        int g;
        int h;
        Point parent_pos;
    };

    struct IntGrid {
        std::vector<int> grid;
        int width;
        int height;
        int scale;

        IntGrid(int w, int h, int s = 1) : grid(w*h, 0), width(w), height(h), scale(s) {}

        int scaled_width() const {
            return width * scale;
        }
        int scaled_height() const {
            return height * scale;
        }

        int& at(int x, int y) {
            int actual_x = x % width;
            int actual_y = y % height;
            return grid[actual_y*width + actual_x];
        }

        int at(int x, int y) const {
            int actual_x = x % width;
            int actual_y = y % height;

            int actual_at = grid[actual_y*width + actual_x];

            int bonus_x = x / width;
            int bonus_y = y / height;

            int real_at = actual_at + bonus_x + bonus_y;
            if (real_at > 9) {
                real_at = real_at - 9;
            }
            return real_at;
        }

        bool valid(const Point& p) const {
            return p.first >= 0 &&
                   p.second >= 0 &&
                   p.first < scaled_width() &&
                   p.second < scaled_height();
        }

        void print() const {
            for (int y = 0; y < scaled_height(); y++) {
                for (int x = 0; x < scaled_width(); x++) {
                    std::cout << at(x, y);
                }
                std::cout << "\n";
            }
        }

        void print(const std::vector<Point>& path) const {
            for (int y = 0; y < scaled_height(); y++) {
                for (int x = 0; x < scaled_width(); x++) {
                    auto it = std::find(path.begin(), path.end(), std::make_pair(x, y));
                    if (it == path.end()) {
                        std::cout << at(x, y);
                    } else {
                        std::cout << '*';
                    }
                }
                std::cout << "\n";
            }
        }

        int score(const std::vector<Point>& path) const {
            int result = 0;
            for (const auto& p: path) {
                result += at(p.first, p.second);
            }
            return result;
        }
    };


    int manhattan_distance(const Point& a, const Point& b) {
        return (std::max(a.first, b.first) - std::min(a.first, b.first)) + (std::max(a.second, b.second) - std::min(a.second, b.second));
    }

    std::array<Point, 4> successors(const Point& p) {
        return {std::make_pair(p.first-1, p.second),
                std::make_pair(p.first+1, p.second),
                std::make_pair(p.first, p.second-1),
                std::make_pair(p.first, p.second+1)};
    }

    std::vector<Point> extract_path(const std::vector<PathNode> nodes, const Point& from, const Point& to) {
        auto p = to;
        std::vector<Point> path{};
        while (p != from) {
            auto it = std::find_if(nodes.begin(), nodes.end(), [&p](const PathNode& o) { return o.pos == p; });
            if (it != nodes.end()) {
                path.push_back(it->pos);
                p = it->parent_pos;
            } else {
                std::cout << " failed to find point in nodes list\n";
                break;
            }
        }
        std::reverse(path.begin(), path.end());
        return path;
    }

    std::vector<Point> find_path(const IntGrid& grid, const Point& from, const Point& to) {
        std::vector<PathNode> open {{from, 0, 0, 0}};
        std::vector<PathNode> closed{};

        while (!open.empty()) {
            auto qit = std::min_element(open.begin(), open.end(),[](const PathNode& a, const PathNode& b) { return a.f < b.f; });
            auto q = *qit;
            open.erase(qit);

            for (const auto& s: successors(q.pos)) {
                if (grid.valid(s) && (std::find_if(closed.begin(), closed.end(), [&s](const PathNode& o) { return o.pos == s; }) == closed.end())) {
                    auto nit = std::find_if(open.begin(), open.end(), [&s](const PathNode& o) {
                        return o.pos == s;
                    });

                    int g = q.g + grid.at(s.first, s.second);
                    if (nit == open.end()) {
                        PathNode n{s, 0, 0, 0};
                        n.g = g;
                        n.h = manhattan_distance(s, to);
                        n.f = n.g + n.h;
                        n.parent_pos = q.pos;

                        open.push_back(n);
                    } else {
                        if (g < nit->g) {
                            nit->g = g;
                            nit->f = g + nit->h;
                            nit->parent_pos = q.pos;
                        }
                    }
                }
            }

            closed.push_back(q);

            if (q.pos == to) {
                return extract_path(closed, from, to);
            }
        }

        fail("Failed to find destination");
        return {};
    }

    IntGrid parse_input(const std::string& path, int scale = 1) {
        auto lines = slurp(path);
        int height = lines.size();
        int width = lines[0].size();

        IntGrid result(width, height, scale);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                result.at(x, y) = lines[y][x] - '0';
            }
        }
        return result;
    }
}

TEST_CASE("day15e", "[aoc2021]") {
    const auto grid = day15::parse_input("day15example.txt");

    //grid.print();

    std::cout << "\n";

    auto path = find_path(grid, std::make_pair(0, 0), std::make_pair(grid.width-1, grid.height-1));

    //grid.print(path);

    REQUIRE(grid.score(path) == 40);
}

TEST_CASE("day15ep2", "[aoc2021]") {
    const auto grid = day15::parse_input("day15example.txt", 5);

    REQUIRE(grid.at(0, 0) == 1);
    REQUIRE(grid.at(grid.width, 0) == (grid.at(0, 0) + 1));
    REQUIRE(grid.at(2*grid.width, 0) == (grid.at(0, 0) + 2));
    REQUIRE(grid.at(0, grid.height) == (grid.at(0, 0) + 1));
    REQUIRE(grid.at(0, grid.height*2) == (grid.at(0, 0) + 2));
    REQUIRE(grid.at(grid.width, grid.height) == (grid.at(0, 0) + 2));

    // wrapped coord
    REQUIRE(grid.at(2, 1) == 8);
    REQUIRE(grid.at(12, 1) == 9);
    REQUIRE(grid.at(22, 1) == 1);


    grid.print();

    std::cout << "\n";

    auto path = find_path(grid, std::make_pair(0, 0), std::make_pair(grid.scaled_width()-1, grid.scaled_height()-1));

    grid.print(path);

    REQUIRE(grid.score(path) == 315);
}

TEST_CASE("day15", "[aoc2021]") {
    auto grid = day15::parse_input("day15.txt");

    auto path = find_path(grid, std::make_pair(0, 0), std::make_pair(grid.scaled_width()-1, grid.scaled_height()-1));

    //grid.print(path);

    REQUIRE(grid.score(path) == 714);
}

TEST_CASE("day15p2", "[aoc2021]") {
    auto grid = day15::parse_input("day15.txt", 5);

    auto path = find_path(grid, std::make_pair(0, 0), std::make_pair(grid.scaled_width()-1, grid.scaled_height()-1));

    REQUIRE(grid.score(path) == 2948);
}
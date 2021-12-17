#include "util.h"

struct Map {
    std::vector<int8_t> map;
    int width;
    int height;

    Map(int w, int h) : width(w), height(h), map(w*h, 0) {}

    int8_t& at(int x, int y) {
        return map[y * width + x];
    }

    const int8_t at(int x, int y) const {
        if (x < 0 || y < 0 || x >= width || y >= height) {
            return 9;
        }
        return map[y * width + x];
    }

    auto adjacent(int x, int y) const {
        return std::make_tuple(at(x-1, y), at(x+1, y), at(x, y-1), at(x, y+1));
    }

    bool is_low(int x, int y) const {
        auto v = at(x, y);
        auto [a, b, c, d] = adjacent(x, y);

        return v < a && v < b && v < c && v < d;
    }

    std::vector<std::pair<int,int>> find_lows() const {
        std::vector<std::pair<int,int>> result;

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (is_low(x, y)) {
                    result.emplace_back(x, y);
                }
            }
        }

        return result;
    }

    void find_basin_r(int x, int y, std::vector<std::pair<int,int>>& basin) const {
        auto p = std::make_pair(x, y);
        if (basin.end() != std::find(basin.begin(), basin.end(), p)) {
            return;
        }
        int8_t h = at(x, y);
        if (h == 9) {
            return;
        }

        basin.push_back(p);

        find_basin_r(x-1, y, basin);
        find_basin_r(x+1, y, basin);
        find_basin_r(x, y+1, basin);
        find_basin_r(x, y-1, basin);
    }

    std::vector<std::pair<int,int>> find_basin(int x, int y) const {
        std::vector<std::pair<int,int>> result;

        find_basin_r(x, y, result);

        return result;
    }

    void print() const {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                std::cout << char('0' + at(x, y));
            }
            std::cout << "\n";
        }
    }
};

Map parse_map(const std::string& path) {
    auto lines = slurp(path);
    int width = lines[0].size();
    int height = lines.size();
    Map result(width, height);

    for (int y = 0; y < height; y++) {
        auto line = lines[y];
        for (int x = 0; x < width; x++) {
            result.at(x, y) = line[x] - '0';
        }
    }

    return result;
}

TEST_CASE("day9", "[aoc2021]") {
    Map m = parse_map("day9.txt");
    //m.print();

    auto lows = m.find_lows();

    int sum;
    for (auto l: lows) {
        auto [x, y] = l;
        sum += 1 + m.at(x, y);

    }
    REQUIRE(sum == 560);

    auto basins = to_vec(lows | std::views::transform([m](auto& pt) { return m.find_basin(pt.first, pt.second); }));
    std::sort(basins.begin(), basins.end(), [](const auto&a, const auto& b) { return b.size() < a.size(); });

    int product = 1;
    for (int i = 0; i < 3; i++) {
        product *= basins[i].size();
    }
    REQUIRE(product == 959136);


}
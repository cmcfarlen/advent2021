//
// Created by cmcfarlen on 12/16/21.
//

#include "util.h"

namespace day11 {
    struct Octopus {
        int energy;
        int flash_time; // update number of the last flash
    };

    struct Map {
        std::vector<Octopus> octopi;
        int width;
        int height;
        int round;
        int total_flashes;
        int first_all_flash;

        Map(int w, int h) : octopi(w*h), width(w), height(h), round(0) {}

        Octopus& at(int x, int y) {
            return octopi[y * width + x];
        }

        void update_octopus(int x, int y, std::function<void(Octopus&)> f) {
           if (x < 0 || y < 0 || x >= height || y >= height) {
               return;
           }
           f(octopi[y * width + x]);
        }

        void flash(int x, int y, int t) {
            update_octopus(x, y,     [t](Octopus& o) { o.flash_time = t; });
            update_octopus(x-1, y-1, [t](Octopus& o) { o.energy++; });
            update_octopus(x-1, y,   [t](Octopus& o) { o.energy++; });
            update_octopus(x-1, y+1, [t](Octopus& o) { o.energy++; });
            update_octopus(x+1, y-1, [t](Octopus& o) { o.energy++; });
            update_octopus(x+1, y,   [t](Octopus& o) { o.energy++; });
            update_octopus(x+1, y+1, [t](Octopus& o) { o.energy++; });
            update_octopus(x, y-1,   [t](Octopus& o) { o.energy++; });
            update_octopus(x, y+1,   [t](Octopus& o) { o.energy++; });
        }

        void update() {
            for (auto& o: octopi) {
                o.energy++;
            }

            int flashes = 0;
            do {
                flashes = 0;

                for (int y = 0; y < height; y++) {
                    for (int x = 0; x < width; x++) {
                        auto& o = octopi[y * width + x];
                        if (o.flash_time != round && o.energy > 9) {
                            flash(x, y, round);
                            flashes++;
                            total_flashes++;
                        }
                    }
                }
            } while (flashes > 0);

            for (auto& o: octopi) {
                if (o.energy > 9) {
                    o.energy = 0;
                }
            }

            round++;
        }

        void update(int count) {
            while(count--) {
                update();
            }
        }

        int update_until_sync() {
            int flashed_this_round = 0;
            do {
                update();

                flashed_this_round = 0;
                int rnd = round-1;

                for (auto& o: octopi) {
                    if (o.flash_time == rnd) {
                        flashed_this_round++;
                    }
                }
            } while (flashed_this_round != octopi.size());
            return round;
        }

        void print() {
            std::cout << "Round: " << round << " flashes: " << total_flashes << "\n";
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    auto &o = octopi[y * width + x];
                    std::cout << o.energy;
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
                result.at(x, y).energy = line[x] - '0';
            }
        }

        return result;
    }
}

TEST_CASE("day11e", "[aoc2021]") {
    day11::Map m = day11::parse_map("day11example.txt");

    m.update(10);

    REQUIRE(m.total_flashes == 204);

    m.update(90);

    REQUIRE(m.total_flashes == 1656);

    int sync = m.update_until_sync();

    REQUIRE(sync == 195);
}

TEST_CASE("day11", "[aoc2021]") {
    day11::Map m = day11::parse_map("day11.txt");

    m.update(100);

    REQUIRE(m.total_flashes == 1601);

    int sync = m.update_until_sync();

    REQUIRE(sync == 368);


}


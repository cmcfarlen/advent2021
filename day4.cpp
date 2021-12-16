//
// Created by cmcfarlen on 12/15/21.
//

#include "util.h"


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
        //std::cout << "move: " << move << "\n";
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
                //std::cout << "board " << maybe->id << " wins after moving " << last_move() << "\n";
                //std::cout << " score: " << maybe->score() * last_move() << "\n";
                winners.emplace_back(maybe->id, maybe->score() * last_move());
            }
        }

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

                //std::cout << "spot: " << mv << "\n";

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

TEST_CASE("day4", "[aoc2021]") {
    BingoGame game = parse_bingo_game("day4.txt");

    //std::cout << "Board count: " << game.boards.size() << "\n";
    //std::cout << "Move count: " << game.moves.size() << "\n";

    game.play();

    //std::cout << "First win: " << game.winners[0].first << " score: " << game.winners[0].second << "\n";

    REQUIRE(game.winners[0].second == 89001);
    REQUIRE(game.winners.back().second == 7296);

    //std::cout << "Last win: " << game.winners.back().first << " score: " << game.winners.back().second << "\n";

}



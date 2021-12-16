//
// Created by cmcfarlen on 12/15/21.
//

#include "util.h"

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

TEST_CASE("day2", "[aoc2021]") {
    auto lines = slurp("day2.txt");
    auto commands = lines | std::views::transform(parseCommand);

    Position final = std::accumulate(std::ranges::begin(commands), std::ranges::end(commands), Position{}, [](const Position& p, const Position& cmd) {
        return p + cmd;
    });

    auto hposition = (final.position * final.depth);
    REQUIRE(hposition == 2322630);

    SubState sstate = std::accumulate(std::ranges::begin(commands), std::ranges::end(commands), SubState{}, [](const SubState& p, const Position& cmd) {
        if (cmd.position == 0) {
            return SubState{p.aim + cmd.depth, p.pos};
        } else {
            return SubState{p.aim, p.pos + Position{cmd.position, cmd.position * p.aim}};
        }
    });

    hposition = (sstate.pos.position * sstate.pos.depth);
    REQUIRE(hposition == 2105273490);
}


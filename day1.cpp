#include "util.h"

TEST_CASE("day1", "[aoc2021]") {
    auto lines = slurp("day1.txt");

    auto ints = lines | std::views::transform(to_int);

    auto count = count_incrementing(ints);

    REQUIRE(count == 1475);

    auto parts = partition(ints, 3);

    //dump(parts | std::views::transform([](auto& r) { return pr_str(r); }));

    auto windowCount = count_incrementing(parts | std::views::transform(sum) );

    REQUIRE(windowCount == 1516);
}


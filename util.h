#ifndef AOC2021_UTIL_H
#define AOC2021_UTIL_H

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

#include <catch2/catch.hpp>
#include <unordered_map>
#include <execution>

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

void fail(const std::string& msg);

std::vector<std::string> slurp(const std::string& name, bool keep_empty = false);

std::vector<std::string> split_char(const std::string& l, char delim);

std::vector<std::string> split_space(const std::string& l);

int to_int(const std::string& s);

uint64_t to_u64(const std::string& s);

int to_int_binary(const std::string& s);

int sum(const std::vector<int>& v);

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

template <std::ranges::viewable_range R, typename T = typename std::ranges::iterator_t<R>::value_type>
auto frequencies(R&& r) {
    std::unordered_map<T,std::size_t> result;

    for (const auto& v: r) {
        result[v]++;
    }

    return result;
}

#endif //AOC2021_UTIL_H

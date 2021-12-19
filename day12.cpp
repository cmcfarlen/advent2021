#include "util.h"

namespace day12 {
    struct Node {
        std::string name;
        bool big;
    };

    bool is_big(const std::string& name) {
        char f = name[0];
        return f >= 'A' && f <= 'Z';
    }

    using Path = std::vector<std::size_t>;

    struct Graph {
        std::vector<Node> nodes{};
        std::vector<std::pair<int,int>> edges{};

        std::size_t find_or_insert(const std::string& name) {
            auto it = std::find_if(nodes.begin(), nodes.end(), [name](const auto& x) { return x.name == name; });
            if (it == nodes.end()) {
                std::size_t id = nodes.size();
                nodes.push_back({name, is_big(name)});
                return id;
            } else {
                return it - nodes.begin();
            }
        }

        void add_edge(const std::string& a, const std::string& b) {
            std::size_t aid = find_or_insert(a);
            std::size_t bid = find_or_insert(b);

            edges.emplace_back(aid, bid);
        }

        // return nodes visible from node n coming from node f (excludes f)
        std::vector<std::size_t> nodes_from(std::size_t n) {
            std::vector<std::size_t> result{};
            for (const auto& e: edges) {
                if (e.first == n) {
                    result.push_back(e.second);
                } else if (e.second == n) {
                    result.push_back(e.first);
                }
            }
            return result;
        }

        bool small(std::size_t n) {
            return !nodes[n].big;
        }

        void print(const Path& path) {
            for (auto n: path) {
                std::cout << nodes[n].name << ",";
            }
            std::cout << "\n";
        }

        std::vector<Path> paths_from(const Path& p, std::size_t end, std::size_t small_visit_max = 1) {
            std::vector<Path> result{};

            for (std::size_t n: nodes_from(p.back())) {
                if (n == p.front()) {
                    // can't return to the start
                    continue;
                }

                Path tmp = p;
                tmp.push_back(n);

                if (n == end) {
                    result.push_back(std::move(tmp));
                    continue;
                }

                if (small(n)) {
                    std::size_t count = std::count(p.begin(), p.end(), n);
                    if (count >= small_visit_max) {
                        // skip small nodes that we have already visited
                        continue;
                    }
                    if (small_visit_max == 2 && count == 1) {
                        auto tmp_result = paths_from(tmp, end, 1);
                        std::copy(tmp_result.begin(), tmp_result.end(), std::back_inserter(result));
                    } else {
                        auto tmp_result = paths_from(tmp, end, small_visit_max);
                        std::copy(tmp_result.begin(), tmp_result.end(), std::back_inserter(result));
                    }
                } else {
                    auto tmp_result = paths_from(tmp, end, small_visit_max);
                    std::copy(tmp_result.begin(), tmp_result.end(), std::back_inserter(result));
                }
            }

            return result;
        }
    };

    Graph parse_graph(const std::string& path) {
        Graph result{};
        auto lines = slurp(path);
        for (const auto& line: lines) {
            auto parts = split_char(line, '-');
            if (parts.size() == 2) {
                result.add_edge(parts[0], parts[1]);
            } else {
                fail(std::format("failed to split line (got {})", parts.size()));
            }
        }

        return result;
    }
}

TEST_CASE("day12e", "[aoc2021]") {
    day12::Graph g = day12::parse_graph("day12example.txt");
    std::size_t count = g.paths_from({g.find_or_insert("start")}, g.find_or_insert("end")).size();
    REQUIRE(count == 10);

    g = day12::parse_graph("day12example2.txt");
    count = g.paths_from({g.find_or_insert("start")}, g.find_or_insert("end")).size();
    REQUIRE(count == 19);
}

TEST_CASE("day12", "[aoc2021]") {
    day12::Graph g = day12::parse_graph("day12.txt");
    std::size_t count = g.paths_from({g.find_or_insert("start")}, g.find_or_insert("end")).size();
    REQUIRE(count == 5252);
}

TEST_CASE("day12ep2", "[aoc2021]") {
    day12::Graph g = day12::parse_graph("day12example.txt");
    std::size_t count = g.paths_from({g.find_or_insert("start")}, g.find_or_insert("end"), 2).size();
    REQUIRE(count == 36);

    g = day12::parse_graph("day12example2.txt");
    count = g.paths_from({g.find_or_insert("start")}, g.find_or_insert("end"), 2).size();
    REQUIRE(count == 103);
}

TEST_CASE("day12p2", "[aoc2021]") {
    day12::Graph g = day12::parse_graph("day12.txt");
    std::size_t count = g.paths_from({g.find_or_insert("start")}, g.find_or_insert("end"), 2).size();
    REQUIRE(count == 147784);
}


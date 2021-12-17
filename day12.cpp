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

    using Path = std::vector<int>;

    struct Graph {
        std::vector<Node> nodes;
        std::vector<std::pair<int,int>> edges;

        int find_or_insert(const std::string& name) {
            auto it = std::find_if(nodes.begin(), nodes.end(), [name](const auto& x) { return x.name == name; });
            if (it == nodes.end()) {
                int id = nodes.size();
                nodes.push_back({name, is_big(name)});
            } else {
                return it - nodes.begin();
            }
        }

        void add_edge(const std::string& a, const std::string& b) {
            int aid = find_or_insert(a);
            int bid = find_or_insert(b);

            edges.emplace_back(aid, bid);
        }

        // return nodes visible from node n coming from node f (excludes f)
        std::vector<int> nodes_from(int n) {
            std::vector<int> result{};
            for (const auto& e: edges) {
                if (e.first == n) {
                    result.push_back(e.second);
                } else if (e.second == n) {
                    result.push_back(e.first);
                }
            }
            return result;
        }

        bool small(int n) {
            return !nodes[n].big;
        }

        std::vector<Path> paths_from(const Path& p, int end) {
            std::vector<Path> result{};

            for (int n: nodes_from(p.back())) {
                if (small(n) && find(p.begin(), p.end(), n) != p.end()) {
                    // skip small nodes that we have already visited
                    continue;
                }

                result.push_back(p);
                result.back().push_back(n);

            }

            return result;
        }
    };

    Graph parse_graph(const std::string& path) {
        Graph result{};
        auto lines = slurp(path);
        for (const auto& line: lines) {
            auto parts = split_char(line, '-');
            result.add_edge(parts[0], parts[1]);
        }

        return result;
    }
}

TEST_CASE("day12e", "[aoc2021]") {
    day12::Graph g = day12::parse_graph("day12example.txt");

    int count = g.paths_from({{g.find_or_insert("start")}}, g.find_or_insert("end")).size();
    REQUIRE(count == 10);
}


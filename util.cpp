#include "util.h"

void fail(const std::string& msg){
    if (errno != -1) {
        auto err = std::format("{}: {}", msg, ::strerror(errno));
        throw std::runtime_error(err.c_str());
    }
    throw std::runtime_error(msg.c_str());
}

std::vector<std::string> slurp(const std::string& name) {
    std::string path = "../input/" + name;

    std::ifstream f(path);
    std::vector<std::string> result{};
    if (f.is_open()) {
        std::string line;
        while (std::getline(f, line)) {
            if (!line.empty()) {
                result.push_back(line);
            }
        }
    } else {
        fail("open(" + path + ")");
    }
    return result;
}

std::vector<std::string> split_char(const std::string& l, char delim) {
    std::vector<std::string> result{};
    std::istringstream ss(l);
    std::string w;

    while (std::getline(ss, w, delim)) {
        result.push_back(w);
    }

    return result;
}

std::vector<std::string> split_space(const std::string& l) {
    return split_char(l, ' ');
}

int to_int(const std::string& s) {
    int i;
    std::from_chars(s.data(), s.data()+s.size(), i, 10);
    return i;
}

uint64_t to_u64(const std::string& s) {
    uint64_t i;
    std::from_chars(s.data(), s.data()+s.size(), i, 10);
    return i;
}

int to_int_binary(const std::string& s) {
    int i;
    std::from_chars(s.data(), s.data()+s.size(), i, 2);
    return i;
}

int sum(const std::vector<int>& v) {
    return std::accumulate(std::begin(v), std::end(v), 0);
}

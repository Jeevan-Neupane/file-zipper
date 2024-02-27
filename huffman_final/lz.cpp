#include "lz.h"

#include <string>
#include <vector>
#include <map>




std::vector<std::pair<int, char>> LZ78::compress(const std::string& input) {
    std::unordered_map<std::string, int> dictionary;
    std::vector<std::pair<int, char>> compressed_data;
    std::string current = "";

    for (char ch : input) {
        current += ch;
        if (dictionary.find(current) == dictionary.end()) {
            if (current.substr(0, current.size() - 1).length() > 0) {
                compressed_data.push_back({ dictionary[current.substr(0, current.size() - 1)], current.back() });
            }
            else {
                compressed_data.push_back({ 0, current.back() });
            }
            dictionary[current] = static_cast<int>(dictionary.size()) + 1;
            current = "";
        }
    }

    if (!current.empty()) {
        compressed_data.push_back({ dictionary[current.substr(0, current.size() - 1)], current.back() });
    }

    return compressed_data;
}

std::string decompress(const std::vector<std::pair<int, char>>& compressedData) {
    std::unordered_map<int, std::string> dictionary;
    std::string result = "";

    for (const auto& entry : compressedData) {
        int code = entry.first;
        char ch = entry.second;

        std::string phrase = dictionary[code];
        std::string new_entry = phrase + ch;
        result += new_entry;
        dictionary[static_cast<int>(dictionary.size())] = new_entry;
    }

    return result;
}

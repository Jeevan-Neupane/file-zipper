#include "lz.h"

#include <string>
#include <vector>
#include <map>



std::vector<std::pair<int, char>> LZ78::compress(const std::string& input) {
    std::vector<std::pair<int, char>> compressedData;

    std::string phrase;
    for (char ch : input) {
        phrase += ch;
        if (dictionary.find(phrase) == dictionary.end()) {
            // If phrase not found in dictionary, add it
            dictionary[phrase] = dictionary.size();
            compressedData.push_back(std::make_pair(dictionary[phrase.substr(0, phrase.length() - 1)], ch));
            phrase.clear();
        }
    }

    // Handle the last phrase
    if (!phrase.empty()) {
        compressedData.push_back(std::make_pair(dictionary[phrase], '\0')); // Use null character to denote end of phrase
    }

    return compressedData;
}

std::string decompress(const std::vector<std::pair<int, char>>& compressedData) {
    std::unordered_map<int, std::string> dictionary;
    dictionary[0] = "";

    std::string decompressedData;
    std::string phrase;

    for (const auto& entry : compressedData) {
        int index = entry.first;
        char ch = entry.second;

        if (index == 0) {
            decompressedData += ch;
            phrase = ch;
        }
        else {
            std::string entryStr = dictionary[index] + ch;
            decompressedData += entryStr;
            phrase = entryStr;
        }

        dictionary[dictionary.size()] = phrase;
    }

    return decompressedData;
}

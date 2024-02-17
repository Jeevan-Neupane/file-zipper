#include "lz.h"

#include <string>
#include <vector>
#include <map>

LZ78::LZ78() {
    // Constructor
    // Initialize dictionary with empty string
    dictionary[""] = 0;
}

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

std::string LZ78::decompress(const std::vector<std::pair<int, char>>& compressedData) {
    std::string decompressedData;

    for (const auto& entry : compressedData) {
        int index = entry.first;
        char ch = entry.second;

        std::string phrase;
        if (index != 0) {
            phrase = dictionary.rbegin()->first; // Get the last entry in the dictionary
        }
        phrase += ch;

        decompressedData += phrase;

        // Add new entry to dictionary
        dictionary[phrase] = dictionary.size();
    }

    return decompressedData;
}

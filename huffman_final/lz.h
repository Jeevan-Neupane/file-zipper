

#include <string>
#include <vector>
#include <map>
#include<unordered_map>

class LZ78 {

public:

    LZ78() {

    }

    // Function to compress input data using LZ78 algorithm
    std::vector<std::pair<int, char>> compress(const std::string& input);

    // Function to decompress LZ78 compressed data
    std::string decompress(const std::vector<std::pair<int, char>>& compressedData);

private:
    // Data structure to represent the dictionary for LZ78 compression
    std::map<std::string, int> dictionary;
};


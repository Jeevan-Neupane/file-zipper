#include <wx/wx.h>
#include <vector>
#include <map>
#include <fstream>

struct Node {
    char data;
    int freq;
    Node* left;
    Node* right;

    Node(char data, int freq);
};

struct Compare {
    bool operator()(Node* a, Node* b) const;
};

Node* buildHuffmanTree(std::map<char, int>& freqMap);
void buildCodeTable(Node* root, std::string code, std::map<char, std::string>& codeTable);
void compressFile(const wxString& inputFilename, const wxString& outputFilename);
void decompressFile(const std::vector<std::pair<int, char>>& compressedData);
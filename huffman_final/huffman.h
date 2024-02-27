#include <wx/wx.h>
#include <vector>
#include <map>
#include <fstream>

struct HuffmanNode {
    char data;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char data, int freq);
};

struct Compare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) const;
};

HuffmanNode* buildHuffmanTree(std::map<char, int>& freqMap);
void buildCodeTable(HuffmanNode* root, std::string code, std::map<char, std::string>& codeTable);
void compressFile(const wxString& inputFilename, const wxString& outputFilename);
void decompressFile(const wxString& inputFilename, const wxString& outputFilename);
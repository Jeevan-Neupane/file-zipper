#include "huffman.h"
#include<queue>
#include "lz.h"
using namespace std;

HuffmanNode::HuffmanNode(char data, int freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}

bool Compare::operator()(HuffmanNode* a, HuffmanNode* b) const {
    return a->freq > b->freq;
}

HuffmanNode* buildHuffmanTree(map<char, int>& freqMap) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> pq;

    for (auto& pair : freqMap) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }
    wxMessageBox("Size of priority queue: " + std::to_string(pq.size()), "Debug Info", wxOK | wxICON_INFORMATION);

    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();
        HuffmanNode* parent = new HuffmanNode('$', left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        pq.push(parent);
    }

    return pq.top();
}

void buildCodeTable(HuffmanNode* root, string code, map<char, string>& codeTable) {
    if (!root)
        return;

    if (root->data != '$') // leaf node
        codeTable[root->data] = code;

    buildCodeTable(root->left, code + "0", codeTable);
    buildCodeTable(root->right, code + "1", codeTable);
}

void saveLZ78CompressedData(const std::vector<std::pair<int, char>>& lz78CompressedData, const wxString& outputFilename) {
    // Open output file for LZ78 compressed data
    ofstream lz78OutputFile(outputFilename.ToStdString() + ".lz78", ios::binary);
    if (!lz78OutputFile.is_open()) {
        wxMessageBox("Failed to open output file for LZ78 compressed data!", "Error", wxOK | wxICON_ERROR);
        return;
    }

    // Write LZ78 compressed data to output file
    for (const auto& entry : lz78CompressedData) {
        lz78OutputFile.write(reinterpret_cast<const char*>(&entry.first), sizeof(int));
        lz78OutputFile.put(entry.second);
    }

    // Close output file for LZ78 compressed data
    lz78OutputFile.close();
}

void writeHuffmanTreeToFile(const map<char, int>& freqMap, ofstream& outputFile) {
    // Write Huffman tree to output file (for decompression)
    // Format: [Character (1 byte)][Frequency (4 bytes)]
    for (auto& pair : freqMap) {
        outputFile.put(pair.first);
        outputFile.write(reinterpret_cast<const char*>(&pair.second), sizeof(int));
    }
    outputFile.put('\0');
    // Mark end of tree
}

void writeEncodedDataToFile(const std::vector<std::pair<int, char>>& lz78CompressedData, const map<char, string>& codeTable, ofstream& outputFile) {
    // Encode LZ78 compressed data using Huffman coding and write to output file
    string buffer;
    for (const auto& entry : lz78CompressedData) {
        char character = entry.second;
        string code = codeTable.at(character); // Retrieve Huffman code for the character
        buffer += code;
        while (buffer.length() >= 8) {
            unsigned char byte = 0;
            for (int i = 0; i < 8; i++) {
                byte <<= 1;
                byte |= buffer[i] - '0';
            }
            outputFile.put(byte);
            buffer.erase(0, 8);
        }
    }

    // Write remaining bits to output file
    if (!buffer.empty()) {
        unsigned char byte = 0;
        for (char bit : buffer) {
            byte <<= 1;
            byte |= bit - '0';
        }
        int padding = 8 - buffer.length();
        byte <<= padding;
        outputFile.put(byte);
    }
}


void compressLZ78WithHuffman(const std::vector<std::pair<int, char>>& lz78CompressedData, const wxString& outputFilename) {
    // Calculate frequency of characters in LZ78 compressed data
    map<char, int> freqMap;
    for (const auto& entry : lz78CompressedData) {
        freqMap[entry.second]++;
    }

    // Build Huffman tree
    HuffmanNode* root = buildHuffmanTree(freqMap);

    // Build code table
    map<char, string> codeTable;
    buildCodeTable(root, "", codeTable);

    // Append ".lh" extension for Huffman compressed data file
    wxString huffmanOutputFilename = outputFilename;

    // Open output file for Huffman compressed data
    ofstream outputFile(huffmanOutputFilename.ToStdString());
    if (!outputFile.is_open()) {
        wxMessageBox("Failed to open output file for Huffman compressed data!", "Error", wxOK | wxICON_ERROR);
        return;
    }

    // Write Huffman tree to output file
    writeHuffmanTreeToFile(freqMap, outputFile);

    // Encode LZ78 compressed data using Huffman coding and write to output file
    writeEncodedDataToFile(lz78CompressedData, codeTable, outputFile);

    // Close output file for Huffman compressed data
    outputFile.close();

    // Free memory
    // TODO: Implement function to delete Huffman tree nodes
}



void compressFile(const wxString& inputFilename, const wxString& outputFilename) {
    // Perform LZ78 compression on the input file
    LZ78 lz78;
    ifstream inputFile(inputFilename.ToStdString(), ios::binary);
    if (!inputFile.is_open()) {
        wxMessageBox("Failed to open input file!", "Error", wxOK | wxICON_ERROR);
        return;
    }

    // Read input file data
    string inputData((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();

    // Compress data using LZ78
    std::vector<std::pair<int, char>> lz78CompressedData = lz78.compress(inputData);




    // Save LZ78 compressed data to a file
    //saveLZ78CompressedData(lz78CompressedData, outputFilename);

    // Perform Huffman compression on the LZ78 compressed data and save to output file
    compressLZ78WithHuffman(lz78CompressedData, outputFilename);
}



























HuffmanNode* readHuffmanTreeFromFile(ifstream& inputFile) {
    map<char, int> freqMap;
    char ch;
    while (inputFile.get(ch) && ch != '\0') {
        int freq;
        inputFile.read(reinterpret_cast<char*>(&freq), sizeof(int));
        freqMap[ch] = freq;
    }

    // Build Huffman tree from frequencies
    return buildHuffmanTree(freqMap);
}


void decodeHuffmanDataToFile(ifstream& inputFile, HuffmanNode* root, const wxString& outputFilename) {
    // Open output file for decompressed data
    ofstream outputFile(outputFilename.ToStdString(), ios::binary);
    if (!outputFile.is_open()) {
        wxMessageBox("Failed to open output file for decompressed data!", "Error", wxOK | wxICON_ERROR);
        return;
    }

    HuffmanNode* currentNode = root;
    char bit;
    while (inputFile.get(bit)) {
        if (bit == '0') {
            currentNode = currentNode->left;
        }
        else {
            currentNode = currentNode->right;
        }

        if (currentNode->left == nullptr && currentNode->right == nullptr) {
            // Leaf node reached, write character to output file
            outputFile.put(currentNode->data);
            currentNode = root; // Reset to root for next character
        }
    }

    // Close output file
    outputFile.close();
}
void decompressFile(const wxString& inputFilename, const wxString& outputFilename) {
    // Open input file for Huffman compressed data
    ifstream inputFile(inputFilename.ToStdString(), ios::binary);
    if (!inputFile.is_open()) {
        wxMessageBox("Failed to open input file for Huffman compressed data!", "Error", wxOK | wxICON_ERROR);
        return;
    }

    // Read Huffman tree from input file
    HuffmanNode* root = readHuffmanTreeFromFile(inputFile);

    // Decode Huffman-encoded data and save to output file
    decodeHuffmanDataToFile(inputFile, root, outputFilename);

    // Close input file
    inputFile.close();

    // Free memory
    // TODO: Implement function to delete Huffman tree nodes
}


#include "huffman.h"
#include<queue>
#include "lz.h"
using namespace std;

Node::Node(char data, int freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}

bool Compare::operator()(Node* a, Node* b) const {
    return a->freq > b->freq;
}

Node* buildHuffmanTree(map<char, int>& freqMap) {
    priority_queue<Node*, vector<Node*>, Compare> pq;

    for (auto& pair : freqMap) {
        pq.push(new Node(pair.first, pair.second));
    }

    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        Node* parent = new Node('$', left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        pq.push(parent);
    }

    return pq.top();
}

void buildCodeTable(Node* root, string code, map<char, string>& codeTable) {
    if (!root)
        return;

    if (root->data != '$') // leaf node
        codeTable[root->data] = code;

    buildCodeTable(root->left, code + "0", codeTable);
    buildCodeTable(root->right, code + "1", codeTable);
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

    // Calculate frequency of characters in LZ78 compressed data
    map<char, int> freqMap;
    for (const auto& entry : lz78CompressedData) {
        freqMap[entry.second]++;
    }

    // Build Huffman tree
    Node* root = buildHuffmanTree(freqMap);

    // Build code table
    map<char, string> codeTable;
    buildCodeTable(root, "", codeTable);

    // Open output file for Huffman compressed data
    ofstream outputFile(outputFilename.ToStdString(), ios::binary);
    if (!outputFile.is_open()) {
        wxMessageBox("Failed to open output file for Huffman compressed data!", "Error", wxOK | wxICON_ERROR);
        return;
    }

    // Write Huffman tree to output file (for decompression)
    // Format: [Character (1 byte)][Frequency (4 bytes)]
    for (auto& pair : freqMap) {
        outputFile.put(pair.first);
        outputFile.write(reinterpret_cast<const char*>(&pair.second), sizeof(int));
    }
    outputFile.put('\0'); // Mark end of tree

    // Encode LZ78 compressed data using Huffman coding and write to output file
    string buffer;
    for (const auto& entry : lz78CompressedData) {
        buffer += codeTable[entry.second];
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

    // Close output file for Huffman compressed data
    outputFile.close();

    // Free memory
    // TODO: Implement function to delete Huffman tree nodes
}

void decompressFile(const wxString& inputFilename, const wxString& outputFilename) {
    // Read compressed data from the input file
    std::ifstream inputFile(inputFilename.ToStdString(), std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open input file for reading!" << std::endl;
        return;
    }
   
    std::vector<std::pair<int, char>> compressedData;
    int index;
    char character;
    while (inputFile.read(reinterpret_cast<char*>(&index), sizeof(int)) && inputFile.get(character)) {
        compressedData.push_back({ index, character });
    }
    inputFile.close();

    // Reconstruct Huffman tree from the frequency map
    std::map<char, int> freqMap;
    for (const auto& entry : compressedData) {
        freqMap[entry.second]++;
    }
    Node* root = buildHuffmanTree(freqMap);

    // Decode LZ78 compressed data using Huffman coding
    std::string decodedData;
    Node* current = root;
    for (const auto& entry : compressedData) {
        int index = entry.first;
        char character = entry.second;

        // Perform LZ78 decoding
        std::string phrase;
        if (index != 0) {
            phrase = decodedData.substr(index - 1, 1);
        }

        // Append decoded character to the result
        decodedData += phrase + character;

        // Traverse Huffman tree to decode LZ78 output
        current = root;
        for (char bit : phrase) {
            if (bit == '0') {
                current = current->left;
            }
            else {
                current = current->right;
            }
        }
        if (!current->left && !current->right) { // Leaf node
            decodedData += current->data;
            current = root; // Reset current node
        }
    }

    // Write decompressed data to file
    std::ofstream outputFile(outputFilename.ToStdString(), std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open output file for writing!" << std::endl;
        return;
    }
    outputFile << decodedData;
    outputFile.close();

    // Free memory
    // TODO: Implement function to delete Huffman tree nodes
}



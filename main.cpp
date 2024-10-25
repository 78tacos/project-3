#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <memory>
#include <unordered_map>


int HashCode(const std::string& str) {
    int h = 0;
    for (char ch : str) {
        h = h * 31 + static_cast<int>(ch);
    }
    return h;
}

// Node to store each word, occurrences, and steps
struct Node {
    std::string word;
    int occurrences;
    int totalSteps;

    Node(const std::string& w, int steps) : word(w), occurrences(1), totalSteps(steps) {}
};

// Linked list to handle collisions at each hash table slot
class LinkedList {
public:
    std::vector<std::unique_ptr<Node>> nodes;

    Node* findOrInsert(const std::string& word, int steps) {
        for (auto& node : nodes) {
            if (node->word == word) {
                node->occurrences++;
                node->totalSteps += steps;
                return node.get();
            }
            steps++;
        }
        nodes.emplace_back(std::make_unique<Node>(word, steps));
        return nodes.back().get();
    }

    // Collect words in the current linked list for CSV output
    void collectWords(std::unordered_map<std::string, std::pair<int, int>>& wordStats) {
        for (const auto& node : nodes) {
            wordStats[node->word] = {node->occurrences, node->totalSteps};
        }
    }
};

// HashTable class to store words and track steps and occurrences
class HashTable {
private:
    std::vector<LinkedList> table;
    int tableSize;

public:
    HashTable(int size) : table(size), tableSize(size) {}

    // Insert or find a word in the hash table
    void insert(const std::string& word) {
        int index = std::abs(HashCode(word) % tableSize);
        int steps = 1; // Initial step count for position in linked list
        table[index].findOrInsert(word, steps);
    }

    // Collect all words in the hash table for output
    void collectAllWords(std::unordered_map<std::string, std::pair<int, int>>& wordStats) {
        for (auto& list : table) {
            list.collectWords(wordStats);
        }
    }
};

// Main function to process the text file and hash table, and export to CSV
class WordCounter {
private:
    std::vector<int> tableSizes = {500, 1000, 2000, 5000, 10000};
    const std::string filename = "LittleWomen.txt"; // Hardcoded filename

public:
    void processFile() {
        for (int size : tableSizes) {
            HashTable hashTable(size);
            std::ifstream file(filename);

            // Check if the file opened successfully
            if (!file.is_open()) {
                throw std::runtime_error("Error: Could not open the file " + filename);
            }

            std::string word;
            // Insert words from file into the hash table
            while (file >> word) {
                hashTable.insert(word);
            }
            file.close();

            // Collect words and their statistics
            std::unordered_map<std::string, std::pair<int, int>> wordStats;
            hashTable.collectAllWords(wordStats);

            // Output results to CSV
            writeCSV(size, wordStats);
            std::cout << "Hash table of size " << size << " processed and saved to CSV.\n";
        }
    }

    void writeCSV(int tableSize, const std::unordered_map<std::string, std::pair<int, int>>& wordStats) {
        std::string csvFilename = "WordStats_TableSize_" + std::to_string(tableSize) + ".csv";
        std::ofstream outFile(csvFilename);
        outFile << "Word,Occurrences,Steps\n";
        for (const auto& [word, stats] : wordStats) {
            outFile << word << "," << stats.first << "," << stats.second << "\n";
        }
        outFile.close();
    }
};

int main() {
    WordCounter counter;
    try {
        counter.processFile();
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << "\n";
        return 1; // Exit with error code
    }
    return 0;
}

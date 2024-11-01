#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <memory>
#include <unordered_map>
#include <algorithm>

using namespace std;

int HashCode(const string &str) {
    int h = 0;
    for (char ch : str) {
        h = h * 31 + ch;
    }
    return h;
}

struct Node {
    string word;
    int occurrences;
    int totalSteps;

    Node(const string& w, int steps) : word(w), occurrences(1), totalSteps(steps) {}
};

class LinkedList {
public:
    vector<unique_ptr<Node>> nodes;

    Node* findOrInsert(const string& word, int& steps) {
        for (auto& node : nodes) {
            steps++;
            if (node->word == word) {
                node->occurrences++;
                node->totalSteps += steps;
                return node.get();
            }
        }
        // Insert new word at the end of the linked list
        nodes.emplace_back(make_unique<Node>(word, steps + 1));
        return nodes.back().get();
    }

    // Collect words in the linked list for CSV output, sorted by occurrences
    void collectWords(vector<Node>& wordStats) {
        for (const auto& node : nodes) {
            wordStats.push_back(*node);
        }
        // Sort by occurrences in ascending order
        sort(wordStats.begin(), wordStats.end(), [](const Node &a, const Node &b) {
            return a.occurrences < b.occurrences;
        });
    }
};

// HashTable class to store words and track steps and occurrences
class HashTable {
private:
    vector<LinkedList> table;
    int tableSize;

public:
    HashTable(int size) : table(size), tableSize(size) {}

    // Insert or find a word in the hash table
    void insert(const string& word) {
        int index = abs(HashCode(word) % tableSize);
        int steps = 0; // Step counter for position in linked list
        table[index].findOrInsert(word, steps);
    }

    // Collect all words in the hash table for output, ordered by slot and occurrences
    void collectAllWords(vector<vector<Node>>& wordStatsBySlot) {
        for (int i = 0; i < tableSize; ++i) {
            vector<Node> wordStats;
            table[i].collectWords(wordStats);
            wordStatsBySlot.push_back(move(wordStats));  // Save sorted list for each slot
        }
    }
};

// Main function to process the text file and hash table, and export to CSV
class WordCounter {
private:
    vector<int> tableSizes = {500, 1000, 2000, 5000, 10000};
    const string filename = "LittleWomen.txt"; // Hardcoded filename, make sure it's in the same directory

public:
    void processFile() {
        for (int size : tableSizes) {
            HashTable hashTable(size);
            ifstream file(filename);

            // Check if the file opened successfully
            if (!file.is_open()) {
                throw runtime_error("Error: Could not open the file " + filename);
            }

            string word;
            // Insert words from file into the hash table
            while (file >> word) {
                hashTable.insert(word);
            }
            file.close();

            // Collect words and their statistics, ordered by hash table slot
            vector<vector<Node>> wordStatsBySlot;
            hashTable.collectAllWords(wordStatsBySlot);

            // Output results to CSV
            writeCSV(size, wordStatsBySlot);
            cout << "Hash table of size " << size << " processed and saved to CSV.\n";
        }
    }

    void writeCSV(int tableSize, const vector<vector<Node>>& wordStatsBySlot) {
        string csvFilename = "WordStats_TableSize_" + to_string(tableSize) + ".csv";
        ofstream outFile(csvFilename);
        outFile << "Word,Occurrences,Steps\n";

        // Write each slot's words, sorted by occurrences
        for (const auto& slot : wordStatsBySlot) {
            for (const auto& node : slot) {
                outFile << node.word << "," << node.occurrences << "," << node.totalSteps << "\n";
            }
        }
        
        outFile.close();
    }
};

int main() {
    WordCounter counter;
    try {
        counter.processFile();
    } catch (const runtime_error& e) { // In case file is not in the same directory
        cerr << e.what() << "\n";
        return 1;
    }
    return 0;
}

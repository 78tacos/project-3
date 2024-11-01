#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <memory>
#include <unordered_map>

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
        // inserting new word at the end of the linked list
        nodes.emplace_back(make_unique<Node>(word, steps + 1));
        return nodes.back().get();
    }

    // collect words in the linked list for CSV output in insertion order
    vector<Node> collectWords() const {
        vector<Node> collectedWords;
        for (const auto& node : nodes) {
            collectedWords.push_back(*node);  // copying each node to collectedWords
        }
        return collectedWords;
    }
};

class HashTable {
private:
    vector<LinkedList> table;
    int tableSize;

public:
    HashTable(int size) : table(size), tableSize(size) {}

    // insert or find a word in the hash table
    void insert(const string& word) {
        int index = abs(HashCode(word) % tableSize);
        int steps = 0;
        table[index].findOrInsert(word, steps);
    }

    // collect all words in the hash table for output, preserving insertion order by slot
    vector<vector<Node>> collectAllWords() const {
        vector<vector<Node>> wordStatsBySlot;
        for (const auto& list : table) {
            wordStatsBySlot.push_back(list.collectWords()); 
        }
        return wordStatsBySlot;
    }
};

//process the text file and hash table, and export to CSV
class WordCounter {
private:
    vector<int> tableSizes = {500, 1000, 2000, 5000, 10000};
    const string filename = "LittleWomen.txt"; // make sure it's in the same directory

public:
    void processFile() {
        for (int size : tableSizes) {
            HashTable hashTable(size);
            ifstream file(filename);

            // Because I sometimes mess this up
            if (!file.is_open()) {
                throw runtime_error("Error: Could not open the file " + filename);
            }

            string word;
            // Putting into the hash table
            while (file >> word) {
                hashTable.insert(word);
            }
            file.close();

            vector<vector<Node>> wordStatsBySlot = hashTable.collectAllWords();

            writeCSV(size, wordStatsBySlot);
            cout << "Hash table of size " << size << " processed and saved to CSV.\n";
        }
    }

    void writeCSV(int tableSize, const vector<vector<Node>>& wordStatsBySlot) {
        string csvFilename = "WordStats_TableSize_" + to_string(tableSize) + ".csv";
        ofstream outFile(csvFilename);
        outFile << "Word,Occurrences,Steps\n";

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
    } catch (const runtime_error& e) { // In case file is not in the same directory lol
        cerr << e.what() << "\n";
        return 1;
    }
    return 0;
}

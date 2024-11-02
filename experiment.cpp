#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <memory>
#include <unordered_map>
#include <iomanip> // for std::setw in visualization

using namespace std;

int HashCodePrimary(const string &str) {
    int h = 0;
    for (char ch : str) {
        h = h * 31 + ch;
    }
    return h;
}

int HashCodeSecondary(const string &str) {
    int h = 5381;
    for (char ch : str) {
        h = ((h << 5) + h) + ch; // hash * 33 + ch
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
        nodes.emplace_back(make_unique<Node>(word, steps + 1));
        return nodes.back().get();
    }

    vector<Node> collectWords() const {
        vector<Node> collectedWords;
        for (const auto& node : nodes) {
            collectedWords.push_back(*node);
        }
        return collectedWords;
    }
};

class HashTable {
private:
    vector<LinkedList> table;
    vector<int> collisionCounts;
    int tableSize;
    int collisionThreshold;
    bool useSecondaryHash;
    
public:
    HashTable(int size, int threshold)
        : table(size), collisionCounts(size, 0), tableSize(size), collisionThreshold(threshold), useSecondaryHash(false) {}

    int getHashIndex(const string& word) {
        int hashValue = useSecondaryHash ? HashCodeSecondary(word) : HashCodePrimary(word);
        return abs(hashValue) % tableSize;
    }

    void insert(const string& word) {
        int index = getHashIndex(word);
        int steps = 0;
        
        if (!table[index].nodes.empty()) {
            collisionCounts[index]++;
            if (collisionCounts[index] > collisionThreshold) {
                cout << "Switching to secondary hash function due to high collisions at bucket " << index << "\n";
                rehash();
                index = getHashIndex(word);  // recalculate index after rehash
            }
        }
        
        table[index].findOrInsert(word, steps);
    }

    void rehash() {
        useSecondaryHash = !useSecondaryHash;
        vector<LinkedList> newTable(tableSize);
        vector<int> newCollisionCounts(tableSize, 0);

        for (auto& list : table) {
            for (const auto& node : list.nodes) {
                int index = getHashIndex(node->word);
                if (!newTable[index].nodes.empty()) {
                    newCollisionCounts[index]++;
                }
                int steps = 0;
                newTable[index].findOrInsert(node->word, steps);
            }
        }
        
        table = move(newTable);
        collisionCounts = move(newCollisionCounts);
    }

    void visualizeCollisions() const {
        cout << "\nCollision Distribution Across Buckets:\n";
        for (int i = 0; i < tableSize; ++i) {
            cout << "Bucket " << setw(4) << i << ": " << setw(2) << collisionCounts[i] << " collisions\n";
        }
    }

    vector<vector<Node>> collectAllWords() const {
        vector<vector<Node>> wordStatsBySlot;
        for (const auto& list : table) {
            wordStatsBySlot.push_back(list.collectWords());
        }
        return wordStatsBySlot;
    }
};

class WordCounter {
private:
    vector<int> tableSizes = {500, 1000, 2000, 5000, 10000};
    const string filename = "LittleWomen.txt";

public:
    void processFile() {
        for (int size : tableSizes) {
            HashTable hashTable(size, 5); // Set collision threshold to 5
            ifstream file(filename);

            if (!file.is_open()) {
                throw runtime_error("Error: Could not open the file " + filename);
            }

            string word;
            while (file >> word) {
                hashTable.insert(word);
            }
            file.close();

            vector<vector<Node>> wordStatsBySlot = hashTable.collectAllWords();

            writeCSV(size, wordStatsBySlot);
            cout << "Hash table of size " << size << " processed and saved to CSV.\n";
            
            // Display the collision visualization
            hashTable.visualizeCollisions();
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
    } catch (const runtime_error& e) {
        cerr << e.what() << "\n";
        return 1;
    }
    return 0;
}

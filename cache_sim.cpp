#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;


// This is the cache class that stimulates cache behavior.
// numSets = # of sets in cache , associativity = # of blocks per set
// tags = 2D array for tag value , valid = 2D array for valid bits
// lastUsed = 2D array for storing LRU last time used , timeCounter = global tracker
class Cache {
private:
    int numSets;
    int associativity;

    int **tags;
    bool **valid;
    int **lastUsed;
    int timeCounter;

public:
    
    // Constructor to build the cache , # of sets = total entries / associativity. Initalize time counter.
    Cache(int numEntries, int assoc) {
        associativity = assoc;
        numSets = numEntries / assoc;
        timeCounter = 0;

        // Dynamicaly allocate memory for arrays.
        tags = new int*[numSets];
        valid = new bool*[numSets];
        lastUsed = new int*[numSets];

        // Creating each set.
        for (int i = 0; i < numSets; i++) {
            tags[i] = new int[associativity];
            valid[i] = new bool[associativity];
            lastUsed[i] = new int[associativity];

            // Initalizing each block in the set.
            // The cache starts empty, and no tags yet and the
            // last used hasnt been used.
            for (int j = 0; j < associativity; j++) {
                valid[i][j] = false;
                tags[i][j] = 0;
                lastUsed[i][j] = -1;
            }
        }
    }

    // The index the address maps to
    int getIndex(int addr) {
        return addr % numSets;
    }

    // The tag which is the remaining part of the address
    int getTag(int addr) {
        return addr / numSets;
    }

    // Stimulate accessing the cache
    bool access(int addr) {
        int index = getIndex(addr);
        int tag = getTag(addr);

        // Check for HIT & update LRU time
        for (int i = 0; i < associativity; i++) {
            if (valid[index][i] && tags[index][i] == tag) {
                lastUsed[index][i] = timeCounter++;
                return true; // HIT
            }
        }

        // Check for MISS empty spot & place a new block
        for (int i = 0; i < associativity; i++) {
            if (!valid[index][i]) {
                valid[index][i] = true;
                tags[index][i] = tag;
                lastUsed[index][i] = timeCounter++;
                return false; // MISS
            }
        }

        // Cache is full so replace LRU & find last used block
        int lru = 0;
        for (int i = 1; i < associativity; i++) {
            if (lastUsed[index][i] < lastUsed[index][lru]) {
                lru = i;
            }
        }

        // Replace that block
        tags[index][lru] = tag;
        lastUsed[index][lru] = timeCounter++;

        return false; // MISS
    }
};

int main(int argc, char* argv[]) {

    // Check correct number of arguments
    if (argc != 4) {
        cout << "Usage: ./cache_sim num_entries associativity input_file" << endl;
        return 0;
    }
    
    // Convert arguments from string to int
    int numEntries = atoi(argv[1]);
    int assoc = atoi(argv[2]);
    
    // Get the input file name
    string filename = argv[3];

    // Open the input file
    ifstream input(filename);
    if (!input.is_open()) {
        cout << "Error opening input file" << endl;
        return 0;
    }

    // Create the output file
    ofstream output("cache_sim_output");

    // Create cache object
    Cache cache(numEntries, assoc);

    int addr;

    // Read each memory address from the file & check if HIT or MISS
    while (input >> addr) {
        if (cache.access(addr)) {
            output << addr << " : HIT" << endl;
        } else {
            output << addr << " : MISS" << endl;
        }
    }

    // Close the files
    input.close();
    output.close();

    return 0;
}

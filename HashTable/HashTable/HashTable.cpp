//============================================================================
// Name        : HashTable.cpp
// Author      : Justin Roberts
// Version     : 2.0
// Copyright   : Copyright © 2023 SNHU COCE
// Description : Lab 4-2 Hash Table
// 
// CHANGE SUMMARY:
// - Implemented hash table with chaining to store bids from CSV files
// - Added proper initialization of hash table buckets
// - Implemented insertion, search, removal, and display logic
// - Fixed forward declaration ordering issue for displayBid()
// - Added destructor logic to prevent memory leaks
//
// All changes are documented inline to explain purpose and reasoning.
//============================================================================

#include <algorithm>
#include <climits>
#include <iostream>
#include <string> // atoi
#include <time.h>

#include "CSVparser.hpp"

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

const unsigned int DEFAULT_SIZE = 179;

// forward declarations
double strToDouble(string str, char ch);

// define a structure to hold bid information
struct Bid {
    string bidId; // unique identifier
    string title;
    string fund;
    double amount;
    Bid() {
        amount = 0.0;
    }
};

// CHANGE:
// Added forward declaration for displayBid here
// The compiler needed know what a Bid is before it could be used as a parameter.
// This fixed the compilation errors related to undeclared identifiers I was receiving.

void displayBid(Bid bid); // Forward declaration for AFTER Bid is defined


//============================================================================
// Hash Table class definition
//============================================================================

/**
 * Define a class containing data members and methods to
 * implement a hash table with chaining.
 */
class HashTable {

private:
    // Define structures to hold bids
    struct Node {
        Bid bid;
        unsigned int key;
        Node *next;

        // default constructor
        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        // initialize with a bid
        Node(Bid aBid) : Node() {
            bid = aBid;
        }

        // initialize with a bid and a key
        Node(Bid aBid, unsigned int aKey) : Node(aBid) {
            key = aKey;
        }
    };

    vector<Node> nodes;

    unsigned int tableSize = DEFAULT_SIZE;

    unsigned int hash(int key);

public:
    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Bid bid);
    void PrintAll();
    void Remove(string bidId);
    Bid Search(string bidId);
    size_t Size();
};

/**
 * Default constructor
 */
HashTable::HashTable() {
    // FIXME (1) complete: Initialize the structures used to hold bids
    // CHANGE:
    // Initialize hash table with DEFAULT_SIZE buckets.
    // REASON:
    // Without resizing, accessing nodes[key] would be undefined behavior.
 
    nodes.resize(tableSize);
}

/**
 * Constructor for specifying size of the table
 * Use to improve efficiency of hashing algorithm
 * by reducing collisions without wasting memory.
 */
HashTable::HashTable(unsigned int size) {
    // CHANGE:
    // Clear and resize ensures a clean table state.
    // invoke local tableSize to size with this->
    // resize nodes size
    this->tableSize = size;
    nodes.clear();
    nodes.resize(this->tableSize);
}


/**
 * Destructor
 */
HashTable::~HashTable() {
    // FIXME (2) complete: Implement logic to free storage when class is destroyed
    // CHANGE:
    // Explicitly free memory used by chained nodes.
    // REASON:
    // Nodes added via `new` during collisions must be deleted
    // to prevent memory leaks.
    for (unsigned int i = 0; i < nodes.size(); ++i) {
        Node* current = nodes[i].next;
        while (current != nullptr) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
        nodes[i].next = nullptr;
        nodes[i].key = UINT_MAX;
        nodes[i].bid = Bid();
    }
    nodes.clear();
}

/**
 * Calculate the hash value of a given key.
 * Note that key is specifically defined as
 * unsigned int to prevent undefined results
 * of a negative list index.
 *
 * @param key The key to hash
 * @return The calculated hash
 */
unsigned int HashTable::hash(int key) {
    // FIXME (3) complete: Implement logic to calculate a hash value
    // CHANGE:
    // Simple modulo-based hash function.
    // REASON:
    // Ensures key maps within bounds of table size.
    
    // return key tableSize
    return key % tableSize;
}

/**
 * Insert a bid
 *
 * @param bid The bid to insert
 */
void HashTable::Insert(Bid bid) {
    // FIXME (4) complete: Implement logic to insert a bid
    
    unsigned int key = hash(atoi(bid.bidId.c_str()));

    // Grab the bucket head (stored directly in the vector).
    Node* bucket = &nodes[key];

    // CHANGE:
    // If bucket is unused, store bid directly.
    
    // Case 1: empty bucket
    if (bucket->key == UINT_MAX) {
        bucket->key = key;
        bucket->bid = bid;
        bucket->next = nullptr;
        return;
    }

    // CHANGE:
    // Collision detected — apply chaining.
    // REASON:
    // Multiple bids may hash to the same index.
    
    // Case 2: collision - append to the end of the chain
    Node* current = bucket;
    while (current->next != nullptr) {
        current = current->next;
    }
    current->next = new Node(bid, key);
}

/**
 * Print all bids
 */
void HashTable::PrintAll() {
    // FIXME (5) complete: Implement logic to print all bids
    // CHANGE:
    // Iterates through each bucket and prints chained bids.
    // REASON:
    // Required to display all hash table contents including collisions.

    for (unsigned int i = 0; i < nodes.size(); ++i) {
        // Skip unused buckets.
        if (nodes[i].key == UINT_MAX) {
            continue;
        }

        // Print the bucket head.
        cout << "Key " << i << ": ";
        displayBid(nodes[i].bid);

        // Print any chained nodes.
        Node* current = nodes[i].next;
        while (current != nullptr) {
            cout << "Key " << i << ": ";
            displayBid(current->bid);
            current = current->next;
        }
    }

}

/**
 * Remove a bid
 *
 * @param bidId The bid id to search for
 */
void HashTable::Remove(string bidId) {
    // FIXME (6) complete: Implement logic to remove a bid
    
    unsigned int key = hash(atoi(bidId.c_str()));
    Node* bucket = &nodes[key];

    // Nothing in this bucket
    if (bucket->key == UINT_MAX) {
        return;
    }

    // CHANGE:
    // Handle removal from bucket head.
    // Remove from head of chain (bucket itself)
    if (bucket->bid.bidId == bidId) {
        if (bucket->next == nullptr) {
            // Single item in bucket
            bucket->key = UINT_MAX;
            bucket->bid = Bid();
            return;
        }

        // Promote the next node into the bucket head
        Node* temp = bucket->next;
        bucket->bid = temp->bid;
        bucket->next = temp->next;
        delete temp;
        return;
    }

    // CHANGE:
    // Remove bid from chained nodes.
    Node* prev = bucket;
    Node* current = bucket->next;
    while (current != nullptr) {
        if (current->bid.bidId == bidId) {
            prev->next = current->next;
            delete current;
            return;
        }
        prev = current;
        current = current->next;
    }
}

/**
 * Search for the specified bidId
 *
 * @param bidId The bid id to search for
 */
Bid HashTable::Search(string bidId) {
    Bid bid;

    // FIXME (7) complete: Implement logic to search for and return a bid

    unsigned int key = hash(atoi(bidId.c_str()));
    Node* bucket = &nodes[key];

    // Nothing in this bucket
    if (bucket->key == UINT_MAX) {
        return bid;
    }

    // Check bucket head
    if (bucket->bid.bidId == bidId) {
        return bucket->bid;
    }

    // Walk the chain
    Node* current = bucket->next;
    while (current != nullptr) {
        if (current->bid.bidId == bidId) {
            return current->bid;
        }
        current = current->next;
    }


    return bid;
}

//============================================================================
// Static methods used for testing
//============================================================================

/**
 * Display the bid information to the console (std::out)
 *
 * @param bid struct containing the bid info
 */
void displayBid(Bid bid) {
    cout << bid.bidId << ": " << bid.title << " | " << bid.amount << " | "
            << bid.fund << endl;
    return;
}

/**
 * Load a CSV file containing bids into a container
 *
 * @param csvPath the path to the CSV file to load
 * @return a container holding all the bids read
 */
void loadBids(string csvPath, HashTable* hashTable) {
    cout << "Loading CSV file " << csvPath << endl;

    // initialize the CSV Parser using the given path
    csv::Parser file = csv::Parser(csvPath);

    // read and display header row - optional
    vector<string> header = file.getHeader();
    for (auto const& c : header) {
        cout << c << " | ";
    }
    cout << "" << endl;

    try {
        // loop to read rows of a CSV file
        for (unsigned int i = 0; i < file.rowCount(); i++) {

            // Create a data structure and add to the collection of bids
            Bid bid;
            bid.bidId = file[i][1];
            bid.title = file[i][0];
            bid.fund = file[i][8];
            bid.amount = strToDouble(file[i][4], '$');

            //cout << "Item: " << bid.title << ", Fund: " << bid.fund << ", Amount: " << bid.amount << endl;

            // push this bid to the end
            hashTable->Insert(bid);
        }
    } catch (csv::Error &e) {
        std::cerr << e.what() << std::endl;
    }
}

/**
 * Simple C function to convert a string to a double
 * after stripping out unwanted char
 *
 * credit: http://stackoverflow.com/a/24875936
 *
 * @param ch The character to strip out
 */
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    return atof(str.c_str());
}

/**
 * The one and only main() method
 */
int main(int argc, char* argv[]) {

    // process command line arguments
    string csvPath, bidKey;
    switch (argc) {
    case 2:
        csvPath = argv[1];
        bidKey = "98223";
        break;
    case 3:
        csvPath = argv[1];
        bidKey = argv[2];
        break;
    default:
        csvPath = "eBid_Monthly_Sales.csv";
        bidKey = "98223";
    }

    // Define a timer variable
    clock_t ticks;

    // Define a hash table to hold all the bids
    HashTable* bidTable;

    Bid bid;
    bidTable = new HashTable();
    
    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Bids" << endl;
        cout << "  2. Display All Bids" << endl;
        cout << "  3. Find Bid" << endl;
        cout << "  4. Remove Bid" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

        case 1:
            
            // Initialize a timer variable before loading bids
            ticks = clock();

            // Complete the method call to load the bids
            loadBids(csvPath, bidTable);

            // Calculate elapsed time and display result
            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;

        case 2:
            bidTable->PrintAll();
            break;

        case 3:
            ticks = clock();

            bid = bidTable->Search(bidKey);

            ticks = clock() - ticks; // current clock ticks minus starting clock ticks

            if (!bid.bidId.empty()) {
                displayBid(bid);
            } else {
                cout << "Bid Id " << bidKey << " not found." << endl;
            }

            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;

        case 4:
            bidTable->Remove(bidKey);
            break;
        }
    }

    cout << "Good bye." << endl;

    return 0;
}

//============================================================================
// Name        : LinkedList.cpp
// Author      : Justin Roberts
// Version     : 2.0
// Copyright   : Copyright © 2023 SNHU COCE
// Description : Lab 3-2 Lists and Searching
//============================================================================
// ASSIGNMENT NOTES:
// The assignment required implementing a singly linked list by completing the public API:
//   - LinkedList() constructor initialization
//   - Append 
//   - Prepend
//   - PrintList
//   - Remove
//   - Search
//
// Additional fixes I made were to support compilation and runtime stability:
//   - A forward declaration for displayBid(Bid) was added after the Bid struct.
//=============================================================================

#include <algorithm>
#include <iostream>
#include <time.h>

#include "CSVparser.hpp"

using namespace std;


//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

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

// ---------------------------------------------------------------------------
// CHANGE (compile fix):
// The starter file defines displayBid() later in the file, but LinkedList::PrintList()
// calls displayBid(). In C++, I was receiving an error that it must be declared before use.
// I added a forward declaration AFTER Bid is defined (so the compiler knows what "Bid" is)
// but BEFORE PrintList() is compiled, which cleared the error.
// ---------------------------------------------------------------------------
void displayBid(Bid bid);

//============================================================================
// Linked-List class definition
//============================================================================

/**
 * Define a class containing data members and methods to
 * implement a linked-list.
 */
class LinkedList {

private:
    //Internal structure for list entries, housekeeping variables
    struct Node {
        Bid bid;
        Node *next;

        // default constructor
        Node() {
            next = nullptr;
        }

        // initialize with a bid
        Node(Bid aBid) {
            bid = aBid;
            next = nullptr;
        }
    };

    Node* head;
    Node* tail;
    int size = 0;

public:
    LinkedList();
    virtual ~LinkedList();
    void Append(Bid bid);
    void Prepend(Bid bid);
    void PrintList();
    void Remove(string bidId);
    Bid Search(string bidId);
    int Size();
};

/**
 * Default constructor
 */
LinkedList::LinkedList() {
    // FIXME (1) complete (Task 1 - Initialize housekeeping variables):
    // Setting pointers to nullptr defines an empty list state.
    head = nullptr;
    tail = nullptr;
    size = 0;
}

/**
 * Destructor
 */
LinkedList::~LinkedList() {
    // start at the head
    Node* current = head;
    Node* temp;

    // loop over each node, detach from list then delete
    while (current != nullptr) {
        temp = current; // hang on to current node
        current = current->next; // make current the next node
        delete temp; // delete the orphan node
    }
}

/**
 * Append a new bid to the end of the list
 */
void LinkedList::Append(Bid bid) {
   // FIXME (2) complete (Task 2 - Implement append logic) :
   // Requirement: add a node to the END of the singly linked list.
    Node* newNode = new Node(bid);

    // Edge case: empty list
    // If head is nullptr, the list has no nodes.
    // In that case, both head and tail should point to the new node.
    if (head == nullptr) {
        head = newNode;
        tail = newNode;
        size = 1;   // first element
        return;
    }

    // Normal case: list already has nodes
    // tail points to the last node. Link that last node to the new node,
    // then move tail forward to become the new node.
    tail->next = newNode;
    tail = newNode;
    size++;
}

/**
 * Prepend a new bid to the start of the list
 */
void LinkedList::Prepend(Bid bid) {
    // FIXME (3) complete (Task 3 - Implement prepend logic):
    // Requirement: add a node to the FRONT of the singly linked list.
    Node* newNode = new Node(bid);

    // Edge case: empty list
    // If the list is empty, new node becomes both head and tail.
    if (head == nullptr) {
        head = newNode;
        tail = newNode;
        size = 1;
        return;
    }

    // Normal case:
    // New node points to the current head, then head is updated to the new node.
    newNode->next = head;
    head = newNode;
    size++;

}

/**
 * Simple output of all bids in the list
 */
void LinkedList::PrintList() {
    // FIXME (4) complete (Task 4 - Implement print logic):
    // Requirement: print all bids by traversing from head to tail.
    Node* current = head;

    // Traverse nodes until reaching nullptr (end of list).
    // Each node's bid is printed using the existing helper displayBid().
    while (current != nullptr) {
        displayBid(current->bid);
        current = current->next;
    }
}

/**
 * Remove a specified bid
 *
 * @param bidId The bid id to remove from the list
 */
void LinkedList::Remove(string bidId) {
   // FIXME (5) complete (Task 5 - Implement remove logic):
   // Requirement: remove the node with a matching bidId from the list.
   //
   // Key considerations taken while building:
   // - Handle empty list safely
   // - Handle removing head (first node)
   // - Handle removing tail (last node) so tail stays correct
   // - Correctly unlink node and free memory (delete)
   // - Decrement size only when a node is actually removed

   // Empty list: nothing to remove
    if (head == nullptr) {
        return; // empty list
    }

    // Case 1: removing the head node
    if (head->bid.bidId == bidId) {
        Node* temp = head;
        head = head->next;

        // If the only node is removed, tail must also be updated.
        // After removing the only node, head becomes nullptr, so tail should too.
        if (temp == tail) {
            tail = head; // will be nullptr if list is now empty
        }

        delete temp;
        size--;
        return;
    }

    // Case 2: removing a node that is not the head
    // Walk the list looking at current->next.
    Node* current = head;

    while (current->next != nullptr) {
        if (current->next->bid.bidId == bidId) {
            Node* temp = current->next;          // node to delete
            current->next = current->next->next; // bypass it

            // if tail is removed, update tail
            if (temp == tail) {
                tail = current;
            }

            delete temp;
            size--;
            return;
        }

        current = current->next;
    }
    
    // If this point is reached, the bidId was not found.
    // No action is needed; list remains unchanged.
}

/**
 * Search for the specified bidId
 *
 * @param bidId The bid id to search for
 */
Bid LinkedList::Search(string bidId) {
    // FIXME (6) complete (Task 6 - Implement search logic):
    // Requirement: find a bid by bidId and return it.
    // If not found, return an "empty" Bid (default constructed).
    Node* current = head;

    // Traverse the list from head to end.
    while (current != nullptr) {
        if (current->bid.bidId == bidId) {
            return current->bid;    // found match
        }
        current = current->next;
    }

    // Not found: return empty bid
    // Caller checks bid.bidId.empty() to determine not found.
    return Bid();
}

/**
 * Returns the current size (number of elements) in the list
 */
int LinkedList::Size() {
    return size;
}

//============================================================================
// Static methods used for testing
//============================================================================

/**
 * Display the bid information
 *
 * @param bid struct containing the bid info
 */
void displayBid(Bid bid) {
    cout << bid.bidId << ": " << bid.title << " | " << bid.amount
         << " | " << bid.fund << endl;
    return;
}

/**
 * Prompt user for bid information
 *
 * @return Bid struct containing the bid info
 */
Bid getBid() {
    Bid bid;

    cout << "Enter Id: ";
    cin.ignore();
    getline(cin, bid.bidId);

    cout << "Enter title: ";
    getline(cin, bid.title);

    cout << "Enter fund: ";
    cin >> bid.fund;

    cout << "Enter amount: ";
    cin.ignore();
    string strAmount;
    getline(cin, strAmount);
    bid.amount = strToDouble(strAmount, '$');

    return bid;
}

/**
 * Load a CSV file containing bids into a LinkedList
 *
 * @return a LinkedList containing all the bids read
 */


void loadBids(string csvPath, LinkedList *list) {
    cout << "Loading CSV file " << csvPath << endl;

    // initialize the CSV Parser
    csv::Parser file = csv::Parser(csvPath);

    try {
        // loop to read rows of a CSV file
        for (int i = 0; i < file.rowCount(); i++) {

            // initialize a bid using data from current row (i)
            Bid bid;
            bid.bidId = file[i][1];
            bid.title = file[i][0];
            bid.fund = file[i][8];
            bid.amount = strToDouble(file[i][4], '$');

            //cout << bid.bidId << ": " << bid.title << " | " << bid.fund << " | " << bid.amount << endl;

            // add this bid to the end
            list->Append(bid);
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
 *
 * @param arg[1] path to CSV file to load from (optional)
 * @param arg[2] the bid Id to use when searching the list (optional)
 */
int main(int argc, char* argv[]) {

    // process command line arguments
    string csvPath, bidKey;
    switch (argc) {
    case 2:
        csvPath = argv[1];
        bidKey = "98109";
        break;
    case 3:
        csvPath = argv[1];
        bidKey = argv[2];
        break;
    default:
        csvPath = "eBid_Monthly_Sales.csv";
        bidKey = "98109";
    }

    clock_t ticks;

    LinkedList bidList;

    Bid bid;

    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Enter a Bid" << endl;
        cout << "  2. Load Bids" << endl;
        cout << "  3. Display All Bids" << endl;
        cout << "  4. Find Bid" << endl;
        cout << "  5. Remove Bid" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            bid = getBid();
            bidList.Append(bid);
            displayBid(bid);

            break;

        case 2:
            ticks = clock();

            loadBids(csvPath, &bidList);

            cout << bidList.Size() << " bids read" << endl;

            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " milliseconds" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

            break;

        case 3:
            bidList.PrintList();

            break;

        case 4:
            ticks = clock();

            bid = bidList.Search(bidKey);

            ticks = clock() - ticks; // current clock ticks minus starting clock ticks

            if (!bid.bidId.empty()) {
                displayBid(bid);
            } else {
            	cout << "Bid Id " << bidKey << " not found." << endl;
            }

            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

            break;

        case 5:
            bidList.Remove(bidKey);

            break;
        }
    }

    cout << "Good bye." << endl;

    return 0;
}

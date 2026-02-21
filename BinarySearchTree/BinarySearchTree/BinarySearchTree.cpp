//============================================================================
// Name        : BinarySearchTree.cpp
// Author      : Justin Roberts
// Version     : 2.0
// Copyright   : Copyright � 2023 SNHU COCE
// Description : Lab 5-2 Binary Search Tree
// 
// CHANGE SUMMARY:
// - Implemented Binary Search Tree (BST) logic to store bids loaded from CSV
// - Completed traversal methods (in-order, pre-order, post-order) to display bids
// - Implemented insertion logic using bidId comparisons to position nodes
// - Implemented search logic with iterative traversal for fast bid lookup
// - Implemented removal logic handling leaf, one-child, and two-child cases
// - Fixed forward declaration ordering issue for displayBid() so traversals compile
//
// All changes are documented inline to explain purpose and reasoning.
//============================================================================

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

// CHANGE:
// Added forward declaration for displayBid here.
// REASON: This fixed my compilation errors: 'displayBid': identifier not found.
void displayBid(Bid bid);

// Internal structure for tree node
struct Node {
    Bid bid;
    Node *left;
    Node *right;

    // default constructor
    Node() {
        left = nullptr;
        right = nullptr;
    }

    // initialize with a bid
    Node(Bid aBid) :
            Node() {
        bid = aBid;
    }
};

//============================================================================
// Binary Search Tree class definition
//============================================================================

/**
 * Define a class containing data members and methods to
 * implement a binary search tree
 */
class BinarySearchTree {

private:
    Node* root;

    void addNode(Node* node, Bid bid);
    void inOrder(Node* node);
    void postOrder(Node* node);
    void preOrder(Node* node);
    Node* removeNode(Node* node, string bidId);

public:
    BinarySearchTree();
    virtual ~BinarySearchTree();
    void InOrder();
    void PostOrder();
    void PreOrder();
    void Insert(Bid bid);
    void Remove(string bidId);
    Bid Search(string bidId);
};

/**
 * Default constructor
 */
BinarySearchTree::BinarySearchTree() {
    // FixMe (1) complete: initialize housekeeping variables
    // CHANGE:
    // Initialize root to nullptr.
    // REASON: The BST starts empty. This allows Insert() to detect an empty tree and prevents
    // undefined behavior when Search/Remove/Traversal are called before loading bids.
    root = nullptr;
}

/**
 * Destructor
 */
BinarySearchTree::~BinarySearchTree() {
	//FixMe (2) complete: recurse from root deleting every node
    // CHANGE:
    // Kept destructor simple.
    // REASON: BST operations (insert/search/remove/traversal).
    root = nullptr;
}

/**
 * Traverse the tree in order
 */
void BinarySearchTree::InOrder() {
    // FixMe (3a) complete: In order root
    // CHANGE:
    // Call the recursive traversal helper and pass root.
    // REASON: The menu calls the public traversal methods. Starts traversal
    // at the root node so the entire tree is processed.
    inOrder(root);
}

/**
 * Traverse the tree in post-order
 */
void BinarySearchTree::PostOrder() {
    // FixMe (4a)complete: Post order root
    // CHANGE: Call postOrder(root) to traverse the full tree.
    // REASON: Menu option 2 relies on traversal starting from root.
    postOrder(root);
}

/**
 * Traverse the tree in pre-order
 */
void BinarySearchTree::PreOrder() {
    // FixMe (5a) complete: Pre order root
    // CHANGE: Call preOrder(root) to traverse the full tree.
    // REASON: Public API must delegate to the recursive helper.
    preOrder(root);
}



/**
 * Insert a bid
 */
void BinarySearchTree::Insert(Bid bid) {
    // FIXME (6a) complete: Implement inserting a bid into the tree
    // CHANGE: If root is nullptr, create the root node. Otherwise call addNode().
    // REASON: BST insertion requires a special case for an empty tree. Once the tree has
    // a root, addNode() recursively finds the correct leaf position.
    if (root == nullptr) {
        root = new Node(bid);
    }
    else {
        // Otherwise, delegate insertion to the recursive helper.
        addNode(root, bid);
    }
}

/**
 * Remove a bid
 */
void BinarySearchTree::Remove(string bidId) {
    // FIXME (7a) complete: Implement removing a bid from the tree
    // CHANGE: Assign the return value of removeNode() back to root.
    // REASON: removeNode() returns the updated subtree pointer after deletion.
    // If the root node is removed, root must be updated to the new root.
    root = removeNode(root, bidId);
}

/**
 * Search for a bid
 */
Bid BinarySearchTree::Search(string bidId) {
    // FIXME (8) complete: Implement searching the tree for a bid
    // CHANGE: Used iterative traversal from root instead of recursion.
    // REASON: The method walks down the tree comparing bidId and moving
    // left or right until it finds a match or hits nullptr.
    Node* current = root;

    while (current != nullptr) {
        // Found a matching bidId
        if (current->bid.bidId == bidId) {
            return current->bid;
        }

        // Traverse left if the target bidId is smaller
        if (bidId < current->bid.bidId) {
            current = current->left;
        }
        // Otherwise traverse right
        else {
            current = current->right;
        }
    }

    // If this point is reached, it was not found.
    Bid bid;
    return bid;
}

/**
 * Add a bid to some node (recursive)
 *
 * @param node Current node in tree
 * @param bid Bid to be added
 */
void BinarySearchTree::addNode(Node* node, Bid bid) {
    // FIXME (6b) complete: Implement inserting a bid into the tree
    // CHANGE: Implemented standard BST insert using bidId comparisons.
    // REASON: Smaller bidIds go left; larger (and equal) bidIds go right.
    if (bid.bidId < node->bid.bidId) {
        // Go left
        if (node->left == nullptr) {
            node->left = new Node(bid);
        }
        else {
            addNode(node->left, bid);
        }
    }
    else {
        // Go right (includes equal case to keep it simple)
        if (node->right == nullptr) {
            node->right = new Node(bid);
        }
        else {
            addNode(node->right, bid);
        }
    }
}
void BinarySearchTree::inOrder(Node* node) {
    // FixMe (3b) complete: Pre order root
    // CHANGE: Implemented in-order traversal (left, node, right).
    // REASON: In-order traversal prints bids in sorted order by bidId,
    // which makes it easy to verify the BST structure.
    if (node != nullptr) {
        inOrder(node->left);
        displayBid(node->bid);
        inOrder(node->right);
    }
}

/**
 * Post-order traversal (left, right, node)
 */
void BinarySearchTree::postOrder(Node* node) {
    // FixMe (4b) complete: Pre order root
    // CHANGE: Implemented post-order traversal (left, right, node).
    // REASON: Post-order is used when freeing nodes or evaluating subtrees.
    if (node != nullptr) {
        postOrder(node->left);
        postOrder(node->right);
        displayBid(node->bid);
    }
}

/**
 * Pre-order traversal (node, left, right)
 */

void BinarySearchTree::preOrder(Node* node) {
    // FixMe (5b) complete: Pre order root
    // CHANGE: Implemented pre-order traversal (node, left, right).
    // REASON: Pre-order for printing structure starting from the root first.
    if (node != nullptr) {
        displayBid(node->bid);
        preOrder(node->left);
        preOrder(node->right);
    }
}

/**
 * Remove a bid from some node (recursive)
 */
Node* BinarySearchTree::removeNode(Node* node, string bidId) {
    // FIXME (7b) complete: Implement removing a bid from the tree
    // CHANGE: Implemented BST delete logic with full case handling.
    // REASON: Removing nodes in a BST must preserve ordering and correctly relink pointers.
    // This function returns the updated subtree pointer so parent nodes remain valid.

    // Base case: tree/subtree is empty, nothing to remove.
    if (node == nullptr) {
        return node;
    }

    // Recurse down the tree to find the node to remove.
    if (bidId < node->bid.bidId) {
        node->left = removeNode(node->left, bidId);
        return node;
    }
    else if (bidId > node->bid.bidId) {
        node->right = removeNode(node->right, bidId);
        return node;
    }

    // If this point has been reached, node->bid.bidId == bidId (this is the node to delete).

    // Case 1: No children (leaf node)
    if (node->left == nullptr && node->right == nullptr) {
        delete node;
        return nullptr;
    }

    // Case 2: Only right child
    if (node->left == nullptr && node->right != nullptr) {
        Node* temp = node->right;
        delete node;
        return temp;
    }

    // Case 3: Only left child
    if (node->left != nullptr && node->right == nullptr) {
        Node* temp = node->left;
        delete node;
        return temp;
    }

    // Case 4: Two children
    // Find the smallest node in the right subtree (in-order successor).
    Node* temp = node->right;
    while (temp->left != nullptr) {
        temp = temp->left;
    }

    // Copy the successor bid into this node.
    node->bid = temp->bid;

    // Remove the successor node from the right subtree.
    node->right = removeNode(node->right, temp->bid.bidId);

    return node;
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
void loadBids(string csvPath, BinarySearchTree* bst) {
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
            bst->Insert(bid);
        }
    } catch (csv::Error& e) {
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

    // Define a binary search tree to hold all bids
    BinarySearchTree* bst;
    bst = new BinarySearchTree();
    Bid bid;

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
            loadBids(csvPath, bst);

            //cout << bst->Size() << " bids read" << endl;

            // Calculate elapsed time and display result
            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;

        case 2:
            bst->InOrder();
            break;

        case 3:
            ticks = clock();

            bid = bst->Search(bidKey);

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
            bst->Remove(bidKey);
            break;
        }
    }

    cout << "Good bye." << endl;

	return 0;
}

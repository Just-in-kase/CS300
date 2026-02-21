//============================================================================
// Name        : VectorSorting.cpp
// Author      : Justin Roberts
// Version     : 2.0
// Copyright   : Copyright © 2023 SNHU COCE
// Description : Vector Sorting Algorithms
//============================================================================
// NOTES OF WHAT I CHANGED AND WHY (if you don't want to go line by line):
// 1) Added missing standard library includes <vector> and <string>.
//    - Why: This file uses std::vector and std::string directly. Some compilers/IDEs
//      may compile without these if other headers pull them in indirectly, but that is
//      not guaranteed, and needed in my case. Adding the headers prevented build
//      errors like "identifier 'vector' is undefined" or IntelliSense E1696 issues.
// 2) Implemented selectionSort(vector<Bid>&) to sort by Bid.title.
//    - Why: The starter file contained only comments. The assignment requires an
//      O(n^2) selection sort over bid titles.
// 3) Implemented quicksort using Hoare partition scheme:
//      - partition(vector<Bid>&, int, int)
//      - quickSort(vector<Bid>&, int, int)
//    - Why: The starter file contained only comments. The assignment requires a
//      working quicksort over bid titles. Hoare partition is efficient and matches
//      the recursive structure required.
// 4) Added menu options to actually run sorts and report timing results.
//    - Why: The starter file had placeholders (FIXME 1b/2b). The assignment requires
//      invoking the sorts and printing timing. I also added printing "X bids sorted"
//      so output matches the provided sample format.
// 5) Added defensive checks before sorting (bids.empty()).
//    - Why: Prevents calling sorts on an empty vector and provides clear guidance
//      to the user (load bids first). This is a runtime safety improvement.
// 6) Added default case to handle invalid menu inputs.
//    - Why: Helps user to see error of invalid input and prompts for the correct choice selection
//
//============================================================================
#include <algorithm>
#include <iostream>
#include <time.h>
// CHANGE #1: Added these headers explicitly.
// Reason: This file uses vector<Bid> and string. Including them directly to avoid
// relying on indirect includes that may differ by compiler/settings.
#include <vector>
#include <string>

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
 * Prompt user for bid information using console (std::in)
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
 * Load a CSV file containing bids into a container
 *
 * @param csvPath the path to the CSV file to load
 * @return a container holding all the bids read
 */
vector<Bid> loadBids(string csvPath) {
    cout << "Loading CSV file " << csvPath << endl;

    // Define a vector data structure to hold a collection of bids.
    vector<Bid> bids;

    // initialize the CSV Parser using the given path
    csv::Parser file = csv::Parser(csvPath);

    try {
        // loop to read rows of a CSV file
        for (int i = 0; i < file.rowCount(); i++) {

            // Create a data structure and add to the collection of bids
            Bid bid;
            bid.bidId = file[i][1];
            bid.title = file[i][0];
            bid.fund = file[i][8];
            bid.amount = strToDouble(file[i][4], '$');

            //cout << "Item: " << bid.title << ", Fund: " << bid.fund << ", Amount: " << bid.amount << endl;

            // push this bid to the end
            bids.push_back(bid);
        }
    }
    catch (csv::Error& e) {
        std::cerr << e.what() << std::endl;
    }
    return bids;
}

// QUICK SORT (FIXME 2a) - Implemented

/**
 * Partition the vector of bids into two parts, low and high
 * 
 * CHANGE #3a: Implemented partition() using Hoare partition scheme.
 * Reason:
 * - The starter file was incomplete and only provided comments.
 * - Hoare partition is efficient and pairs nicely with recursion.
 * - We compare Bid objects by their 'title' field because the assignment
 *   requires sorting by bid.title.
 *
 * 
 * @param bids Address of the vector<Bid> instance to be partitioned
 * @param begin Beginning index to partition
 * @param end Ending index to partition
 */
int partition(vector<Bid>& bids, int begin, int end) {
    int low = begin;
    int high = end;

    // Calculate the middle element
    int middlePoint = begin + (end - begin) / 2;

    // Set pivot as middlePoint element title
    string pivot = bids[middlePoint].title;

    bool done = false;
    while (!done) {
        // keep incrementing low index while bids[low].title < pivot
        while (bids[low].title < pivot) {
            ++low;
        }

        // keep decrementing high index while pivot < bids[high].title
        while (pivot < bids[high].title) {
            --high;
        }

        /* If there are zero or one elements remaining,
           all bids are partitioned. Return high */
        if (low >= high) {
            done = true;
        }
        else {
            // swap the low and high bids
            swap(bids[low], bids[high]);

            // move low and high closer
            ++low;
            --high;
        }
    }
    // Hoare partition returns the final high index that splits partitions.
    return high;
}

/**
 * Perform a quick sort on bid title
 * Average performance: O(n log(n))
 * Worst case performance O(n^2))
 *
 * CHANGE #3b: Implemented quickSort() recursion using Hoare partition return.
 * Reason:
 * - The starter file had placeholders only.
 * - Base case prevents infinite recursion.
 * - Recursively sorts two partitions created by partition().
 * 
 * @param bids address of the vector<Bid> instance to be sorted
 * @param begin the beginning index to sort on
 * @param end the ending index to sort on
 */
void quickSort(vector<Bid>& bids, int begin, int end) {
    //set mid equal to 0

    /* Base case: If there are 1 or zero bids to sort,
     partition is already sorted otherwise if begin is greater
     than or equal to end then return*/
    if (begin >= end) {
        return;
    }

     /* Partition bids into low and high such that
      midpoint is location of last element in low */
    int mid = partition(bids, begin, end);
      // recursively sort low partition (begin to mid)
    quickSort(bids, begin, mid);
      // recursively sort high partition (mid+1 to end)
    quickSort(bids, mid + 1, end);
}

// SELECTION SORT (FIXME 1a) - Implemented

/**
 * Perform a selection sort on bid title
 * Average performance: O(n^2))
 * Worst case performance O(n^2))
 *
 * CHANGE #2: Implemented selectionSort() over Bid.title.
 * Reason:
 * - The starter file had placeholders only.
 * - The assignment specifically requires selection sort logic.
 * - This implementation searches for the smallest title in the unsorted
 *   section and swaps it into the current position.
 * 
 * @param bid address of the vector<Bid>
 *            instance to be sorted
 */
void selectionSort(vector<Bid>& bids) {
    //define min as int (index of the current minimum bid)

    // Use size_t for indices because vector::size() returns size_t.
    // This helped me avoid signed/unsigned mismatch warnings.
    size_t size = bids.size();
    

    // pos separates sorted [0..pos-1] and unsorted [pos..end]
    for (size_t pos = 0; pos < size; ++pos) {
        size_t min = pos;

        // Finds the smallest title to the right of pos
        for (size_t j = pos + 1; j < size; ++j) {
            // if this element's title is less than minimum title
            if (bids[j].title < bids[min].title) {
                min = j;
            }
        }
                
        // Swaps the smallest element found into position pos
        if(min != pos) {
            swap(bids[pos], bids[min]);
        }
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
    string csvPath;
    switch (argc) {
    case 2:
        csvPath = argv[1];
        break;
    default:
        csvPath = "eBid_Monthly_Sales.csv";
    }

    // Define a vector to hold all the bids
    vector<Bid> bids;

    // Define a timer variable
    clock_t ticks;

    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Bids" << endl;
        cout << "  2. Display All Bids" << endl;
        cout << "  3. Selection Sort All Bids" << endl;
        cout << "  4. Quick Sort All Bids" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

        case 1:
            // Initialize a timer variable before loading bids
            ticks = clock();

            // Complete the method call to load the bids
            bids = loadBids(csvPath);

            cout << bids.size() << " bids read" << endl;

            // Calculate elapsed time and display result
            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

            break;

        case 2:
            // Loop and display the bids read
            for (int i = 0; i < bids.size(); ++i) {
                displayBid(bids[i]);
            }
            cout << endl;

            break;

/** CHANGE #4: Implemented menu option 3 (FIXME 1b in starter)
*
* Reason:
* - Calls selectionSort() and print timing.
* - The assignment requires running the sort and reporting elapsed time.
* - Also prints "X bids sorted" to match the sample output format.
*/
        case 3:
            if (bids.empty()) {
                cout << "No bids loaded. Choose option 1 first." << endl;
                break;
            }

            ticks = clock();
            selectionSort(bids);
            ticks = clock() - ticks;

            cout << bids.size() << " bids sorted" << endl;   
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;
        /** CHANGE #4: Implemented menu option 4 (FIXME 2b in starter)
        *
        * Reason:
        * - Calls quickSort() and print timing.
        * - The assignment requires running quicksort and reporting elapsed time.
        * - Also prints "X bids sorted" to match the sample output format.
        */
        case 4:
            // CHANGE #5: Defensive check (same reason as option 3).
            if (bids.empty()) {
                cout << "No bids loaded. Choose option 1 first." << endl;
                break;
            }

            ticks = clock();
            quickSort(bids, 0, (int)bids.size() - 1);
            ticks = clock() - ticks;

            cout << bids.size() << " bids sorted" << endl;   
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;

            // CHANGE #6: Default case to handle invalid menu inputs.
           default:
               cout << "Invalid choice. Try again." << endl;
               break;
        }
    }

    cout << "Good bye." << endl;

    return 0;
}
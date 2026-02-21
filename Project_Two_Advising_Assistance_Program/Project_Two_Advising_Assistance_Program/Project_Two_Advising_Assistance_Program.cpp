//============================================================================
// Name        : Project_Two_Advising_Assistance_Program.cpp 
// Author      : Justin Roberts
// Version     : 1.0
// Description : CS300 Project Two: Advising Assistance Program using a Binary Search Tree (BST)
// Date        : February 21, 2026
//
// Single-file implementation 
// Stores Course objects in a Binary Search Tree keyed by courseNumber.
// Supports:
// 1) Load file data into BST
// 2) Print alphanumeric course list (in-order traversal)
// 3) Print a single course + prerequisites
// 9) Exit
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <limits>   // Required for numeric_limits<streamsize>::max()

using namespace std;

// ---------------------------- Utility Helpers ----------------------------
// These helper functions standardize user input and CSV tokens.
// Trimming removes leading/trailing spaces, and toUpper normalizes course IDs.
// This prevents user input like "csci400" or " CSCI400 " from failing the search.

static inline string ltrim(const string& s) {
    size_t start = 0;
    while (start < s.size() && isspace(static_cast<unsigned char>(s[start]))) {
        start++;
    }
    return s.substr(start);
}

static inline string rtrim(const string& s) {
    // size_t is unsigned. This implementation protects against the "all spaces" case by returning "" when end hits 0.
    if (s.empty()) return s;
    size_t end = s.size() - 1;
    while (end != string::npos && isspace(static_cast<unsigned char>(s[end]))) {
        if (end == 0) return "";
        end--;
    }
    return s.substr(0, end + 1);
}

static inline string trim(const string& s) {
    return rtrim(ltrim(s));
}

static inline string toUpper(string s) {
    // Course numbers in the CSV are already uppercase, but user input may not be.
    // Normalizing everything to uppercase ensures a consistent BST key.
    for (char& c : s) {
        c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
    }
    return s;
}

// Split a CSV line by commas. Assumes no quoted commas.
static vector<string> splitCSV(const string& line) {
    vector<string> tokens;
    string token;
    stringstream ss(line);

    while (getline(ss, token, ',')) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

// ---------------------------- Domain Model ----------------------------

struct Course {
    string courseNumber;              
    string courseTitle;                
    vector<string> prerequisites;      

    Course() = default;
};

// ---------------------------- BST Implementation ----------------------------
// The BST stores courses by courseNumber so an in-order traversal prints in alphanumeric order.

struct Node {
    Course course;
    Node* left;
    Node* right;

    explicit Node(const Course& c) : course(c), left(nullptr), right(nullptr) {}
};

class BinarySearchTree {
private:
    Node* root;

    void inOrderHelper(Node* node) const {
        if (node == nullptr) return;
        // In-order traversal prints keys in sorted (ascending) order for a BST.
        inOrderHelper(node->left);
        cout << node->course.courseNumber << ", " << node->course.courseTitle << endl;
        inOrderHelper(node->right);
    }

    void destroy(Node* node) {
        // Post-order delete ensures children are freed before the parent node.
        if (node == nullptr) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    BinarySearchTree() : root(nullptr) {}
    ~BinarySearchTree() { destroy(root); }

    bool isEmpty() const { return root == nullptr; }

    // Insert returns true if inserted, false if duplicate key.
    bool Insert(const Course& newCourse) {
        // BST insertion is O(log n) average, O(n) worst-case (if tree becomes unbalanced).
        if (root == nullptr) {
            root = new Node(newCourse);
            return true;
        }

        Node* current = root;

        while (true) {
            if (newCourse.courseNumber < current->course.courseNumber) {
                if (current->left == nullptr) {
                    current->left = new Node(newCourse);
                    return true;
                }
                current = current->left;
            }
            else if (newCourse.courseNumber > current->course.courseNumber) {
                if (current->right == nullptr) {
                    current->right = new Node(newCourse);
                    return true;
                }
                current = current->right;
            }
            else {
                // For duplicate courseNumber
                return false;
            }
        }
    }

    // Search returns a pointer to the course if found, otherwise nullptr.
    const Course* Search(const string& courseNumber) const {
        // Normalize input so the user can type "csci400" or " CSCI400 ".
        string key = toUpper(trim(courseNumber));
        Node* current = root;

        // BST search is O(log n) average, O(n) worst-case if unbalanced.
        while (current != nullptr) {
            if (key == current->course.courseNumber) {
                return &(current->course);
            }
            else if (key < current->course.courseNumber) {
                current = current->left;
            }
            else {
                current = current->right;
            }
        }
        return nullptr;
    }

    void InOrder() const {
        inOrderHelper(root);
    }
};

// ---------------------------- Loading + Validation ----------------------------
// LoadCoursesFromFile reads the CSV file, validates formatting, normalizes course numbers,
// and inserts courses into the BST. The BST becomes the single source of course data.

static bool LoadCoursesFromFile(const string& fileName, BinarySearchTree& courseTree) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Error: Could not open file \"" << fileName << "\"." << endl;
        return false;
    }

    string line;
    int lineNumber = 0;
    bool hadFatalFormatError = false;

    while (getline(file, line)) {
        lineNumber++;

        // Skip empty lines 
        if (trim(line).empty()) {
            continue;
        }

        vector<string> tokens = splitCSV(line);

        // Must have at least: courseNumber, courseTitle
        if (tokens.size() < 2) {
            cout << "Error: Formatting issue on line " << lineNumber
                << ". Each row must contain at least course number and title." << endl;
            hadFatalFormatError = true;
            break;
        }

        Course c;
        c.courseNumber = toUpper(trim(tokens[0]));
        c.courseTitle = trim(tokens[1]);

        // Remaining tokens (if any) are prerequisites
        for (size_t i = 2; i < tokens.size(); i++) {
            string prereq = toUpper(trim(tokens[i]));
            if (!prereq.empty()) {
                c.prerequisites.push_back(prereq);
            }
        }

        // Insert into BST (report duplicates but continue)
        if (!courseTree.Insert(c)) {
            cout << "Warning: Duplicate course \"" << c.courseNumber
                << "\" found on line " << lineNumber << ". Skipping duplicate." << endl;
        }
    }

    file.close();

    if (hadFatalFormatError) {
        return false;
    }

    return true;
}

static void PrintCourseWithPrereqs(const BinarySearchTree& courseTree, const string& courseNumberInput) {
    // Normalize user input before searching so minor input differences don’t cause a miss.
    string key = toUpper(trim(courseNumberInput));
    const Course* course = courseTree.Search(key);

    if (course == nullptr) {
        cout << "Course not found." << endl;
        return;
    }

    cout << course->courseNumber << ", " << course->courseTitle << endl;

    if (course->prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
        return;
    }

    cout << "Prerequisites: ";

    for (size_t i = 0; i < course->prerequisites.size(); i++) {
        const string& prereqNum = course->prerequisites[i];

        const Course* prereqCourse = courseTree.Search(prereqNum);

        if (prereqCourse != nullptr) {
            // Format: CSCI301 (Advanced Programming in C++)
            cout << prereqCourse->courseNumber << " (" << prereqCourse->courseTitle << ")";
        }
        else {
            // If not found, still print the number (and keep going)
            cout << prereqNum;
        }

        if (i + 1 < course->prerequisites.size()) {
            cout << ", ";
        }
    }

    cout << endl;
}

// ---------------------------- Menu ----------------------------
// Menu options to match the sample.
// Option 2 and 3 are gated by dataLoaded to keep "load first" requirements.

static void PrintMenu() {
    cout << "1. Load Data Structure." << endl;
    cout << "2. Print Course List." << endl;
    cout << "3. Print Course." << endl;
    cout << "9. Exit" << endl;
}

int main() {
    BinarySearchTree courseTree;
    bool dataLoaded = false;

    cout << "Welcome to the course planner." << endl;

    int choice = 0;
    while (choice != 9) {
        PrintMenu();
        cout << "What would you like to do? ";

        // Read menu choice 
        if (!(cin >> choice)) {
            // Non-numeric input recovery. Prevents the program from crashing or looping incorrectly if the user types letters.
            cin.clear(); // clear error flags
			cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discards characters until newline hit or max limit reached
			cout << endl; // Empty line for spacing before menu prints again
            cout << "Input was not a valid number." << endl;
            continue;
        }

        // Clear newline so getline works for Option 3
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discards characters until newline hit or max limit reached
		cout << endl; // Empty line for spacing before menu prints again

        switch (choice) {
        case 1: {
            string fileName = "CS 300 ABCU_Advising_Program_Input.csv";
            cout << "Loading file: " << fileName << endl;

            if (LoadCoursesFromFile(fileName, courseTree)) {
                dataLoaded = true;
                cout << "Data loaded successfully." << endl;
            }
            else {
                // Keep dataLoaded false so Options 2 and 3 cannot run on incomplete/failed load.
                dataLoaded = false;
            }

            break;
        }
        case 2:
            if (!dataLoaded) {
                cout << "Please load data first (Option 1)." << endl;
            }
            else {
                // In-order traversal prints an alphanumeric list of all courses.
                cout << "Here is a sample schedule:" << endl;
                courseTree.InOrder();
            }
            break;

        case 3:
            if (!dataLoaded) {
                cout << "Please load data first (Option 1)." << endl;
            }
            else {
                cout << "What course do you want to know about? ";
                string courseNumber;
                getline(cin, courseNumber);
                PrintCourseWithPrereqs(courseTree, courseNumber);
            }
            break;

        case 9:
            cout << "Thank you for using the course planner!" << endl;
            break;

        default:
            // Handles out-of-range menu values
            cout << choice << " is not a valid option." << endl;
            break;
        }
        cout << endl;  // This creates the spacing before the menu prints again
        
    }

    return 0;
}
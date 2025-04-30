#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cctype>
#include "CleanUtils.h"

using namespace std;

struct Review {
    string productID, customerID, reviewText;
    int rating;
};

struct ReviewNode {
    Review data;
    ReviewNode* next;
    ReviewNode(const Review& r) : data(r), next(nullptr) {}
};

class ReviewList {
private:
    ReviewNode* head;
    int count;

public:
    ReviewList() : head(nullptr), count(0) {}
    
    ~ReviewList() {
        ReviewNode* current = head;
        while (current != nullptr) {
            ReviewNode* next = current->next;
            delete current;
            current = next;
        }
    }

    void add(const Review& r) {
        ReviewNode* newNode = new ReviewNode(r);
        newNode->next = head;
        head = newNode;
        count++;
    }

    int getCount() const { return count; }

    void toArray(Review*& arr) {
        arr = new Review[count];
        ReviewNode* current = head;
        int i = count - 1;
        while (current != nullptr) {
            arr[i] = current->data;
            current = current->next;
            i--;
        }
    }
};


string cleanText(string text) {
    string result;
    for(char c : text) {
        if(isalnum(c) || isspace(c)) {
            result += c;
        }
    }
    return result;
}

int cleanReviews(Review *&reviews, int &size) {
    ifstream inFile("data/reviews.csv");
    if (!inFile.is_open()) {
        cout << "Error: Cannot open data/reviews.csv\n";
        return 0;
    }

    ofstream outFile("data/reviewsClean.csv");
    if (!outFile.is_open()) {
        cout << "Error: Cannot create data/reviewsClean.csv\n";
        inFile.close();
        return 0;
    }

    string line;
    getline(inFile, line);  // Read header
    outFile << line << endl;  // Write header to new file

    ReviewList validReviews;
    int lineNumber = 1;

    while (getline(inFile, line)) {
        lineNumber++;
        stringstream ss(line);
        string productID, customerID, ratingStr, reviewText;
        
        getline(ss, productID, ',');
        getline(ss, customerID, ',');
        getline(ss, ratingStr, ',');
        getline(ss, reviewText);

        bool isValid = true;

        if (productID.empty() || customerID.empty() || ratingStr.empty()) {
            cout << "Line " << lineNumber << ": Missing required field(s)\n";
            isValid = false;
        }

        if (ratingStr == "Invalid Rating") {
            cout << "Line " << lineNumber << ": Invalid Rating found\n";
            isValid = false;
        }

        if (!isValidInteger(ratingStr)) {
            cout << "Line " << lineNumber << ": Rating must be a number\n";
            isValid = false;
        } else {
            int rating = stoi(ratingStr);
            if (rating < 1 || rating > 5) {
                cout << "Line " << lineNumber << ": Rating must be between 1 and 5\n";
                isValid = false;
            }
        }

        reviewText = cleanText(toLowerCase(reviewText));
        if (reviewText.empty()) {
            cout << "Line " << lineNumber << ": Empty review text after cleaning\n";
            isValid = false;
        }

        if (!isValid) continue;

        Review r;
        r.productID = productID;
        r.customerID = customerID;
        r.rating = stoi(ratingStr);
        r.reviewText = reviewText;
        validReviews.add(r);

        outFile << productID << "," << customerID << "," 
               << ratingStr << "," << reviewText << endl;
    }

    inFile.close();
    outFile.close();

    cout << "Cleaned reviews saved to data/reviewsClean.csv\n";

    size = validReviews.getCount();
    validReviews.toArray(reviews);

    return size;
}

int main() {
    Review *reviews = nullptr;
    int reviewSize = 0;

    cout << "Cleaning reviews...\n";
    cleanReviews(reviews, reviewSize);
    cout << "Loaded " << reviewSize << " valid reviews.\n";

    delete[] reviews;
    return 0;
}

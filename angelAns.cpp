#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include "CustomString.hpp"
#include "Transaction.hpp"
#include "Review.hpp"
#include "Algorithms.hpp"

using namespace std;

#define MAX_TRANSACTIONS 5000
#define MAX_REVIEWS 1000
#define MAX_WORDS 5000

Transaction transactions[MAX_TRANSACTIONS];
int transactionCount = 0;

Review reviews[MAX_REVIEWS];
int reviewCount = 0;

WordFreq wordList[MAX_WORDS];
int wordCount = 0;

// Function to clean word: remove punctuation, lowercase
MyString cleanWord(const MyString &word) {
    MyString result;
    CharNode* current = word.head;
    while (current) {
        if (isalnum(current->data)) {
            char lowered = tolower(current->data);
            char str[2] = {lowered, '\0'};
            result = result + MyString(str);
        }
        current = current->next;
    }
    return result;
}

// Partition function for quicksort
int partition(Transaction arr[], int low, int high) {
    Transaction pivot = arr[high];
    int i = (low - 1);
    
    for (int j = low; j <= high - 1; j++) {
        // Compare dates using < operator
        if (!(pivot.date < arr[j].date)) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return (i + 1);
}

// Quicksort implementation
void quickSort(Transaction arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// Wrapper function for sorting by date
void sortByDate(Transaction arr[], int n) {
    quickSort(arr, 0, n - 1);
}

// Binary search for a category (assumes sorted by category)
bool binarySearchCategory(Transaction arr[], int n, const MyString &target) {
    int left = 0, right = n - 1;
    while (left <= right) {
        int mid = (left + right) / 2;
        if (arr[mid].category == target) return true;
        else if (arr[mid].category < target) left = mid + 1;
        else right = mid - 1;
    }
    return false;
}

void addWord(const MyString &w) {
    for (int i = 0; i < wordCount; ++i) {
        if (wordList[i].word == w) {
            wordList[i].count++;
            return;
        }
    }
    wordList[wordCount++] = WordFreq(w, 1);
}

void processReviewText(const MyString& text) {
    MyString word;
    CharNode* curr = text.head;
    while (curr) {
        // Skip leading spaces
        while (curr && isspace(curr->data)) {
            curr = curr->next;
        }
        if (!curr) break;
        
        // Find end of word
        CharNode* wordStart = curr;
        while (curr && !isspace(curr->data)) {
            curr = curr->next;
        }
        
        // Create word
        MyString currWord;
        CharNode* temp = wordStart;
        while (temp && temp != curr) {
            char str[2] = {temp->data, '\0'};
            currWord = currWord + MyString(str);
            temp = temp->next;
        }
        
        MyString cleaned = cleanWord(currWord);
        if (!cleaned.empty()) {
            addWord(cleaned);
        }
    }
}

void sortWordsByFrequency() {
    for (int i = 1; i < wordCount; ++i) {
        WordFreq key = wordList[i];
        int j = i - 1;
        while (j >= 0 && wordList[j].count < key.count) {
            wordList[j + 1] = wordList[j];
            --j;
        }
        wordList[j + 1] = key;
    }
}

void loadTransactions(const std::string &filename) {
    std::ifstream file(filename);
    std::string line;
    getline(file, line); // Skip header
    while (getline(file, line) && transactionCount < MAX_TRANSACTIONS) {
        std::stringstream ss(line);
        std::string customer, product, category, price, date, payment;
        
        // Split by | first
        size_t pipe_pos = line.find('|');
        if (pipe_pos != std::string::npos) {
            customer = line.substr(0, pipe_pos);
            std::string rest = line.substr(pipe_pos + 1);
            
            // Parse the rest with commas
            std::stringstream ss_rest(rest);
            getline(ss_rest, product, ',');
            getline(ss_rest, category, ',');
            getline(ss_rest, price, ',');
            getline(ss_rest, date, ',');
            getline(ss_rest, payment, '\n');
            
            // Clean up any whitespace
            category = category.substr(category.find_first_not_of(" \t"));
            payment = payment.substr(payment.find_first_not_of(" \t"));
            
            transactions[transactionCount++] = Transaction(
                MyString(customer.c_str()), 
                MyString(product.c_str()), 
                stod(price), 
                MyString(date.c_str()), 
                MyString(category.c_str()), 
                MyString(payment.c_str())
            );
        }
    }
    file.close();
}

void loadReviews(const std::string &filename) {
    std::ifstream file(filename);
    std::string line;
    getline(file, line); // Skip header
    while (getline(file, line) && reviewCount < MAX_REVIEWS) {
        std::stringstream ss(line);
        std::string pid, cid, rating, text;
        getline(ss, pid, ',');
        getline(ss, cid, ',');
        getline(ss, rating, ',');
        getline(ss, text, '\n');
        reviews[reviewCount++] = Review(
            MyString(pid.c_str()), 
            MyString(cid.c_str()), 
            MyString(text.c_str()), 
            stoi(rating)
        );
    }
    file.close();
}

int main() {
    loadTransactions("transactionsClean.csv");
    loadReviews("reviewsClean.csv");

    // Q1: Sort by date
    cout << "Q1: Analyzing transactions...\n";
    auto start = std::chrono::high_resolution_clock::now();
    
    sortByDate(transactions, transactionCount);
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    
    cout << "Total transactions analyzed: " << transactionCount << "\n";
    cout << "Time taken: " << duration.count() << " seconds\n\n";

    cout << "Total Transactions: " << transactionCount << "\nTotal Reviews: " << reviewCount << endl;

    // Q2: Electronics category paid with Credit Card
    int electronicsTotal = 0, electronicsCC = 0;
    for (int i = 0; i < transactionCount; ++i) {
        if (transactions[i].category == "Electronics" || 
            transactions[i].category == "Electronics ") {  // Handle potential trailing space
            electronicsTotal++;
            if (transactions[i].paymentMethod == "Credit Card" || 
                transactions[i].paymentMethod == "Credit Card ") {  // Handle potential trailing space
                electronicsCC++;
            }
        }
    }
    double percent = electronicsTotal ? (electronicsCC * 100.0 / electronicsTotal) : 0;
    cout << "\nQ2: Electronics Analysis:\n";
    cout << "Total Electronics purchases: " << electronicsTotal << endl;
    cout << "Electronics purchases with Credit Card: " << electronicsCC << endl;
    cout << "Percentage: " << percent << "%\n";

    // Q3: Word frequency in 1-star reviews
    for (int i = 0; i < reviewCount; ++i) {
        if (reviews[i].rating == 1) {
            processReviewText(reviews[i].reviewText);
        }
    }

    sortWordsByFrequency();
    cout << "\nQ3: Top words in 1-star reviews:\n";
    for (int i = 0; i < min(wordCount, 5); ++i) {
        cout << wordList[i].word << " - " << wordList[i].count << endl;
    }

    cout << "\nPress Enter to exit..."; 
    cin.get();
    return 0;
}

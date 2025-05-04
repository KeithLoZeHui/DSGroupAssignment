#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include "CustomString.hpp"
#include "Transaction.hpp"
#include "Review.hpp"
#include "Algorithms.hpp"

using namespace std;

// Dynamic array setup
Transaction* transactions = nullptr;
int transactionCount = 0;
int transactionCapacity = 100;

Review* reviews = nullptr;
int reviewCount = 0;
int reviewCapacity = 100;

WordFreq* wordList = nullptr;
int wordCount = 0;
int wordCapacity = 100;

// Helper function to resize Transaction array
void ensureTransactionCapacity() {
    if (transactionCount >= transactionCapacity) {
        transactionCapacity *= 2;
        Transaction* newArr = new Transaction[transactionCapacity];
        for (int i = 0; i < transactionCount; ++i) newArr[i] = transactions[i];
        delete[] transactions;
        transactions = newArr;
    }
}
// Helper function to resize Review array
void ensureReviewCapacity() {
    if (reviewCount >= reviewCapacity) {
        reviewCapacity *= 2;
        Review* newArr = new Review[reviewCapacity];
        for (int i = 0; i < reviewCount; ++i) newArr[i] = reviews[i];
        delete[] reviews;
        reviews = newArr;
    }
}
// Helper function to resize WordFreq array
void ensureWordCapacity() {
    if (wordCount >= wordCapacity) {
        wordCapacity *= 2;
        WordFreq* newArr = new WordFreq[wordCapacity];
        for (int i = 0; i < wordCount; ++i) newArr[i] = wordList[i];
        delete[] wordList;
        wordList = newArr;
    }
}

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
    ensureWordCapacity();
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

// Quick sort implementation for WordFreq array based on frequency count (descending order)
int partitionWordFreq(WordFreq arr[], int low, int high) {
    WordFreq pivot = arr[high];
    int i = (low - 1);
    
    for (int j = low; j <= high - 1; j++) {
        // Sort in descending order of count
        if (arr[j].count >= pivot.count) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return (i + 1);
}

void quickSortWordFreq(WordFreq arr[], int low, int high) {
    if (low < high) {
        int pi = partitionWordFreq(arr, low, high);
        quickSortWordFreq(arr, low, pi - 1);
        quickSortWordFreq(arr, pi + 1, high);
    }
}

void sortWordsByFrequency() {
    // Use quick sort for O(n log n) performance
    quickSortWordFreq(wordList, 0, wordCount - 1);
}

void loadTransactions(const std::string &filename) {
    std::ifstream file(filename);
    std::string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
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
            
            ensureTransactionCapacity();
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
    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string pid, cid, rating, text;
        getline(ss, pid, ',');
        getline(ss, cid, ',');
        getline(ss, rating, ',');
        getline(ss, text, '\n');
        ensureReviewCapacity();
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
    transactions = new Transaction[transactionCapacity];
    reviews = new Review[reviewCapacity];
    wordList = new WordFreq[wordCapacity];

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
    cout << "\nQ2: Electronics purchases with Credit Card\n";
    cout << "Total Electronics purchases: " << electronicsTotal << endl;
    cout << "Electronics purchases with Credit Card: " << electronicsCC << endl;
    if (electronicsTotal > 0) {
        double percent = (double)electronicsCC / electronicsTotal * 100.0;
        cout << "Percentage: " << percent << "%" << endl;
    } else {
        cout << "No Electronics purchases found.\n";
    }

    cout << "\n3. Word Frequency Analysis Performance:\n";
    auto start_q3 = std::chrono::high_resolution_clock::now();
    
    // Clear word list first to ensure we're only counting words from 1-star reviews
    wordCount = 0;
    // Process all 1-star reviews
    for (int i = 0; i < reviewCount; ++i) {
        if (reviews[i].rating == 1) {
            processReviewText(reviews[i].reviewText);
        }
    }
    
    // Sort words by frequency using quick sort O(n log n)
    sortWordsByFrequency();
    
    // Display the top 10 most frequent words
    cout << "--- First 10 Words Frequency ---\n";
    int displayCount = (wordCount < 10) ? wordCount : 10;
    for (int i = 0; i < displayCount; ++i) {
        cout << wordList[i].word << ": " << wordList[i].count << " occurrences" << endl;
    }

    // Sort wordList alphabetically by word for binary search
    auto compareWords = [](const WordFreq& a, const WordFreq& b) {
        return a.word < b.word;
    };
    // Simple insertion sort for alphabetic order (since STL is not allowed)
    for (int i = 1; i < wordCount; ++i) {
        WordFreq key = wordList[i];
        int j = i - 1;
        while (j >= 0 && wordList[j].word > key.word) {
            wordList[j + 1] = wordList[j];
            --j;
        }
        wordList[j + 1] = key;
    }

    // Prompt user for a word to search
    cout << "\nEnter a word to search for its frequency in 1-star reviews: ";
    std::string searchInput;
    getline(cin, searchInput);
    // Convert input to lowercase using cleanWord
    MyString searchWord(searchInput.c_str());
    MyString cleanedSearch = cleanWord(searchWord);

    // Binary search for the word
    int left = 0, right = wordCount - 1, foundIdx = -1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (wordList[mid].word == cleanedSearch) {
            foundIdx = mid;
            break;
        } else if (wordList[mid].word < cleanedSearch) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    if (foundIdx != -1) {
        cout << "'" << wordList[foundIdx].word << "' found with frequency: " << wordList[foundIdx].count << endl;
    } else {
        cout << "'" << searchInput << "' not found in 1-star reviews." << endl;
    }
    
    auto end_q3 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_q3 = end_q3 - start_q3;
    cout << "Time taken for word frequency analysis: " << duration_q3.count() << " seconds\n";

    cout << "\nPress Enter to exit..."; 
    cin.get();

    delete[] transactions;
    delete[] reviews;
    delete[] wordList;
    return 0;
}

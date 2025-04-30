#include "LinkedList.hpp"
#include "CustomString.hpp"
#include "Array.hpp"
#include "StringUtils.hpp"
#include "Algorithms.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <stdexcept>
#include <cmath>

// Structure to store transaction data
struct Transaction {
    String customerId;
    String productId;
    double price;
    String date;
    String category;
    String paymentMethod;

    bool operator>(const Transaction& other) const {
        return date > other.date;
    }

    bool operator<(const Transaction& other) const {
        return date < other.date;
    }
};

// Structure to store review data
struct Review {
    String productId;
    String customerId;
    int rating;
    String reviewText;

    bool operator>(const Review& other) const {
        return rating > other.rating;
    }

    bool operator<(const Review& other) const {
        return rating < other.rating;
    }
};

// Structure for word frequency counting
struct WordFreq {
    String word;
    int frequency;

    bool operator>(const WordFreq& other) const {
        return frequency > other.frequency;
    }

    bool operator<(const WordFreq& other) const {
        return frequency < other.frequency;
    }
};

// Helper function to process a single word
void processWord(String& word, LinkedList<WordFreq>& wordFrequencies) {
    word.toLower();
    bool found = false;
    for (auto it = wordFrequencies.begin(); it != wordFrequencies.end(); ++it) {
        if (it->word == word) {
            it->frequency++;
            found = true;
            break;
        }
    }
    if (!found) {
        WordFreq wf;
        wf.word = word;
        wf.frequency = 1;
        wordFrequencies.insert(wf);
    }
}

// Helper function to safely convert string to double
double safeStod(const String& str) {
    try {
        return std::stod(str.c_str());
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: Invalid number format in transaction price: " << str << std::endl;
        return 0.0;
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: Number out of range in transaction price: " << str << std::endl;
        return 0.0;
    }
}

// Helper function to safely convert string to int
int safeStoi(const String& str) {
    try {
        return std::stoi(str.c_str());
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: Invalid number format in review rating: " << str << std::endl;
        return 0;
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: Number out of range in review rating: " << str << std::endl;
        return 0;
    }
}

// Helper function to split customer and product ID
void splitCustomerProduct(const String& combined, String& customerId, String& productId) {
    const char* str = combined.c_str();
    String temp = "";
    bool foundPipe = false;
    
    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] == '|') {
            customerId = temp;
            temp = "";
            foundPipe = true;
        } else {
            char c[2] = {str[i], '\0'};
            temp = temp + String(c);
        }
    }
    if (foundPipe) {
        productId = temp;
    }
}

// Helper function to process transactions
void processTransaction(const StringArray& parts, LinkedList<Transaction>& transactions,
                      int& totalTransactions, int& electronicsCredit, int& totalElectronics) {
    if (parts.getSize() >= 5) {  // Changed from 6 to 5 due to combined customer|product field
        Transaction t;
        String combined = parts[0];
        splitCustomerProduct(combined, t.customerId, t.productId);
        t.category = parts[1];
        t.price = safeStod(parts[2]);
        t.date = parts[3];
        t.paymentMethod = parts[4];
        
        transactions.insert(t);
        totalTransactions++;

        if (t.category == String("Electronics")) {
            totalElectronics++;
            if (t.paymentMethod == String("Credit Card")) {
                electronicsCredit++;
            }
        }
    }
}

// Helper function to process reviews
void processReview(const StringArray& parts, LinkedList<Review>& reviews,
                  LinkedList<WordFreq>& wordFrequencies) {
    if (parts.getSize() >= 4) {
        Review r;
        r.productId = parts[0];
        r.customerId = parts[1];
        r.rating = safeStoi(parts[2]);
        r.reviewText = parts[3];
        
        reviews.insert(r);

        // Process 1-star reviews for word frequency
        if (r.rating == 1) {
            String current;
            const char* cstr = r.reviewText.c_str();
            
            for (size_t i = 0; i < strlen(cstr); i++) {
                if (isspace(cstr[i])) {
                    if (strlen(current.c_str()) > 0) {
                        processWord(current, wordFrequencies);
                        current = String("");
                    }
                } else if (isalpha(cstr[i])) {
                    char temp[2] = {cstr[i], '\0'};
                    current = String(current.c_str()) + String(temp);
                }
            }
            if (strlen(current.c_str()) > 0) {
                processWord(current, wordFrequencies);
            }
        }
    }
}

// Helper function to measure sorting time
template<typename T>
long long measureSortTime(void (*sortFunc)(LinkedList<T>&), LinkedList<T>& list) {
    auto start = std::chrono::high_resolution_clock::now();
    sortFunc(list);
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

// Merge sort for Array<T>
template<typename T>
void mergeSortArray(Array<T>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortArray(arr, left, mid);
        mergeSortArray(arr, mid + 1, right);
        // Merge
        int n1 = mid - left + 1;
        int n2 = right - mid;
        Array<T> L(n1), R(n2);
        for (int i = 0; i < n1; ++i) L.push_back(arr[left + i]);
        for (int j = 0; j < n2; ++j) R.push_back(arr[mid + 1 + j]);
        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (!(L[i] > R[j])) arr[k++] = L[i++];
            else arr[k++] = R[j++];
        }
        while (i < n1) arr[k++] = L[i++];
        while (j < n2) arr[k++] = R[j++];
    }
}

template<typename T>
bool jumpSearchArray(const Array<T>& arr, const T& target) {
    int n = arr.getSize();
    if (n == 0) return false;
    int step = sqrt(n);
    int prev = 0;
    while (prev < n && !(arr[std::min(step, n) - 1] > target) && arr[std::min(step, n) - 1] < target) {
        prev = step;
        step += sqrt(n);
        if (prev >= n) return false;
    }
    while (prev < n && arr[prev] < target) {
        prev++;
        if (prev == std::min(step, n)) return false;
    }
    if (prev < n && !(arr[prev] > target) && !(target > arr[prev])) return true;
    return false;
}

int main() {
    LinkedList<Transaction> transactions;
    LinkedList<Review> reviews;
    LinkedList<WordFreq> wordFrequencies;
    int totalTransactions = 0;
    int electronicsCredit = 0;
    int totalElectronics = 0;

    // Read transactions
    std::ifstream transFile("transactionsClean.csv");
    if (!transFile.is_open()) {
        std::cerr << "Error: Could not open transactionsClean.csv" << std::endl;
        return 1;
    }
    readCSVLine(transFile); // Skip header
    
    while (transFile.good()) {
        auto parts = readCSVLine(transFile);
        if (parts.getSize() > 0) {
            processTransaction(parts, transactions, totalTransactions, electronicsCredit, totalElectronics);
        }
    }

    // Read reviews
    std::ifstream reviewFile("reviewsClean.csv");
    if (!reviewFile.is_open()) {
        std::cerr << "Error: Could not open reviewsClean.csv" << std::endl;
        return 1;
    }
    readCSVLine(reviewFile); // Skip header
    
    while (reviewFile.good()) {
        auto parts = readCSVLine(reviewFile);
        if (parts.getSize() > 0) {
            processReview(parts, reviews, wordFrequencies);
        }
    }

    // 1. Sort transactions by date using merge sort
    std::cout << "\n1. Transaction Analysis:" << std::endl;
    std::cout << "Total number of transactions: " << totalTransactions << std::endl;
    std::cout << "Total number of reviews: " << reviews.getSize() << std::endl;

    LinkedList<Transaction> transactionsCopy = transactions;
    auto startSort = std::chrono::high_resolution_clock::now();
    SortingAlgorithms<Transaction>::mergeSort(transactionsCopy);
    auto endSort = std::chrono::high_resolution_clock::now();
    double mergeSortTime = std::chrono::duration_cast<std::chrono::microseconds>(endSort - startSort).count() / 1e6;

    std::cout << "\nSorting Performance:" << std::endl;
    std::cout << std::fixed << std::setprecision(20);
    std::cout << "Merge Sort (Linked List) time: " << mergeSortTime << " seconds" << std::endl;

    // --- ARRAY VERSION for transactions ---
    Array<Transaction> transactionsArr(transactions.getSize());
    for (auto it = transactions.begin(); it != transactions.end(); ++it) {
        transactionsArr.push_back(*it);
    }
    auto startArrSort = std::chrono::high_resolution_clock::now();
    mergeSortArray(transactionsArr, 0, transactionsArr.getSize() - 1);
    auto endArrSort = std::chrono::high_resolution_clock::now();
    double arrSortTime = std::chrono::duration_cast<std::chrono::microseconds>(endArrSort - startArrSort).count() / 1e6;
    std::cout << "Merge Sort (Array) time: " << arrSortTime << " seconds" << std::endl;

    // 2. Calculate percentage of Electronics purchases made with Credit Card
    std::cout << "\n2. Electronics Category Analysis:" << std::endl;
    
    // Measure sorting time
    auto startSort1 = std::chrono::high_resolution_clock::now();
    SortingAlgorithms<Transaction>::mergeSort(transactions);
    auto endSort1 = std::chrono::high_resolution_clock::now();
    auto sortTime1 = std::chrono::duration_cast<std::chrono::milliseconds>(endSort1 - startSort1).count();
    
    // Count electronics purchases (without timing)
    int electronicsTotal = 0, electronicsCC = 0;
    for (auto it = transactions.begin(); it != transactions.end(); ++it) {
        if (it->category == String("Electronics")) {
            electronicsTotal++;
            if (it->paymentMethod == String("Credit Card")) {
                electronicsCC++;
            }
        }
    }
    
    double percentage = (electronicsTotal > 0) ? 
        (static_cast<double>(electronicsCC) / electronicsTotal) * 100 : 0;
    
    std::cout << "Total Electronics purchases: " << electronicsTotal << std::endl;
    std::cout << "Electronics purchases with Credit Card: " << electronicsCC << std::endl;
    std::cout << "Percentage of Electronics purchases made with Credit Card: " 
              << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;
    
    std::cout << "\nPerformance Metrics:" << std::endl;
    std::cout << "Sorting Time: " << sortTime1 << " milliseconds" << std::endl;

    // --- ARRAY VERSION for stats ---
    Array<Transaction> transactionsArray(transactions.getSize());
    for (auto it = transactions.begin(); it != transactions.end(); ++it) {
        transactionsArray.push_back(*it);
    }
    
    // Measure sorting time for array
    auto startArraySort = std::chrono::high_resolution_clock::now();
    mergeSortArray(transactionsArray, 0, transactionsArray.getSize() - 1);
    auto endArraySort = std::chrono::high_resolution_clock::now();
    double arraySortTime = std::chrono::duration_cast<std::chrono::microseconds>(endArraySort - startArraySort).count() / 1e6;

    // Measure searching time for array
    auto startArraySearch = std::chrono::high_resolution_clock::now();
    int totalElectronicsArray = 0;
    int creditCardElectronicsArray = 0;
    for (int i = 0; i < transactionsArray.getSize(); ++i) {
        if (transactionsArray[i].category == "Electronics") {
            totalElectronicsArray++;
            if (transactionsArray[i].paymentMethod == "Credit Card") {
                creditCardElectronicsArray++;
            }
        }
    }
    auto endArraySearch = std::chrono::high_resolution_clock::now();
    double arraySearchTime = std::chrono::duration_cast<std::chrono::microseconds>(endArraySearch - startArraySearch).count() / 1e6;

    // Calculate percentage for array
    double percentageArray = (totalElectronicsArray > 0) ? (static_cast<double>(creditCardElectronicsArray) / totalElectronicsArray * 100) : 0.0;

    // Display results for array
    std::cout << "\nArray Implementation Results:\n";
    std::cout << "Total Electronics purchases: " << totalElectronicsArray << "\n";
    std::cout << "Electronics purchases with Credit Card: " << creditCardElectronicsArray << "\n";
    std::cout << "Percentage of Electronics purchases made with Credit Card: " << std::fixed << std::setprecision(2) << percentageArray << "%\n";
    std::cout << "Performance Metrics:\n";
    std::cout << "  Sorting time: " << std::fixed << std::setprecision(6) << arraySortTime << " seconds\n";
    std::cout << "  Searching time: " << std::fixed << std::setprecision(6) << arraySearchTime << " seconds\n";
    std::cout << "  Total processing time: " << std::fixed << std::setprecision(6) << (arraySortTime + arraySearchTime) << " seconds\n";

    // 3. Sort and display most frequent words in 1-star reviews with timing
    std::cout << "\n3. Word Frequency Analysis Performance:" << std::endl;
    
    // Sort word frequencies (without timing)
    SortingAlgorithms<WordFreq>::mergeSort(wordFrequencies);
    
    // Measure searching time for word frequencies
    auto startWordSearch = std::chrono::high_resolution_clock::now();
    int count = 0;
    for (auto it = wordFrequencies.begin(); it != wordFrequencies.end(); ++it) {
        if (count++ >= 10) break;  // Show top 10 words
        if (it->word.size() > 2) {  // Filter out very short words
            std::cout << it->word << ": " << it->frequency << " occurrences" << std::endl;
        }
    }
    auto endWordSearch = std::chrono::high_resolution_clock::now();
    auto wordSearchTime = std::chrono::duration_cast<std::chrono::milliseconds>(endWordSearch - startWordSearch).count();
    
    std::cout << "\nPerformance Metrics:" << std::endl;
    std::cout << "Searching Time: " << wordSearchTime << " milliseconds" << std::endl;
    std::cout << "Total Processing Time: " << wordSearchTime << " milliseconds" << std::endl;

    std::cout << "\nPress Enter to exit..."; 
    std::cin.get();
    return 0;
} 
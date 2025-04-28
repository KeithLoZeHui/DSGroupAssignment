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
    auto startStats = std::chrono::high_resolution_clock::now();
    double percentage = (totalElectronics > 0) ? 
        (static_cast<double>(electronicsCredit) / totalElectronics) * 100 : 0;
    auto endStats = std::chrono::high_resolution_clock::now();
    double statsTime = std::chrono::duration_cast<std::chrono::microseconds>(endStats - startStats).count() / 1e6;
    
    std::cout << "\n2. Electronics Category Analysis:" << std::endl;
    std::cout << "Total Electronics purchases: " << totalElectronics << std::endl;
    std::cout << "Electronics purchases with Credit Card: " << electronicsCredit << std::endl;
    std::cout << "Percentage of Electronics purchases made with Credit Card: " 
              << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;
    std::cout << std::fixed << std::setprecision(20);
    std::cout << "Electronics statistics calculation time: " << statsTime << " seconds" << std::endl;

    // --- ARRAY VERSION for stats ---
    auto startArrStats = std::chrono::high_resolution_clock::now();
    int arrTotalElectronics = 0, arrElectronicsCredit = 0;
    for (int i = 0; i < transactionsArr.getSize(); ++i) {
        if (transactionsArr[i].category == String("Electronics")) {
            arrTotalElectronics++;
            if (transactionsArr[i].paymentMethod == String("Credit Card")) {
                arrElectronicsCredit++;
            }
        }
    }
    double arrPercentage = (arrTotalElectronics > 0) ? (static_cast<double>(arrElectronicsCredit) / arrTotalElectronics) * 100 : 0;
    auto endArrStats = std::chrono::high_resolution_clock::now();
    double arrStatsTime = std::chrono::duration_cast<std::chrono::microseconds>(endArrStats - startArrStats).count() / 1e6;
    std::cout << "Electronics stats (Array) calculation time: " << arrStatsTime << " seconds" << std::endl;
    std::cout << "Total Electronics purchases (Array): " << arrTotalElectronics << std::endl;
    std::cout << "Electronics purchases with Credit Card (Array): " << arrElectronicsCredit << std::endl;
    std::cout << "Percentage of Electronics purchases made with Credit Card (Array): " << std::fixed << std::setprecision(2) << arrPercentage << "%" << std::endl;

    // 3. Sort and display most frequent words in 1-star reviews using merge sort
    auto startWordSort = std::chrono::high_resolution_clock::now();
    SortingAlgorithms<WordFreq>::mergeSort(wordFrequencies);
    auto endWordSort = std::chrono::high_resolution_clock::now();
    double wordSortTime = std::chrono::duration_cast<std::chrono::microseconds>(endWordSort - startWordSort).count() / 1e6;
    
    std::cout << "\n3. Most Frequent Words in 1-Star Reviews:" << std::endl;
    int count = 0;
    for (auto it = wordFrequencies.begin(); it != wordFrequencies.end(); ++it) {
        if (count++ >= 10) break;  // Show top 10 words
        if (it->word.size() > 2) {  // Filter out very short words
            std::cout << it->word << ": " << it->frequency << " occurrences" << std::endl;
        }
    }
    std::cout << "Word frequency sorting time: " << wordSortTime << " seconds" << std::endl;

    // --- ARRAY VERSION for word frequency ---
    Array<WordFreq> wordFreqArr(wordFrequencies.getSize());
    for (auto it = wordFrequencies.begin(); it != wordFrequencies.end(); ++it) {
        wordFreqArr.push_back(*it);
    }
    auto startArrWordSort = std::chrono::high_resolution_clock::now();
    mergeSortArray(wordFreqArr, 0, wordFreqArr.getSize() - 1);
    auto endArrWordSort = std::chrono::high_resolution_clock::now();
    double arrWordSortTime = std::chrono::duration_cast<std::chrono::microseconds>(endArrWordSort - startArrWordSort).count() / 1e6;
    std::cout << "Word frequency sorting (Array) time: " << arrWordSortTime << " seconds" << std::endl;
    std::cout << "Most Frequent Words in 1-Star Reviews (Array):" << std::endl;
    int arrCount = 0;
    for (int i = 0; i < wordFreqArr.getSize(); ++i) {
        if (arrCount++ >= 10) break;
        if (wordFreqArr[i].word.size() > 2) {
            std::cout << wordFreqArr[i].word << ": " << wordFreqArr[i].frequency << " occurrences" << std::endl;
        }
    }

    // 4. Demonstrate jump search on sorted transactions
    std::cout << "\n4. Jump Search Demonstration:" << std::endl;
    Transaction searchTarget;
    searchTarget.date = "2023-01-01";  // Example date to search for
    
    bool found = SortingAlgorithms<Transaction>::jumpSearch(transactionsCopy, searchTarget);
    std::cout << "Transaction with date " << searchTarget.date << " was " 
              << (found ? "found" : "not found") << " in the sorted list." << std::endl;

    // --- ARRAY VERSION for jump search ---
    std::cout << "\nJump Search Demonstration (Array):" << std::endl;
    Transaction searchTargetArr;
    searchTargetArr.date = "2023-01-01";
    auto startArrJump = std::chrono::high_resolution_clock::now();
    bool foundArr = jumpSearchArray(transactionsArr, searchTargetArr);
    auto endArrJump = std::chrono::high_resolution_clock::now();
    double arrJumpTime = std::chrono::duration_cast<std::chrono::microseconds>(endArrJump - startArrJump).count() / 1e6;
    std::cout << "Transaction with date " << searchTargetArr.date << " was " << (foundArr ? "found" : "not found") << " in the sorted array." << std::endl;
    std::cout << "Jump Search (Array) time: " << arrJumpTime << " seconds" << std::endl;

    std::cout << "\nPress Enter to exit..."; 
    std::cin.get();
    return 0;
} 
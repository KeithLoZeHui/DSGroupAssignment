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

// Helper function to process transactions
void processTransaction(const StringArray& parts, LinkedList<Transaction>& transactions,
                      int& totalTransactions, int& electronicsCredit, int& totalElectronics) {
    if (parts.getSize() >= 6) {
        Transaction t;
        t.customerId = parts[0];
        t.productId = parts[1];
        t.price = safeStod(parts[2]);
        t.date = parts[3];
        t.category = parts[4];
        t.paymentMethod = parts[5];
        
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

    // 1. Sort transactions by date using quick sort
    std::cout << "\n1. Transaction Analysis:" << std::endl;
    std::cout << "Total number of transactions: " << totalTransactions << std::endl;
    std::cout << "Total number of reviews: " << reviews.getSize() << std::endl;

    LinkedList<Transaction> transactionsCopy = transactions;
    auto quickSortTime = measureSortTime(SortingAlgorithms<Transaction>::quickSort, transactionsCopy);

    std::cout << "\nSorting Performance:" << std::endl;
    std::cout << "Quick Sort time: " << quickSortTime << " milliseconds" << std::endl;

    // 2. Calculate percentage of Electronics purchases made with Credit Card
    double percentage = (totalElectronics > 0) ? 
        (static_cast<double>(electronicsCredit) / totalElectronics) * 100 : 0;
    
    std::cout << "\n2. Electronics Category Analysis:" << std::endl;
    std::cout << "Total Electronics purchases: " << totalElectronics << std::endl;
    std::cout << "Electronics purchases with Credit Card: " << electronicsCredit << std::endl;
    std::cout << "Percentage of Electronics purchases made with Credit Card: " 
              << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;

    // 3. Sort and display most frequent words in 1-star reviews
    SortingAlgorithms<WordFreq>::quickSort(wordFrequencies);
    
    std::cout << "\n3. Most Frequent Words in 1-Star Reviews:" << std::endl;
    int count = 0;
    for (auto it = wordFrequencies.begin(); it != wordFrequencies.end(); ++it) {
        if (count++ >= 10) break;  // Show top 10 words
        if (it->word.size() > 2) {  // Filter out very short words
            std::cout << it->word << ": " << it->frequency << " occurrences" << std::endl;
        }
    }

    std::cout << "\nPress Enter to exit..."; 
    std::cin.get();
    return 0;
} 
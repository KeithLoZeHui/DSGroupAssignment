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
        // Sort by frequency in descending order
        if (frequency != other.frequency) {
            return frequency > other.frequency;
        }
        // If frequencies are equal, sort alphabetically
        return word < other.word;
    }

    bool operator<(const WordFreq& other) const {
        // Sort by frequency in descending order
        if (frequency != other.frequency) {
            return frequency < other.frequency;
        }
        // If frequencies are equal, sort alphabetically
        return word > other.word;
    }
    
    // Equality operator for comparison
    bool operator==(const WordFreq& other) const {
        return word == other.word;
    }
};

// Helper function to process a single word
void processWord(String& word, LinkedList<WordFreq>& wordFrequencies) {
    // Skip empty words or words with non-printable characters
    if (word.size() == 0) return;
    
    // Convert to lowercase
    word.toLower();
    
    // Skip words that are just numbers or special characters
    bool hasLetter = false;
    const char* str = word.c_str();
    for (size_t i = 0; i < strlen(str); i++) {
        if (isalpha(str[i])) {
            hasLetter = true;
            break;
        }
    }
    if (!hasLetter) return;
    
    // Add or update word frequency
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

        // Process only 1-star reviews for word frequency analysis
        if (r.rating == 1) {
            String current = "";
            const char* cstr = r.reviewText.c_str();
            
            for (size_t i = 0; i < strlen(cstr); i++) {
                // If we hit a space or punctuation, process the current word
                if (isspace(cstr[i]) || ispunct(cstr[i])) {
                    if (current.size() > 0) {
                        processWord(current, wordFrequencies);
                        current = "";
                    }
                } 
                // Only add alphanumeric characters to the current word
                else if (isalpha(cstr[i]) || isdigit(cstr[i])) {
                    char c[2] = {cstr[i], '\0'};
                    // Convert uppercase to lowercase
                    if (c[0] >= 'A' && c[0] <= 'Z') {
                        c[0] = c[0] + ('a' - 'A');
                    }
                    current = current + String(c);
                }
            }
            if (strlen(current.c_str()) > 0) {
                processWord(current, wordFrequencies);
            }
        }
    }
}

// Note: The sorting and searching functions have been moved to Algorithms.hpp

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
        String trimmedCategory = it->category.trim();
        String trimmedPayment = it->paymentMethod.trim();
        if (trimmedCategory == String("Electronics")) {
            electronicsTotal++;
            if (trimmedPayment == String("Credit Card")) {
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

    // 3. Word Frequency Analysis with Jump Search timing
    std::cout << "\n3. Word Frequency Analysis Performance:" << std::endl;
    
    // Create a sorted array of word frequencies for easier manipulation
    Array<WordFreq> sortedWords(wordFrequencies.getSize());
    for (auto it = wordFrequencies.begin(); it != wordFrequencies.end(); ++it) {
        sortedWords.push_back(*it);
    }
    
    // Sort the array by frequency in descending order
    for (int i = 0; i < sortedWords.getSize(); i++) {
        for (int j = i + 1; j < sortedWords.getSize(); j++) {
            if (sortedWords[i].frequency < sortedWords[j].frequency) {
                WordFreq temp = sortedWords[i];
                sortedWords[i] = sortedWords[j];
                sortedWords[j] = temp;
            }
        }
    }
    
    // Display the top 10 words
    std::cout << "--- First 10 Words Frequency ---" << std::endl;
    int displayCount = 0;
    for (int i = 0; i < sortedWords.getSize() && displayCount < 10; i++) {
        // Skip words that are too short (less than 3 characters)
        if (sortedWords[i].word.size() < 3) continue;
        
        std::cout << sortedWords[i].word << ": " << sortedWords[i].frequency << " occurrences" << std::endl;
        displayCount++;
    }
    
    // Create a larger list for jump search measurement
    Array<WordFreq> searchArray(10000);
    
    // Fill the array with copies of the sorted words to make it larger
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < sortedWords.getSize(); j++) {
            searchArray.push_back(sortedWords[j]);
        }
    }
    
    // The array is already sorted by frequency, but we need to sort it again for jump search
    // which requires the array to be sorted by the comparison operators
    mergeSortArray(searchArray, 0, searchArray.getSize() - 1);
    
    std::cout << "\nMeasuring jump search time for " << searchArray.getSize() << " items..." << std::endl;
    
    // Get a target to search for (use the most frequent word)
    WordFreq target;
    if (sortedWords.getSize() > 0) {
        target = sortedWords[0];  // Most frequent word
    }
    
    // Measure jump search time
    auto startSearch = std::chrono::high_resolution_clock::now();
    
    // Perform multiple searches to get a measurable time
    bool found = false;
    for (int i = 0; i < 10000; i++) {
        found = jumpSearchArray(searchArray, target);
    }
    
    auto endSearch = std::chrono::high_resolution_clock::now();
    double searchTime = std::chrono::duration_cast<std::chrono::microseconds>(endSearch - startSearch).count() / 1e6;
    
    std::cout << "\nPerformance Metrics:" << std::endl;
    std::cout << "Jump Search Time: " << std::fixed << std::setprecision(6) << searchTime << " seconds" << std::endl;
    std::cout << "Search Result: " << (found ? "Found" : "Not Found") << std::endl;

    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    return 0;
} 
 #include <iostream>        
 #include <fstream>         
 #include <vector>          
 #include <chrono>         
 #include <cmath>          
 #include <stdexcept>       
 #include <iomanip>         
 #include <cstring>        
 #include <string>          
 #include <cctype>          
 #include "LinkedList.hpp"   
 #include "Array.hpp"        
 #include "CustomString.hpp"   
 #include "StringUtils.hpp" 
 
 
 using StringArray = Array<String>;
 
 // Stores transaction data
 struct Transaction {
     String customerId; String productId; double price; String date; String category; String paymentMethod;
     Transaction() : price(0.0) {}
     bool operator>(const Transaction& other) const { return date > other.date; }
     bool operator<(const Transaction& other) const { return date < other.date; }
 };
 
 // Stores review data
 struct Review {
     String productId; String customerId; int rating; String reviewText;
     Review() : rating(0) {}
     bool operator>(const Review& other) const { return rating > other.rating; }
     bool operator<(const Review& other) const { return rating < other.rating; }
 };
 
 // Stores word frequency data
 struct WordFreq {
     String word; int count;
     WordFreq() : count(0) {}
     WordFreq(const String& w, int c) : word(w), count(c) {}
     bool operator>(const WordFreq& other) const { if (count != other.count) return count > other.count; return word < other.word; }
     bool operator<(const WordFreq& other) const { if (count != other.count) return count < other.count; return word > other.word; }
     bool operator==(const WordFreq& other) const { return strcmp(word.c_str(), other.word.c_str()) == 0; }
 };
 
 
 // --- Utility Functions ---
 
 // Safely converts String to double.
 double safeStod(const String& str) {
     if (str.size() == 0 || (str.size() > 0 && std::isspace(str.c_str()[0]))) return 0.0;
     try {
         return std::stod(str.c_str());
     } catch (const std::invalid_argument&) { return 0.0; }
       catch (const std::out_of_range&) { return 0.0; }
 }
 
 // Safely converts String to int.
 int safeStoi(const String& str) {
     if (str.size() == 0 || (str.size() > 0 && std::isspace(str.c_str()[0]))) return 0;
     try {
         return std::stoi(str.c_str());
     } catch (const std::invalid_argument&) { return 0; }
       catch (const std::out_of_range&) { return 0; }
 }
 
 // Splits "CustomerID|ProductID" string.
 void splitCustomerProduct(const String& combined, String& customerId, String& productId) {
     const char* str = combined.c_str(); String temp = ""; bool foundPipe = false; size_t len = strlen(str);
     for (size_t i = 0; i < len; i++) {
         if (str[i] == '|') { customerId = temp; temp = ""; foundPipe = true; }
         else { char c[2] = {str[i], '\0'}; temp = temp + String(c); }
     }
     if (foundPipe) { productId = temp; }
     else { customerId = combined; productId = String(""); }
 }
 
 // Processes a single word for frequency counting.
 void processWord(String& word, LinkedList<WordFreq>& wordFrequencies) {
     if (word.size() == 0) return;
     word.toLower();
     bool hasLetter = false; const char* str = word.c_str(); size_t len = strlen(str);
     for (size_t i = 0; i < len; i++) { if (isalpha(static_cast<unsigned char>(str[i]))) { hasLetter = true; break; } }
     if (!hasLetter) return;
     bool found = false;
     for (auto it = wordFrequencies.begin(); it != wordFrequencies.end(); ++it) {
         if (strcmp(it->word.c_str(), word.c_str()) == 0) { it->count++; found = true; break; }
     }
     if (!found) { wordFrequencies.insert(WordFreq(word, 1)); }
 }
 
 // Processes a transaction data line.
 void processTransaction(const StringArray& parts, LinkedList<Transaction>& transactions,
                         int& electronicsCreditCard, int& electronicsTotal) {
     if (parts.getSize() >= 5) {
         Transaction t; String tempCustId, tempProdId;
         splitCustomerProduct(parts[0], tempCustId, tempProdId);
         t.customerId = tempCustId; t.productId = tempProdId;
         t.category = parts[1]; t.price = safeStod(parts[2]); t.date = parts[3]; t.paymentMethod = parts[4];
         transactions.insert(t);
         String elec("Electronics"); String cc("Credit Card");
         if (strcmp(t.category.c_str(), elec.c_str()) == 0) {
             electronicsTotal++;
             if (strcmp(t.paymentMethod.c_str(), cc.c_str()) == 0) { electronicsCreditCard++; }
         }
     }
 }
 
 // Processes a review data line.
 void processReview(const StringArray& parts, LinkedList<Review>& reviews,
                    LinkedList<WordFreq>& wordFrequencies) {
     if (parts.getSize() >= 4) {
         Review r; r.productId = parts[0]; r.customerId = parts[1];
         r.rating = safeStoi(parts[2]); r.reviewText = parts[3];
         reviews.insert(r);
         if (r.rating == 1) {
             String currentWord = ""; const char* cstr = r.reviewText.c_str(); size_t len = strlen(cstr);
             for (size_t i = 0; i < len; i++) {
                 if (isspace(static_cast<unsigned char>(cstr[i])) || ispunct(static_cast<unsigned char>(cstr[i]))) {
                     if (currentWord.size() > 0) { processWord(currentWord, wordFrequencies); currentWord = ""; }
                 } else if (isalnum(static_cast<unsigned char>(cstr[i]))) {
                     char c[2] = {cstr[i], '\0'}; currentWord = currentWord + String(c);
                 }
             }
              if (currentWord.size() > 0) { processWord(currentWord, wordFrequencies); }
         }
     }
 }
 // Heapify helper for Heap Sort.
 template <typename T>
 void heapify(Array<T>& arr, int n, int i) {
     int largest = i;
     int left = 2 * i + 1;
     int right = 2 * i + 2;
     if (left < n && arr[left] > arr[largest]) largest = left;
     if (right < n && arr[right] > arr[largest]) largest = right;
     if (largest != i) {
         T temp = arr[i]; arr[i] = arr[largest]; arr[largest] = temp;
         heapify(arr, n, largest);
     }
 }
 
 // Heap Sort for Array (ascending).
 template <typename T>
 void heapSortArray(Array<T>& arr) {
     int n = arr.getSize();
     for (int i = n / 2 - 1; i >= 0; i--) { heapify(arr, n, i); }
     for (int i = n - 1; i > 0; i--) { T temp = arr[0]; arr[0] = arr[i]; arr[i] = temp; heapify(arr, i, 0); }
 }
 
 // Converts LinkedList to Array.
 template <typename T>
 Array<T> linkedListToArray(LinkedList<T>& list) {
     Array<T> arr(list.getSize());
     Node<T>* current = list.getHead();
     while (current != nullptr) { arr.push_back(current->data); current = current->next; }
     return arr;
 }
 
 // Updates LinkedList node data from Array.
 template <typename T>
 void updateLinkedListFromArray(LinkedList<T>& list, const Array<T>& arr) {
     if (list.getSize() != arr.getSize()) throw std::runtime_error("List/Array size mismatch");
     Node<T>* current = list.getHead(); int i = 0;
     while (current != nullptr && i < arr.getSize()) { current->data = arr[i]; current = current->next; i++; }
 }
 
 // Sorts LinkedList via array conversion using Heap Sort.
 template <typename T>
 void heapSortLinkedList(LinkedList<T>& list) {
     if (list.getSize() < 2) return;
     Array<T> tempArray = linkedListToArray(list);
     heapSortArray(tempArray);
     updateLinkedListFromArray(list, tempArray);
 }
 
 // --- Linear Search Implementation Definitions ---
 
 // Linear search for Array.
 template <typename T, typename Func>
 int linearSearchArray(const Array<T>& arr, Func matchCriteria) {
     for (int i = 0; i < arr.getSize(); ++i) if (matchCriteria(arr[i])) return i;
     return -1;
 }
 
 // Linear search for LinkedList.
 template <typename T, typename Func>
 Node<T>* linearSearchLinkedList(LinkedList<T>& list, Func matchCriteria) {
     Node<T>* current = list.getHead();
     while (current != nullptr) { if (matchCriteria(current->data)) return current; current = current->next; }
     return nullptr;
 }
 
 
 // ========================== Main Program Execution ==========================
 int main() {
     
     LinkedList<Transaction> transactionList; 
     LinkedList<Review> reviewList;           
     LinkedList<WordFreq> wordFrequencies;  
 
     int electronicsTotalCount = 0;
     int electronicsCreditCardCount = 0;
 
     // --- Load Data ---
     std::cout << "Loading data..." << std::endl;
     std::ifstream transFile("transactionsClean.csv"); 
     std::ifstream reviewFile("reviewsClean.csv");   
 
     // Check if files opened successfully
     if (!transFile.is_open()) {
         std::cerr << "Error opening transactionsClean.csv" << std::endl;
         return 1; 
     }
     if (!reviewFile.is_open()) {
         std::cerr << "Error opening reviewsClean.csv" << std::endl;
         return 1; 
     }
 
     std::string headerLine; 
 
     // --- Load Transactions ---
     // Read and discard the first (header) line
     if (!std::getline(transFile, headerLine)) {
         std::cerr << "Error reading header from transactionsClean.csv or file is empty." << std::endl;
         transFile.close();
         reviewFile.close();
         return 1;
     }
 
     // Loop through the rest of the transaction file line by line
     while (transFile.good() && !transFile.eof()) {
          StringArray parts = readCSVLine(transFile); 
          if (parts.empty() && transFile.eof()) break;
          if (parts.empty()) continue;
          processTransaction(parts, transactionList, electronicsCreditCardCount, electronicsTotalCount);
     }
     transFile.close(); 
     std::cout << "Loaded " << transactionList.getSize() << " transactions." << std::endl;
 
 
     // --- Load Reviews ---
     // Read and discard the first (header) line
      if (!std::getline(reviewFile, headerLine)) {
         std::cerr << "Error reading header from reviewsClean.csv or file is empty." << std::endl;
         reviewFile.close();
         return 1;
      }
 
     // Loop through the rest of the review file line by line
     while (reviewFile.good() && !reviewFile.eof()) {
          StringArray parts = readCSVLine(reviewFile); 
          
           if (parts.empty() && reviewFile.eof()) break;
          if (parts.empty()) continue;
          
          processReview(parts, reviewList, wordFrequencies);
      }
     reviewFile.close();
     std::cout << "Loaded " << reviewList.getSize() << " reviews." << std::endl;
     std::cout << "Processed " << wordFrequencies.getSize() << " unique words from 1-star reviews." << std::endl;
 
 
     // --- Data Analysis ---
     std::cout << "\n--- Analysis ---" << std::endl;
 
 
     // --- Q1: Sort Transactions by Date (Heap Sort) & Count ---
     std::cout << "\n--- Q1: Sorting Transactions by Date (Heap Sort) ---" << std::endl;
     std::cout << "Total transactions loaded: " << transactionList.getSize() << std::endl;
 
     // Create an Array copy of transactions for sorting and Q2 comparison
     Array<Transaction> transactionArray = linkedListToArray(transactionList);
 
     // Time sorting the LinkedList (via array conversion method)
     auto startListSort = std::chrono::high_resolution_clock::now();
     heapSortLinkedList(transactionList); // Sorts the original list
     auto endListSort = std::chrono::high_resolution_clock::now();
     std::chrono::duration<double, std::milli> listSortTime = endListSort - startListSort;
 
     // Time sorting the Array directly
     auto startArraySort = std::chrono::high_resolution_clock::now();
     heapSortArray(transactionArray); // Sorts the array copy
     auto endArraySort = std::chrono::high_resolution_clock::now();
     std::chrono::duration<double, std::milli> arraySortTime = endArraySort - startArraySort;
 
     // Print Q1 timing results
     std::cout << std::fixed << std::setprecision(3); // Use 3 decimal places for ms
     std::cout << "Heap Sort Time (LinkedList): " << listSortTime.count() << " ms" << std::endl;
     std::cout << "Heap Sort Time (Array):                         " << arraySortTime.count() << " ms" << std::endl;
 
     /*// Print ALL sorted transactions from the array to the console
     std::cout << "\n--- All Transactions (Sorted by Date from Array) ---" << std::endl;
     for (int i = 0; i < transactionArray.getSize(); ++i) {
         const Transaction& t = transactionArray[i];
         // Print selected fields for readability
         std::cout << "  Date: " << t.date.c_str()
                   << ", Cat: " << t.category.c_str()
                   << ", Cust: " << t.customerId.c_str()
                   << ", Prod: " << t.productId.c_str()
                   << ", Price: " << std::fixed << std::setprecision(2) << t.price // Format price
                   << ", Payment: " << t.paymentMethod.c_str()
                   << std::endl;
     }
     std::cout << "--- End of Sorted Transactions ---" << std::endl;*/
 
     // Reset precision for subsequent timing outputs
     std::cout << std::fixed << std::setprecision(3);
 
 
     // --- Q2: Electronics Purchases Percentage (Linear Search/Filter) ---
     std::cout << "\n--- Q2: Electronics Purchases with Credit Card ---" << std::endl;
     String electronicsCategory("Electronics"); // Target category string
     String creditCardPayment("Credit Card"); // Target payment method string
     const int Q2_ITERATIONS = 100; // Number of repetitions for timing average
 
     // Time LinkedList search/filter (averaged over Q2_ITERATIONS runs)
     auto startListSearch = std::chrono::high_resolution_clock::now();
     for(int k=0; k < Q2_ITERATIONS; ++k) { 
         Node<Transaction>* current = transactionList.getHead();
         while (current != nullptr) { 
             if (strcmp(current->data.category.c_str(), electronicsCategory.c_str()) == 0) {
                  if (strcmp(current->data.paymentMethod.c_str(), creditCardPayment.c_str()) == 0) { } 
             }
             current = current->next;
         }
     }
     auto endListSearch = std::chrono::high_resolution_clock::now();
     // Calculate average time in microseconds
     std::chrono::duration<double, std::micro> listSearchTime = (endListSearch - startListSearch) / Q2_ITERATIONS;
 
 
     // Time Array search/filter (averaged over Q2_ITERATIONS runs)
     auto startArraySearch = std::chrono::high_resolution_clock::now();
      for(int k=0; k < Q2_ITERATIONS; ++k) { 
         for (int i = 0; i < transactionArray.getSize(); ++i) { 
              if (strcmp(transactionArray[i].category.c_str(), electronicsCategory.c_str()) == 0) {
                 if (strcmp(transactionArray[i].paymentMethod.c_str(), creditCardPayment.c_str()) == 0) { } 
             }
         }
     }
     auto endArraySearch = std::chrono::high_resolution_clock::now();
     // Calculate average time in microseconds
     std::chrono::duration<double, std::micro> arraySearchTime = (endArraySearch - startArraySearch) / Q2_ITERATIONS;
 
 
     // Display Q2 results (using counts gathered during loading)
     // Calculate percentage
     double percentage = (electronicsTotalCount == 0) ? 0.0 : (static_cast<double>(electronicsCreditCardCount) / electronicsTotalCount) * 100.0;
     // Print counts
     std::cout << "Total 'Electronics' purchases found: " << electronicsTotalCount << std::endl;
     std::cout << "'Electronics' purchases using 'Credit Card': " << electronicsCreditCardCount << std::endl;
     // Print percentage
     std::cout << std::fixed << std::setprecision(2); // Set precision for percentage
     std::cout << "Percentage of Electronics purchases made with Credit Card: " << percentage << "%" << std::endl;
     // Print timings (microseconds)
     std::cout << std::fixed << std::setprecision(3); // Set precision for microseconds
     std::cout << "Linear Filter Avg Time (LinkedList): " << listSearchTime.count() << " us" << std::endl;
     std::cout << "Linear Filter Avg Time (Array):      " << arraySearchTime.count() << " us" << std::endl;
 
 
     // --- Q3: Frequent Words in 1-Star Reviews (Heap Sort) ---
     std::cout << "\n--- Q3: Most Frequent Words in 1-Star Reviews (FILTERED - Heap Sort) ---" << std::endl;
 
     // Define list of common words to ignore in output
     const char* stopWordsArr[] = {
         "a", "an", "and", "the", "in", "on", "at", "to", "for", "of",
         "it", "is", "am", "are", "was", "were", "be", "being", "been",
         "i", "you", "he", "she", "we", "they", "me", "him", "her", "us", "them",
         "my", "your", "his", "its", "our", "their", "mine", "yours", "hers", "ours", "theirs",
         "this", "that", "these", "those", "with", "by", "from", "as", "but", "or",
         "not", "no", "so", "very", "too", "just", "also", "have", "has", "had",
         "do", "does", "did", "product", "item", "review"
         // Add more words if needed
     };
     const int numStopWords = sizeof(stopWordsArr) / sizeof(stopWordsArr[0]);
 
     // Check if any words were actually processed
     if (wordFrequencies.getSize() == 0) {
         std::cout << "No 1-star reviews found or no words extracted." << std::endl;
     } else {
          // Create Array version for sorting comparison
          Array<WordFreq> wordArray = linkedListToArray(wordFrequencies);
 
         // Time LinkedList sort (single run, microseconds)
         auto startListWordSort = std::chrono::high_resolution_clock::now();
         heapSortLinkedList(wordFrequencies); // Sorts original list
         auto endListWordSort = std::chrono::high_resolution_clock::now();
         std::chrono::duration<double, std::micro> listWordSortTime = endListWordSort - startListWordSort;
 
          // Time Array sort (single run, microseconds)
          auto startArrayWordSort = std::chrono::high_resolution_clock::now();
          heapSortArray(wordArray); // Sorts the array copy
          auto endArrayWordSort = std::chrono::high_resolution_clock::now();
          std::chrono::duration<double, std::micro> arrayWordSortTime = endArrayWordSort - startArrayWordSort;
 
          // Print Q3 timing results (microseconds)
          std::cout << std::fixed << std::setprecision(3);
          std::cout << "Heap Sort Time (Word Freq LinkedList): " << listWordSortTime.count() << " us" << std::endl;
          std::cout << "Heap Sort Time (Word Freq Array):      " << arrayWordSortTime.count() << " us" << std::endl;
 
          // Display top N non-stop words
          int topN = 10;          
          int displayedCount = 0; 
          std::cout << "\nTop " << topN << " MOST FREQUENT  words in 1-star reviews:" << std::endl;
          for (int i = wordArray.getSize() - 1; i >= 0 && displayedCount < topN; --i) {
               bool isStopWord = false; 
               for (int j = 0; j < numStopWords; ++j) {
                   if (strcmp(wordArray[i].word.c_str(), stopWordsArr[j]) == 0) {
                       isStopWord = true; 
                       break;             
                   }
               }
               if (!isStopWord) {
                   displayedCount++; 
                   std::cout << "  " << displayedCount << ". \"" << wordArray[i].word.c_str()
                             << "\" (" << wordArray[i].count << " times)" << std::endl;
               }
               
          }
          // Print message if fewer than N words found
          if (displayedCount == 0) {
              std::cout << "  No non-common frequent words found meeting criteria." << std::endl;
          } else if (displayedCount < topN) {
              std::cout << "  (Found only " << displayedCount << " non-common words)" << std::endl;
          }
     }
 
     // --- End of Program ---
     std::cout << "\nAnalysis complete." << std::endl;
     return 0; // Indicate successful execution
 }
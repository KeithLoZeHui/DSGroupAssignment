#include <fstream>
#include <sstream>
#include <ctime>
#include <iostream>
#include "Structure.hpp"
#include <algorithm>
#include <cctype>
using namespace std;

// Function Prototypes


// Load structures from files
void loadTransactions(const string& filename, Array<Transaction>& transactions);
void loadTransactions(const string& filename, LinkedList<Transaction>& transactions);
void loadReviews(const string& filename, Array<Review>& reviews);
void loadReviews(const string& filename, LinkedList<Review>& reviews);

//Sort Q1
void insertionSortByDate(Array<Transaction>& arr);
void insertionSortByDate(LinkedList<Transaction>& list);

//Search Q2
void sentinelLinearSearch(Array<Transaction>& arr);
void sentinelLinearSearch(LinkedList<Transaction>& list);

//Sort and Search Q3
void sentinelLinearSearch1StarReviews(Array<Review>& reviews, Array<string>& words);
void insertionSortWords(Array<string>& words);
void sentinelLinearSearch1StarReviews(LinkedList<Review>& reviews, LinkedList<string>& words);
void insertionSortWords(LinkedList<string>& words);

int main(){
    Array<Transaction> transactions;
    Array<Review> reviews;
    LinkedList<Transaction> transactionsLinkedList;
    LinkedList<Review> reviewsLinkedList;

    // Q1 - Sorting Transactions

    cout << "========== DATA LOADING ==========" << endl;
    loadTransactions("transactionsClean.csv", transactions);
    loadReviews("reviewsClean.csv", reviews);
    loadTransactions("transactionsClean.csv", transactionsLinkedList);
    loadReviews("reviewsClean.csv", reviewsLinkedList);
    cout << "Loaded " << transactions.getSize() << " transactions (Array)." << endl;
    cout << "Loaded " << reviews.getSize() << " reviews (Array)." << endl;
    cout << "Loaded " << transactionsLinkedList.getSize() << " transactions (LinkedList)." << endl;
    cout << "Loaded " << reviewsLinkedList.getSize() << " reviews (LinkedList)." << endl;

    cout << "\n========== QUESTION 1: Sort by Date ==========" << endl;
    clock_t start = clock();
    insertionSortByDate(transactions);
    clock_t end = clock();
    double arraySortTime = double(end - start) / CLOCKS_PER_SEC;

    start = clock();
    insertionSortByDate(transactionsLinkedList);
    end = clock();
    double linkedListSortTime = double(end - start) / CLOCKS_PER_SEC;

    cout << "First 5 sorted transactions (Array):" << endl;
    for(int i = 0; i < 5 && i < transactions.getSize(); i++){
        cout << transactions.get(i).customerID << " | "
             << transactions.get(i).product << " | "
             << transactions.get(i).date << endl;
    }
    cout << "Array Insertion Sort Time: " << arraySortTime << " seconds" << endl;
    cout << "Linked List Insertion Sort Time: " << linkedListSortTime << " seconds" << endl;

    // Q2 - Search Electronics paid with Credit Card
    cout << "\n========== QUESTION 2: Electronics Paid by Credit Card ==========" << endl;

    cout << "[Array Version]" << endl;
    start = clock();
    sentinelLinearSearch(transactions);
    end = clock();
    cout << "Array Search Time: " << double(end - start) / CLOCKS_PER_SEC << " seconds" << endl;

    cout << "\n[Linked List Version]" << endl;
    start = clock();
    sentinelLinearSearch(transactionsLinkedList);
    end = clock();
    cout << "Linked List Search Time: " << double(end - start) / CLOCKS_PER_SEC << " seconds" << endl;

    // Q3 - Most common words in 1-star reviews
    cout << "\n========== QUESTION 3: Common Words in 1-Star Reviews ==========" << endl;
    Array<string> OneStarWords;
    start = clock();
    sentinelLinearSearch1StarReviews(reviews, OneStarWords);
    insertionSortWords(OneStarWords);
    end = clock();
    cout << "Array 1-Star Reviews Search and Sort Time: " << double(end - start) / CLOCKS_PER_SEC << " seconds" << endl;

    LinkedList<string> OneStarWordsLinkedList;
    start = clock();
    sentinelLinearSearch1StarReviews(reviewsLinkedList, OneStarWordsLinkedList);
    insertionSortWords(OneStarWordsLinkedList);
    end = clock();
    cout << "Linked List 1-Star Reviews Search and Sort Time: " << double(end - start) / CLOCKS_PER_SEC << " seconds" << endl;

    cout << "\n===============================================" << endl;
    return 0;  
}

void loadTransactions(const string& filename, Array<Transaction>& transactions){
    ifstream file(filename);

    if(!file.is_open()){
        cout << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    getline(file, line); // Skip header line

    while(getline(file, line)){
        stringstream ss(line);
        string customerID, product, priceStr, date, category, paymentMethod;

        getline(ss, customerID, '|');
        getline(ss, product, ',');
        getline(ss, category, ',');
        getline(ss, priceStr, ',');
        getline(ss, date, ',');
        getline(ss, paymentMethod, ',');

        Transaction t;
        t.customerID = customerID;
        t.product = product;
        t.category = category;
        t.price = stod(priceStr); // Convert string to double
        t.date = date;
        t.paymentMethod = paymentMethod;

        transactions.add(t);
    }

    file.close();
}

void loadTransactions(const string& filename, LinkedList<Transaction>& transactions){
    ifstream file(filename);

    if(!file.is_open()){
        cout << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    getline(file, line); // Skip header line

    while(getline(file, line)){
        stringstream ss(line);
        string customerID, product, priceStr, date, category, paymentMethod;

        getline(ss, customerID, '|');
        getline(ss, product, ',');
        getline(ss, category, ',');
        getline(ss, priceStr, ',');
        getline(ss, date, ',');
        getline(ss, paymentMethod, ',');

        Transaction t;
        t.customerID = customerID;
        t.product = product;
        t.category = category;
        t.price = stod(priceStr); // Convert string to double
        t.date = date;
        t.paymentMethod = paymentMethod;

        transactions.add(t);
    }

    file.close();
}

void loadReviews(const string& filename, Array<Review>& reviews){
    ifstream file(filename);

    if(!file.is_open()){
        cout << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    getline(file, line); // Skip header line

    while (getline(file, line)){
        stringstream ss(line);
        string productID, customerID, ratingStr, reviewText;

        getline(ss, productID, ',');
        getline(ss, customerID, ',');
        getline(ss, ratingStr, ',');
        getline(ss, reviewText, ',');

        Review r;
        r.productID = productID;
        r.customerID = customerID;
        r.rating = stoi(ratingStr); // Convert string to int
        r.reviewText = reviewText;

        reviews.add(r);
    }

    file.close();
}

void loadReviews(const string& filename, LinkedList<Review>& reviews){
    ifstream file(filename);

    if(!file.is_open()){
        cout << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    getline(file, line); // Skip header line

    while (getline(file, line)){
        stringstream ss(line);
        string productID, customerID, ratingStr, reviewText;

        getline(ss, productID, ',');
        getline(ss, customerID, ',');
        getline(ss, ratingStr, ',');
        getline(ss, reviewText, ',');

        Review r;
        r.productID = productID;
        r.customerID = customerID;
        r.rating = stoi(ratingStr); // Convert string to int
        r.reviewText = reviewText;

        reviews.add(r);
    }

    file.close();
}

void insertionSortByDate(Array<Transaction>& arr){
    int n = arr.getSize();

    for(int i = 1; i < n; i++){
        Transaction key = arr.get(i);
        int j = i - 1;

        //Move elements greater than key.date to one position ahead
        while(j>= 0 && arr.get(j).date > key.date){
            arr.get(j + 1) = arr.get(j);
            j--;
        }
        arr.get(j + 1) = key;
    }
}

void insertionSortByDate(LinkedList<Transaction>& list){
    using Node = LinkedList<Transaction>::Node;

    Node* sorted = nullptr; // Start with an empty sorted list
    Node* current = list.getHead();

    while(current != nullptr){
        Node* next = current->next; // Store next before changing current->next
        current->next = nullptr;

        // Insert current into sorted list at the correct position
        if(!sorted || current->data.date < sorted->data.date){
            // Insert at head
            current->next = sorted;
            sorted = current;
        }else{
            Node* temp = sorted;
            while(temp->next != nullptr && temp->next->data.date <current->data.date){
                temp = temp->next;
            }
            current->next = temp->next;
            temp->next = current;
        }
        current = next; // Move to the next node in the original list
    }

    // Update the original list's head and tail
    list.setHead(sorted);
}

void sentinelLinearSearch(Array<Transaction>& arr){
    int n = arr.getSize();

    Transaction sentinel;
    sentinel.category = "Electronics";
    sentinel.paymentMethod = "Credit Card";
    arr.add(sentinel);

    int totalElectronics = 0;
    int creditCardElectronics = 0;

    int i = 0;
    for (int i = 0; i < n; i++) { // 🔁 only iterate up to n
        if (arr.get(i).category == "Electronics") {
            totalElectronics++;
            if (arr.get(i).paymentMethod == "Credit Card") {
                creditCardElectronics++;
            }
        }
    }

    arr.removeLast(); // Remove the sentinel

    if(totalElectronics == 0){
        cout << "No Electronics transactions found." << endl;
        return;
    }

    double percent = (static_cast<double>(creditCardElectronics) / totalElectronics) * 100;

    cout << "Total Electronics transactions: " << totalElectronics << endl;
    cout << "Total Electronics transactions paid by Credit Card: " << creditCardElectronics << endl;
    cout << "Percentage of Electronics transactions paid by Credit Card: " << percent << "%" << endl;

}

void sentinelLinearSearch(LinkedList<Transaction>& list){
    using Node = LinkedList<Transaction>::Node;

    int originalSize = list.getSize();

    // Create a sentinel transaction
    Transaction sentinel;
    sentinel.category = "Electronics";
    sentinel.paymentMethod = "Credit Card";
    list.add(sentinel);

    int totalElectronics = 0;
    int creditCardElectronics = 0;

    Node* current = list.getHead();
    int index = 0;

    while(current != nullptr){
        if(index == originalSize) break; // Stop before processing the sentinel

        if(current->data.category == "Electronics"){
            totalElectronics++;

            if(current->data.paymentMethod == "Credit Card"){
                creditCardElectronics++;
            }
        }
        current = current->next;
        index++;
    }
    list.removeLast();

    if(totalElectronics == 0){
        cout << "No Electronics transactions found." << endl;
        return;
    }

    double percent = (static_cast<double>(creditCardElectronics) / totalElectronics) * 100.0;

    cout << "Total Electronics transactions: " << totalElectronics << endl;
    cout << "Total Electronics paid by Credit Card: " << creditCardElectronics << endl;
    cout << "Percentage: " << percent << "%" << endl;
}

void sentinelLinearSearch1StarReviews(Array<Review>& reviews, Array<string>& words){
    int n = reviews.getSize();

    // Add a sentinel
    Review sentinel;
    sentinel.rating = 1;
    sentinel.reviewText = ""; // Left empty as it will be counted
    reviews.add(sentinel);

    int i = 0;
    while(true){
        if (reviews.get(i).rating == 1){
            stringstream ss(reviews.get(i).reviewText);
            string word;

            while(ss >> word){
                words.add(word); // Add each word to the array
            }
        }

        if(i == n) break;
        i++;
    }

    reviews.removeLast();
}

struct WordFrequency{
    string word;
    int count;
};

void insertionSortWords(Array<string>& words){
    Array<WordFrequency> frequencies;

    for(int i = 0; i < words.getSize(); i++){
        string word = words.get(i);
        bool found = false;

        for(int j = 0; j < frequencies.getSize(); j++){
            if(frequencies.get(j).word == word){
                frequencies.get(j).count++;
                found = true;
                break;
            }
        }

        if(!found){
            WordFrequency wf = {word, 1};
            frequencies.add(wf);
        }
    }

    // Insertion sort
    for(int i = 1; i < frequencies.getSize(); i++){
        WordFrequency key = frequencies.get(i);
        int j = i - 1;

        while(j >= 0 && frequencies.get(j).count < key.count){
            frequencies.get(j + 1) = frequencies.get(j);
            j--;
        }
        frequencies.get(j + 1) = key;
    }

    // Display top 10
    cout << "\nTop 10 words in 1-star reviews:" << endl;
    for(int i = 0; i < 10 && i < frequencies.getSize(); i++){
        cout << frequencies.get(i).word << ": " << frequencies.get(i).count << endl;
    }
}

void sentinelLinearSearch1StarReviews(LinkedList<Review>& reviews, LinkedList<string>& words){
    using Node = LinkedList<Review>::Node;
    Node* current = reviews.getHead();

    while(current != nullptr){
        if(current->data.rating == 1){
            stringstream ss(current->data.reviewText);
            string word;

            while(ss >> word){
                words.add(word); // Add each word to the linked list
            }
        }
        current = current->next;
    }
}

void insertionSortWords(LinkedList<string>& words){
    LinkedList<WordFrequency> frequencies;

    using WordNode = LinkedList<string>::Node;
    WordNode* current = words.getHead();

    // Count word frequencies
    while(current != nullptr){
        string word = current->data;
        bool found = false;

        using FreqNode = LinkedList<WordFrequency>::Node;
        FreqNode* freqNode = frequencies.getHead();

        while(freqNode != nullptr){
            if(freqNode->data.word == word){
                freqNode->data.count++;
                found = true;
                break;
            }
            freqNode = freqNode->next;
        }

        if(!found){
            WordFrequency wf = {word, 1};
            frequencies.add(wf);
        }
        
        current = current->next;
    
    }

    // Insertion sort
    using Node = LinkedList<WordFrequency>::Node;
    Node* sorted = nullptr;
    Node* unsorted = frequencies.getHead();

    while(unsorted != nullptr){
        Node* next = unsorted->next; // Store next before changing unsorted->next
        unsorted->next = nullptr;

        // Insert unsorted into sorted list at the correct position
        if(!sorted || unsorted->data.count > sorted->data.count){
            unsorted->next = sorted;
            sorted = unsorted;
        }else{
            Node* temp = sorted;
            while(temp->next && temp->next->data.count > unsorted->data.count){
                temp = temp->next;
            }
            unsorted->next = temp->next;
            temp->next = unsorted;
        }
        unsorted = next;
    }

    // Update the original list's head and tail
    frequencies.setHead(sorted);

    // Display top 10
    cout << "\nTop 10 words in 1-star reviews:" << endl;
    Node* top = frequencies.getHead();
    int count = 0;

    while(top != nullptr && count < 10){
        cout << top->data.word << ": " << top->data.count << endl;
        top = top->next;
        count++;
    }
}
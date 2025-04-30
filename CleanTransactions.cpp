#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cctype>
#include "CleanUtils.h"

using namespace std;

struct Transaction {
    string customerID, product, date, category, paymentMethod;
    float price;
};

struct TransactionNode {
    Transaction data;
    TransactionNode* next;
    TransactionNode(const Transaction& t) : data(t), next(nullptr) {}
};

class TransactionList {
private:
    TransactionNode* head;
    int count;

public:
    TransactionList() : head(nullptr), count(0) {}
    
    ~TransactionList() {
        TransactionNode* current = head;
        while (current != nullptr) {
            TransactionNode* next = current->next;
            delete current;
            current = next;
        }
    }

    void add(const Transaction& t) {
        TransactionNode* newNode = new TransactionNode(t);
        newNode->next = head;
        head = newNode;
        count++;
    }

    int getCount() const { return count; }

    void toArray(Transaction*& arr) {
        arr = new Transaction[count];
        TransactionNode* current = head;
        int i = count - 1;
        while (current != nullptr) {
            arr[i] = current->data;
            current = current->next;
            i--;
        }
    }
};

bool isValidFloat(const string &str) {
    stringstream ss(str);
    float f;
    return (ss >> f) && (ss.eof());
}

bool isValidDate(const string &date) {
    if(date.length() != 10) return false;
    if(date[2] != '/' || date[5] != '/') return false;
    
    string dayStr = date.substr(0,2);
    string monthStr = date.substr(3,2);
    string yearStr = date.substr(6,4);
    
    if(!isValidInteger(dayStr) || !isValidInteger(monthStr) || !isValidInteger(yearStr))
        return false;
        
    int day = stoi(dayStr);
    int month = stoi(monthStr);
    int year = stoi(yearStr);
    
    return (year >= 2000 && year <= 2024) && 
           (month >= 1 && month <= 12) && 
           (day >= 1 && day <= 31);
}

bool isValidCategory(const string &category) {
    string validCategories[] = {
        "Electronics", "Fashion", "Books", "Automotive", "Beauty",
        "Sports", "Toys", "Furniture", "Groceries", "Home Appliances"
    };
    string cat = toLowerCase(category);
    for(const string &valid : validCategories) {
        if(toLowerCase(valid) == cat) return true;
    }
    return false;
}

bool isValidPaymentMethod(const string &method) {
    string validMethods[] = {
        "Credit Card", "Debit Card", "Cash", "PayPal",
        "Bank Transfer", "Cash on Delivery"
    };
    string m = toLowerCase(method);
    for(const string &valid : validMethods) {
        if(toLowerCase(valid) == m) return true;
    }
    return false;
}

int cleanTransactions(Transaction *&transactions, int &size) {
    ifstream inFile("data/transactions.csv");
    if (!inFile.is_open()) {
        cout << "Error: Cannot open data/transactions.csv\n";
        return 0;
    }

    ofstream outFile("data/transactionsClean.csv");
    if (!outFile.is_open()) {
        cout << "Error: Cannot create data/transactionsClean.csv\n";
        inFile.close();
        return 0;
    }

    string line;
    getline(inFile, line);
    outFile << "Customer|Product,Category,Price,Date,Payment Method" << endl;

    TransactionList validTransactions;
    int lineNumber = 1;

    while (getline(inFile, line)) {
        lineNumber++;
        stringstream ss(line);
        string customerID, product, category, priceStr, date, paymentMethod;
        
        getline(ss, customerID, ',');
        getline(ss, product, ',');
        getline(ss, category, ',');
        getline(ss, priceStr, ',');
        getline(ss, date, ',');
        getline(ss, paymentMethod);

        bool isValid = true;

        if (customerID.empty() || product.empty() || category.empty() || 
            priceStr.empty() || date.empty() || paymentMethod.empty()) {
            cout << "Line " << lineNumber << ": Missing required field(s)\n";
            isValid = false;
        }

        if (!isValidFloat(priceStr)) {
            cout << "Line " << lineNumber << ": Invalid price format\n";
            isValid = false;
        } else {
            float price = stof(priceStr);
            if (price <= 0) {
                cout << "Line " << lineNumber << ": Price must be positive\n";
                isValid = false;
            }
        }

        if (!isValidDate(date)) {
            cout << "Line " << lineNumber << ": Invalid date format (use MM/DD/YYYY)\n";
            isValid = false;
        }

        if (!isValidCategory(category)) {
            cout << "Line " << lineNumber << ": Invalid category\n";
            isValid = false;
        }

        if (!isValidPaymentMethod(paymentMethod)) {
            cout << "Line " << lineNumber << ": Invalid payment method\n";
            isValid = false;
        }

        if (!isValid) continue;

        Transaction t;
        t.customerID = customerID;
        t.product = product;
        t.category = category;
        t.price = stof(priceStr);
        t.date = date;
        t.paymentMethod = paymentMethod;
        validTransactions.add(t);

        outFile << customerID << "|" << product << ","
               << category << "," << priceStr << "," 
               << date << "," << paymentMethod << endl;
    }

    inFile.close();
    outFile.close();

    cout << "Cleaned transactions saved to data/transactionsClean.csv\n";

    size = validTransactions.getCount();
    validTransactions.toArray(transactions);

    return size;
}

int main() {
    Transaction *transactions = nullptr;
    int transSize = 0;

    cout << "Cleaning transactions...\n";
    cleanTransactions(transactions, transSize);
    cout << "Loaded " << transSize << " valid transactions.\n";

    delete[] transactions;
    return 0;
}

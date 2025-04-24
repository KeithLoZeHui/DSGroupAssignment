#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include "CustomString.hpp"

struct Transaction {
    MyString customerID, product, date, category, paymentMethod;
    double price;
    
    Transaction() : price(0.0) {}
    
    Transaction(const MyString& cid, const MyString& p, double pr, 
               const MyString& d, const MyString& c, const MyString& pm)
        : customerID(cid), product(p), price(pr), date(d), category(c), paymentMethod(pm) {}
};

#endif // TRANSACTION_HPP

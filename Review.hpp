#ifndef REVIEW_HPP
#define REVIEW_HPP

#include "CustomString.hpp"

struct Review {
    MyString productID, customerID, reviewText;
    int rating;
    
    Review() : rating(0) {}
    
    Review(const MyString& pid, const MyString& cid, const MyString& text, int r)
        : productID(pid), customerID(cid), reviewText(text), rating(r) {}
};

struct WordFreq {
    MyString word;
    int count;
    
    WordFreq() : count(0) {}
    WordFreq(const MyString& w, int c) : word(w), count(c) {}
};

#endif // REVIEW_HPP

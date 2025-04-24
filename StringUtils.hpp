#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include "CustomString.hpp"
#include "Array.hpp"
#include <fstream>
#include <sstream>

// Type alias for String array
using StringArray = Array<String>;

// Function to split string by delimiter
inline StringArray split(const String& str, char delimiter) {
    StringArray tokens;
    const char* cstr = str.c_str();
    size_t start = 0;
    size_t end = 0;
    
    while (end < strlen(cstr)) {
        if (cstr[end] == delimiter) {
            if (end > start) {
                tokens.push_back(String(cstr + start, end - start));
            }
            start = end + 1;
        }
        end++;
    }
    
    if (end > start) {
        tokens.push_back(String(cstr + start, end - start));
    }
    
    return tokens;
}

// Helper function to read CSV line
inline StringArray readCSVLine(std::ifstream& file) {
    if (!file.good()) {
        return StringArray();
    }

    char buffer[1024];
    if (file.getline(buffer, 1024)) {
        return split(String(buffer), ',');
    }
    return StringArray();
}

#endif // STRING_UTILS_HPP 
#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include "CustomString.hpp"
#include "Array.hpp"
#include <fstream>

// Type alias for String array
using StringArray = Array<String>;

// Function to split string by delimiter
inline StringArray split(const String& str, char delimiter) {
    StringArray tokens;
    String current;
    const char* cstr = str.c_str();
    
    for (size_t i = 0; i < strlen(cstr); i++) {
        if (cstr[i] == delimiter) {
            if (strlen(current.c_str()) > 0) {
                tokens.push_back(current);
                current = String("");
            }
        } else {
            char temp[2] = {cstr[i], '\0'};
            current = String(current.c_str()) + String(temp);
        }
    }
    if (strlen(current.c_str()) > 0) {
        tokens.push_back(current);
    }
    return tokens;
}

// Helper function to read CSV line
inline StringArray readCSVLine(std::ifstream& file) {
    char buffer[1024];
    if (file.getline(buffer, 1024)) {
        return split(String(buffer), ',');
    }
    return StringArray();
}

#endif // STRING_UTILS_HPP 
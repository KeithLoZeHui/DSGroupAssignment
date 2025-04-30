#ifndef CLEANUTILS_H
#define CLEANUTILS_H

#include <string>
#include <cctype>

inline std::string toLowerCase(std::string str) {
    for (char &c : str) c = std::tolower(c);
    return str;
}

inline bool isValidInteger(const std::string &str) {
    if (str.empty()) return false;
    for(char c : str) {
        if(!std::isdigit(c)) return false;
    }
    return true;
}

#endif // CLEANUTILS_H

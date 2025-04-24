#ifndef CUSTOM_STRING_HPP
#define CUSTOM_STRING_HPP

#include <cstring>
#include <iostream>

class String {
private:
    char* data;
    size_t length;

public:
    String() : data(new char[1]), length(0) {
        data[0] = '\0';
    }

    String(const char* str) {
        length = strlen(str);
        data = new char[length + 1];
        strcpy(data, str);
    }

    String(const String& other) {
        length = other.length;
        data = new char[length + 1];
        strcpy(data, other.data);
    }

    ~String() {
        delete[] data;
    }

    String& operator=(const String& other) {
        if (this != &other) {
            delete[] data;
            length = other.length;
            data = new char[length + 1];
            strcpy(data, other.data);
        }
        return *this;
    }

    String operator+(const String& other) const {
        size_t newLength = length + other.length;
        char* newData = new char[newLength + 1];
        strcpy(newData, data);
        strcat(newData, other.data);
        String result;
        delete[] result.data;
        result.data = newData;
        result.length = newLength;
        return result;
    }

    bool operator==(const String& other) const {
        return strcmp(data, other.data) == 0;
    }

    bool operator>(const String& other) const {
        return strcmp(data, other.data) > 0;
    }

    bool operator<(const String& other) const {
        return strcmp(data, other.data) < 0;
    }

    char& operator[](size_t index) {
        return data[index];
    }

    const char& operator[](size_t index) const {
        return data[index];
    }

    size_t size() const {
        return length;
    }

    const char* c_str() const {
        return data;
    }

    void toLower() {
        for (size_t i = 0; i < length; i++) {
            if (data[i] >= 'A' && data[i] <= 'Z') {
                data[i] = data[i] + ('a' - 'A');
            }
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const String& str) {
        os << str.data;
        return os;
    }

    friend std::istream& operator>>(std::istream& is, String& str) {
        char buffer[1024];
        is >> buffer;
        delete[] str.data;
        str.length = strlen(buffer);
        str.data = new char[str.length + 1];
        strcpy(str.data, buffer);
        return is;
    }
};

#endif // CUSTOM_STRING_HPP 
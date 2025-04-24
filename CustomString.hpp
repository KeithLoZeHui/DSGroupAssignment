#ifndef CUSTOM_STRING_HPP
#define CUSTOM_STRING_HPP

#include <cstring>
#include <iostream>
#include <cctype>

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

// Custom string implementation using linked list
struct CharNode {
    char data;
    CharNode* next;
    CharNode(char c) : data(c), next(nullptr) {}
};

class MyString {
    friend MyString cleanWord(const MyString& word);
    friend void processReviewText(const MyString& text);
    CharNode* head;
    
    public:
    MyString() : head(nullptr) {}
    
    MyString(const char* str) : head(nullptr) {
        if (!str) return;
        CharNode* current = nullptr;
        while (*str) {
            CharNode* newNode = new CharNode(*str);
            if (!head) {
                head = newNode;
                current = head;
            } else {
                current->next = newNode;
                current = newNode;
            }
            str++;
        }
    }
    
    MyString(const MyString& other) : head(nullptr) {
        CharNode* otherCurrent = other.head;
        CharNode* current = nullptr;
        while (otherCurrent) {
            CharNode* newNode = new CharNode(otherCurrent->data);
            if (!head) {
                head = newNode;
                current = head;
            } else {
                current->next = newNode;
                current = newNode;
            }
            otherCurrent = otherCurrent->next;
        }
    }
    
    ~MyString() {
        while (head) {
            CharNode* temp = head;
            head = head->next;
            delete temp;
        }
    }
    
    MyString& operator=(const MyString& other) {
        if (this != &other) {
            this->~MyString();
            head = nullptr;
            CharNode* otherCurrent = other.head;
            CharNode* current = nullptr;
            while (otherCurrent) {
                CharNode* newNode = new CharNode(otherCurrent->data);
                if (!head) {
                    head = newNode;
                    current = head;
                } else {
                    current->next = newNode;
                    current = newNode;
                }
                otherCurrent = otherCurrent->next;
            }
        }
        return *this;
    }
    
    bool operator==(const MyString& other) const {
        CharNode* curr1 = head;
        CharNode* curr2 = other.head;
        while (curr1 && curr2) {
            if (curr1->data != curr2->data) return false;
            curr1 = curr1->next;
            curr2 = curr2->next;
        }
        return curr1 == curr2; // both should be nullptr
    }
    
    bool operator<(const MyString& other) const {
        CharNode* curr1 = head;
        CharNode* curr2 = other.head;
        while (curr1 && curr2) {
            if (curr1->data != curr2->data)
                return curr1->data < curr2->data;
            curr1 = curr1->next;
            curr2 = curr2->next;
        }
        return !curr1 && curr2; // shorter string is less
    }
    
    bool operator>(const MyString& other) const {
        return other < *this;
    }
    
    friend std::ostream& operator<<(std::ostream& os, const MyString& str) {
        CharNode* current = str.head;
        while (current) {
            os << current->data;
            current = current->next;
        }
        return os;
    }
    
    friend std::istream& operator>>(std::istream& is, MyString& str) {
        char c;
        str.~MyString();
        str.head = nullptr;
        CharNode* current = nullptr;
        while (is.get(c) && !isspace(c)) {
            CharNode* newNode = new CharNode(c);
            if (!str.head) {
                str.head = newNode;
                current = str.head;
            } else {
                current->next = newNode;
                current = newNode;
            }
        }
        return is;
    }

    MyString operator+(const MyString& other) const {
        MyString result(*this);
        if (!other.head) return result;
        
        CharNode* current = result.head;
        if (!current) {
            result = other;
            return result;
        }
        
        while (current->next) {
            current = current->next;
        }
        
        CharNode* otherCurrent = other.head;
        while (otherCurrent) {
            current->next = new CharNode(otherCurrent->data);
            current = current->next;
            otherCurrent = otherCurrent->next;
        }
        return result;
    }

    bool empty() const {
        return head == nullptr;
    }
};

#endif // CUSTOM_STRING_HPP 
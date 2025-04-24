#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <stdexcept>

template<typename T>
class Array {
private:
    T* data;
    int size;
    int capacity;

public:
    // Constructors and destructor
    Array(int initialCapacity = 10) : data(new T[initialCapacity]), size(0), capacity(initialCapacity) {}
    
    ~Array() {
        delete[] data;
    }

    // Copy constructor
    Array(const Array& other) : data(new T[other.capacity]), size(other.size), capacity(other.capacity) {
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }

    // Assignment operator
    Array& operator=(const Array& other) {
        if (this != &other) {
            delete[] data;
            capacity = other.capacity;
            size = other.size;
            data = new T[capacity];
            for (int i = 0; i < size; i++) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    // Element access
    T& operator[](int index) {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Array index out of bounds");
        }
        return data[index];
    }

    const T& operator[](int index) const {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Array index out of bounds");
        }
        return data[index];
    }

    // Capacity
    bool empty() const { return size == 0; }
    int getSize() const { return size; }

    // Modifiers
    void push_back(const T& value) {
        if (size == capacity) {
            capacity *= 2;
            T* newData = new T[capacity];
            for (int i = 0; i < size; i++) {
                newData[i] = data[i];
            }
            delete[] data;
            data = newData;
        }
        data[size++] = value;
    }
};

#endif // ARRAY_HPP 
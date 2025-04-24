#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <stdexcept>
#include <memory>

template<typename T>
class Array {
private:
    std::unique_ptr<T[]> data;
    int size;
    int capacity;

    void resize(int newCapacity) {
        std::unique_ptr<T[]> newData = std::make_unique<T[]>(newCapacity);
        for (int i = 0; i < size; i++) {
            newData[i] = std::move(data[i]);
        }
        data = std::move(newData);
        capacity = newCapacity;
    }

public:
    // Constructors and destructor
    Array(int initialCapacity = 10) : data(std::make_unique<T[]>(initialCapacity)), size(0), capacity(initialCapacity) {}
    
    ~Array() {
        // Destructor is implicitly called when std::unique_ptr goes out of scope
    }

    // Copy constructor
    Array(const Array& other) : data(std::make_unique<T[]>(other.capacity)), size(other.size), capacity(other.capacity) {
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }

    // Assignment operator
    Array& operator=(const Array& other) {
        if (this != &other) {
            std::unique_ptr<T[]> newData = std::make_unique<T[]>(other.capacity);
            for (int i = 0; i < other.size; i++) {
                newData[i] = other.data[i];
            }
            data = std::move(newData);
            size = other.size;
            capacity = other.capacity;
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
            resize(capacity * 2);
        }
        data[size++] = value;
    }
};

#endif // ARRAY_HPP 
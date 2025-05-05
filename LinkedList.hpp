#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include <iostream>
#include <string>
#include <iterator>

template<typename T>
class Node {
public:
    T data;
    Node* next;
    
    Node(T value) : data(value), next(nullptr) {}
};

template<typename T>
class LinkedList {
private:
    Node<T>* head;
    int size;

public:
    // Iterator class definition
    class Iterator {
    private:
        Node<T>* current;
    
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator(Node<T>* node = nullptr) : current(node) {}

        T& operator*() { return current->data; }
        T* operator->() { return &current->data; }

        Iterator& operator++() {
            if (current) current = current->next;
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const Iterator& other) const {
            return current == other.current;
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }

        Node<T>* getNode() const { return current; }
    };

    // Iterator methods
    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); }

    LinkedList() : head(nullptr), size(0) {}
    
    // Deep copy constructor
    LinkedList(const LinkedList& other) : head(nullptr), size(0) {
        Node<T>* current = other.head;
        while (current != nullptr) {
            insert(current->data);
            current = current->next;
        }
    }

    // Deep copy assignment operator
    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            // Clean up current list
            Node<T>* current = head;
            while (current != nullptr) {
                Node<T>* next = current->next;
                delete current;
                current = next;
            }
            head = nullptr;
            size = 0;
            // Copy from other
            current = other.head;
            while (current != nullptr) {
                insert(current->data);
                current = current->next;
            }
        }
        return *this;
    }

    ~LinkedList() {
        Node<T>* current = head;
        while (current != nullptr) {
            Node<T>* next = current->next;
            delete current;
            current = next;
        }
    }

    void insert(T value) {
        Node<T>* newNode = new Node<T>(value);
        if (head == nullptr) {
            head = newNode;
        } else {
            Node<T>* current = head;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newNode;
        }
        size++;
    }

    void insertAtBeginning(T value) {
        Node<T>* newNode = new Node<T>(value);
        newNode->next = head;
        head = newNode;
        size++;
    }

    bool remove(T value) {
        if (head == nullptr) return false;

        if (head->data == value) {
            Node<T>* temp = head;
            head = head->next;
            delete temp;
            size--;
            return true;
        }

        Node<T>* current = head;
        while (current->next != nullptr && !(current->next->data == value)) {
            current = current->next;
        }

        if (current->next != nullptr) {
            Node<T>* temp = current->next;
            current->next = temp->next;
            delete temp;
            size--;
            return true;
        }

        return false;
    }

    Node<T>* search(T value) {
        Node<T>* current = head;
        while (current != nullptr) {
            if (current->data == value) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }

    Node<T>* getHead() const { return head; }
    
    int getSize() const { return size; }
    
    void setHead(Node<T>* newHead) { head = newHead; }

    void display() {
        for (auto it = begin(); it != end(); ++it) {
            std::cout << *it << " -> ";
        }
        std::cout << "nullptr" << std::endl;
    }
};

#endif // LINKED_LIST_HPP 
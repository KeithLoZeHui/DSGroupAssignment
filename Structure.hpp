#ifndef STRUCTURE_HPP
#define STRUCTURE_HPP

#include <iostream>
#include <string>
using namespace std;

template <typename T>
class Array{
    private:
        T* data;
        int capacity;
        int size;
    
    public:
        Array(int initial_capacity = 1000){
            capacity = initial_capacity;
            data = new T[capacity];
            size = 0;
        }

        ~Array(){
            delete[] data;
        }

        void add(const T& element){
            if (size >= capacity){
                resize();
            }
            data[size++] = element;
        }

        T& get(int index){
            if (index >= 0 && index < size){
                return data[index];
            }
            else{
                throw std::out_of_range("Index out of range");
            }
        }

        int getSize() const{
            return size;
        }

        void removeLast(){
            if (size > 0) {
                size--;
            }
        }

    private:
        void resize(){
            int new_capacity = capacity * 2;
            T* new_data = new T[new_capacity];
            for (int i = 0; i < size; i++){
                new_data[i] = data[i];
            }
            delete[] data;
            data = new_data;
            capacity = new_capacity;
        }
    };

template <typename T>
class LinkedList {
    public:
        struct Node{
            T data;
            Node* next;
        };

        Node* head;
        Node* tail;
        int size;
    
    public:
    LinkedList(){
        head = nullptr;
        tail = nullptr;
        size = 0;
    }

    ~LinkedList(){
        Node* current = head;
        while(current != nullptr){
            Node* next = current->next;
            delete current;
            current = next;
        }
    }

    void setHead(Node* newHead){
        head = newHead;
        tail = nullptr;
        size = 0;

        Node* current = newHead;
        while(current != nullptr){
            tail = current;
            size++;
            current = current->next;
        }
    }

    void add(const T& value){
        Node* newNode = new Node{ value, nullptr};
        if(!head){
            head = newNode;
            tail = newNode;
        }else{
            tail->next = newNode;
            tail = newNode;
        }
        size++;
    }

    T& get(int index){
        Node* current = head;
        for(int i = 0; i < index && current != nullptr; i++){
            current = current->next;
        }
        if(!current) throw out_of_range("Index out of bounds");
        return current->data;
    }

    int getSize() const{
        return size;
    }

    void removeLast(){
        if(!head) return;

        if (head == tail){
            delete head;
            head = tail = nullptr;
        }else{
            Node* current = head;
            while(current->next !=tail){
                current = current->next;
            }
            delete tail;
            current->next = nullptr;
            tail = current;
        }
        size--;
    }

    Node* getHead() const{
        return head;
    }
};

struct Review{
    string productID;
    string customerID;
    int rating;
    string reviewText;
};

struct Transaction{
    string customerID;
    string product;
    double price;
    string date;
    string category;
    string paymentMethod;
};

#endif
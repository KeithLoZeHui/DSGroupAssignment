#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP

#include <iostream>
#include <cmath>
#include <algorithm>
#include "LinkedList.hpp"

template<typename T>
class SortingAlgorithms {
private:
    // Helper function to create array of node pointers
    static Node<T>** createNodeArray(LinkedList<T>& list, int n) {
        Node<T>** arr = new Node<T>*[n];
        Node<T>* current = list.getHead();
        for (int i = 0; i < n; i++) {
            arr[i] = current;
            current = current->next;
        }
        return arr;
    }

    // Helper function for quicksort
    static int partition(Node<T>** arr, int low, int high) {
        T pivot = arr[high]->data;
        int i = (low - 1);

        for (int j = low; j <= high - 1; j++) {
            if (!(arr[j]->data > pivot)) {
                i++;
                std::swap(arr[i]->data, arr[j]->data);
            }
        }
        std::swap(arr[i + 1]->data, arr[high]->data);
        return (i + 1);
    }

    static void quickSortHelper(Node<T>** arr, int low, int high) {
        if (low < high) {
            int pi = partition(arr, low, high);
            quickSortHelper(arr, low, pi - 1);
            quickSortHelper(arr, pi + 1, high);
        }
    }

    // Helper function for merge sort
    static void merge(Node<T>** arr, int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;

        // Create temporary arrays
        Node<T>** L = new Node<T>*[n1];
        Node<T>** R = new Node<T>*[n2];

        // Copy data to temporary arrays
        for (int i = 0; i < n1; i++)
            L[i] = arr[left + i];
        for (int j = 0; j < n2; j++)
            R[j] = arr[mid + 1 + j];

        // Merge the temporary arrays back
        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (!(L[i]->data > R[j]->data)) {  // Using > operator only
                arr[k] = L[i];
                i++;
            } else {
                arr[k] = R[j];
                j++;
            }
            k++;
        }

        // Copy remaining elements
        while (i < n1) {
            arr[k] = L[i];
            i++;
            k++;
        }
        while (j < n2) {
            arr[k] = R[j];
            j++;
            k++;
        }

        delete[] L;
        delete[] R;
    }

    static void mergeSortHelper(Node<T>** arr, int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            mergeSortHelper(arr, left, mid);
            mergeSortHelper(arr, mid + 1, right);
            merge(arr, left, mid, right);
        }
    }

public:
    // Quick Sort Implementation - O(n log n) average case
    static void quickSort(LinkedList<T>& list) {
        int n = list.getSize();
        if (n <= 1) return;

        Node<T>** arr = createNodeArray(list, n);
        quickSortHelper(arr, 0, n - 1);
        delete[] arr;
    }

    // Merge Sort Implementation - O(n log n)
    static void mergeSort(LinkedList<T>& list) {
        int n = list.getSize();
        if (n <= 1) return;

        Node<T>** arr = createNodeArray(list, n);
        mergeSortHelper(arr, 0, n - 1);
        
        // Update the linked list with sorted values
        Node<T>* current = list.getHead();
        for (int i = 0; i < n; i++) {
            current->data = arr[i]->data;
            current = current->next;
        }
        
        delete[] arr;
    }

    // Binary Search Implementation - O(log n)
    static bool binarySearch(LinkedList<T>& list, const T& target) {
        int n = list.getSize();
        if (n == 0) return false;

        Node<T>** arr = createNodeArray(list, n);
        bool found = false;
        int left = 0;
        int right = n - 1;

        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (arr[mid]->data == target) {
                found = true;
                break;
            }
            if (arr[mid]->data > target)
                right = mid - 1;
            else
                left = mid + 1;
        }

        delete[] arr;
        return found;
    }

    // Jump Search Implementation - O(√n)
    static bool jumpSearch(LinkedList<T>& list, const T& target) {
        int n = list.getSize();
        if (n == 0) return false;

        Node<T>** arr = createNodeArray(list, n);
        bool found = false;
        
        // Finding block size to be jumped
        int step = sqrt(n);
        int prev = 0;

        // Finding the block where element is present
        while (prev < n && !(arr[std::min(step, n) - 1]->data > target) && arr[std::min(step, n) - 1]->data < target) {
            prev = step;
            step += sqrt(n);
            if (prev >= n) {
                delete[] arr;
                return false;
            }
        }

        // Doing a linear search for target in block beginning with prev
        while (prev < n && arr[prev]->data < target) {
            prev++;
            if (prev == std::min(step, n)) {
                delete[] arr;
                return false;
            }
        }

        // If element is found (using only < and > operators)
        if (prev < n && !(arr[prev]->data > target) && !(target > arr[prev]->data)) {
            found = true;
        }

        delete[] arr;
        return found;
    }
};

#endif // ALGORITHMS_HPP 
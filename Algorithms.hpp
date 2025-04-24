#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP

#include <iostream>
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

public:
    // Quick Sort Implementation - O(n log n) average case
    static void quickSort(LinkedList<T>& list) {
        int n = list.getSize();
        if (n <= 1) return;

        Node<T>** arr = createNodeArray(list, n);
        quickSortHelper(arr, 0, n - 1);
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
};

#endif // ALGORITHMS_HPP 
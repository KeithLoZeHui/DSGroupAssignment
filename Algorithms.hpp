#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP

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

    // Merge Sort Helper Functions
    static Node<T>* merge(Node<T>* first, Node<T>* second) {
        if (!first) return second;
        if (!second) return first;

        Node<T>* result = nullptr;

        if (!(first->data > second->data)) {
            result = first;
            result->next = merge(first->next, second);
        } else {
            result = second;
            result->next = merge(first, second->next);
        }

        return result;
    }

    static Node<T>* getMiddle(Node<T>* head) {
        if (!head) return head;
        
        Node<T>* slow = head;
        Node<T>* fast = head->next;

        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }

        return slow;
    }

    // Heap Sort Helper Functions
    static void heapify(Node<T>* arr[], int n, int i) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < n && arr[left]->data > arr[largest]->data)
            largest = left;

        if (right < n && arr[right]->data > arr[largest]->data)
            largest = right;

        if (largest != i) {
            std::swap(arr[i]->data, arr[largest]->data);
            heapify(arr, n, largest);
        }
    }

    // Jump Sort Helper Functions
    static int getJumpGap(int n) {
        return (n <= 1) ? 0 : (int)(n / 2.0);
    }

public:
    // Merge Sort Implementation
    static Node<T>* mergeSort(Node<T>* head) {
        if (!head || !head->next) return head;

        Node<T>* middle = getMiddle(head);
        Node<T>* nextToMiddle = middle->next;

        middle->next = nullptr;

        Node<T>* left = mergeSort(head);
        Node<T>* right = mergeSort(nextToMiddle);

        return merge(left, right);
    }

    // Heap Sort Implementation
    static void heapSort(LinkedList<T>& list) {
        int n = list.getSize();
        if (n <= 1) return;

        Node<T>** arr = createNodeArray(list, n);

        // Build heap
        for (int i = n / 2 - 1; i >= 0; i--)
            heapify(arr, n, i);

        // Extract elements from heap
        for (int i = n - 1; i > 0; i--) {
            std::swap(arr[0]->data, arr[i]->data);
            heapify(arr, i, 0);
        }

        delete[] arr;
    }

    // Jump Sort Implementation
    static void jumpSort(LinkedList<T>& list) {
        int n = list.getSize();
        if (n <= 1) return;

        Node<T>** arr = createNodeArray(list, n);

        // Jump Sort implementation
        int gap = getJumpGap(n);
        while (gap > 0) {
            for (int i = 0; i + gap < n; i++) {
                if (arr[i]->data > arr[i + gap]->data) {
                    std::swap(arr[i]->data, arr[i + gap]->data);
                    // Back-check
                    int j = i;
                    while (j - gap >= 0 && arr[j - gap]->data > arr[j]->data) {
                        std::swap(arr[j - gap]->data, arr[j]->data);
                        j -= gap;
                    }
                }
            }
            gap = getJumpGap(gap);
        }

        delete[] arr;
    }
};

#endif // ALGORITHMS_HPP 
#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

// A growable array, written for this project instead of using std::vector.
// Stores items in a block of memory and allocates a bigger block when full.

template <typename T>
struct DynamicArray {
    T* data;
    int size;
    int capacity;

    DynamicArray() {
        capacity = 4;
        size = 0;
        data = new T[capacity];
    }

    // Copy constructor. Needed because this array holds a raw pointer.
    // Without it, copying an array would copy the pointer, and two arrays
    // would free the same memory.
    DynamicArray(const DynamicArray& other) {
        capacity = other.capacity;
        size = other.size;
        data = new T[capacity];
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }

    // Copy assignment, for the same reason.
    DynamicArray& operator=(const DynamicArray& other) {
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

    ~DynamicArray() {
        delete[] data;
    }

    // Add an item at the end. If the block is full, allocate one twice as
    // big, copy everything across, and free the old block.
    void push_back(const T& item) {
        if (size == capacity) {
            int newCapacity = capacity * 2;
            T* newData = new T[newCapacity];
            for (int i = 0; i < size; i++) {
                newData[i] = data[i];
            }
            delete[] data;
            data = newData;
            capacity = newCapacity;
        }
        data[size] = item;
        size++;
    }

    T& operator[](int i) {
        return data[i];
    }

    const T& operator[](int i) const {
        return data[i];
    }

    bool empty() const {
        return size == 0;
    }
};

#endif

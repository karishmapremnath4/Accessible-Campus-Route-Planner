#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include "dynamic_array.h"

// One entry waiting in the queue: a building, and the cost of reaching it on
// the best route found so far.
struct HeapItem {
    int building;
    double priority;
};

// A binary min-heap, written for this project instead of using
// std::priority_queue. The smallest priority is always at the front.
//
// The items live in a DynamicArray and are treated as a tree: the parent of
// position i is at (i - 1) / 2, and its children are at 2i + 1 and 2i + 2.
struct MinHeap {
    DynamicArray<HeapItem> items;

    bool empty() const {
        return items.size == 0;
    }

    int size() const {
        return items.size;
    }

    // Put an item at the end, then move it up while it is smaller than its
    // parent. The tree is log n deep, so this takes at most log n swaps.
    void push(int building, double priority) {
        HeapItem item;
        item.building = building;
        item.priority = priority;
        items.push_back(item);

        int i = items.size - 1;
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (items[parent].priority <= items[i].priority) {
                break;
            }
            HeapItem temp = items[parent];
            items[parent] = items[i];
            items[i] = temp;
            i = parent;
        }
    }

    // Take the item at the front, which is the smallest. Move the last item
    // into its place and push it down while it is larger than its smaller
    // child. Check empty() before calling this: popping an empty heap returns
    // an item with building -1 and leaves the heap unchanged.
    HeapItem pop() {
        if (empty()) {
            HeapItem none;
            none.building = -1;
            none.priority = 0.0;
            return none;
        }
        HeapItem smallest = items[0];
        items[0] = items[items.size - 1];
        items.size--;

        int i = 0;
        while (true) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            int lowest = i;

            if (left < items.size && items[left].priority < items[lowest].priority) {
                lowest = left;
            }
            if (right < items.size && items[right].priority < items[lowest].priority) {
                lowest = right;
            }
            if (lowest == i) {
                break;
            }

            HeapItem temp = items[lowest];
            items[lowest] = items[i];
            items[i] = temp;
            i = lowest;
        }
        return smallest;
    }
};

#endif

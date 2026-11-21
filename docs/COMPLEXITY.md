# Time and space complexity

## Variables and assumptions

- V is the number of buildings. In the current dataset V = 39.
- E is the number of pathways. In the current dataset E = 151. Each pathway is
  stored twice, once in each direction, so the adjacency list holds 2E = 302
  entries.
- The graph is connected, so E >= V - 1.
- A pathway that is closed, or that has stairs when the user asked to avoid
  stairs, is skipped during relaxation. It is still stored, so it still counts
  towards E.
- Both searches stop as soon as the destination is removed from the priority
  queue.
- Distances are doubles, so comparing or adding two of them is one operation.

## DynamicArray

| Operation | Best | Worst |
| --- | --- | --- |
| `push_back` | Theta(1) | Theta(n) |
| `push_back`, amortised over n calls | Theta(1) | Theta(1) |
| `operator[]` | Theta(1) | Theta(1) |
| copy constructor, copy assignment | Theta(n) | Theta(n) |
| destructor | Theta(1) | Theta(1) |

A single `push_back` is Theta(n) in the worst case because when the block is
full it allocates a block twice as large and copies all n items across. That
happens only when n reaches the capacity. Since the capacity doubles, the
copying cost is spread over the pushes that follow, so n pushes cost
Theta(n) in total and each push costs Theta(1) on average.

Space is Theta(capacity), which stays below 2n.

## MinHeap

The priority queue is a binary min-heap stored in a DynamicArray. For a heap
holding n items:

| Operation | Best | Worst |
| --- | --- | --- |
| `push` | Theta(1) | Theta(log n) |
| `pop` | Theta(1) | Theta(log n) |
| `empty` | Theta(1) | Theta(1) |

`push` puts the item at the end and moves it up while it is smaller than its
parent. The height of the heap is Theta(log n), so that is the worst case. The
best case is Theta(1), when the item is already larger than its parent and
does not move.

`pop` takes the item at the front, moves the last item there, and moves it
down while it is larger than its smaller child. Again the worst case is the
height, Theta(log n).

Space is Theta(n).

## loadCampusData

Reading both files is Theta(V + E).

Each line is read once and split into a fixed number of fields, which is
Theta(1) per line. There are V lines in buildings.csv and E lines in
pathways.csv. Each pathway line causes two `push_back` calls, which are
Theta(1) amortised.

Space is Theta(V + E): Theta(V) for the buildings and Theta(E) for the
adjacency list.

## Dijkstra's algorithm

| Case | Bound |
| --- | --- |
| Worst | Theta((V + E) log V) |
| Best | Theta(1) |
| Space | Theta(V + E) |

Worst case: the destination is the last building removed from the queue, so
every building is pushed and popped and every pathway is relaxed. Each push
and pop costs Theta(log V). Pushes are bounded by the number of relaxations
that improve a distance, which is at most E, so the total is
Theta((V + E) log V).

Best case: the starting building is the destination. It is popped
immediately, the search stops, and no pathway is relaxed, so the cost is
Theta(1).

Space: Theta(V + E) for the adjacency list, plus Theta(V) each for the
distance array, the previous-building array, the finalised flags, and the
heap.

## A* search

| Case | Bound |
| --- | --- |
| Worst | Theta((V + E) log V) |
| Best | Theta(1) |
| Space | Theta(V + E) |

A* has the same bounds as Dijkstra. It differs in the order it removes
buildings from the queue, not in the amount of work each step costs. The
heuristic is the straight-line distance between two buildings computed from
their x and y values, which is Theta(1) to evaluate.

The worst case occurs when the heuristic gives no useful information and A*
expands the same buildings Dijkstra does. In practice it expands fewer,
because the heuristic pushes the search towards the destination. That
difference is a constant factor on this dataset, not a change in the bound,
which is why it has to be measured rather than derived. The measurement is
the number of buildings each algorithm removes from the queue for the same
route request.

The heuristic never overestimates the remaining distance, because no pathway
in the dataset is shorter than the straight-line distance between the two
buildings it connects. That is checked for all 151 pathways. An admissible
heuristic is what guarantees A* returns the same shortest distance Dijkstra
does.

# Time and space complexity

V = number of buildings = 39
E = number of pathways = 151

Each pathway is stored twice, once in each direction, so the adjacency list
holds 302 entries. The graph is connected. Both searches stop as soon as the
destination comes off the queue.

## DynamicArray

| Operation | Best | Worst |
| --- | --- | --- |
| push_back | Theta(1) | Theta(n) |
| push_back over n calls | Theta(1) each | Theta(1) each |
| operator[] | Theta(1) | Theta(1) |
| copy | Theta(n) | Theta(n) |

push_back is Theta(n) when the array is full, because it has to copy
everything into a bigger block. The capacity doubles each time, so this gets
rarer as the array grows. Over n pushes the total work is Theta(n), which is
Theta(1) per push on average.

Space is Theta(n).

## MinHeap

n = number of items in the heap.

| Operation | Best | Worst |
| --- | --- | --- |
| push | Theta(1) | Theta(n) |
| push over n calls | Theta(1) each | Theta(log n) each |
| pop | Theta(1) | Theta(log n) |
| empty | Theta(1) | Theta(1) |

The heap is a tree of height log n. push moves an item up at most that far,
pop moves one down at most that far. Best case is when the item does not move.

push is Theta(n) in the worst case because it adds the item with DynamicArray
push_back, which copies everything when the array is full. That gets rarer as
the heap grows, so over many pushes each one costs at most Theta(log n) on
average.

Space is Theta(n).

## loadCampusData

Theta(V + E) time and Theta(V + E) space.

Each line is read once and split into 7 fields. There are V lines in
buildings.csv and E lines in pathways.csv. Each pathway is also checked
against the straight-line distance, which is Theta(1). The finished graph is
copied into g once at the end, which is also Theta(V + E).

## Dijkstra

| Case | Bound |
| --- | --- |
| Worst | Theta((V + E) log V) |
| Best | Theta(V) |
| Space | Theta(V + E) |

Worst case is when the destination is the last building taken off the queue.
Every pathway is checked. MinHeap cannot lower the priority of an item already
in it, so a building is pushed again each time a shorter route to it is found,
and the old entry is skipped when it comes off. Each pathway is stored in both
directions, so the heap can hold up to 2E items, and each heap operation costs
log 2E. Here 2E is less than V squared, so that is at most 2 log V, and the
bound is still (V + E) log V.

Best case is when the start is the destination. It comes off the queue first
and the search stops, but the arrays for distances, previous buildings and
finalised flags have already been filled, one entry per building, so it is
Theta(V).

Space is the adjacency list, plus one array each for distances, previous
buildings and finalised flags.

## A*

Same bounds as Dijkstra. The heuristic changes which buildings come off the
queue first, not how much each step costs. The heuristic itself is Theta(1):
two subtractions, two multiplications, one addition and a square root.

In practice A* looks at fewer buildings than Dijkstra, but that is a constant
factor on a graph this size, not a different bound. It has to be measured, by
counting how many buildings each one takes off the queue.

The heuristic is consistent: for every pathway from u to v,
h(u) <= cost(u, v) + h(v). That follows from the rule that no pathway is
shorter than the straight line between its two buildings, because the straight
line from u to the destination is never longer than going to v first and then
straight to the destination. A consistent heuristic also never overestimates.
Consistency is what lets A* finalise a building the first time it comes off
the queue and still return the same shortest distance as Dijkstra. The 1.5
outdoor weight only makes pathways cost more, so this holds under all three
preferences. The rule was checked for all 151 pathways, and loadCampusData()
rejects any row that breaks it.

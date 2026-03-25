/*
CHALLENGE 7: The adjaceny matrix of a graph G is a matrix A that holds a value true or false in element A[j][i] if there is an arc from node i to node j. 
At this point, can you use an adjacency matrix to conduct a breadth-first search in a graph G? Can you find connected components? Can you find a spanning tree?

CHALLENGE 8: Extend the idea of an adjacency matrix of a graph G to a distance matrix F that holds the distance when going from point i to point j. 
Mark the absence of a direct arc with a very large value, such as INT_MAX. Can you find the shortest path between two nodes x and y given as an input?
*/

// fix size_t things 

#include <stdio.h>
#include <limits.h>

#define NUM_NODES 5
const int STANDARD_MATRIX[NUM_NODES][NUM_NODES] = {
    {0, 0, 0, 1, 1},
    {0, 0, 1, 0, 1},
    {0, 1, 0, 1, 0},
    {1, 0, 1, 0, 0},
    {1, 1, 0, 0, 0},
};
const int WEIGHTED_MATRIX[NUM_NODES][NUM_NODES] = {
    {INT_MAX, 15, INT_MAX, 1, 7},
    {15, INT_MAX, 2, INT_MAX, 7},
    {INT_MAX, 2, INT_MAX, INT_MAX, 1},
    {1, INT_MAX, INT_MAX, INT_MAX, INT_MAX},
    {7, 7, 1, INT_MAX, INT_MAX},
};

void bfsTraversal(){
    int nodesSeen[NUM_NODES] = {0};
    nodesSeen[0] = 1;

    int queue[NUM_NODES] = {0};
    int head = 0;
    int tail = 1;

    printf("Traversing Tree:\n");

    while (tail > head){
        int currentNode = queue[head];
        head++;

        printf("Node %c\n", currentNode + 65); // move through letters

        for (int currentNeighbor = 0; currentNeighbor < NUM_NODES; currentNeighbor++) {
            if (nodesSeen[currentNeighbor] == 1) {
                continue;
            }

            if (STANDARD_MATRIX[currentNode][currentNeighbor] == 1) {
                queue[tail] = currentNeighbor;
                nodesSeen[currentNeighbor] = 1; // mark as seen 
                tail++;
            }
        }
    }
}

typedef struct pair pair;
struct pair {
    size_t node;
    size_t cost;
};

void swap(pair queue[NUM_NODES], size_t i, size_t j) {
    pair temp = queue[i];
    queue[i] = queue[j];
    queue[j] = temp;
}

pair dequeue(pair queue[NUM_NODES], size_t size) {
    pair root = queue[0];
    queue[0] = queue[--size];

    size_t currentIndex = 0;
    while (2 * currentIndex + 1 < size) {
        size_t nextChild = 1;

        // move towards the lower value
        if (2 * currentIndex + 2 < size && 
            queue[2 * currentIndex + 2].cost < queue[2 * currentIndex + 1].cost) {
            nextChild = 2;
        }

        if (queue[currentIndex].cost <= queue[2 * currentIndex + nextChild].cost) {
            break;
        }

        swap(queue, currentIndex, 2 * currentIndex + nextChild);
        currentIndex = 2 * currentIndex + nextChild;
    }

    return root;
}

void bubble_up(pair queue[NUM_NODES], size_t currentIndex) {
    while (currentIndex > 0) {
        size_t parentIndex = (currentIndex - 1) / 2;
        if (queue[parentIndex].cost <= queue[currentIndex].cost) {
            break;
        }
        swap(queue, parentIndex, currentIndex);
        currentIndex = parentIndex;
    }
}

void enqueue(pair queue[NUM_NODES], size_t node, size_t cost, size_t size) {
    queue[size] = (pair){node, cost};
    bubble_up(queue,size);
}

void decrease_key(pair queue[NUM_NODES], size_t node, size_t newCost, size_t size){
    size_t currentIndex = 0; 
    while (currentIndex < size) {
        if (queue[currentIndex].node == node) {
            queue[currentIndex].cost = newCost;
            bubble_up(queue, currentIndex);
            break;
        }
        currentIndex++;
    }
}


size_t findShortestPathWeight(size_t startingNode, size_t endingNode) {
    size_t nodesSeen[NUM_NODES * NUM_NODES];
    for (size_t i = 0; i < NUM_NODES; i++) {
        nodesSeen[i] = INT_MAX;
    }
    nodesSeen[startingNode] = 0;

    size_t nodesVisited[NUM_NODES] = {0};

    pair priorityQueue[NUM_NODES];
    priorityQueue[0] = (pair){startingNode, 0};
    int nodesInQueue = 1;

    while (nodesInQueue > 0) { 
        pair currentInfo = dequeue(priorityQueue, nodesInQueue);
        nodesInQueue--;

        size_t currentNode = currentInfo.node;
        size_t currentCost = currentInfo.cost;

        if (nodesVisited[currentNode] == 1) {
            continue;
        }
        nodesVisited[currentNode] = 1;

        if (currentNode == endingNode) {
            return currentCost;
        }

        for (size_t currentNeighbor = 0; currentNeighbor < NUM_NODES; currentNeighbor++) {
            if (nodesVisited[currentNeighbor] == 1) {
                // already found optimal cost
                continue;
            }
            if (WEIGHTED_MATRIX[currentNode][currentNeighbor] == INT_MAX) {
                // no link
                continue;
            }

            size_t costToMove = currentCost + WEIGHTED_MATRIX[currentNode][currentNeighbor];

            if (nodesSeen[currentNeighbor] == INT_MAX) {
                enqueue(priorityQueue, currentNeighbor, costToMove, nodesInQueue);
                nodesSeen[currentNeighbor] = costToMove;
                nodesInQueue++;
            }
            else if (nodesSeen[currentNeighbor] > costToMove){
                decrease_key(priorityQueue, currentNeighbor, costToMove, nodesInQueue);
                nodesSeen[currentNeighbor] = costToMove;
            }
        }
    }

    return INT_MAX;
}

int main(){
    // bfsTraversal();

    const size_t START = 0;
    const size_t END = 1;
    size_t total = findShortestPathWeight(START, END);

    printf("The smallest total cost to get from %c to %c is %zu\n", (int)(START + 65), (int)(END + 65), total);
}
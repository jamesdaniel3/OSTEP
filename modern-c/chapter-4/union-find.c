/* PROBLEM WRITEUP:

Can you implement a forest data structure in an index table of base type size_t called parent? Here, a value in the table 
SIZE_MAX would mean a position represent a root of one of the trees; another number represetns the position of the parent 
of the corresponding tree. One of htei mportnat features of starting the implementation is an initialization function that
makes parent the singleton partiion; that is, the parition where each element is the root of its own set.

With this index table, can you implement a Find function, that, for a given index, finds the root of it's tree?

Can you implement a FindReplace function that changes all parent entries on a path to the root (including) to a 
specific value?

Can you implement a FindCompress function that changes all parent entries to the root that has been found?

Can you implement a Union function that, for two given elements, combines their trees into one? Use FindCompress for one 
side and FindReplace for the other. 


TO WATCH: 
- X https://www.youtube.com/watch?v=ibjEGG7ylHk
- X https://www.youtube.com/watch?v=JZBQLXgSGfs
- X https://www.youtube.com/watch?v=0jNmHPfA_yE
- X https://www.youtube.com/watch?v=VHRhJWacxis

*/
  
#include <stdlib.h>

const int NUM_NODES = 100;
size_t parent[NUM_NODES];

int main() {
    // initialize array
    for (int i = 0; i < NUM_NODES; i++){
        parent[i] = SIZE_MAX;
    }
}

size_t find(size_t index) {
    while (parent[index] != SIZE_MAX) {
        index = parent[index];
    }

    return index;
}

void findReplace(size_t currentIndex, size_t newValue) {
    size_t rootIndex = find(currentIndex); 

    while (currentIndex != rootIndex) {
        size_t temp = parent[currentIndex];
        parent[currentIndex] = newValue;
        currentIndex = temp;
    }

    parent[rootIndex] = newValue;
    return;
}

void findCompress(size_t currentIndex) {
    size_t rootIndex = find(currentIndex); 

    while (currentIndex != rootIndex) {
        size_t next = parent[currentIndex];
        parent[currentIndex] = rootIndex;
        currentIndex = next;
    }

    return;
} 

void unionFunction(size_t right, size_t left){
    findCompress(left); // I feel like it makes more sense to compress both sides but this is what the prompt requested

    size_t leftRoot = find(left);
    size_t rightRoot = find(right);

    if (leftRoot == rightRoot) {
        return;
    }

    findReplace(right, leftRoot);
}
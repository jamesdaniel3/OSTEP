// Challenge: write a function that does vector-to-vector or matrix-to-vector multiplication:

// Dot product multiplication: https://www.khanacademy.org/math/linear-algebra/vectors-and-spaces/dot-cross-products/v/vector-dot-product-and-vector-length

#include <stdio.h>
#include <stddef.h>

// dot product multiplication: given two vectors, take the product of each of the corresponding components and sum them 
double vectorDotProduct(const double firstVector[], const double secondVector[], size_t vectorSize){
    double result = 0.0;
    for(size_t currentIndex = 0; currentIndex < vectorSize; currentIndex++) {
        result += firstVector[currentIndex] * secondVector[currentIndex];
    }

    return result;
}

int main() {
    // EDIT THESE TO SEE NEW DOT PRODUCTS (could also take user input but I don't want to atm)
    double firstVector[] = {1, 2, 3};
    double secondVector[] = {4, 5, 6};

    size_t firstVectorSize = sizeof(firstVector) / sizeof(firstVector[0]);
    size_t secondVectorSize = sizeof(secondVector) / sizeof(secondVector[0]);

    if (firstVectorSize != secondVectorSize) {
        printf("Error: vectors must have the same length.\n");
        return -1;
    }
    
    size_t vectorSize = firstVectorSize;

    printf("Here are your vectors: \n[ ");
    for(size_t currentIndex = 0; currentIndex < vectorSize; currentIndex++) {
        printf("%.2lf ", firstVector[currentIndex]);
    }
    printf("]\n[ ");
    for(size_t currentIndex = 0; currentIndex < vectorSize; currentIndex++) {
        printf("%.2lf ", secondVector[currentIndex]);
    }
    printf("]\n");

    double dotProductResult = vectorDotProduct(firstVector, secondVector, vectorSize);
    printf("The dot product of your vectors is %.2lf\n", dotProductResult);

    return 0;
}
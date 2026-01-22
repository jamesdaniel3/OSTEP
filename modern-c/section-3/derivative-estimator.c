#include <math.h>
#include <stdlib.h>
#include <stdio.h>

int main (int argc, char** argv) {
    /*
    Float and double both store non-integers. 
    Each float is 4 bytes and can store a number precisley up to 7 digits after the decimal point.
    Each double is 8 bytes and can store a number precisley up to 8 digits after the decimal point 
    */
    if (argc != 2) {
        printf("You passed the wrong number of arguments!\n");
        return -1;
    }

    char* end;
    double inputValue = strtod(argv[1], &end);
    if (end == argv[1]) {
        printf("Invalid input :(");
        return -1;
    }

    double DIFFERENCE = 0.00005;

    double leftVal = sin(inputValue - DIFFERENCE);
    double rightVal = sin(inputValue + DIFFERENCE);

    double slope = (rightVal - leftVal) / (DIFFERENCE * 2);
    double realResult = cos(inputValue); // I believe trig functions in C use radians (they would)

    printf("The approximate derivative of the sin function at %lf is: %lf\n", inputValue, slope);
    printf("The actual derivative as calculated by the cos function at %lf is: %lf\n", inputValue, realResult);

    return 0;
}
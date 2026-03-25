/*
See whether you can implement a program factor that receives a number N on the command line and prints out 

N: F0 F1 F2 ...

where F0 and so on are al the prime factors of N. 
The core of your implementation should be a function that, given a value of type size_t, returns its smallest prime factor.
Extend this program to receive a list of such numbers and ouptu such a line of each of them. 
*/
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

bool is_prime(size_t num){
    if (num < 2) {
        return false;
    }

    for(size_t potential_factor = 2; (potential_factor * potential_factor) <= num; potential_factor++) {
        if (num % potential_factor == 0) {
            return false;
        }
    }
    return true;
}

size_t find_next_prime_number(size_t num) {
    num++;

    while (!is_prime(num)) {
        num++;
    }

    return num;
}

size_t find_smallest_prime_factor(size_t num){
    size_t candidate = 2;

    while (num % candidate != 0) {
        candidate = find_next_prime_number(candidate);
    }

    return candidate;
}

int main(int argc, char *argv[] ) {
    if (argc < 2) {
        printf("You must pass at least one number to this program. Make sure it is a positive integer. \n");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        char *end = NULL;
        int val = strtol(argv[i], &end, 10);

        if (end == argv[i]) {
            printf("Bad input! All of your arguments should be positive ints. \n");
            return 1;
        }
        if (val < 0) {
            printf("Bad input! All of your arguments should be positive ints. \n");
            return 1;
        }
        size_t num = (size_t) val;

        printf("%zu:", num);
        while (num > 1) {
            size_t smallest_prime_factor = find_smallest_prime_factor(num);
            num = num / smallest_prime_factor;
            printf(" %zu", smallest_prime_factor);
        }
        printf("\n");
         
    }
}
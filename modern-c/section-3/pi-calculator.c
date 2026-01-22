#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// I don't totally understand why long double is a better choice than unsigned long long for things like the factorials

long double getFactorial(int num) {
    if (num == 0 || num == 1) return 1.0L;
    long double result = 1.0L;
    for (int i = 2; i <= num; i++) result *= i;
    return result;
}

int main(int argc, char** argv){
    if (argc != 2){
        printf("You ran this wrong!\n");
        return -1;    
    }

    char * end;
    int desiredDigits = strtol(argv[1], &end, 10);
    if (end == argv[1]) {
        printf("Bad input!\n");
        return -1;
    }

    // using the series defined here: https://en.wikipedia.org/wiki/Approximations_of_pi#20th_and_21st_centuries
    // we get 8 decimal places per term in our series 

    int termsNeeded = (desiredDigits + 7 ) / 8; // force round up

    long double seriesCurrentValue = 0.0L;

    for (int i = 0; i <= termsNeeded; i++) {
        long double numerator = getFactorial(4*i) * (1103 + 26390*i);
        long double denominator = powl(getFactorial(i), 4) * powl(396.0L, 4*i);
        seriesCurrentValue += numerator / denominator;
    }

    long double constant = 2.0L * sqrtl(2.0L) / 9801.0L;
    long double pi = 1.0L / (constant * seriesCurrentValue);

    double scale = pow(10, desiredDigits);
    long double result = trunc(pi * scale) / scale;

    printf("Pi calculated up to %d digits after the decimal point is %.*Lf\n", desiredDigits, desiredDigits, result);

    return 0;
}
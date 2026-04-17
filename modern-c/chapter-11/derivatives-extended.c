/*
CHALLENGE 12

Can you extend the real and complex derivatives (challenges 3 and 5) such that they receive the function F and the value x as a parameter?
Can you use the generic real derivatives to implement Newton's method for finding roots?
Can you find the real zeros of polynomials?
Can you find the complex zeros of polynomials?


Note: skipping complex zeros because I don't want to figure out working with the complex types 
*/

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define DIFFERENCE 0.00005

typedef double math_function(double input_value);

double find_generic_derivative(math_function* function, double input_value) {
    return (function(input_value + DIFFERENCE) - function(input_value - DIFFERENCE)) / (DIFFERENCE * 2);
}

double first_function(double input) {
    // f(x) = x^3 - 3x^2 + 11x - 8
    double first_term = input * input * input; // 1
    double second_term = 3 * input * input; // 3
    double third_term = 11 * input; // 8

    return first_term - second_term + third_term - 8;
}

double find_function_root(math_function* function, double first_guess) {
    // first_guess is x_0
    // x_(n + 1) = x_n - (f(x)/f'(x))
    // keep iterating to get closer to the real value

    double last_guess = first_guess;

    while(true) {
        double f_n = function(last_guess);
        double f_prime_n = find_generic_derivative(function, last_guess);
        double new_guess = last_guess - (f_n / f_prime_n);

        if (fabs(new_guess - last_guess) < 1e-6) {
            return new_guess;
        }

        last_guess = new_guess;
    }

}

int main () {
    double result = find_function_root(first_function, 1.0);

    printf("The approximate root of the given function is: %lf\n", result);

    return 0;
}


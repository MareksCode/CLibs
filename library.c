#include "library.h"

#include <signal.h>
#include <stdio.h>

void hello(void) {
    printf("Hello, World!\n");
}

//Returns a linearly interpolated number between <x1> and <x2> using <alpha>
double lerp(double x1, double x2, double alpha) {
    if (alpha < 0 || alpha > 1) { //ask prof if this is needed
        printf("Wrong usage! Alpha must be between 0 and 1.\n");
        alpha = 0.5;
    }

    double diff = x2-x1;
    return (x1 + alpha*diff);
}

//Takes an array <numbers> with <numberArrayLength> as the length and scales every value by <alpha>. Every number gets set to <minimum> as soon as it's smaller
//For easy usability, it returns a pointer to the inputted array so you can wrap your array call
double *scaled(double *numbers, int numberArrayLength, double minimum, double alpha) {
    for (int i = 0; i < numberArrayLength; i++) {
        numbers[i] *= alpha;
        if (numbers[i] < minimum) {
            numbers[i] = minimum;
        }
    }

    return numbers;
}


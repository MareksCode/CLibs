#include "library.h"

#include <stdio.h>

void hello(void) {
    printf("Hello, World!\n");
}

//Returns a linearly interpolated number between x1 and x2 using alpha
double lerp(double x1, double x2, double alpha) {
    double diff = x2-x1;
    return (x1 + alpha*diff);
}
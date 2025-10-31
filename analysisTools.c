#include "analysisTools.h"

#include <math.h>

double getMaxFromArray(double *array, int arrayLength) {
    double currentMax = -INFINITY;
    for (int i = 0; i < arrayLength; i++) {
        if (array[i] > currentMax) {
            currentMax = array[i];
        }
    }

    return currentMax;
}

double getMinFromArray(double *array, int arrayLength) {
    double currentMin = INFINITY;
    for (int i = 0; i < arrayLength; i++) {
        if (array[i] < currentMin) {
            currentMin = array[i];
        }
    }

    return currentMin;
}

double getMaxFromArrayInRange(double *array, int arrayLength, int startX, int endX) {
    int newArrayLength = endX - startX;
    double *newArrayStart = &array[startX];

    return getMaxFromArray(newArrayStart, newArrayLength);
}

double getMinFromArrayInRange(double *array, int arrayLength, int startX, int endX) {
    int newArrayLength = endX - startX;
    double *newArrayStart = &array[startX];

    return getMinFromArray(newArrayStart, newArrayLength);
}

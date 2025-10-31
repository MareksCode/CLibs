#include "analysisTools.h"

#include <math.h>
#include <stdio.h>

//returns the maximum value of a given array
double getMaxFromArray(double *array, int arrayLength) {
    double currentMax = -INFINITY;
    for (int i = 0; i < arrayLength; i++) {
        if (array[i] > currentMax) {
            currentMax = array[i];
        }
    }

    return currentMax;
}

//returns the minimum value of a given array
double getMinFromArray(double *array, int arrayLength) {
    double currentMin = INFINITY;
    for (int i = 0; i < arrayLength; i++) {
        if (array[i] < currentMin) {
            currentMin = array[i];
        }
    }

    return currentMin;
}

//returns a maximum value from a given range in an array
double getMaxFromArrayInRange(double *array, int startX, int endX) {
    int newArrayLength = endX - startX;
    double *newArrayStart = &array[startX];

    return getMaxFromArray(newArrayStart, newArrayLength);
}

//returns a minimum value from a given range in an array
double getMinFromArrayInRange(double *array, int startX, int endX) {
    int newArrayLength = endX - startX;
    double *newArrayStart = &array[startX];

    return getMinFromArray(newArrayStart, newArrayLength);
}

//returns an approximation of the area under a given signal array
//stepsize = abtastrate
double getArea(double *array, int arrayLength, double stepSize) {
    double totalSize = 0;
    for (int i = 0; i < arrayLength; i++) {
        totalSize += array[i] * stepSize;
    }

    return totalSize;
}

//returns the average of all the values in the given array
double getAverage(double *array, int arrayLength) {
    double total = 0;
    for (int i = 0; i < arrayLength; i++) {
        total += array[i];
    }

    return total / arrayLength;
}

//returns the squared variance of all the values in the given array
//based on: https://studyflix.de/statistik/varianz-berechnen-2015
double getVariance(double *array, int arrayLength) {
    double avg = getAverage(array, arrayLength);

    double sum = 0;
    for (int i = 0; i < arrayLength; i++) {
        array[i] -= avg;
        array[i] = array[i] * array[i];
        array[i] = array[i] * 1/arrayLength;
        sum += array[i];
    }

    return sum;
}

double bubbleSort(double *array, int arraySize) {
    for (int i = 0; i < arraySize - 1; i++) {
        for (int j = 0; j < arraySize - i - 1; j++) {
            if (array[j] > array[j + 1]) {
                double temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

//gets the Median for a given array
double getMedian(double *array, int arraySize) {
    bubbleSort(array, arraySize); //sort array

    if (arraySize % 2 == 0) {
        double firstValue = array[arraySize / 2];
        double secondValue = array[(arraySize / 2) + 1];

        return (firstValue+secondValue)/2;
    }
    return array[(int)(arraySize / 2 + 0.5)];
}

//prints a Histogram representing the given array.
//the biggest line has a size of <consoleWidth>
//the smallest line has a size of 0
void createHistogram(double *array, int arrayLength, int consoleWidth) {
    double max = getMaxFromArray(array, arrayLength);
    double min = getMinFromArray(array, arrayLength);
    double biggestDiff = max-min;

    printf("Histogram:\n-------\n");

    for (int i = 0; i < arrayLength; i++) {
        double ratioFromMinToMax = (array[i]-min)/biggestDiff;
        int visualSize = (int)round(consoleWidth * ratioFromMinToMax);

        printf("%03i: ", i);
        for (int j = 0; j < visualSize; j++) {
            printf("|");
        }

        printf(" : %f\n", array[i]);
    }
    printf("-------\n");
}

void createEntropy() {

}

int main() { //varianz test
    double arr[] = {25,36,37,28,100,30,94};

    createHistogram(arr, 7, 60);
}
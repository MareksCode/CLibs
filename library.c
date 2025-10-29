#include "library.h"

#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

//creates a new array picturing an entire sine wave multiplied by <amp>. The number of values is determined by <samplingRate> (how many values should be calculated)
double *createSineArray(double samplingRate, double amp) {
    double stepsize = 6.28 / samplingRate; //forced cast
    int length = samplingRate + 1;
    double *sineArray = calloc(length, sizeof(double));

    if (sineArray == NULL) {
        printf("calloc failed\n");
        return 0;
    }

    for (int i = 0; i < length; i+=1) {
        sineArray[i] = sin(i * stepsize) * amp;
    }

    return sineArray;
}

//Takes a pointer to an array and creates a savedArray.txt containing all the values
//Returns 1 when everything worked, 0 if there was an error
int createArrayFile(double *array, int arrayLength) {
    FILE *filePointer;

    filePointer = fopen("./savedArray.txt", "w");

    if (filePointer == NULL) {
        printf("Error creating new file\n");
        return 0;
    }

    for (int i = 0; i < arrayLength; i++) {
        fprintf(filePointer, "%f\n", array[i]);
    }

    fclose(filePointer);

    printf("File created\n");
    return 1;
}

//reads a "savedArray.txt" in the parent directory and returns a pointer to the parsed array
#include <stdio.h>
#include <stdlib.h>

double* readSavedArrayFile() {
    FILE *filePointer = fopen("./savedArray.txt", "r");
    if (filePointer == NULL) {
        printf("Could not open file\n");
        return NULL;
    }

    int length = 0;
    char ch;
    while ((ch = fgetc(filePointer)) != EOF) {
        if (ch == '\n')
            length++;
    }

    rewind(filePointer);

    double *savedArray = calloc(length, sizeof(double));
    if (savedArray == NULL) {
        printf("calloc failed\n");
        fclose(filePointer);
        return NULL;
    }

    char *currentString = NULL;
    int currentStringLength = 0;
    int currentDoubleInSavedArray = 0;

    while ((ch = fgetc(filePointer)) != EOF) {
        currentString = realloc(currentString, currentStringLength + 2);
        currentString[currentStringLength++] = ch;
        currentString[currentStringLength] = '\0';

        if (ch == '\n') {
            savedArray[currentDoubleInSavedArray++] = strtod(currentString, NULL);
            free(currentString);
            currentString = NULL;
            currentStringLength = 0;
        }
    }

    if (currentString != NULL) free(currentString);
    fclose(filePointer);
    return savedArray;
}

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
    int length = 6.28 / samplingRate; //forced cast
    length+=1; //make sure the entire sine is included

    double *sineArray = calloc(length, sizeof(double));

    if (sineArray == NULL) {
        printf("calloc failed\n");
        return 0;
    }

    for (int i = 0; i < length; i+=1) {
        sineArray[i] = sin(i * samplingRate) * amp;
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
double* readSavedArrayFile() {
    FILE *filePointer;

    FILE *pointerToBeginning = filePointer;

    filePointer = fopen("./savedArray.txt", "r");
    if (filePointer == NULL) {
        printf("Could not open file\n");
        return 0;
    }

    int length = 0; //count all doubles
    char ch;
    while ((ch = fgetc(filePointer)) != EOF) { //get all chars till end of file
        if (ch == '\n') {
            length += 1;
        }
    }

    double *savedArray = calloc(length, sizeof(double));
    if (savedArray == NULL) {
        printf("calloc failed\n");
        return 0;
    }

    filePointer = pointerToBeginning; //reset reading head

    char *currentString;
    int currentStringLength = 0;
    int currentDoubleInSavedArray = 0;
    while ((ch = fgetc(filePointer)) != EOF) { //get all chars till end of file
        currentString = realloc(currentString, currentStringLength + 1); //dynamically load new string of double
        currentString[currentStringLength] = ch;

        if (ch == '\n') { //end of the double was reached
            //parse string & put into finished array
            char *endPointer = &currentString[currentStringLength];
            savedArray[currentDoubleInSavedArray] = strtod(currentString, &endPointer);

            //reset string values
            free(currentString);
            currentString = 0;
        } else {
            currentStringLength += 1;
        }
    }

    fclose(filePointer);
    return savedArray;
}
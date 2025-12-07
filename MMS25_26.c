#include "MMS25_26.h"

#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Returns a linearly interpolated number between <x1> and <x2> using <alpha>
//Note: renam to scale
double interpolateDigitsByAlpha(double x1, double x2, double alpha) {
    if (alpha < 0 || alpha > 1) {
        printf("Wrong usage! Alpha must be between 0 and 1.\n");
        exit(3);
    }

    double diff = x2-x1;
    return (x1 + alpha*diff);
}

//linearly interpolates 2d points represented by arrays by <alpha>
double *interpolate2DPointsByAlpha(double *p1, double *p2, double alpha) {
    double *resultingPoint = calloc(2, sizeof(double));
    resultingPoint[0] = interpolateDigitsByAlpha(p1[0], p2[0], alpha);
    resultingPoint[1] = interpolateDigitsByAlpha(p1[1], p2[1], alpha);

    return resultingPoint;
}

//linearly interpolates 2d points represented by arrays using an x inbetween them
double *interpolate2DPointsWithX(double *p1, double *p2, double x) {
    double x1 = p1[0];
    double x2 = p2[0];

    double fullLength = fabs(x2 - x1);
    double alpha = 0;

    if (x1 <= x2) {
        alpha = (x-x1)/fullLength;
        return interpolate2DPointsByAlpha(p1,p2,alpha);
    } else {
        alpha = (x-x2)/fullLength;
        return interpolate2DPointsByAlpha(p2,p1,alpha);
    }
}

//Returns a y value between two points represented by p1 = (<x1>,<y1>) and p2 = (<x2>,<y2>) using x between <x1> and <x2>
double interpolateLine(double x1, double y1, double x2, double y2, double xb) {
    if (xb > x2 || xb < x1) {
        exit(6);
    }

    double smallerX = x1;
    if (x2 < x1) {
        smallerX = x2;
    }

    double alpha = (xb-smallerX)/fabs(x1-x2);

    return interpolateDigitsByAlpha(y1, y2, alpha);
}

//TODO: nochmal nachfragen
double *scaleValuesInArray(int numberOfValues, double *values, double min, double scalingFactor) {
    if (numberOfValues <= 0) {
        exit(7);
    }

    double minimumInArray = INFINITY;
    for (int i = 0; i< numberOfValues; i++) {
        if (values[i]<minimumInArray) {
            minimumInArray = values[i];
        }
    }

    double newScaleFactor = minimumInArray/min;

    printf("SCALEFACTOR: %f / %f", newScaleFactor, minimumInArray*newScaleFactor);

    for (int i = 0; i< numberOfValues; i++) {
        values[i]*= values[i] * newScaleFactor;
    }

    return values;
}

//TODO
double *createSineArray(int totalSamples, int samplesPerPeriod, double amplitude) {
    double stepsize = (3.1415*2) / samplingRate; //forced cast
    int length = samplingRate + 1;
    double *sineArray = calloc(length, sizeof(double));

    if (sineArray == NULL) {
        printf("calloc failed\n");
        exit(8);
    }

    for (int i = 0; i < length; i+=1) {
        sineArray[i] = sin(i * stepsize) * amp;
    }

    return sineArray;
}

//Takes a pointer to an array and creates a <filePath.txt> containing all the values
//Returns 1 when everything worked, 0 if there was an error
int writeArrayFile(char *filePath, double *array, int arrayLength) {
    FILE *filePointer;

    filePointer = fopen(filePath, "w");

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

typedef struct Node {
    char data;
    struct Node* next;
    struct Node* prev;
} Node;
typedef struct NumNode {
    double data;
    struct NumNode* next;
    struct NumNode* prev;
} NumNode;

//reads <filePath> and returns the array length of the passed array to the parsed numbersequence
int readArrayFile(char *fileName, double *value) {
    FILE *filePointer = fopen(filePath, "r");
    if (filePointer == NULL) {
        printf("Could not open file\n");
        return -1;
    }

    //Linkin Park list
    Node *numberReadHead = malloc(sizeof(Node));
    numberReadHead->next = NULL;
    numberReadHead->prev = NULL;

    NumNode *doubleArrayHead = malloc(sizeof(NumNode));
    doubleArrayHead->next = NULL;
    doubleArrayHead->prev = NULL;

    char ch;

    while ((ch = fgetc(filePointer)) != EOF) { //get all chars till end of file
        int currentNumberLength = 0;

        //create tmp node for the linked list that saves the current double string
        Node *newNode = malloc(sizeof(Node));
        newNode->data = ch;
        newNode->next = NULL;

        //get the current string length & the next node to add the char to
        Node *previousNode = numberReadHead;
        while (previousNode->next != NULL) {
            previousNode = previousNode->next;
            currentNumberLength+=1;
        }

        previousNode->next = newNode;
        newNode->prev = previousNode;

        //string finished? Parse string to double & reset the numberReadHead linked list
        if (ch == '\n') {
            //write chars in array
            char *doubleAsString = malloc(sizeof(char) * currentNumberLength);

            int i = 0;
            Node *iterator = numberReadHead;
            while (iterator->next != NULL) {
                iterator = iterator->next;
                doubleAsString[i] = iterator->data;
                i+=1;
            }

            //parse the string to a double and add it to the final linked list
            double parsedResult = strtod(doubleAsString, NULL);
            NumNode *lastDoubleArrayNode = doubleArrayHead;
            while (lastDoubleArrayNode->next != NULL) {
                lastDoubleArrayNode = lastDoubleArrayNode->next;
            }

            NumNode *newDoubleArrayNode = malloc(sizeof(NumNode));
            newDoubleArrayNode->data = parsedResult;
            newDoubleArrayNode->next = NULL;
            newDoubleArrayNode->prev = lastDoubleArrayNode;
            lastDoubleArrayNode->next = newDoubleArrayNode;

            free(doubleAsString);

            //reduce the linkedlist to its header again
            Node *nextNode = newNode;
            Node *deletingNode;
            while (nextNode->prev != NULL) {
                deletingNode = nextNode;
                nextNode = nextNode->prev;

                free(deletingNode);
            }
            numberReadHead->next = NULL;
        }
    }

    free(numberReadHead);

    //Create an array using the doubleArray linked list
    int arrayLength = 0;
    NumNode *iterator = doubleArrayHead;
    while (iterator->next != NULL) {
        arrayLength+=1;
        iterator = iterator->next;
    }

    values = realloc(values, sizeof(double) * arrayLength);

    if (values == NULL) {
        free(values);
        fclose(filePointer);
        printf("realloc failed\n");
        return -2;
    }

    //write in the array & delete linked list
    while (iterator->prev != NULL) {
        arrayLength-=1;
        values[arrayLength] = iterator->data;

        NumNode *thisNode = iterator;
        iterator = iterator->prev;

        free(thisNode);
    }

    free(doubleArrayHead);

    fclose(filePointer);
    return arrayLength;
}

int main() { //test for write & read file //TODO: REMOVE
    double *numbers = malloc(sizeof(double)*4);
    numbers[0] = 1.0;
    numbers[1] = 1.3;
    numbers[2] = 1.8;
    numbers[3] = 2.23;
    scaleValuesInArray(4, numbers, 0.5, 1);
    return 0;
}
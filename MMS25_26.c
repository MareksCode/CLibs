#include "MMS25_26.h"

#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Returns a linearly interpolated number between <x1> and <x2> using <alpha>
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
double getYForXInterpolatedBetween2Points(double x1, double y1, double x2, double y2, double x) {
    if (x > x2 || x < x1) {
        exit(6);
    }

    double smallerX = x1;
    if (x2 < x1) {
        smallerX = x2;
    }

    double alpha = (x-smallerX)/fabs(x1-x2);

    return interpolateDigitsByAlpha(y1, y2, alpha);
}

//Takes an array <numbers> with <numberArrayLength> as the length and scales every value by <alpha>. Every number gets set to <minimum> as soon as it's smaller
//For easy usability, it returns a pointer to the inputted array so you can wrap your array call
double *scaleNumbersInArray(double *numbers, int numberArrayLength, double minimum, double alpha) {
    if (numberArrayLength <= 0) {
        exit(7);
    }

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

//legacy function //TODO: REMOVE
double* readSavedArrayFileOLD() {
    FILE *filePointer = fopen("./savedArray.txt", "r");
    if (filePointer == NULL) {
        printf("Could not open file\n");
        return NULL;
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
        fclose(filePointer);
        return NULL;
    }

    rewind(filePointer);

    //Linkin Park list
    Node *head = malloc(sizeof(Node));
    head->next = NULL;
    head->prev = NULL;

    int doubleCounter = 0;

    while ((ch = fgetc(filePointer)) != EOF) { //get all chars till end of file
        int currentNumberLength = 0;

        Node *newNode = malloc(sizeof(Node));
        newNode->data = ch;
        newNode->next = NULL;

        Node *previousNode = head;
        while (previousNode->next != NULL) {
            previousNode = previousNode->next;
            currentNumberLength+=1;
        }

        previousNode->next = newNode;
        newNode->prev = previousNode;

        if (ch == '\n') {
            //write char in array
            char *doubleAsString = malloc(sizeof(char) * currentNumberLength);

            int i = 0;
            Node *iterator = head;
            do {
                //printf("char: %c\n", iterator->data);
                iterator = iterator->next;
                doubleAsString[i] = iterator->data;
                i+=1;
            } while (iterator->next != NULL); //to include the last char

            savedArray[doubleCounter] = strtod(doubleAsString, NULL);
            doubleCounter += 1;

            free(doubleAsString);

            //reduce the linkedlist to its header again
            Node *nextNode = newNode;
            Node *deletingNode;
            while (nextNode->prev != NULL) {
                deletingNode = nextNode;
                nextNode = nextNode->prev;

                free(deletingNode);
            }
            head->next = NULL;
        }
    }

    free(head);

    fclose(filePointer);
    return savedArray;
}

//reads a "savedArray.txt" in the parent directory and returns a pointer to the parsed numbersequence (put in an array)
double* readSavedArrayFile() {
    FILE *filePointer = fopen("./savedArray.txt", "r");
    if (filePointer == NULL) {
        printf("Could not open file\n");
        return NULL;
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

    double *savedArray = calloc(arrayLength, sizeof(double));

    //write in the array & delete linked list
    while (iterator->prev != NULL) {
        arrayLength-=1;
        savedArray[arrayLength] = iterator->data;

        NumNode *thisNode = iterator;
        iterator = iterator->prev;

        free(thisNode);
    }

    free(doubleArrayHead);

    fclose(filePointer);
    return savedArray;
}

int main() { //test for write & read file //TODO: REMOVE
    double array[] = {1.03,2.5,3.44,4.123,5,691,70,80.9999,9.999,10.1};
    createArrayFile(array, 10);
    double* a = readSavedArrayFile();
    for (int i = 0; i < 10; i++) {
        printf("%f\n", a[i]);
    }
    free(a);
    return 0;
}
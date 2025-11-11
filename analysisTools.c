#include "analysisTools.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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

typedef struct Node {
    double sample;
    int sampleCount;
    struct Node* next;
} Node;
typedef struct SampleBlock {
    double sample;
    int sampleCount;
} SampleBlock;

//prints a Histogram representing the given array.
//the biggest line has a size of <consoleWidth>
//the smallest line has a size of 0
void createVisualisation(SampleBlock *array, int arrayLength, int consoleWidth, double BiggestSample, double SmallestSample) {
    double biggestDiff = BiggestSample-SmallestSample;

    printf("Histogram:\n-------\n");

    for (int i = 0; i < arrayLength; i++) {
        double ratioFromMinToMax = (array[i].sampleCount-SmallestSample)/biggestDiff;
        int visualSize = (int)round(consoleWidth * ratioFromMinToMax);

        printf("%03f: ", array[i].sample);
        for (int j = 0; j < visualSize; j++) {
            printf("|");
        }

        printf(" : %d\n", array[i].sampleCount);
    }
    printf("-------\n");
}

void incrementSampleInLinkedList(Node *head, double sample) {
    Node *nextNode = head->next;
    Node *previousNode = head;
    while (nextNode != NULL) {
        if (nextNode->sample == sample) {
            nextNode->sampleCount++;
            return; //A member of the linkedlist was incremented
        }
        previousNode = nextNode;
        nextNode = nextNode->next;
    }

    //a new member has to be added
    Node *newNode = malloc(sizeof(Node));

    if (newNode == NULL) { //TODO: schauen ob hier noch der speicher der liste freigegeben werden muss
        printf("Malloc failed\n");
        exit(998);
    }

    previousNode->next = newNode;
    newNode->sample = sample;
    newNode->next = NULL;
    newNode->sampleCount = 1;
}
void createHistogram(double *array, int arrayLength, int consoleWidth) {
    Node *head = malloc(sizeof(Node));
    head->next = NULL;

    if (head == NULL) {
        printf("Malloc failed\n");
        exit(999);
    }

    printf("setting up linked list\n");
    //set up linked list
    for (int i = 0; i < arrayLength; i++) {
        incrementSampleInLinkedList(head, array[i]);
    }

    printf("converting\n");
    //convert linked list into an array
    int newArraySize = 0;
    Node *currentNode = head;
    while (currentNode != NULL) {
        currentNode = currentNode->next;
        newArraySize++;
    }
    SampleBlock *histogramArray = calloc(newArraySize, sizeof(SampleBlock));
    if (histogramArray == NULL) {
        printf("Calloc failed\n");
        exit(997);
    }

    printf("finding\n");
    currentNode = head;
    int i = 0;
    double biggestSample = -INFINITY;
    double smallestSample = INFINITY;
    while (currentNode != NULL) {
        currentNode = currentNode->next;
        if (currentNode != NULL) {
            histogramArray[i].sample = currentNode->sample;
            histogramArray[i].sampleCount = currentNode->sampleCount;
            i++;

            if (biggestSample < currentNode->sample) {
                biggestSample = currentNode->sample;
            }
            if (smallestSample > currentNode->sample) {
                smallestSample = currentNode->sample;
            }
        }
    }

    printf("clearing\n");
    //clear the linkedList
    Node *deletingNode = head;
    Node *nextNode = head->next;
    while (nextNode != NULL) {
        free(deletingNode);
        deletingNode = nextNode;
        nextNode = deletingNode->next;
    }
    free(deletingNode);

    //visualize
    printf("printing\n");
    createVisualisation(histogramArray, newArraySize, consoleWidth, biggestSample, smallestSample);

    //free
    free(histogramArray);
}

void createEntropy() {

}

int main() { //test
    double arr[] = {1,2,2,3,3,3,4,4,4,5,5,5,5,5,6,6,6,7,8,8,9};

    createHistogram(arr, 7, 60);
}
#include "MMS25_26.h"

#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

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
//TODO: Nachfragen, ob die funktion richtig angewendet wird (mit main methoden beispielen)
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

double *scaleValuesInArray(int numberOfValues, double *values, double min, double scalingFactor) {
    if (numberOfValues <= 0) {
        exit(7);
    }

    for (int i = 0; i< numberOfValues; i++) {
        values[i] = values[i] * scalingFactor - min;
    }

    return values;
}

double PI = 3.14159265359;
double *createSineArray(int totalSamples, int samplesPerPeriod, double amplitude) {
    double *sineArray = calloc(totalSamples, sizeof(double));

    if (sineArray == NULL) {
        printf("creating the sine array failed\n");
        exit(8);
    }

    for (int i = 0; i < totalSamples; i+=1) {
        sineArray[i] = sin(((double)i/(double)samplesPerPeriod)* PI*2); //2 PI is one period
    }

    return sineArray;
}

//Takes a pointer to an array and creates a <filePath.txt> containing all the values
//Returns 1 if everything worked, 0 if there was an error
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
int readArrayFile(char *fileName, double *values) {
    FILE *filePointer = fopen(fileName, "r");
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

MMSignal *createSignal_array(int numberOfValues, double *values) {
    MMSignal *newSignal = malloc(sizeof(MMSignal));

    newSignal->numberOfSamples = numberOfValues;
    newSignal->samples = values;

    newSignal->area = -1;
    newSignal->mean = -1;
    newSignal->localExtrema = NULL;

    return newSignal;
}

MMSignal *createSignal_file(char *fileName) {
    double *signalArray = malloc(sizeof(double));
    int numberOfSamples = readArrayFile(fileName, signalArray);

    return createSignal_array(numberOfSamples, signalArray);
}

void deleteMMSignal(MMSignal *In) {
    if (In == NULL) {
        return;
    }

    free(In->localExtrema);
    free(In->samples);

    free(In);
}
void writeSignal(MMSignal *In, char *fileName) {
    writeArrayFile(fileName, In->samples, In->numberOfSamples);
}

MMSignal *createSineSignal(int totalSamples, int samplesPerPeriod, double amplitude) {
    MMSignal *newSignal = createSignal_array(totalSamples, createSineArray(totalSamples, samplesPerPeriod, amplitude));
    return newSignal;
}

// A2

int *getHistogram(int numberOfValues, double *values, int numberOfBins)
{
    if (numberOfValues <= 0 || numberOfBins <= 0 || values == NULL) {
        exit(-99);
    }

    int *histogram = calloc(numberOfBins, sizeof(int));
    if (histogram == NULL) {
        exit(-98);
    }

    double min = DBL_MAX;
    double max = -DBL_MAX;

    for (int i = 0; i < numberOfValues; i++) {
        if (values[i] < min) {
            min = values[i];
        }
        if (values[i] > max) {
            max = values[i];
        }
    }

    //wenn alle Werte gleich
    if (min == max) {
        histogram[0] = numberOfValues;
        return histogram;
    }

    double binWidth = (max - min) / numberOfBins;

    for (int i = 0; i < numberOfValues; i++) {
        int bin = (int)((values[i] - min) / binWidth);

        if (bin == numberOfBins) {
            bin = numberOfBins - 1;
        }

        histogram[bin]++;
    }

    return histogram;
}

Histogram *createHistogram_empty()
{
    Histogram *h = malloc(sizeof(Histogram));
    if (h == NULL) {
        exit(-98);
    }

    h->numberOfBins = -1;
    h->bins = NULL;
    h->minimum = 0;
    h->maximum = 0;
    h->binWidth = 0;

    return h;
}

Histogram *createHistogram_bins(int numberOfBins)
{
    if (numberOfBins <= 0) {
        exit(-97);
    }

    Histogram *h = createHistogram_empty(); //error handling schon durch exit in createHistogram_empty

    h->numberOfBins = numberOfBins;
    h->bins = calloc(numberOfBins, sizeof(int));

    if (h->bins == NULL) {
        free(h);
        exit(-96);
    }

    return h;
}

Histogram *createHistogram_array(int numberOfValues, double *values, int numberOfBins)
{
    if (numberOfValues <= 0 || numberOfBins <= 0 || values == NULL) {
        exit(-95);
    }

    Histogram *h = createHistogram_empty(); //error handling schon durch exit in createHistogram_empty

    double min = DBL_MAX;
    double max = -DBL_MAX;

    for (int i = 0; i < numberOfValues; i++) {
        if (values[i] < min) {
            min = values[i];
        }
        if (values[i] > max) {
            max = values[i];
        }
    }

    h->minimum = min;
    h->maximum = max;
    h->numberOfBins = numberOfBins;

    h->bins = getHistogram(numberOfValues, values, numberOfBins);

    if (min == max) {
        h->binWidth = 0.0;
    } else {
        h->binWidth = (max - min) / numberOfBins;
    }

    return h;
}

void deleteHistogram(Histogram *In)
{
    if (In == NULL) {
        return;
    }

    free(In->bins);
    free(In);
}

double computeArea(MMSignal *In)
{
    if (In == NULL) {
        exit(-94);
    }

    if (In->samples == NULL || In->numberOfSamples <= 0) {
        exit(-93);
    }

    double area = 0;

    for (int i = 0; i < In->numberOfSamples; i++) {
        area += In->samples[i];
    }

    In->area = area;
    return area;
}

double computeMean(MMSignal *In)
{
    if (In == NULL) {
        exit(-92);
    }

    if (In->samples == NULL || In->numberOfSamples <= 0) {
        exit(-91);
    }

    In->mean = computeArea(In) / In->numberOfSamples;
    return In->mean;
}

double computeStandardDeviation(MMSignal *In)
{
    if (In == NULL) {
        exit(-90);
    }

    if (In->samples == NULL || In->numberOfSamples <= 0) {
        exit(-89);
    }

    double mean = computeMean(In);

    double sum = 0;

    for (int i = 0; i < In->numberOfSamples; i++) {
        double diff = In->samples[i] - mean;
        sum += diff * diff;
    }

    return sqrt(sum / In->numberOfSamples);
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

double computeMedian(MMSignal *In)
{
    if (In == NULL) {
        exit(-88);
    }

    if (In->samples == NULL || In->numberOfSamples <= 0) {
        exit(-87);
    }

    int n = In->numberOfSamples;

    double *tmp = malloc(n * sizeof(double));
    if (tmp == NULL) {
        exit(-86);
    }

    for (int i = 0; i < n; i++) {
        tmp[i] = In->samples[i];
    }

    bubbleSort(tmp, n);

    double median;

    if (n % 2 == 0) {
        median = (tmp[n / 2 - 1] + tmp[n / 2]) / 2;
    } else {
        median = tmp[n / 2];
    }

    free(tmp);
    return median;
}

//TODO: sattelstelle?
LocalExtrema *computeExtrema(MMSignal *In) {
    LocalExtrema *newExtrema = malloc(sizeof(LocalExtrema));
    newExtrema->numberOfMinimumPositions = 0;
    newExtrema->numberOfMaximumPositions = 0;
    newExtrema->maximumPositionArray = malloc(sizeof(int) * BLOCK_SIZE);
    newExtrema->minimumPositionArray = malloc(sizeof(int) * BLOCK_SIZE);

    int currentMaxArraySize = BLOCK_SIZE;
    int currentMinArraySize = BLOCK_SIZE;

    for (int i = 1; i < In->numberOfSamples-1; i++) {
        if (In->samples[i - 1] < In->samples[i] && In->samples[i + 1] < In->samples[i]) {
            newExtrema->numberOfMinimumPositions+=1;

            if (newExtrema->numberOfMinimumPositions > currentMaxArraySize) {
                newExtrema->minimumPositionArray = realloc(newExtrema->minimumPositionArray, sizeof(int) * currentMaxArraySize);

                if (newExtrema->minimumPositionArray == NULL) {
                    exit(-85);
                }

                currentMaxArraySize += BLOCK_SIZE;
            }

            newExtrema->maximumPositionArray[newExtrema->numberOfMinimumPositions - 1] = i;
        } else if (In->samples[i - 1] > In->samples[i] && In->samples[i + 1] > In->samples[i]) {
            newExtrema->numberOfMaximumPositions+=1;

            if (newExtrema->numberOfMaximumPositions > currentMinArraySize) {
                newExtrema->maximumPositionArray = realloc(newExtrema->maximumPositionArray, sizeof(int) * currentMinArraySize);

                if (newExtrema->maximumPositionArray == NULL) {
                    exit(-84);
                }

                currentMinArraySize += BLOCK_SIZE;
            }

            newExtrema->maximumPositionArray[newExtrema->numberOfMaximumPositions - 1] = i;
        }
    }

    return newExtrema;
}

int main() {
    //test for median //TODO: REMOVE
    double arr[] = {1,3,3,5,7,8,10};
    int i = 7;

    printf("%f", computeMedian(createSignal_array(i, arr)));

    free(arr);
    return 0;
}
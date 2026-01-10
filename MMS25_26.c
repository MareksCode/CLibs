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
    double diff = x2 - x1;
    return (x1 + alpha * diff);
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
        alpha = (x - x1) / fullLength;
        return interpolate2DPointsByAlpha(p1, p2, alpha);
    } else {
        alpha = (x - x2) / fullLength;
        return interpolate2DPointsByAlpha(p2, p1, alpha);
    }
}

//Returns a y value between two points represented by p1 = (<x1>,<y1>) and p2 = (<x2>,<y2>) using x between <x1> and <x2>
//TODO: Nachfragen, ob die funktion richtig angewendet wird (mit main methoden beispielen)
double interpolateLine(double x1, double y1, double x2, double y2, double xb) {
    if (xb > x2 || xb < x1) {
        exit(4);
    }
    if (x1 == x2) {
        exit(5);
    }

    double smallerX = x1;
    if (x2 < x1) {
        smallerX = x2;
    }

    double alpha = (xb - smallerX) / fabs(x1 - x2);

    return interpolateDigitsByAlpha(y1, y2, alpha);
}

double *scaleValuesInArray(int numberOfValues, double *values, double min, double scalingFactor) {
    if (numberOfValues <= 0) {
        exit(6);
    }

    for (int i = 0; i < numberOfValues; i++) {
        values[i] = values[i] * scalingFactor - min;
    }

    return values;
}

double PI = 3.14159265359;

double *createSineArray(int totalSamples, int samplesPerPeriod, double amplitude) {
    double *sineArray = calloc(totalSamples, sizeof(double));

    if (sineArray == NULL) {
        printf("creating the sine array failed\n");
        exit(7);
    }

    for (int i = 0; i < totalSamples; i += 1) {
        sineArray[i] = sin(((double) i / (double) samplesPerPeriod) * PI * 2) * amplitude; //2 PI is one period
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
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct NumNode {
    double data;
    struct NumNode *next;
    struct NumNode *prev;
} NumNode;

//reads <filePath> and returns the array length of the passed array to the parsed numbersequence
double *readArrayFile(char *fileName, int *arrayLength) {
    FILE *filePointer = fopen(fileName, "r");
    if (filePointer == NULL) {
        printf("Could not open file\n");
        exit(2);
    }

    Node *numberReadHead = malloc(sizeof(Node));
    numberReadHead->next = NULL;
    numberReadHead->prev = NULL;

    NumNode *doubleArrayHead = malloc(sizeof(NumNode));
    doubleArrayHead->next = NULL;
    doubleArrayHead->prev = NULL;

    int ch; //fgetc braucht int
    while ((ch = fgetc(filePointer)) != EOF) {
        int currentNumberLength = 0;

        Node *newNode = malloc(sizeof(Node));
        newNode->data = (char) ch; //int zurück zu char
        newNode->next = NULL;

        Node *previousNode = numberReadHead;
        while (previousNode->next != NULL) {
            previousNode = previousNode->next;
            currentNumberLength++;
        }

        previousNode->next = newNode;
        newNode->prev = previousNode;

        if (ch == '\n') {
            char *doubleAsString = malloc(currentNumberLength + 1); //+1 für '\0'

            int i = 0;
            Node *iterator = numberReadHead;
            while (iterator->next != NULL) {
                iterator = iterator->next;
                doubleAsString[i++] = iterator->data;
            }
            doubleAsString[i - 1] = '\0'; //wegen stringdarstellung in c

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

            Node *n = numberReadHead->next;
            while (n != NULL) {
                Node *tmp = n;
                n = n->next;
                free(tmp);
            }
            numberReadHead->next = NULL;
        }
    }

    free(numberReadHead);

    //Länge bestimmen
    *arrayLength = 0;
    NumNode *iterator = doubleArrayHead;
    while (iterator->next != NULL) {
        (*arrayLength)++;
        iterator = iterator->next;
    }

    double *values = malloc(sizeof(double) * (*arrayLength));
    if (values == NULL) {
        fclose(filePointer);
        free(doubleArrayHead);
        exit(2);
    }

    int i = *arrayLength;
    while (iterator != doubleArrayHead) {
        i--;
        values[i] = iterator->data;

        NumNode *tmp = iterator;
        iterator = iterator->prev;
        free(tmp);
    }

    free(doubleArrayHead);
    fclose(filePointer);
    return values;
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
    int arraySize = 0;
    double *signalArray = readArrayFile(fileName, &arraySize);

    return createSignal_array(arraySize, signalArray);
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

int *getHistogram(int numberOfValues, double *values, int numberOfBins) {
    if (numberOfValues <= 0 || numberOfBins <= 0 || values == NULL) {
        exit(8);
    }

    int *histogram = calloc(numberOfBins, sizeof(int));
    if (histogram == NULL) {
        exit(9);
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
        int bin = (int) ((values[i] - min) / binWidth);

        if (bin == numberOfBins) {
            bin = numberOfBins - 1;
        }

        histogram[bin]++;
    }

    return histogram;
}

Histogram *createHistogram_empty() {
    Histogram *h = malloc(sizeof(Histogram));
    if (h == NULL) {
        exit(10);
    }

    h->numberOfBins = -1;
    h->bins = NULL;
    h->minimum = 0;
    h->maximum = 0;
    h->binWidth = 0;

    return h;
}

Histogram *createHistogram_bins(int numberOfBins) {
    if (numberOfBins <= 0) {
        exit(11);
    }

    Histogram *h = createHistogram_empty(); //error handling schon durch exit in createHistogram_empty

    h->numberOfBins = numberOfBins;
    h->bins = calloc(numberOfBins, sizeof(int));

    if (h->bins == NULL) {
        free(h);
        exit(12);
    }

    return h;
}

Histogram *createHistogram_array(int numberOfValues, double *values, int numberOfBins) {
    if (numberOfValues <= 0 || numberOfBins <= 0 || values == NULL) {
        exit(13);
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

void deleteHistogram(Histogram *In) {
    if (In == NULL) {
        return;
    }

    free(In->bins);
    free(In);
}

double computeArea(MMSignal *In) {
    if (In == NULL) {
        exit(14);
    }

    if (In->samples == NULL || In->numberOfSamples <= 0) {
        exit(15);
    }

    double area = 0;

    for (int i = 0; i < In->numberOfSamples; i++) {
        area += In->samples[i];
    }

    In->area = area;
    return area;
}

double computeMean(MMSignal *In) {
    if (In == NULL) {
        exit(16);
    }

    if (In->samples == NULL || In->numberOfSamples <= 0) {
        exit(17);
    }

    In->mean = computeArea(In) / In->numberOfSamples;
    return In->mean;
}

double computeStandardDeviation(MMSignal *In) {
    if (In == NULL) {
        exit(18);
    }

    if (In->samples == NULL || In->numberOfSamples <= 0) {
        exit(19);
    }

    double mean = computeMean(In);

    double sum = 0;

    for (int i = 0; i < In->numberOfSamples; i++) {
        double diff = In->samples[i] - mean;
        sum += diff * diff;
    }

    return sqrt(sum / In->numberOfSamples);
}

void bubbleSort(double *array, int arraySize) {
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

double computeMedian(MMSignal *In) {
    if (In == NULL) {
        exit(20);
    }

    if (In->samples == NULL || In->numberOfSamples <= 0) {
        exit(21);
    }

    int n = In->numberOfSamples;

    double *tmp = malloc(n * sizeof(double));
    if (tmp == NULL) {
        exit(22);
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

static void appendIndex(int **indexArray, int *numberOfEntries, int *capacityInInts, int index) {
    if (*numberOfEntries >= *capacityInInts) {
        *capacityInInts += BLOCK_SIZE / sizeof(int);

        *indexArray = realloc(*indexArray,
                              (*capacityInInts) * sizeof(int));
        if (!*indexArray) {
            exit(42);
        }
    }

    (*indexArray)[(*numberOfEntries)++] = index;
}

LocalExtrema *computeExtrema(MMSignal *signal) {
    LocalExtrema *extrema = malloc(sizeof(*extrema));

    extrema->numberOfMaximumPositions = 0;
    extrema->numberOfMinimumPositions = 0;

    int maximumCapacityInInts = BLOCK_SIZE / sizeof(int);
    int minimumCapacityInInts = BLOCK_SIZE / sizeof(int);

    extrema->maximumPositionArray = malloc(BLOCK_SIZE);
    extrema->minimumPositionArray = malloc(BLOCK_SIZE);

    for (int sampleIndex = 1; sampleIndex < signal->numberOfSamples - 1;) {
        double leftSample = signal->samples[sampleIndex - 1];
        double centerSample = signal->samples[sampleIndex];
        double rightSample = signal->samples[sampleIndex + 1];

        //maximum
        if (leftSample < centerSample &&
            centerSample > rightSample) {
            appendIndex(&extrema->maximumPositionArray,
                        &extrema->numberOfMaximumPositions,
                        &maximumCapacityInInts,
                        sampleIndex);
            sampleIndex++;
            continue;
        }

        //minimum
        if (leftSample > centerSample &&
            centerSample < rightSample) {
            appendIndex(&extrema->minimumPositionArray,
                        &extrema->numberOfMinimumPositions,
                        &minimumCapacityInInts,
                        sampleIndex);
            sampleIndex++;
            continue;
        }

        //plateau
        if (centerSample == rightSample) {
            int plateauStartIndex = sampleIndex;

            while (sampleIndex < signal->numberOfSamples - 1 &&
                   signal->samples[sampleIndex] ==
                   signal->samples[sampleIndex + 1]) {
                sampleIndex++;
            }

            int plateauEndIndex = sampleIndex;

            if (plateauStartIndex > 0 &&
                plateauEndIndex < signal->numberOfSamples - 1) {
                int leftNeighborSample =
                        signal->samples[plateauStartIndex - 1];
                int rightNeighborSample =
                        signal->samples[plateauEndIndex + 1];

                if (leftNeighborSample < centerSample &&
                    centerSample > rightNeighborSample) {
                    for (int p = plateauStartIndex;
                         p <= plateauEndIndex; p++)
                        appendIndex(&extrema->maximumPositionArray,
                                    &extrema->numberOfMaximumPositions,
                                    &maximumCapacityInInts,
                                    p);
                } else if (leftNeighborSample > centerSample &&
                           centerSample < rightNeighborSample) {
                    for (int p = plateauStartIndex;
                         p <= plateauEndIndex; p++)
                        appendIndex(&extrema->minimumPositionArray,
                                    &extrema->numberOfMinimumPositions,
                                    &minimumCapacityInInts,
                                    p);
                }
            }
        }

        sampleIndex++;
    }

    return extrema;
}


double computeEntropy(Histogram *histogramIn) {
    if (histogramIn == NULL || histogramIn->bins == NULL || histogramIn->numberOfBins <= 0) {
        exit(25);
    }

    double entropy = 0;
    double totalValues = 0;

    for (int i = 0; i < histogramIn->numberOfBins; i += 1) {
        totalValues += histogramIn->bins[i];
    }

    //relative Häufigkeit berechnen
    for (int i = 0; i < histogramIn->numberOfBins; i += 1) {
        if (histogramIn->bins[i] > 0) {
            double probability = (double) histogramIn->bins[i] / totalValues;
            entropy = entropy - probability * log2(probability);
        }
    }
    return entropy;
}

// A 3
MMSignal *convoluteSignals(MMSignal *In1, MMSignal *In2) {
    if (In1 == NULL || In2 == NULL || In1->samples == NULL || In2->samples == NULL || In1->numberOfSamples <= 0 || In2->numberOfSamples <= 0) {
        exit(26);
    }

    int length = In1->numberOfSamples + In2->numberOfSamples - 1;
    double *result = malloc(length * sizeof(double));

    if (result == NULL) {
        exit(27);
    }

    for (int i = 0; i < length; i += 1) {
        result[i] = 0;
    }

    for (int i = 0; i < In1->numberOfSamples; i += 1) {
        for (int j = 0; j < In2->numberOfSamples; j += 1) {
            result[i + j] = result[i + j] + In1->samples[i] * In2->samples[j];
        }
    }

    MMSignal *resultSignal = createSignal_array(length, result);
    if (resultSignal == NULL) {
        free(result);
        exit(28);
    }

    return resultSignal;
}

MMSignal *approximateGaussianBellCurve(int pascalLineNumber) {
    if (pascalLineNumber < 1) {
        exit(29);
    }

    double *values = malloc(pascalLineNumber * sizeof(double));
    if (values == NULL) {
        exit(30);
    }

    //pascalzeile berechnen
    values[0] = 1;
    for (int i = 1; i < pascalLineNumber; i++) {
        values[i] = values[i - 1]
                  * (double)(pascalLineNumber - i)
                  / (double)i;
    }

    //normierung (Summe = 1 → Gauss-Approximation)
    double sum = 0;
    for (int i = 0; i < pascalLineNumber; i++) {
        sum += values[i];
    }

    for (int i = 0; i < pascalLineNumber; i++) {
        values[i] /= sum;
    }

    MMSignal *gaussSignal = createSignal_array(pascalLineNumber, values);
    return gaussSignal;
}

// A 4

void getCartesianToPolar(int numberOfValues, double *realIn, double *imaginaryIn, double *amplitudesOut, double *angelsOut) {
    if (realIn == NULL || imaginaryIn == NULL || amplitudesOut == NULL || angelsOut == NULL || numberOfValues <= 0) {
        exit(31);
    }

    for (int i = 0; i < numberOfValues; i++) {
        amplitudesOut[i] = sqrt(realIn[i] * realIn[i] + imaginaryIn[i] * imaginaryIn[i]); // Radius
        angelsOut[i] = atan2(imaginaryIn[i], realIn[i]); // Winkel
    }
}

void getPolarToCartesian(int numberOfValues, double *amplitudesIn, double *angelsIn, double *realOut,
                         double *imaginaryOut) {
    if (amplitudesIn == NULL || angelsIn == NULL || realOut == NULL || imaginaryOut == NULL || numberOfValues <= 0) {
        exit(32);
    }

    for (int i = 0; i < numberOfValues; i++) {
        realOut[i] = amplitudesIn[i] * cos(angelsIn[i]); //Realteil
        imaginaryOut[i] = amplitudesIn[i] * sin(angelsIn[i]); //Imaginärteil
    }
}

void dft(int numberOfValues, double *realIn, double *imaginaryIn, double *realOut, double *imaginaryOut, int Direction) {
    for (int k = 0; k < numberOfValues; k++) {
        realOut[k] = 0.0;
        imaginaryOut[k] = 0.0;
        for (int n = 0; n < numberOfValues; n++) {
            double angle = Direction * 2 * PI * k * n / numberOfValues;
            realOut[k] += realIn[n] * cos(angle) + imaginaryIn[n] * sin(angle);
            imaginaryOut[k] += -realIn[n] * sin(angle) + imaginaryIn[n] * cos(angle);
        }
    }
}
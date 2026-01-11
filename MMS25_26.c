#include "MMS25_26.h"

#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

//***** Constants: *****

double PI = 3.14159265359;

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

//***** Helper functions: *****

//Returns a linearly interpolated number between <x1> and <x2> using <alpha>
double interpolateDigitsByAlpha(double x1, double x2, double alpha) {
    if (alpha < 0 || alpha > 1) {
        printf("Wrong usage! Alpha must be between 0 and 1.\n");
        exit(3);
    }
    double diff = x2 - x1;
    return x1 + alpha * diff;
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

//***** Header function implementations: *****

//Returns a y value between two points represented by p1 = (<x1>,<y1>) and p2 = (<x2>,<y2>) using x between <x1> and <x2>
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

//using the formula value * scalingfactor - min
double *scaleValuesInArray(int numberOfValues, double *values, double min, double scalingFactor) {
    if (numberOfValues <= 0 || !values) {
        exit(6);
    }

    for (int i = 0; i < numberOfValues; i++) {
        values[i] = values[i] * scalingFactor - min;
    }

    return values;
}

//creates a sine array of the length <totalSamples>.
double *createSineArray(int totalSamples, int samplesPerPeriod, double amplitude) {
    double *sineArray = calloc(totalSamples, sizeof(double));

    if (sineArray == NULL || totalSamples <= 0 || samplesPerPeriod <= 0 || amplitude <= 0) {
        printf("creating the sine array failed\n");
        exit(7);
    }

    for (int i = 0; i < totalSamples; i += 1) {
        sineArray[i] = sin(((double) i / (double) samplesPerPeriod) * PI * 2) * amplitude; //2 PI is one period
    }

    return sineArray;
}

//Takes a pointer to an array and creates a <filePath> containing all the values
//Returns 1 if everything worked, 0 if there was an error
int writeArrayFile(char *filePath, double *array, int arrayLength) {
    if (!filePath || !array || arrayLength <= 0) {
        exit(2);
    }

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

//reads <filePath> and returns the array length of the passed array to the parsed numbersequence
double *readArrayFile(char *fileName, int *arrayLength) {
    if (!fileName || !arrayLength) {
        exit(2);
    }

    //Initialize everything with NULL for the cleanup
    FILE *filePointer = NULL;
    Node *numberReadHead = NULL;
    NumNode *doubleArrayHead = NULL;
    double *values = NULL;

    filePointer = fopen(fileName, "r");
    if (!filePointer) {
        printf("Could not open file\n");
        exit(2);
    }

    //Node for parsing a number:
    // [head] -> <char>[1] -> <char>[.] -> <char>[2]
    numberReadHead = malloc(sizeof(Node));
    if (!numberReadHead) {
        goto cleanup;
    }
    numberReadHead->next = NULL;
    numberReadHead->prev = NULL;

    //Node for the numbers:
    // [head] -> <double>[x1] -> <double>[x2] -> <double>[x3]
    doubleArrayHead = malloc(sizeof(NumNode));
    if (!doubleArrayHead) {
        goto cleanup;
    }
    doubleArrayHead->next = NULL;
    doubleArrayHead->prev = NULL;

    int ch; //This is an int because fgetc returns characters interpreted as integers; EOF is interpreted as -1
    int currentNumberLength = 0; //Counter for the current numberLength
    while ((ch = fgetc(filePointer)) != EOF) { //Loop through the file
        //Determine if the number contained in the linked list is finished
        if (ch == '\n') { // CASE: it's finished
            // 1) Allocate space for the double as a string
            char *doubleAsString = malloc(currentNumberLength + 1);
            if (!doubleAsString) {
                goto cleanup;
            }

            // 2) Write the data of the numberRead linked list into the newly created array
            int i = 0;
            Node *iterator = numberReadHead;
            while (iterator->next) {
                iterator = iterator->next;
                doubleAsString[i++] = iterator->data;
            }
            doubleAsString[i - 1] = '\0'; //A string has to end with \0

            // 3) Parse the string to a double value
            double parsedResult = strtod(doubleAsString, NULL);

            // 4) Free the allocated array
            free(doubleAsString);

            // 5) Get last instance of the linked list containing the final doubles
            NumNode *last = doubleArrayHead;
            while (last->next) {
                last = last->next;
            }

            // 6) Create a new double node & push it to the final linked list
            NumNode *newDouble = malloc(sizeof(NumNode));
            if (!newDouble) {
                goto cleanup;
            }

            newDouble->data = parsedResult;
            newDouble->next = NULL;
            newDouble->prev = last;
            last->next = newDouble;

            // 7) Reset/Clear the current number linked list
            Node *n = numberReadHead->next;
            while (n) {
                Node *tmp = n;
                n = n->next;
                free(tmp);
            }
            numberReadHead->next = NULL;

            // 8) Reset the counter
            currentNumberLength = 0;
        }
        else { //CASE: It's not finished
            // 1) Create a new node for the newly read character
            Node *newNode = malloc(sizeof(Node));
            if (!newNode) {
                goto cleanup;
            }

            newNode->data = (char) ch; //Interpret the int as a char
            newNode->next = NULL;

            // 2) Get the end of the current numberRead Linked List
            Node *previousNode = numberReadHead;
            while (previousNode->next) {
                previousNode = previousNode->next;
            }

            // 3) Add the numberRead node to the linked list
            previousNode->next = newNode;
            newNode->prev = previousNode;

            // 4) Increment the current number length
            currentNumberLength++;
        }
    }

    if (numberReadHead->next) { //Case: File doesnt end with \n; Attack the user with an exit()
        goto cleanup;
    }

    //Get&Set final array length
    *arrayLength = 0;
    NumNode *it = doubleArrayHead->next;
    while (it) {
        (*arrayLength)++;
        it = it->next;
    }

    //Create the array
    values = malloc(sizeof(double) * (*arrayLength));
    if (!values) {
        goto cleanup;
    }

    //Write into the array & free the linked list
    it = doubleArrayHead->next;
    for (int i = 0; i < *arrayLength; i++) {
        values[i] = it->data;
        NumNode *tmp = it;
        it = it->next;
        free(tmp);
    }

    //Free the rest
    free(doubleArrayHead);
    fclose(filePointer);
    free(numberReadHead);
    return values;

    // ----------- CLEANUP ---------------------------------------------------
    cleanup:
        if (numberReadHead) {
            Node *n = numberReadHead->next;
            while (n) {
                Node *tmp = n;
                n = n->next;
                free(tmp);
            }
            free(numberReadHead);
        }
        if (doubleArrayHead) {
            NumNode *m = doubleArrayHead->next;
            while (m) {
                NumNode *tmp = m;
                m = m->next;
                free(tmp);
            }
          free(doubleArrayHead);
        }

        if (filePointer) {
           fclose(filePointer);
        }

        free(values);
        exit(2);
}

MMSignal *createSignal_array(int numberOfValues, double *values) {
    if (numberOfValues <= 0 || !values) {
        exit(2);
    }

    MMSignal *newSignal = malloc(sizeof(MMSignal));

    newSignal->numberOfSamples = numberOfValues;
    newSignal->samples = values;

    newSignal->area = -1;
    newSignal->mean = -1;
    newSignal->localExtrema = NULL;

    return newSignal;
}

MMSignal *createSignal_file(char *fileName) {
    if (!fileName) {
        exit(2);
    }

    int arraySize = 0;
    double *signalArray = readArrayFile(fileName, &arraySize);

    return createSignal_array(arraySize, signalArray);
}

void deleteMMSignal(MMSignal *In) {
    if (In == NULL) {
        exit(2);
    }

    free(In->localExtrema);
    free(In->samples);

    free(In);
}

void writeSignal(MMSignal *In, char *fileName) {
    if (!In || !fileName) {
        exit(2);
    }

    writeArrayFile(fileName, In->samples, In->numberOfSamples);
}

MMSignal *createSineSignal(int totalSamples, int samplesPerPeriod, double amplitude) {
    if (totalSamples <= 0 || samplesPerPeriod <= 0 || amplitude <= 0) {
        exit(2);
    }
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
        h->binWidth = 0;
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

LocalExtrema *computeExtrema(MMSignal *signal) {
    if (!signal) {
        exit(2);
    }

    LocalExtrema *extrema = malloc(sizeof(*extrema));

    if (!extrema) {
        exit(2);
    }

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
                double leftNeighborSample =
                        signal->samples[plateauStartIndex - 1];
                double rightNeighborSample =
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
    if (pascalLineNumber < 0) {
        exit(29);
    }

    double *values = malloc((pascalLineNumber+1) * sizeof(double));
    if (values == NULL) {
        exit(30);
    }

    //pascalzeile berechnen
    values[0] = 1;
    for (int i = 1; i < pascalLineNumber+1; i++) {
        values[i] = values[i - 1]
                  * (double)(pascalLineNumber+1 - i)
                  / (double)i;
    }

    //normierung (Summe = 1 → Gauss-Approximation)
    double sum = 0;
    for (int i = 0; i < pascalLineNumber+1; i++) {
        sum += values[i];
    }

    for (int i = 0; i < pascalLineNumber+1; i++) {
        values[i] /= sum;
    }

    MMSignal *gaussSignal = createSignal_array(pascalLineNumber+1, values);
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

void getPolarToCartesian(int numberOfValues, double *amplitudesIn, double *angelsIn, double *realOut, double *imaginaryOut) {
    if (amplitudesIn == NULL || angelsIn == NULL || realOut == NULL || imaginaryOut == NULL || numberOfValues <= 0) {
        exit(32);
    }

    for (int i = 0; i < numberOfValues; i++) {
        realOut[i] = amplitudesIn[i] * cos(angelsIn[i]); //Realteil
        imaginaryOut[i] = amplitudesIn[i] * sin(angelsIn[i]); //Imaginärteil
    }
}

void dft(int numberOfValues, double *realIn, double *imaginaryIn, double *realOut, double *imaginaryOut, int Direction) {
    if(realIn == NULL || imaginaryIn == NULL || realOut == NULL || imaginaryOut == NULL || numberOfValues <= 0) {
        exit(33);
    }
    if (Direction != 1 && Direction != -1) {
        exit(34);
    }
    for (int k = 0; k < numberOfValues; k++) {
        realOut[k] = 0.0;
        imaginaryOut[k] = 0.0;
        for (int n = 0; n < numberOfValues; n++) {
            double angle = Direction * 2 * PI * k * n / numberOfValues;
            realOut[k] += realIn[n] * cos(angle) + imaginaryIn[n] * sin(angle);
            imaginaryOut[k] += -realIn[n] * sin(angle) + imaginaryIn[n] * cos(angle);
        }
    }
    if(Direction == -1) {
        for (int k = 0; k < numberOfValues; k++) {
            realOut[k] /= numberOfValues;
            imaginaryOut[k] /= numberOfValues;
        }
    }
}
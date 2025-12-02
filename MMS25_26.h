#ifndef MMS25_26
#define MMS25_26
#define BLOCK_SIZE 500

typedef struct {
    int numberOfBins;//-1
    int *bins;//0
    double minimum;//0
    double maximum;//0
    double binWidth;//0
} Histogram;

typedef struct {
    int minimumPositionArrayLength;
    int *minimumPositionArray;
    int maximumPositionArrayLength;
    int *maximumPositionArray;
} LocalExtrema;

typedef struct {
    int numberOfSamples;
    double *samples;
    double area;
    double mean;
    LocalExtrema *localExtrema;
} MMSignal;

//AUFGABE 1
double interpolateLine(double x1, double y1, double x2, double y2, double xb);
double *scaleValuesInArray(int valueArrayLength, double *values, double min, double scalingFactor);
double *createSineArray(int totalSamples, int samplesPerPeriod, double amplitude);
int writeArrayFile(char *filePath, double *array, int arrayLength);
int readArrayFile(char *fileName, double *value);
MMSignal *createSignal_array(int valueArrayLength, double *values);
MMSignal *createSignal_file(char *fileName);
void deleteMMSignal(MMSignal *In);
void writeSignal(MMSignal *In, char * fileName);
MMSignal *createSineSignal(int totalSamples, int samplesPerPeriod, double amplitude);

//AUFGABE 2
int *getHistogram(int numberOfValues, double *values, int numberOfBins);
Histogram *createHistogram_empty();
Histogram *createHistogram_bins(int numberOfBins);
Histogram *createHistogram_array(int numberOfValues, double *values, int numberOfBins);
void deleteHistogram(Histogram *In);
double computeArea(MMSignal *In);
double computeMean(MMSignal *In);
double computeStandardDeviation(MMSignal *In);
double computeMedian(MMSignal *In);
LocalExtrema *computeExtrema(MMSignal *In);
double computeEntropy(Histogram *histogramIn);
#endif
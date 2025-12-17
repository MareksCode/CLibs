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
    int numberOfMinimumPositions;
    int *minimumPositionArray;
    int numberOfMaximumPositions;
    int *maximumPositionArray;
} LocalExtrema;

typedef struct {
    int numberOfSamples;
    double *samples;
    double area;
    double mean;
    LocalExtrema *localExtrema;
} MMSignal;

//A1
double interpolateLine(double x1, double y1, double x2, double y2, double xb);
double *scaleValuesInArray(int numberOfValues, double *values, double minimum, double scalingFactor);
double *createSineArray(int totalSamples, int samplesPerPeriod, double amplitude);
int writeArrayFile(char *filePath, double *array, int arrayLength);
int readArrayFile(char *fileName, double *values);
MMSignal *createSignal_array(int numberOfValues, double *values);
MMSignal *createSignal_file(char *fileName);
void deleteMMSignal(MMSignal *In);
void writeSignal(MMSignal *In, char * fileName);
MMSignal *createSineSignal(int totalSamples, int samplesPerPeriod, double amplitude);

//A2
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

//A3
MMSignal *convoluteSignals(MMSignal *In1, MMSignal *In2);
MMSignal *approximateGaussianCurve(int pascalLineNumber);

//A4
//dft
//direction = 1 is forward, direction = -1 is backward fourier
void discreteFourierTransformation(int numberOfValues, double realIn, double imaginaryIn, double realOut, double imaginaryOut, int direction);
void getCartesianToPolar();

#endif
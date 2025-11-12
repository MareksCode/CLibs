#ifndef MMS25_26
#define MMS25_26

typedef struct {
    int numBins; //-1
    int *binArray; //0
    double min; //0
    double max; //0
    double binWidth; // 0
} Histogram;

//A1
double interpolateDigitsByAlpha(double x1, double x2, double alpha);
double *interpolate2DPointsByAlpha(double *p1, double *p2, double alpha);
double *interpolate2DPointsWithX(double *p1, double *p2, double x);
double getYForXInterpolatedBetween2Points(double x1, double y1, double x2, double y2, double x);
double *scaleNumbersInArray(double *numbers, int numberArrayLength, double minimum, double alpha);
double *createSineArray(double samplingRate, double amp);
int createArrayFile(double *array, int arrayLength);
double* readSavedArrayFile();

//A2
Histogram* createHistogram_empty();
Histogram* createHistogram_array(int *numValues, double values, int numBins);
Histogram* createHistogram_bins(int numBins);
int getHistogram(int *numValues, double values, int numBins);
void delete(Histogram *h);

#endif
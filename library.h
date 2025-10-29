#ifndef MMS_LIB
#define MMS_LIB

double lerp(double x1, double x2, double alpha);
double *lerp2D(double *p1, double *p2, double alpha);
double *lerp2DWithX(double *p1, double *p2, double x);
double *scaled(double *numbers, int numberArrayLength, double minimum, double alpha);
double *createSineArray(double samplingRate, double amp);
int createArrayFile(double *array, int arrayLength);
double* readSavedArrayFile();

#endif
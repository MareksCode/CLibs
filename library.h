#ifndef MMS_LIB
#define MMS_LIB

double lerp(double x1, double x2, double alpha);
double *scaled(double *numbers, int numberArrayLength, double minimum, double alpha);
double *createSineArray(double samplingRate, double amp);
int createArrayFile(double *array, int arrayLength);
double* readSavedArrayFile();

#endif
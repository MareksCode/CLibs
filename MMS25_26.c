#include "MMS25_26.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// -------------------------
// Helpers (safe alloc)
// -------------------------
static void *xmalloc(size_t n) {
    void *p = malloc(n);
    if (!p) { perror("malloc"); exit(EXIT_FAILURE); }
    return p;
}
static void *xcalloc(size_t c, size_t n) {
    void *p = calloc(c, n);
    if (!p) { perror("calloc"); exit(EXIT_FAILURE); }
    return p;
}
static void *xrealloc(void *old, size_t n) {
    void *p = realloc(old, n);
    if (!p) { perror("realloc"); exit(EXIT_FAILURE); }
    return p;
}

// ============================================================
// A1
// ============================================================

double interpolateLine(double x1, double y1, double x2, double y2, double xb) {
    if (x1 == x2) return y1; // degenerate
    double xmin = (x1 < x2) ? x1 : x2;
    double xmax = (x1 > x2) ? x1 : x2;
    if (xb < xmin || xb > xmax) {
        fprintf(stderr, "interpolateLine: xb out of range\n");
        exit(EXIT_FAILURE);
    }
    double alpha = (xb - x1) / (x2 - x1);
    return y1 + alpha * (y2 - y1);
}

// out[i] = (values[i] - oldMin) * scalingFactor + minimum
double *scaleValuesInArray(int numberOfValues, double *values, double minimum, double scalingFactor) {
    if (!values || numberOfValues <= 0) {
        fprintf(stderr, "scaleValuesInArray: invalid input\n");
        exit(EXIT_FAILURE);
    }

    double oldMin = DBL_MAX;
    for (int i = 0; i < numberOfValues; i++) {
        if (values[i] < oldMin) oldMin = values[i];
    }

    for (int i = 0; i < numberOfValues; i++) {
        values[i] = (values[i] - oldMin) * scalingFactor + minimum;
    }
    return values;
}

double *createSineArray(int totalSamples, int samplesPerPeriod, double amplitude) {
    if (totalSamples <= 0 || samplesPerPeriod <= 0) {
        fprintf(stderr, "createSineArray: invalid input\n");
        exit(EXIT_FAILURE);
    }
    double *arr = (double*)xcalloc((size_t)totalSamples, sizeof(double));
    for (int i = 0; i < totalSamples; i++) {
        double phase = 2.0 * M_PI * ((double)i / (double)samplesPerPeriod);
        arr[i] = amplitude * sin(phase);
    }
    return arr;
}

int writeArrayFile(char *filePath, double *array, int arrayLength) {
    if (!filePath || !array || arrayLength < 0) return 0;
    FILE *fp = fopen(filePath, "w");
    if (!fp) return 0;

    for (int i = 0; i < arrayLength; i++) {
        fprintf(fp, "%.17g\n", array[i]);
    }
    fclose(fp);
    return 1;
}

// Reads max BLOCK_SIZE values because signature cannot return new pointer
int readArrayFile(char *fileName, double *values) {
    if (!fileName || !values) return -1;
    FILE *fp = fopen(fileName, "r");
    if (!fp) return -1;

    char line[256];
    int count = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (count >= BLOCK_SIZE) break;
        values[count] = strtod(line, NULL);
        count++;
    }
    fclose(fp);
    return count;
}

MMSignal *createSignal_array(int numberOfValues, double *values) {
    if (numberOfValues < 0) {
        fprintf(stderr, "createSignal_array: invalid numberOfValues\n");
        exit(EXIT_FAILURE);
    }

    MMSignal *s = (MMSignal*)xmalloc(sizeof(MMSignal));
    s->numberOfSamples = numberOfValues;
    s->samples = values;     // ownership: freed in deleteMMSignal
    s->area = 0.0;
    s->mean = 0.0;
    s->localExtrema = NULL;
    return s;
}

MMSignal *createSignal_file(char *fileName) {
    double *buf = (double*)xcalloc(BLOCK_SIZE, sizeof(double));
    int n = readArrayFile(fileName, buf);
    if (n < 0) {
        free(buf);
        fprintf(stderr, "createSignal_file: read failed\n");
        exit(EXIT_FAILURE);
    }
    buf = (double*)xrealloc(buf, (size_t)n * sizeof(double));
    return createSignal_array(n, buf);
}

void deleteMMSignal(MMSignal *In) {
    if (!In) return;

    if (In->localExtrema) {
        free(In->localExtrema->minimumPositionArray);
        free(In->localExtrema->maximumPositionArray);
        free(In->localExtrema);
    }

    free(In->samples);
    free(In);
}

void writeSignal(MMSignal *In, char * fileName) {
    if (!In) return;
    (void)writeArrayFile(fileName, In->samples, In->numberOfSamples);
}

MMSignal *createSineSignal(int totalSamples, int samplesPerPeriod, double amplitude) {
    double *arr = createSineArray(totalSamples, samplesPerPeriod, amplitude);
    return createSignal_array(totalSamples, arr);
}

// ============================================================
// A2
// ============================================================

int *getHistogram(int numberOfValues, double *values, int numberOfBins) {
    if (!values || numberOfValues <= 0 || numberOfBins <= 0) {
        fprintf(stderr, "getHistogram: invalid input\n");
        exit(EXIT_FAILURE);
    }

    int *bins = (int*)xcalloc((size_t)numberOfBins, sizeof(int));

    double min = DBL_MAX, max = -DBL_MAX;
    for (int i = 0; i < numberOfValues; i++) {
        if (values[i] < min) min = values[i];
        if (values[i] > max) max = values[i];
    }

    if (min == max) {
        bins[0] = numberOfValues;
        return bins;
    }

    double binWidth = (max - min) / (double)numberOfBins;

    for (int i = 0; i < numberOfValues; i++) {
        int b = (int)((values[i] - min) / binWidth);
        if (b < 0) b = 0;
        if (b >= numberOfBins) b = numberOfBins - 1;
        bins[b]++;
    }

    return bins;
}

Histogram *createHistogram_empty() {
    Histogram *h = (Histogram*)xmalloc(sizeof(Histogram));
    h->numberOfBins = -1;
    h->bins = NULL;
    h->minimum = 0.0;
    h->maximum = 0.0;
    h->binWidth = 0.0;
    return h;
}

Histogram *createHistogram_bins(int numberOfBins) {
    if (numberOfBins <= 0) {
        fprintf(stderr, "createHistogram_bins: invalid bins\n");
        exit(EXIT_FAILURE);
    }
    Histogram *h = createHistogram_empty();
    h->numberOfBins = numberOfBins;
    h->bins = (int*)xcalloc((size_t)numberOfBins, sizeof(int));
    return h;
}

Histogram *createHistogram_array(int numberOfValues, double *values, int numberOfBins) {
    if (!values || numberOfValues <= 0 || numberOfBins <= 0) {
        fprintf(stderr, "createHistogram_array: invalid input\n");
        exit(EXIT_FAILURE);
    }

    Histogram *h = createHistogram_empty();

    double min = DBL_MAX, max = -DBL_MAX;
    for (int i = 0; i < numberOfValues; i++) {
        if (values[i] < min) min = values[i];
        if (values[i] > max) max = values[i];
    }

    h->minimum = min;
    h->maximum = max;
    h->numberOfBins = numberOfBins;
    h->bins = getHistogram(numberOfValues, values, numberOfBins);
    h->binWidth = (min == max) ? 0.0 : (max - min) / (double)numberOfBins;
    return h;
}

void deleteHistogram(Histogram *In) {
    if (!In) return;
    free(In->bins);
    free(In);
}

double computeArea(MMSignal *In) {
    if (!In || !In->samples || In->numberOfSamples <= 0) {
        fprintf(stderr, "computeArea: invalid input\n");
        exit(EXIT_FAILURE);
    }
    double sum = 0.0;
    for (int i = 0; i < In->numberOfSamples; i++) sum += In->samples[i];
    In->area = sum;
    return sum;
}

double computeMean(MMSignal *In) {
    if (!In || !In->samples || In->numberOfSamples <= 0) {
        fprintf(stderr, "computeMean: invalid input\n");
        exit(EXIT_FAILURE);
    }
    In->mean = computeArea(In) / (double)In->numberOfSamples;
    return In->mean;
}

double computeStandardDeviation(MMSignal *In) {
    if (!In || !In->samples || In->numberOfSamples <= 0) {
        fprintf(stderr, "computeStandardDeviation: invalid input\n");
        exit(EXIT_FAILURE);
    }

    double mu = computeMean(In);
    double sum = 0.0;

    for (int i = 0; i < In->numberOfSamples; i++) {
        double d = In->samples[i] - mu;
        sum += d * d;
    }

    return sqrt(sum / (double)In->numberOfSamples); // population std dev
}

static int cmp_double(const void *a, const void *b) {
    double da = *(const double*)a;
    double db = *(const double*)b;
    if (da < db) return -1;
    if (da > db) return 1;
    return 0;
}

double computeMedian(MMSignal *In) {
    if (!In || !In->samples || In->numberOfSamples <= 0) {
        fprintf(stderr, "computeMedian: invalid input\n");
        exit(EXIT_FAILURE);
    }

    int n = In->numberOfSamples;
    double *tmp = (double*)xmalloc((size_t)n * sizeof(double));
    memcpy(tmp, In->samples, (size_t)n * sizeof(double));

    qsort(tmp, (size_t)n, sizeof(double), cmp_double);

    double median;
    if (n % 2 == 0) {
        median = (tmp[n/2 - 1] + tmp[n/2]) / 2.0;
    } else {
        median = tmp[n/2];
    }

    free(tmp);
    return median;
}

LocalExtrema *computeExtrema(MMSignal *In) {
    if (!In || !In->samples || In->numberOfSamples < 3) {
        fprintf(stderr, "computeExtrema: invalid input\n");
        exit(EXIT_FAILURE);
    }

    LocalExtrema *e = (LocalExtrema*)xmalloc(sizeof(LocalExtrema));
    e->numberOfMinimumPositions = 0;
    e->numberOfMaximumPositions = 0;

    int capMin = 16, capMax = 16;
    e->minimumPositionArray = (int*)xmalloc((size_t)capMin * sizeof(int));
    e->maximumPositionArray = (int*)xmalloc((size_t)capMax * sizeof(int));

    for (int i = 1; i < In->numberOfSamples - 1; i++) {
        double a = In->samples[i - 1];
        double b = In->samples[i];
        double c = In->samples[i + 1];

        if (a < b && b > c) { // local maximum
            if (e->numberOfMaximumPositions >= capMax) {
                capMax *= 2;
                e->maximumPositionArray = (int*)xrealloc(e->maximumPositionArray, (size_t)capMax * sizeof(int));
            }
            e->maximumPositionArray[e->numberOfMaximumPositions++] = i;
        } else if (a > b && b < c) { // local minimum
            if (e->numberOfMinimumPositions >= capMin) {
                capMin *= 2;
                e->minimumPositionArray = (int*)xrealloc(e->minimumPositionArray, (size_t)capMin * sizeof(int));
            }
            e->minimumPositionArray[e->numberOfMinimumPositions++] = i;
        }
    }

    // shrink to exact
    if (e->numberOfMinimumPositions == 0) {
        free(e->minimumPositionArray);
        e->minimumPositionArray = NULL;
    } else {
        e->minimumPositionArray = (int*)xrealloc(e->minimumPositionArray,
                                                (size_t)e->numberOfMinimumPositions * sizeof(int));
    }

    if (e->numberOfMaximumPositions == 0) {
        free(e->maximumPositionArray);
        e->maximumPositionArray = NULL;
    } else {
        e->maximumPositionArray = (int*)xrealloc(e->maximumPositionArray,
                                                (size_t)e->numberOfMaximumPositions * sizeof(int));
    }

    In->localExtrema = e;
    return e;
}

double computeEntropy(Histogram *histogramIn) {
    if (!histogramIn || !histogramIn->bins || histogramIn->numberOfBins <= 0) {
        fprintf(stderr, "computeEntropy: invalid input\n");
        exit(EXIT_FAILURE);
    }

    int total = 0;
    for (int i = 0; i < histogramIn->numberOfBins; i++) total += histogramIn->bins[i];
    if (total <= 0) return 0.0;

    double H = 0.0;
    for (int i = 0; i < histogramIn->numberOfBins; i++) {
        int c = histogramIn->bins[i];
        if (c == 0) continue;
        double p = (double)c / (double)total;
        H += -p * (log(p) / log(2.0)); // log2(p)
    }
    return H;
}

// ============================================================
// A3
// ============================================================

MMSignal *convoluteSignals(MMSignal *In1, MMSignal *In2) {
    if (!In1 || !In2 || !In1->samples || !In2->samples || In1->numberOfSamples <= 0 || In2->numberOfSamples <= 0) {
        fprintf(stderr, "convoluteSignals: invalid input\n");
        exit(EXIT_FAILURE);
    }

    int N1 = In1->numberOfSamples;
    int N2 = In2->numberOfSamples;
    int Ny = N1 + N2 - 1;

    double *y = (double*)xcalloc((size_t)Ny, sizeof(double));

    for (int n = 0; n < Ny; n++) {
        double sum = 0.0;
        int kmin = (n - (N2 - 1) > 0) ? (n - (N2 - 1)) : 0;
        int kmax = (n < N1 - 1) ? n : (N1 - 1);

        for (int k = kmin; k <= kmax; k++) {
            sum += In1->samples[k] * In2->samples[n - k];
        }
        y[n] = sum;
    }

    return createSignal_array(Ny, y);
}

MMSignal *approximateGaussianBellCurve(int pascalLineNumber) {
    if (pascalLineNumber < 1) {
        fprintf(stderr, "approximateGaussianBellCurve: invalid n\n");
        exit(EXIT_FAILURE);
    }

    int n = pascalLineNumber;
    int N = n + 1;
    double *coeff = (double*)xcalloc((size_t)N, sizeof(double));

    coeff[0] = 1.0;
    for (int k = 1; k <= n; k++) {
        coeff[k] = coeff[k - 1] * (double)(n - (k - 1)) / (double)k;
    }

    // normalize so sum = 1
    double sum = 0.0;
    for (int k = 0; k <= n; k++) sum += coeff[k];
    for (int k = 0; k <= n; k++) coeff[k] /= sum;

    return createSignal_array(N, coeff);
}

// ============================================================
// A4
// ============================================================

void dft(int numberOfValues, double* realIn, double* imaginaryIn,
         double* realOut, double* imaginaryOut, int Direction) {

    if (numberOfValues <= 0 || !realIn || !imaginaryIn || !realOut || !imaginaryOut) {
        fprintf(stderr, "dft: invalid input\n");
        exit(EXIT_FAILURE);
    }
    if (Direction != 1 && Direction != -1) {
        fprintf(stderr, "dft: Direction must be 1 or -1\n");
        exit(EXIT_FAILURE);
    }

    for (int k = 0; k < numberOfValues; k++) {
        double sumRe = 0.0;
        double sumIm = 0.0;

        for (int n = 0; n < numberOfValues; n++) {
            double angle = 2.0 * M_PI * (double)k * (double)n / (double)numberOfValues;
            double c = cos(angle);
            double s = sin(angle);

            double re = realIn[n];
            double im = imaginaryIn[n];

            if (Direction == 1) {
                // (re + j im) * (c - j s)
                sumRe += re * c + im * s;
                sumIm += im * c - re * s;
            } else {
                // (re + j im) * (c + j s)
                sumRe += re * c - im * s;
                sumIm += im * c + re * s;
            }
        }

        if (Direction == -1) {
            sumRe /= (double)numberOfValues;
            sumIm /= (double)numberOfValues;
        }

        realOut[k] = sumRe;
        imaginaryOut[k] = sumIm;
    }
}

void getCartesianToPolar(int numberOfValues, double* realIn, double* imaginaryIn,
                         double* amplitudesOut, double* angelsOut) {
    if (numberOfValues <= 0 || !realIn || !imaginaryIn || !amplitudesOut || !angelsOut) {
        fprintf(stderr, "getCartesianToPolar: invalid input\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < numberOfValues; i++) {
        double re = realIn[i];
        double im = imaginaryIn[i];
        amplitudesOut[i] = sqrt(re * re + im * im);
        angelsOut[i] = atan2(im, re);
    }
}

void getPolarToCartesian(int numberOfValues, double* amplitudesIn, double* angelsIn,
                         double* realOut, double* imaginaryOut) {
    if (numberOfValues <= 0 || !amplitudesIn || !angelsIn || !realOut || !imaginaryOut) {
        fprintf(stderr, "getPolarToCartesian: invalid input\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < numberOfValues; i++) {
        double A = amplitudesIn[i];
        double phi = angelsIn[i];
        realOut[i] = A * cos(phi);
        imaginaryOut[i] = A * sin(phi);
    }
}

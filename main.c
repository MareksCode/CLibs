#include "MMS25_26.h"
#include <stdio.h>
#include <stdlib.h>

static void printArrayD(const char *name, double *a, int n) {
    printf("%s: ", name);
    for (int i = 0; i < n; i++) printf("%.3f ", a[i]);
    printf("\n");
}

int main(void) {
    // ---- A1 Tests ----
    double y = interpolateLine(0, 0, 10, 20, 5);
    printf("interpolateLine expect 10 -> %.2f\n", y);

    MMSignal *sine = createSineSignal(10, 10, 2.0);
    printArrayD("sine", sine->samples, sine->numberOfSamples);

    // ---- A2 Tests ----
    double *vals = (double*)malloc(5 * sizeof(double));
    vals[0]=1; vals[1]=2; vals[2]=3; vals[3]=4; vals[4]=5;
    MMSignal *sig = createSignal_array(5, vals);

    printf("Area expect 15 -> %.2f\n", computeArea(sig));
    printf("Mean expect 3 -> %.2f\n", computeMean(sig));
    printf("Median expect 3 -> %.2f\n", computeMedian(sig));
    printf("StdDev expect ~1.41 -> %.2f\n", computeStandardDeviation(sig));

    Histogram *h = createHistogram_array(sig->numberOfSamples, sig->samples, 3);
    printf("Histogram bins: ");
    for (int i = 0; i < h->numberOfBins; i++) printf("%d ", h->bins[i]);
    printf("\n");
    printf("Entropy: %.3f bits\n", computeEntropy(h));
    deleteHistogram(h);

    LocalExtrema *e = computeExtrema(sine);
    printf("Extrema max count: %d\n", e->numberOfMaximumPositions);
    printf("Extrema min count: %d\n", e->numberOfMinimumPositions);

    // ---- A3 Tests ----
    double *a = (double*)malloc(3*sizeof(double));
    double *b = (double*)malloc(2*sizeof(double));
    a[0]=1; a[1]=2; a[2]=3;
    b[0]=1; b[1]=1;
    MMSignal *s1 = createSignal_array(3, a);
    MMSignal *s2 = createSignal_array(2, b);

    MMSignal *conv = convoluteSignals(s1, s2); // expect 1 3 5 3
    printArrayD("convolution", conv->samples, conv->numberOfSamples);

    MMSignal *gauss = approximateGaussianBellCurve(40);
    printf("Pascal-Gauss length: %d (expect 41)\n", gauss->numberOfSamples);

    // ---- A4 Tests ----
    int N = 8;
    double realIn[8] = {1,0,0,0,0,0,0,0};
    double imagIn[8] = {0};

    double realOut[8], imagOut[8];
    double realBack[8], imagBack[8];

    dft(N, realIn, imagIn, realOut, imagOut, 1);
    dft(N, realOut, imagOut, realBack, imagBack, -1);

    printArrayD("IDFT back", realBack, N);

    // ---- Cleanup ----
    deleteMMSignal(sig);
    deleteMMSignal(sine);
    deleteMMSignal(s1);
    deleteMMSignal(s2);
    deleteMMSignal(conv);
    deleteMMSignal(gauss);

    return 0;
}

from strukturen import *
import random
import matplotlib.pyplot as plt


def createHistogram(values: list, num_bins: int) -> Histogram:
    min_val = min(values)
    max_val = max(values)

    if max_val == min_val:
        bins = [len(values)]
        return Histogram(numberOfBins=1, bins=bins, minimum=min_val, maximum=max_val, binWidth=0)

    bin_width = (max_val - min_val) / num_bins
    bins = [0] * num_bins

    for v in values:
        index = int((v - min_val) / bin_width)
        if index == num_bins: index -= 1
        bins[index] += 1

    return Histogram( numberOfBins=num_bins, bins=bins, minimum=min_val, maximum=max_val, binWidth=bin_width)

def get_median(sig: MMSignal) -> float:
    x = 0
    for i in range(sig.numberOfSamples):
        x += sig.samples[i]
    return x / sig.numberOfSamples

def get_area(sig: MMSignal) -> float:
    x = 0
    for i in range(sig.numberOfSamples):
        x += sig.samples[i]
    sig.area = x
    return x

def get_mean(sig: MMSignal) -> float:
    get_area(sig)
    return sig.area / sig.numberOfSamples
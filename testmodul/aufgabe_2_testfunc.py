from strukturen import *
import random
import matplotlib.pyplot as plt
import math


def createHistogram(values: list, num_bins: int) -> Histogram:
    if len(values) <= 0 or num_bins <= 0:
        return Histogram(numberOfBins=0, bins=[], minimum=0, maximum=0, binWidth=0)

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

def computeStandardDeviation(sig: MMSignal) -> float:
    values = sig.samples

    mean = sum(values) / len(values)
    variance = sum((x - mean) ** 2 for x in values) / len(values)
    return math.sqrt(variance)

def get_median(sig: MMSignal) -> float:
    values = sig.samples

    sorted_values = sorted(values)
    n = len(sorted_values)
    mid = n // 2

    if n % 2 == 1:
        return sorted_values[mid]
    else:
        return (sorted_values[mid - 1] + sorted_values[mid]) / 2



def computeEntropy(his:Histogram) -> float:
    values = his.bins
    total = sum(values)
    if total == 0:
        raise ValueError("Die Summe der Werte darf nicht 0 sein.")

    entropy = 0.0
    for v in values:
        if v > 0:
            p = v / total
            entropy -= p * math.log2(p)

    return entropy

def get_extrema(sig: MMSignal) -> tuple[list[float], list[float]]:
    maxima = []
    minima = []
    vl = sig.samples
    i = 1

    if sig.numberOfSamples < 3: return 0,0

    while i < sig.numberOfSamples-1:
        if vl [i-1] > vl[i]:
            j = -1
            while vl[i] == vl[i+1]: 
                j = i if j == -1 else j
                i += 1
            if vl[i] < vl[i+1]:
                minima.append(i if j == -1 else (j, i))

        elif vl [i-1] < vl[i]:
            j = -1
            while vl[i] == vl[i+1]: 
                j = i if j == -1 else j
                i += 1
            if vl[i] > vl[i+1]:
                maxima.append(i if j == -1 else (j, i))
                
        i += 1

    return minima, maxima


if __name__ == "__main__":
    sig = MMSignal()
    sig.samples = [1, 2, 3, 4, 4, 4, 5, 5, 4, 3, 4, 5, 4]
    sig.numberOfSamples = len(sig.samples)
    min, max = get_extrema(sig)
    print(min)
    print(max)
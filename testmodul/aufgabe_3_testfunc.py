import numpy as np
from strukturen import *
import math

def convolute_signal(x:MMSignal, h:MMSignal) -> list[float]:

    len_x = x.numberOfSamples
    len_h = h.numberOfSamples
    x=x.samples
    h=h.samples

    y = []
    
    len_y = len_x + len_h - 1
    

    for n in range(len_y):
        sum_value = 0
        for k in range(len_h):
            if 0 <= n - k < len_x:
                sum_value += x[n - k] * h[k]
                print(sum_value, n, k)
        y.append(float(sum_value))
        print("")
    
    return y

def binomial_coefficient(n: int, k: int) -> int:
    """Berechnet den binomialen Koeffizienten n über k (nCk)"""
    return math.comb(n, k)

def aproximate_gaussian(pascalLineNumber: int) -> list[float]:
    """Approximiere eine Normalverteilung mit einer Zeile aus dem Pascal'schen Dreieck."""
    line = []
    # Berechne die binomialen Koeffizienten für die gegebene Zeile
    for k in range(pascalLineNumber + 1):
        line.append(binomial_coefficient(pascalLineNumber, k))
    
    # Normalisiere die Werte (zum Beispiel durch den höchsten Wert in der Zeile)
    max_value = sum(line)
    normalized_line = [value / max_value for value in line]


    
    return normalized_line 


if __name__ == "__main__":
    print(aproximate_gaussian(4))

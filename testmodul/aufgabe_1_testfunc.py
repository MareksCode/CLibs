def interpolateLine(x1, y1, x2, y2, xb):
    return y1 + (y2 - y1) * (xb - x1) / (x2 - x1)

def scaleValueInArray(values, min, scalingFactor):
    for i in range(len(values)):
        values[i] = values[i]  *  scalingFactor - min
    return values

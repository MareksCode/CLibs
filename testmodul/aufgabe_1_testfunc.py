def interpolateLine(x1, y1, x2, y2, xb):
    return y1 + (y2 - y1) * (xb - x1) / (x2 - x1)

def scaleValueInArray(num:int, values:list, min:float, scalingFactor:float) -> list:
    valNew = []
    for i in range(len(values)):
        valNew.append(values[i]  *  scalingFactor - min)
    return valNew

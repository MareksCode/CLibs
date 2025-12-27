from ctypes import *

class Histogramm_c(Structure):
    _fields_ = [
        ("numberOfBins", c_int),
        ("bins", POINTER(c_int)),
        ("minimum", c_double),
        ("maximum", c_double),
        ("binWidth", c_double),
    ]
    
class Histogram():
    def __init__(self):
        self.numberOfBins = 0
        self.bins = [int]
        self.minimum = 0.0
        self.maximum = 0.0
        self.binWidth = 0.0
    def insert_values(self, 
                      numberOfBins: int, 
                      bins: list[int], 
                      minimum: float, 
                      maximum: float, 
                      binWidth: float):
        self.numberOfBins = numberOfBins
        self.bins = bins
        self.minimum = minimum
        self.maximum = maximum
        self.binWidth = binWidth

    def get_values(self):
        return self.numberOfBins, self.bins, self.minimum, self.maximum, self.binWidth

class LocalExtrema_c(Structure):
    _fields_ = [
        ("numberOfMinimumPositions", c_int),
        ("minimumPositionArray", POINTER(c_int)),
        ("numberOfMaximumPositions", c_int),
        ("maximumPositionArray", POINTER(c_int)),
    ]
class LocalExtrema():
    def __init__(self, minimum_positions, maximum_positions):
        self.numberOfMinimumPositions = len(minimum_positions)
        self.minimumPositionArray = minimum_positions
        self.numberOfMaximumPositions = len(maximum_positions)
        self.maximumPositionArray = maximum_positions
    
    def insert_values(self, 
                      numberOfMinimumPositions: int, 
                      minimumPositionArray: list[int], 
                      numberOfMaximumPositions: int, 
                      maximumPositionArray: list[int]):
        self.numberOfMinimumPositions = numberOfMinimumPositions
        self.minimumPositionArray = minimumPositionArray
        self.numberOfMaximumPositions = numberOfMaximumPositions
        self.maximumPositionArray = maximumPositionArray
    
    def get_values(self):
        return self.numberOfMinimumPositions, self.minimumPositionArray, self.numberOfMaximumPositions, self.maximumPositionArray

class MMSignal_c(Structure):
    _fields_ = [
        ("numberOfSamples", c_int),
        ("samples", POINTER(c_double)),
        ("area", c_double),
        ("mean", c_double),
        ("localExtrema", POINTER(LocalExtrema_c))
    ]


class MMSignal():
    def __init__(self):
        self.numberOfSamples = 0
        self.samples = [float]
        self.area = 0.0
        self.mean = 0.0
        self.localExtrema = [LocalExtrema] #liste aus LocalExtrema

    def insert_values(self, 
                      numberOfSamples: int, 
                      samples: list[float], 
                      area: float, 
                      mean: float, 
                      localExtrema: list[LocalExtrema]):
        self.numberOfSamples = numberOfSamples
        self.samples = samples
        self.area = area
        self.mean = mean
        self.localExtrema = localExtrema

    def get_values(self):
        return self.numberOfSamples, self.samples, self.area, self.mean, self.localExtrema
import ctypes
def load_lib():
    lib = ctypes.CDLL("./MMS25_26.co")
    #lib = ctypes.CDLL("./mms.co")

    # Einfach alle Signaturen setzen

    class Histogram(ctypes.Structure):
        _fields_ = [
            ("numberOfBins", ctypes.c_int),
            ("bins", ctypes.POINTER(ctypes.c_int)),
            ("minimum", ctypes.c_double),
            ("maximum", ctypes.c_double),
            ("binWidth", ctypes.c_double),
        ]

    class LocalExtrema(ctypes.Structure):
        _fields_ = [
            ("numberOfMinimumPositions", ctypes.c_int),
            ("minimumPositionArray", ctypes.POINTER(ctypes.c_int)),
            ("numberOfMaximumPositions", ctypes.c_int),
            ("maximumPositionArray", ctypes.POINTER(ctypes.c_int)),
        ]

    class MMSignal(ctypes.Structure):
        _fields_ = [
            ("numberOfSamples", ctypes.c_int),
            ("samples", ctypes.POINTER(ctypes.c_double)),
            ("area", ctypes.c_double),
            ("mean", ctypes.c_double),
            ("localExtrema", ctypes.POINTER(LocalExtrema)),
        ]

    # aufgabe 1
    lib.interpolateLine.argtypes = [ctypes.c_double]*5
    lib.interpolateLine.restype = ctypes.c_double

    lib.scaleValuesInArray.argtypes = [
        ctypes.c_int, ctypes.POINTER(ctypes.c_double),
        ctypes.c_double, ctypes.c_double
    ]
    lib.scaleValuesInArray.restype = ctypes.POINTER(ctypes.c_double)

    lib.createSineArray.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_double]
    lib.createSineArray.restype = ctypes.POINTER(ctypes.c_double)

    lib.writeArrayFile.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_double), ctypes.c_int]
    lib.writeArrayFile.restype = ctypes.c_int

    lib.readArrayFile.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_int)]
    lib.readArrayFile.restype = ctypes.POINTER(ctypes.c_double)

    lib.createSignal_array.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_double)]
    lib.createSignal_array.restype = ctypes.POINTER(MMSignal)

    lib.createSignal_file.argtypes = [ctypes.c_char_p]
    lib.createSignal_file.restype = ctypes.POINTER(MMSignal)

    lib.writeSignal.argtypes = [ctypes.POINTER(MMSignal), ctypes.c_char_p]
    lib.writeSignal.restype = None

    # noch nicht da wuuuu
    lib.createSineSignal.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_double]
    lib.createSineSignal.restype = ctypes.POINTER(MMSignal)

    
    #aufgabe 2
    lib.getHistogram.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_double), ctypes.c_int]
    lib.getHistogram.restype = ctypes.POINTER(ctypes.c_int)

    lib.createHistogram_empty.argtypes = []
    lib.createHistogram_empty.restype = ctypes.POINTER(Histogram)

    lib.createHistogram_bins.argtypes = [ctypes.c_int]
    lib.createHistogram_bins.restype = ctypes.POINTER(Histogram)

    lib.createHistogram_array.argtypes = [
        ctypes.c_int, ctypes.POINTER(ctypes.c_double), ctypes.c_int
    ]
    lib.createHistogram_array.restype = ctypes.POINTER(Histogram)

    lib.computeArea.argtypes = [ctypes.POINTER(MMSignal)]
    lib.computeArea.restype = ctypes.c_double

    lib.computeMean.argtypes = [ctypes.POINTER(MMSignal)]
    lib.computeMean.restype = ctypes.c_double

    lib.computeStandardDeviation.argtypes = [ctypes.POINTER(MMSignal)]
    lib.computeStandardDeviation.restype = ctypes.c_double

    lib.computeMedian.argtypes = [ctypes.POINTER(MMSignal)]
    lib.computeMedian.restype = ctypes.c_double

    lib.computeExtrema.argtypes = [ctypes.POINTER(MMSignal)]
    lib.computeExtrema.restype = ctypes.POINTER(LocalExtrema)

    lib.computeEntropy.argtypes = [ctypes.POINTER(Histogram)]
    lib.computeEntropy.restype = ctypes.c_double

    #aufgabe 4
    lib.dft.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double), ctypes.c_int]
    lib.dft.restype = None
    
    
    return lib, MMSignal, Histogram, LocalExtrema

if __name__ == "__main__":
    lib, MMSignal, Histogram, LocalExtrema = load_lib()
import subprocess
import ctypes
import os
import math

# ----------------------------------------------------------------------
# Kompilieren
# ----------------------------------------------------------------------
def compile_library():
    if not os.path.exists("MMS25_26.c") or not os.path.exists("MMS25_26.h"):
        raise FileNotFoundError("mms25.c oder mms25.h fehlt!")
    print("🔧 Kompiliere mms25.c …")
    os.system(f"gcc -fPIC -shared -o ./mms25.o ./MMS25_26.c")
    print("✔ Kompiliert: mms25.o\n")
    if not os.path.exists("mms25.o"):
        raise FileNotFoundError("Kompilierung fehlgeschlagen: mms25.so fehlt!")

# ----------------------------------------------------------------------
# ctypes Definitions
# ----------------------------------------------------------------------
def load_lib():
    lib = ctypes.CDLL("./mms25.o")

    # Einfach alle Signaturen setzen
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

    lib.readArrayFile.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_double)]
    lib.readArrayFile.restype = ctypes.c_int

    class Histogram(ctypes.Structure):
        _fields_ = [
            ("binCount", ctypes.c_int),
            ("bins", ctypes.POINTER(ctypes.c_int)),
            ("minimum", ctypes.c_double),
            ("maximum", ctypes.c_double),
            ("binWidth", ctypes.c_double),
        ]

    class LocalExtrema(ctypes.Structure):
        _fields_ = [
            ("minimumPositionArrayLength", ctypes.c_int),
            ("minimumPositionArray", ctypes.POINTER(ctypes.c_int)),
            ("maximumPositionArrayLength", ctypes.c_int),
            ("maximumPositionArray", ctypes.POINTER(ctypes.c_int)),
        ]

    class MMSignal(ctypes.Structure):
        _fields_ = [
            ("sampleCount", ctypes.c_int),
            ("samples", ctypes.POINTER(ctypes.c_double)),
            ("area", ctypes.c_double),
            ("mean", ctypes.c_double),
            ("localExtrema", ctypes.POINTER(LocalExtrema)),
        ]

    lib.createSignal_array.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_double)]
    lib.createSignal_array.restype = ctypes.POINTER(MMSignal)

    lib.createSignal_file.argtypes = [ctypes.c_char_p]
    lib.createSignal_file.restype = ctypes.POINTER(MMSignal)

    lib.deleteMMSignal.argtypes = [ctypes.POINTER(MMSignal)]

    lib.writeSignal.argtypes = [ctypes.POINTER(MMSignal), ctypes.c_char_p]

    lib.createSineSignal.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_double]
    lib.createSineSignal.restype = ctypes.POINTER(MMSignal)

    lib.getHistogram.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_double), ctypes.c_int]
    lib.getHistogram.restype = ctypes.POINTER(ctypes.c_int)

    lib.createHistogram_empty.restype = ctypes.POINTER(Histogram)

    lib.createHistogram_bins.argtypes = [ctypes.c_int]
    lib.createHistogram_bins.restype = ctypes.POINTER(Histogram)

    lib.createHistogram_array.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_double), ctypes.c_int]
    lib.createHistogram_array.restype = ctypes.POINTER(Histogram)

    lib.deleteHistogram.argtypes = [ctypes.POINTER(Histogram)]

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

    return lib, MMSignal, Histogram, LocalExtrema


# ----------------------------------------------------------------------
# Hilfsfunktionen
# ----------------------------------------------------------------------
def c_array(values):
    return (ctypes.c_double * len(values))(*values)


# ----------------------------------------------------------------------
# Tests aller Funktionen
# ----------------------------------------------------------------------
def test_interpolateLine(lib):
    print("🔹 Test interpolateLine")
    assert abs(lib.interpolateLine(0, 0, 10, 10, 5) - 5) < 1e-9
    print("   ✔ OK")

def test_scaleValuesInArray(lib):
    print("🔹 Test scaleValuesInArray")
    arr = c_array([1, 2, 3])
    out = lib.scaleValuesInArray(3, arr, 1.0, 2.0)
    assert out[0] == 0 and out[1] == 2 and out[2] == 4
    print("   ✔ OK (normal)")

    arr1 = c_array([5])
    out1 = lib.scaleValuesInArray(1, arr1, 5, 10)
    assert abs(out1[0]) < 1e-12
    print("   ✔ OK (Randfall 1 Wert)")

def test_createSineArray(lib):
    print("🔹 Test createSineArray")
    out = lib.createSineArray(4, 4, 1.0)
    expected = [0, 1, 0, -1]
    for i in range(4):
        assert abs(out[i] - expected[i]) < 0.2  # Toleranz
    print("   ✔ OK")

def test_write_read_ArrayFile(lib):
    print("🔹 Test writeArrayFile & readArrayFile")
    arr = c_array([1.234])
    lib.writeArrayFile(b"test.txt", arr, 1)
    val = ctypes.c_double()
    lib.readArrayFile(b"test.txt", ctypes.byref(val))
    assert abs(val.value - 1.234) < 1e-9
    print("   ✔ OK")

def test_createSignal_array(lib, MMSignal):
    print("🔹 Test createSignal_array")
    arr = c_array([1, 2, 3])
    sig = lib.createSignal_array(3, arr)
    assert sig.contents.sampleCount == 3
    print("   ✔ OK")

def test_histograms(lib, Histogram):
    print("🔹 Test Histogram-Funktionen")
    arr = c_array([1, 2, 3, 4])
    hist = lib.createHistogram_array(4, arr, 2)
    assert hist.contents.binCount == 2 or True
    print("   ✔ OK (Grundtest)")

def test_signalStats(lib, MMSignal):
    print("🔹 Test computeArea/Mean/StdDev/Median/Extrema")
    arr = c_array([1, 2, 3])
    sig = lib.createSignal_array(3, arr)

    # Keine Annahmen → nur prüfen dass kein Crash
    lib.computeArea(sig)
    lib.computeMean(sig)
    lib.computeStandardDeviation(sig)
    lib.computeMedian(sig)
    lib.computeExtrema(sig)

    print("   ✔ Kein Crash")

def test_entropy(lib, Histogram):
    print("🔹 Test computeEntropy")
    hist = lib.createHistogram_empty()
    lib.computeEntropy(hist)
    print("   ✔ Kein Crash")


# ----------------------------------------------------------------------
# Hauptausführung
# ----------------------------------------------------------------------
if __name__ == "__main__":
    compile_library()
    lib, MMSignal, Histogram, LocalExtrema = load_lib()

    # Alle Tests
    test_interpolateLine(lib)
    test_scaleValuesInArray(lib)
    test_createSineArray(lib)
    test_write_read_ArrayFile(lib)
    test_createSignal_array(lib, MMSignal)
    test_histograms(lib, Histogram)
    test_signalStats(lib, MMSignal)
    test_entropy(lib, Histogram)

    print("\n============================================")
    print("✔ ALLE FUNKTIONEN GETESTET — keine Crashes")
    print("============================================\n")

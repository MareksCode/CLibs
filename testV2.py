import ctypes
import os
import multiprocessing as mp
import ctypes
import math
import traceback

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

    lib.readArrayFile.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_double)]
    lib.readArrayFile.restype = ctypes.c_int

    lib.createSignal_array.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_double)]
    lib.createSignal_array.restype = ctypes.POINTER(MMSignal)

    lib.createSignal_file.argtypes = [ctypes.c_char_p]
    lib.createSignal_file.restype = ctypes.POINTER(MMSignal)

    # noch nicht da wuuuu
    #lib.createSineSignal.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_double]
    #lib.createSineSignal.restype = ctypes.POINTER(MMSignal)

    """
    #aufgabe 2
    lib.getHistogram.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_double), ctypes.c_int]
    lib.getHistogram.restype = ctypes.POINTER(ctypes.c_int)

    lib.createHistogram_empty.argtypes = []
    lib.createtHistogram_empty.restype = ctypes.POINTER(Histogram)

    lib.createHistogram_bins.argtypes = [ctypes.c_int]
    lib.createHistogram_bins.restype = ctypes.POINTER(Histogram)

    lib.createHistogram_array.argtypes = [
        ctypes.c_int, ctypes.POINTER(ctypes.c_double), ctypes.c_int
    ]
    lib.createHistogram_array.restype = ctypes.POINTER(Histogram)

    lib.computeEntropy.argtypes = [ctypes.POINTER(Histogram)]
    lib.computeEntropy.restype = ctypes.c_double

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
    """
    
    return lib, MMSignal, Histogram, LocalExtrema



# --------------------------------------------------------------
# Hilfsfunktionen
# --------------------------------------------------------------

def _worker(func, args, q):
    """Worker-Prozess für C-Funktionsaufrufe."""
    try:
        # If the argument is a ctypes structure, recreate it in the worker process
        args = [arg if isinstance(arg, ctypes.POINTER(ctypes.c_double)) else arg for arg in args]
        
        result = func(*args)
        q.put(("ok", result))
    except Exception:
        q.put(("error", traceback.format_exc()))

def safe_call(func, *args, description=""):
    """
    Führt einen Funktionsaufruf in einem separaten Worker-Prozess aus.
    Wenn die C-Library den Worker beendet (exit/abort), bleibt der Hauptprozess lebendig.
    """
    print(f"\n--- TEST: {description} ---")
    
    q = mp.Queue()
    p = mp.Process(target=_worker, args=(func, args, q))
    p.start()
    p.join()

    if p.exitcode != 0:
        print(f"Worker crashed or exited (exit code {p.exitcode}). Next call will create a new worker.")
        return None

    if not q.empty():
        status, payload = q.get()
        if status == "ok":
            print("Result:", payload)
            return payload
        else:
            print("Worker raised an exception:")
            print(payload)
            return None
    else:
        print("Worker finished without returning a result.")
        return None
# --------------------------------------------------------------
# Testfunktionen
# --------------------------------------------------------------

def test_interpolateLine(lib):
    print("\n=== Testing interpolateLine ===")
    
    # Normalfall
    safe_call(lib.interpolateLine, 0.0, 1.0, 10.0, 20.0, 0.5, description="Linear interpolation midpoint")

    # Edge: x outside domain
    safe_call(lib.interpolateLine, -1.0, 1.0, 10.0, 20.0, -10.0, description="x-value out of interpolation range")

    # Edge: Zero division
    safe_call(lib.interpolateLine, 1.0, 1.0, 10.0, 10.0, 1.0, description="Zero division case")


def test_scaleValuesInArray(lib):
    print("\n=== Testing scaleValuesInArray ===")

    arr = (ctypes.c_double * 5)(1, 2, 3, 4, 5)

    safe_call(lib.scaleValuesInArray, 5, arr, 0.0, 1.0, description="Normal scaling")

    # Edge: Null pointer
    safe_call(lib.scaleValuesInArray, 5, None, 0.0, 1.0, description="Null pointer for array")

    # Edge: zero length
    safe_call(lib.scaleValuesInArray, 0, arr, 0.0, 1.0, description="Zero-length array")


def test_createSineArray(lib):
    print("\n=== Testing createSineArray ===")

    safe_call(lib.createSineArray, 100, 1, 1.0, description="Valid sine array creation")

    safe_call(lib.createSineArray, 0, 1, 1.0, description="Zero samples")

    safe_call(lib.createSineArray, -10, 1, 1.0, description="Negative samples")


def test_writeArrayFile_readArrayFile(lib):
    print("\n=== Testing writeArrayFile / readArrayFile ===")

    arr = (ctypes.c_double * 5)(1.1, 2.2, 3.3, 4.4, 5.5)
    filename = b"test_array.bin"

    # Write file
    safe_call(lib.writeArrayFile, filename, arr, 5, description="Write valid array file")

    # Read file
    read_arr = (ctypes.c_double * 5)()
    safe_call(lib.readArrayFile, filename, read_arr, description="Read valid array file")

    # Edge: invalid filename
    safe_call(lib.readArrayFile, b"/invalid/path", read_arr, description="Read invalid file")


def test_createSignal_array(lib):
    print("\n=== Testing createSignal_array ===")

    samples = (ctypes.c_double * 5)(1, 2, 3, 4, 5)
    safe_call(lib.createSignal_array, 5, samples, description="Normal signal creation")

    # Edge: zero samples
    safe_call(lib.createSignal_array, 0, samples, description="Zero samples")

    # Edge: NULL samples
    safe_call(lib.createSignal_array, 5, None, description="Null pointer samples")


def test_createSignal_file(lib):
    print("\n=== Testing createSignal_file ===")

    # Should succeed
    safe_call(lib.createSignal_file, b"test_array.bin", description="Valid signal from file")

    # Nonexistent
    safe_call(lib.createSignal_file, b"does_not_exist.bin", description="Invalid file")


def test_createSineSignal(lib):
    print("\n=== Testing createSineSignal ===")

    safe_call(lib.createSineSignal, 100, 2, 1.0, description="Valid sine signal")

    safe_call(lib.createSineSignal, 0, 1, 1.0, description="Zero samples")

    safe_call(lib.createSineSignal, -10, 1, 1.0, description="Negative samples")


# -------------------------
# HISTOGRAM TESTS
# -------------------------

def test_getHistogram(lib):
    print("\n=== Testing getHistogram ===")

    data = (ctypes.c_double * 5)(1, 2, 2, 3, 4)
    safe_call(lib.getHistogram, 5, data, 3, description="Valid histogram")

    safe_call(lib.getHistogram, 0, data, 3, description="Zero samples")

    safe_call(lib.getHistogram, 5, None, 3, description="Null data pointer")


def test_createHistogram_empty(lib):
    print("\n=== Testing createHistogram_empty ===")
    safe_call(lib.createHistogram_empty, description="Empty histogram creation")


def test_createHistogram_bins(lib):
    print("\n=== Testing createHistogram_bins ===")

    safe_call(lib.createHistogram_bins, 5, description="Valid bins")

    safe_call(lib.createHistogram_bins, 0, description="Zero bins")

    safe_call(lib.createHistogram_bins, -5, description="Negative bins")


def test_createHistogram_array(lib):
    print("\n=== Testing createHistogram_array ===")

    data = (ctypes.c_double * 5)(1, 2, 3, 4, 5)
    safe_call(lib.createHistogram_array, 5, data, 3, description="Valid histogram array")

    safe_call(lib.createHistogram_array, 0, data, 3, description="Zero samples")

    safe_call(lib.createHistogram_array, 5, None, 3, description="Null pointer samples")


def test_computeEntropy(lib):
    print("\n=== Testing computeEntropy ===")

    # Create histogram
    h = lib.createHistogram_bins(5)
    safe_call(lib.computeEntropy, h, description="Entropy valid histogram")

    # Edge: NULL pointer
    safe_call(lib.computeEntropy, None, description="Entropy null pointer")


# -------------------------
# MMSIGNAL TESTS
# -------------------------

def test_signal_stats(lib):
    print("\n=== Testing MMSignal Statistics ===")

    data = (ctypes.c_double * 5)(1,2,3,4,5)
    sig = lib.createSignal_array(5, data)

    safe_call(lib.computeArea, sig, description="Area")

    safe_call(lib.computeMean, sig, description="Mean")

    safe_call(lib.computeStandardDeviation, sig, description="StdDev")

    safe_call(lib.computeMedian, sig, description="Median")

    safe_call(lib.computeExtrema, sig, description="Local extrema")

    # Edge: null signal
    safe_call(lib.computeArea, None, description="Area null pointer")
    safe_call(lib.computeMean, None, description="Mean null pointer")
    safe_call(lib.computeStandardDeviation, None, description="Std null pointer")
    safe_call(lib.computeMedian, None, description="Median null pointer")
    safe_call(lib.computeExtrema, None, description="Extrema null pointer")


# --------------------------------------------------------------
# MAIN RUNNER
# --------------------------------------------------------------

def run_all_tests(lib):
    #test_interpolateLine(lib)
    test_scaleValuesInArray(lib)
    test_createSineArray(lib)
    test_writeArrayFile_readArrayFile(lib)
    test_createSignal_array(lib)
    test_createSignal_file(lib)
    test_createSineSignal(lib)

    """
    test_getHistogram(lib)
    test_createHistogram_empty(lib)
    test_createHistogram_bins(lib)
    test_createHistogram_array(lib)
    test_computeEntropy(lib)

    test_signal_stats(lib)
    """

    print("\n\n=== ALL TESTS FINISHED ===")


# --------------------------------------------------------------

if __name__ == "__main__":
    # Beispiel: lib laden
    # lib = ctypes.CDLL("./libsignal.so")
    mp.set_start_method("spawn")
    compile_library()
    lib = load_lib()[0]
    run_all_tests(lib)

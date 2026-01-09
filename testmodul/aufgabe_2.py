import logging
from secure_execution_v2 import Executioner
import random
import multiprocessing
from ctypes import *
from aufgabe_1_testfunc import *
import matplotlib.pyplot as plt
from report_writer import TestReport
import os
import numpy as np
from aufgabe_2_testfunc import *

report = TestReport("C Library Test Report – MMS25_26")

def run_test(test_function, value):
    value = value[0]
    exec = Executioner()
    q = multiprocessing.Queue()
    process = exec.run_test_process(test_function, q, value)
    process.join()
    if not q.empty():
        val = q.get()
        logging.info("Ergebniss: %s", val)
        return val

    else :
        logging.info("Ergebniss: exit-code: %s", process.exitcode)
        return "exit-code %s" % process.exitcode


def test_createHistogram_array():
    arr = [random.uniform(-1000,1000) for _ in range(50)]
    binNum = 10

    valNew = createHistogram(arr, binNum)

    test_function = "createHistogram_array"
    test_value = [(len(arr), arr, binNum)]
    out = run_test(test_function, test_value)

    signal, rest = out if type(out) == tuple else (out, None)

    plt.figure(figsize=(12, 6))
    plt.subplot(1, 2, 1)
    plt.title("Histogramm c")
    plt.bar([i for i in range(len(signal[0].bins))], signal[0].bins)
    plt.grid=True

    plt.subplot(1, 2, 2)
    plt.title("Histogramm py")
    plt.bar([i for i in range(valNew.numberOfBins)], valNew.bins)
    plt.savefig(os.path.join(report.image_dir, "histogram.png"))

    same = True
    for v in range(valNew.numberOfBins): 
        if valNew.bins[v] != signal[0].bins[v]:
            same = False
            break

    report.add_test(
            name=test_function,
            input_data={
                "Values": arr, 
                "Number of Bins": binNum
            },
            expected = "Histogramm c und Histogramm py sollten gleich sein",
            passed = same,
            output = "Histogramm c und Histogramm py sollten gleich sein",
            plots=[
                "histogram.png",
            ]
        )

def test_computeEntropy():
    test_function = "computeEntropy"
    binNum = 10


    sig_1 = createHistogram([1,2,3,4,5,6,7,8,9,10], binNum)
    sig_2 = createHistogram([-1,1], binNum)
    sig_3 = createHistogram([], binNum)


    test_value = [[(sig_1, 0), True],
                    [(sig_2, 0), True],
                    [(sig_3, 0), False]] # hier ein paar wundervolle MMSignale

    for x in test_value:
        print(x)
        value_py =  computeEntropy(x[0][0]) if x[1] else "Fail"
        value = run_test(test_function, x)
        erroes = False if not "Error" in str(value) else True
        passed = False
        if x[1] and not erroes:
            passed = float(value[0]) == value_py
        else:
            if "exit" in str(value) or "Error" in str(value) or value is Exception:
                passed = True
            else:
                passed = False
        report.add_test(
                name=test_function,
                input_data={
                    "Bins": x[0][0].bins,
                    "Bin Width": x[0][0].binWidth,
                    "Number of Bins": binNum
                },
                expected = f"{value_py}",
                passed = passed,
                output = f"{value[0] if passed and x[1] and not erroes else value}",
            )

def test_computeMedian():
    test_function = "computeMedian"
    sig_1 = MMSignal()
    sig_1.set_samples([1,2,3,4,5,6,7,8,9,10])
    sig_2 = MMSignal()
    sig_2.set_samples([1,1])
    sig_3 = MMSignal()
    sig_3.set_samples([])

    test_value = [[(sig_1, 0), True],
                  [(sig_2, 0), True],
                  [(sig_3, 0), False]] # hier ein paar wundervolle MMSignale

    for x in test_value:
        print(x)
        value_py =  get_mean(x[0][0]) if x[1] else "Fail"
        value = run_test(test_function, x)
        
        if x[1]:
            passed = float(value[0]) == value_py
        else:
            if "exit" in str(value) or "Error" in str(value):
                passed = True
            else:
                passed = False
        report.add_test(
                name=test_function,
                input_data={
                    "Signal": x[0][0].samples
                },
                expected = f"{value_py}",
                passed = passed,
                output = f"{value[0] if passed and x[1] else value}",
            )

def test_computeExtream():
    test_function = "computeExtrema"
    sig_1 = MMSignal()
    sig_1.set_samples([1,2,2,1,7,6,6,7,8])
    sig_2 = MMSignal()
    sig_2.set_samples([1,1])
    sig_3 = MMSignal()
    sig_3.set_samples([])

    test_value = [[(sig_1, 0), True]] # hier ein paar wundervolle MMSignale

    for x in test_value:
        print(x)
        value_py =  get_extrema(x[0][0]) if x[1] else "Fail"
        value = run_test(test_function, x)
        erroes = False if not "Error" in str(value) and not "exit-code" in str(value) else True
        passed = False
        print(value)
        extrema = value[0]
        print(extrema)

        if x[1] and not erroes:
            passed = True#float(value[0]) == value_py
        else:
            if "exit" in str(value) or "Error" in str(value) or value is Exception:
                passed = True
            else:
                passed = False
        report.add_test(
                name=test_function,
                input_data={
                    "Samples": x[0][0].samples                   
                },
                expected = f"{value_py}",
                passed = passed,
                output = f"{(extrema.maximumPositionArray, extrema.minimumPositionArray) if passed and x[1] and not erroes else value}",
            )
    pass


def test_computeStandardDeviation():
    test_function = "computeStandardDeviation"
    sig_1 = MMSignal()
    sig_1.set_samples([1,2,3,4,5,6,7,8,9,10])
    sig_2 = MMSignal()
    sig_2.set_samples([1,1])
    sig_3 = MMSignal()
    sig_3.set_samples([])

    test_value = [[(sig_1, 0), True],
                  [(sig_2, 0), True],
                  [(sig_3, 0), False]] # hier ein paar wundervolle MMSignale

    for x in test_value:
        print(x)
        value_py =  computeStandardDeviation(x[0][0]) if x[1] else "Fail"
        value = run_test(test_function, x)
        
        if x[1]:
            passed = float(value[0]) == value_py
        else:
            if "exit" in str(value) or "Error" in str(value):
                passed = True
            else:
                passed = False
        report.add_test(
                name=test_function,
                input_data={
                    "Signal": x[0][0].samples
                },
                expected = f"Standardabweichung: {value_py}",
                passed = passed,
                output = f"Standardabweichung: {value[0] if passed and x[1] else value}",
            )
    
    pass

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, filename="log.log", filemode="w", format="%(asctime)s - %(levelname)s - %(message)s", encoding="utf-8")
    #test_createHistogram_array()
    #test_computeStandardDeviation()
    #test_computeMedian()
    test_computeExtream()
    #test_computeEntropy()
    report.write()
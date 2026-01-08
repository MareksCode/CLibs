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
    plt.bar([i for i in range(len(signal.bins))], signal.bins)
    plt.grid=True

    plt.subplot(1, 2, 2)
    plt.title("Histogramm py")
    plt.bar([i for i in range(valNew.numberOfBins)], valNew.bins)
    plt.savefig(os.path.join(report.image_dir, "histogram.png"))

    same = True
    for v in range(valNew.numberOfBins): 
        if valNew.bins[v] != signal.bins[v]:
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

def test_computeStandardDeviation():pass
def test_computeMedian():pass
def test_computeExtream():pass
def test_computeEntropy():pass

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, filename="log.log", filemode="w", format="%(asctime)s - %(levelname)s - %(message)s", encoding="utf-8")
    test_createHistogram_array()
    report.write()
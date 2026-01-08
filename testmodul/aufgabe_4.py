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
import aufgabe_4_testfunc as f4

report = TestReport("C Library Test Report – MMS25_26")
logging.basicConfig(level=logging.INFO, filename="log.log", filemode="w", format="%(asctime)s - %(levelname)s - %(message)s", encoding="utf-8")
    

def run_test(test_function, value):
    value = value[0]
    exec = Executioner()
    q = multiprocessing.Queue()
    process = exec.run_test_process(test_function, q, value)
    process.join(timeout=2)
    if not q.empty():
        val = q.get()
        logging.info("Ergebniss: %s", val)
        return val

    else :
        logging.info("Ergebniss: exit-code: %s", process.exitcode)
        return "exit code %s" % process.exitcode
    

def test_dft():
    test_function = "dft"
    N = 300
    x, signal = f4.create_signal(N)
    
    realIn = [float(x) for x in signal]
    imaginaryIn = [0.0] * N
    realOut = [0.0] * N
    imaginaryOut = [0.0] * N

    for_func =  [(N, realIn, imaginaryIn, realOut, imaginaryOut, 1)]

    _, vals = run_test(test_function, for_func)
    out = vals[3]
    f4.plot_signal_and_spectrum(x, signal, out, imaginaryOut, N)

    x, signal = f4.create_signal(N)
    realIn = signal
    imaginaryIn = [0.0] * N
    realOut = [0.0] * N
    imaginaryOut = [0.0] * N
    

    f4.dft(N, realIn, imaginaryIn, realOut, imaginaryOut, Direction=1)

    maxAbw = (0,0)
    abw = 0.0001
    for i in range(N):
        out_c = float(out[i])
        out_py = float(realOut[i])
        if abs(out_c - out_py) > abw:
            maxAbw = (abs(realOut[i] - signal[i]), i)
    print(maxAbw)

    #f4.plot_signal_and_spectrum(x, signal, realOut, imaginaryOut, N)

    

if __name__ == "__main__":    
    test_dft()
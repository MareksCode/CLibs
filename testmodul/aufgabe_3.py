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
from aufgabe_3_testfunc import *

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

def test_convolute_signal():
    test_function = "convoluteSignals"
    sig_1 = MMSignal()
    sig_1.set_samples([1,7,15,19,14,16,12,12,6,1])
    sig_2 = MMSignal()
    sig_2.set_samples([1,2,1])
    

    test_value = [[(sig_2, sig_1), True]] # hier ein paar wundervolle MMSignale

    for x in test_value:
        print(x)
        value_py =  convolute_signal(*x[0]) if x[1] else "Fail"
        logging.info(value_py)
        value = run_test(test_function, x)
        erroes = False if not "Error" in str(value) and not "exit-code" in str(value) else True
        passed = False

        signal = value[0]

        if x[1] and not erroes:
            for i in range(len(signal.samples)):
                if signal.samples[i] != value_py[i]:
                    passed = False
                    break
            passed = True
        else:
            if "exit" in str(value) or "Error" in str(value) or value is Exception:
                passed = True
            else:
                passed = False
        report.add_test(
                name=test_function,
                input_data={
                    "Filter": x[0][0].samples,
                    "Signal": x[0][1].samples              
                },
                expected = f"{value_py}",
                passed = passed,
                output = f"{signal.samples if passed and x[1] and not erroes else value}",
            )
    pass

def test_approx_bell():
    test_function = "approximateGaussianBellCurve"
    

    test_value = [[(10, 0), True]] # hier ein paar wundervolle MMSignale

    for x in test_value:
        value_py =  aproximate_gaussian(x[0][0]) if x[1] else "Fail"
        value = run_test(test_function, x)
        erroes = False if not "Error" in str(value) and not "exit-code" in str(value) else True
        passed = True

        signal = value[0]
        print(signal.samples)
        max_abw = 1.0e-2

        if x[1] and not erroes:
            
            for i in range(len(signal.samples)):
                if abs(signal.samples[i] - value_py[i]) > max_abw:
                    logging.info("Abweichung: %s", abs(signal.samples[i] - value_py[i]))
                    passed = False
                    break
            
        else:
            if "exit" in str(value) or "Error" in str(value) or value is Exception:
                passed = True
            else:
                passed = False
        report.add_test(
                name=test_function,
                input_data={
                    "Pascal Number": x[0][0]             
                },
                expected = f"{value_py}",
                passed = passed,
                output = f"{signal.samples if x[1] and not erroes else value}",
                notes = f"Maximale Abweichung: {max_abw}"
            )
        print(sum(signal.samples), sum(value_py))
    pass


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, filename="log.log", filemode="w", format="%(asctime)s - %(levelname)s - %(message)s", encoding="utf-8")
    #test_convolute_signal()
    test_approx_bell()
    report.write()
import logging
from secure_execution_v2 import Executioner
import random
import multiprocessing
from ctypes import *
from aufgabe_1_testfunc import *
import matplotlib.pyplot as plt



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
    

def test_interpolateLine():
    test_function = "interpolateLine"
    test_values = [[(1.0, 2.0, 3.0, 4.0, 5.0), False],
                   [(1.0, 2.0, 3.0, 4.0, 2.5), True],
                   [(1.0, 2.0, 3.0, 4.0, -0.1), False],
                   [(1.0, 2.0, 1.0, 2.0, 1.0), False],
                   [(1.0, None, 3.0, 4.0, 5.0), False]]
    for value in test_values: 
        resp = value[1]
        run_test(test_function, value)
        x = "Fehlschlag" if not resp else f"Ergebniss: {interpolateLine(*value[0])}"
        logging.info(f"Erwartet: {x}")
    
def test_scaleValuesInArray():
    numbers = [random.uniform(-1000,1000) for _ in range(5)]

    test_function = "scaleValuesInArray"
    test_values = [[(len(numbers), numbers, 3.0, 4.0), True],
                   [(7, numbers, 3.0, -4.0), False],
                   [(1, numbers, None, 4.0),False]]
    for value in test_values:
        resp = value[1]
        run_test(test_function, value)
        x = "Fehlschlag" if not resp else f"Ergebniss: {scaleValueInArray(value[0][1], value[0][2], value[0][3])}"
        logging.info(f"Erwartet: {x}")

# signal sine erzeugen, speichern, lesen über mms sig
def test_writeCreateArrayFile(): 
    test_function = "createSineSignal"
    test_value = [(100, 100, 2.0)]
    signal, _ = run_test(test_function, test_value)
    run_test("writeSignal", [(signal, "sine_output.txt")])
    values = [0.0] * 100
    out = run_test("readArrayFile", [("sine_output.txt", values)])
    plt.plot(signal.samples)
    plt.show()




if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, filename="log.log", filemode="w", format="%(asctime)s - %(levelname)s - %(message)s", encoding="utf-8")
    #test_interpolateLine()
    #test_scaleValuesInArray()
    test_writeCreateArrayFile()
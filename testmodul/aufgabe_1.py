import logging
from secure_execution import Executioner
import random
import multiprocessing
from ctypes import *

def run_test(test_function, test_values):
    for value in test_values:
        exec = Executioner()
        q = multiprocessing.Queue()
        process = exec.run_test_process(test_function, q, value)
        process.join()
        if not q.empty():
            logging.info("Eingabewerte: %s Das ergebniss ist: %s", value, q.get())
        else :
            logging.info("Test fehlgeschlagen mit exit-code: %s", process.exitcode)


def test_interpolateLine():
    test_function = "interpolateLine"
    test_values = [(1.0, 2.0, 3.0, 4.0, 5.0),
                   (1.0, 2.0, 3.0, 4.0, 2.5),
                   (1.0, 2.0, None, 4.0, 1.5),]
    run_test(test_function, test_values)
    
def test_scaleValuesInArray():
    numbers = [random.uniform(-1000,1000) for _ in range(5)]
    arr_type = c_double * len(numbers)
    c_array = arr_type(*numbers)
    test_function = "scaleValuesInArray"
    test_values = [(3, numbers, 3.0, 4.0),
                   (7, numbers, 3.0, -4.0),
                   (1, numbers, None, 4.0),]
    run_test(test_function, test_values)



if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, filename="log.log", filemode="w", format="%(asctime)s - %(levelname)s - %(message)s", encoding="utf-8")
    test_scaleValuesInArray()
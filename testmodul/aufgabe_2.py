import logging
from secure_execution import Executioner
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


def test_createHistogram_array():
    arr = [random.uniform(-1000,1000) for _ in range(1000)]
    test_function = "createHistogram_array"
    test_value = [(len(arr), arr, 10)]
    signal = run_test(test_function, test_value)
    plt.bar([i for i in range(len(signal.bins))], signal.bins)
    plt.show()



if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, filename="log.log", filemode="w", format="%(asctime)s - %(levelname)s - %(message)s", encoding="utf-8")
    test_createHistogram_array()
from ctypes import *
import logging
import threading
import loader
import multiprocessing
from collections.abc import Sequence

# Logging-Konfiguration
class Executioner:    
    def run_test_process(self, funcname, q, args):
        process = multiprocessing.Process(target=self.thread_func, args=(funcname, q, args))
        process.start()
        return process

    def thread_func(self, funcname, q, args):
        def c_array_to_string(result, length):
            """
            Wandelt ctypes-Array oder double* in String um
            """
            values = []

            if isinstance(result, Array):
                values = list(result)

            elif isinstance(result, POINTER(c_double)):
                for i in range(length):
                    values.append(result[i])

            else:
                return str(result)

            return ",".join(f"{v:.6f}" for v in values)
        
        lib = loader.load_lib()
        try:
            func = getattr(lib[0], funcname)
            converted_args = []
            array_length = 0
            for arg in args:
                if isinstance(arg, Sequence) and not isinstance(arg, (str, bytes)):
                    array_length = len(arg)
                    arr_type = c_double * len(arg)
                    c_array = arr_type(*arg)
                    converted_args.append(c_array)
                else:
                    converted_args.append(arg)

            result = func(*converted_args)
            print(type(result))
            if isinstance(result, (Array, POINTER(c_double))):
                result = c_array_to_string(result, array_length)
            q.put(result)
        except Exception as e:
            q.put(str(e))


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, filename="log.log", filemode="w", format="%(asctime)s - %(levelname)s - %(message)s", encoding="utf-8")
    test_functions = ["interpolateLine"]
    lib = loader.load_lib()
    exec = Executioner()
    q = multiprocessing.Queue()
    process = exec.run_test_process(test_functions[0], q, (1.0,2.0,3.0,4.0,5.0))
    process.join()
    print(q.empty())
    if not q.empty():logging.info("Das ergebniss ist: %s", q.get())


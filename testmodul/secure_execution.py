from ctypes import *
import logging
import loader
from strukturen import *
import multiprocessing
from collections.abc import Sequence
from collections import deque

# Logging-Konfiguration
class Executioner:    
    def __init__(self):
        self._keep_alive = []

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
            #formatieren der argumente, aber neu da sonst nicht pickable
            converted_args = []
            array_length = deque()

            for arg in args:
                if isinstance(arg, Histogram):
                    new_obj = Histogram()
                    new_obj.insert_values(*arg.get_values())
                    converted_args.append(new_obj)
                
                elif isinstance(arg, MMSignal):
                    samples_c = (c_double * arg.numberOfSamples)(*arg.samples)

                    new_obj = lib[1]()

                    new_obj.numberOfSamples = arg.numberOfSamples
                    new_obj.samples = samples_c
                    new_obj.area = arg.area
                    new_obj.mean = arg.mean
                    new_obj.localExtrema = None

                    self._keep_alive.append(samples_c)
                    self._keep_alive.append(new_obj)

                    converted_args.append(new_obj)
                
                elif isinstance(arg, LocalExtrema):
                    new_obj = LocalExtrema()
                    new_obj.insert_values(*arg.get_values())
                    converted_args.append(new_obj)
                
                elif isinstance(arg, (str, bytes)):
                    arr_type = c_char_p
                    c_array = arr_type(arg.encode("utf-8"))
                    converted_args.append(c_array)

                elif isinstance(arg, Sequence) and not isinstance(arg, (str, bytes)):
                    array_length.append(len(arg))
                    arr_type = c_double * len(arg)
                    c_array = arr_type(*arg)
                    converted_args.append(c_array)
                else:
                    converted_args.append(arg)

            #ausführen der funktion
            func = getattr(lib[0], funcname)
            result = func(*converted_args)

            # rückumwandlung

            # c_double array in string
            if isinstance(result, (Array, POINTER(c_double))):
                result = c_array_to_string(result, array_length.popleft())
            
            # LP_MMSignal in MMSignal
            if isinstance(result, POINTER(lib[1])):
                print("mmsignal detected")
                c_sig = result.contents
                py_sig = MMSignal()
                samples = [c_sig.samples[i] for i in range(c_sig.numberOfSamples)]

                local_extrema = None
                if c_sig.localExtrema:
                    c_ext = c_sig.localExtrema.contents

                    minimum_positions = [c_ext.minimumPositionArray[i] for i in range(c_ext.numberOfMinimumPositions)]
                    maximum_positions = [c_ext.maximumPositionArray[i] for i in range(c_ext.numberOfMaximumPositions)]

                    local_extrema = LocalExtrema(minimum_positions, maximum_positions)

                py_sig.insert_values(
                    numberOfSamples=c_sig.numberOfSamples,
                    samples=samples,
                    area=c_sig.area,
                    mean=c_sig.mean,
                    localExtrema=local_extrema
                )

                result = py_sig

            # LP_Histogram in Histogram - status unklar (funktionalität)
            if isinstance(result, POINTER(lib[2])):
                c_sig = result.contents
                py_sig = Histogram()    

                bins = [c_sig.bins[i] for i in range(c_sig.numberOfBins)]

                py_sig.insert_values(
                    numberOfBins=c_sig.numberOfBins,
                    bins=bins,
                    minimum=c_sig.minimum,
                    maximum=c_sig.maximum,
                    binWidth=c_sig.binWidth
                )      

                result = py_sig  

            q.put(result)
            
        except Exception as e:
            q.put(str(e))
            print(e)


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


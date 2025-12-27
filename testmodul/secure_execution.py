from ctypes import *
import logging
import loader
from strukturen import *
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

        #formatieren der argumente, aber neu da sonst nicht pickable
        try:
            func = getattr(lib[0], funcname)
            converted_args = []
            array_length = 0
            for arg in args:
                if isinstance(arg, Histogram):
                    new_obj = Histogram()
                    new_obj.insert_values(*arg.get_values())
                    converted_args.append(new_obj)
                
                elif isinstance(arg, MMSignal):
                    new_obj = MMSignal()
                    new_obj.insert_values(*arg.get_values())
                    converted_args.append(new_obj)
                
                elif isinstance(arg, LocalExtrema):
                    new_obj = LocalExtrema()
                    new_obj.insert_values(*arg.get_values())
                    converted_args.append(new_obj)

                elif isinstance(arg, Sequence) and not isinstance(arg, (str, bytes)):
                    array_length = len(arg)
                    arr_type = c_double * len(arg)
                    c_array = arr_type(*arg)
                    converted_args.append(c_array)
                else:
                    converted_args.append(arg)

            #ausführen der funktion
            result = func(*converted_args)


            print(type(result))

            #todo: rückumwandlunng in objekte
            if isinstance(result, (Array, POINTER(c_double))):
                result = c_array_to_string(result, array_length)
            
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


            if isinstance(result, POINTER(lib[2])):
                print("histogram detected")
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
            print(e.__traceback__)


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


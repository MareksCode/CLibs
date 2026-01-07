import logging
from secure_execution import Executioner
import loader
import multiprocessing
from strukturen import *
from ctypes import *
from collections import deque
from collections.abc import Sequence
import matplotlib.pyplot as plt
import os

    
class Executioner():
    def __init__(self):
        self._keep_alive = []
    
    def run_test_process(self, funcname, q, args):
        process = multiprocessing.Process(target=self.thread_func, args=(funcname, q, args))
        process.start()
        return process

    def prepare_args(self, lib, args):
        c_args = []
        meta = []            # merkt sich, wie rückzuwandeln ist

        for arg in args:
            # ---------- MMSignal ----------
            if isinstance(arg, MMSignal):
                samples_c = (c_double * arg.numberOfSamples)(*arg.samples)

                c_sig = lib[1]()   # MMSignal_c
                c_sig.numberOfSamples = arg.numberOfSamples
                c_sig.samples = samples_c
                c_sig.area = arg.area
                c_sig.mean = arg.mean
                c_sig.localExtrema = None

                self._keep_alive += [samples_c, c_sig]
                c_args.append(byref(c_sig))
                meta.append(("MMSignal", c_sig))

            # ---------- String ----------
            elif isinstance(arg, str):
                c_str = c_char_p(arg.encode("utf-8"))
                c_args.append(c_str)
                meta.append(("str", None))

            # ---------- Dynamisches Zahlenarray ----------
            elif isinstance(arg, Sequence) and not isinstance(arg, (str, bytes)):
                arr = (c_double * len(arg))(*arg)
                self._keep_alive.append(arr)
                c_args.append(arr)
                meta.append(("array", arr, len(arg)))

            # ---------- Primitive ----------
            else:
                c_args.append(arg)
                meta.append(("primitive", None))

        return c_args, meta

    def restore_result(self, lib, result, meta):        # MMSignal*
        if isinstance(result, POINTER(lib[1])):
            c_sig = result.contents

            samples = [c_sig.samples[i] for i in range(c_sig.numberOfSamples)]

            py_sig = MMSignal()
            py_sig.insert_values(
                numberOfSamples=c_sig.numberOfSamples,
                samples=samples,
                area=c_sig.area,
                mean=c_sig.mean,
                localExtrema=None
            )
            return py_sig

        # double*
        if isinstance(result, POINTER(c_double)):
    # Länge aus meta holen (erstes Array-Argument)
            for entry in meta:
                if entry[0] == "array":
                    _, _, length = entry
                    values = [result[i] for i in range(length)]
                    return ",".join(f"{v:.6f}" for v in values)

            raise ValueError("Keine Länge für double* gefunden")
        
        return result

    def restore_args(self, meta):
        restored = []

        for entry in meta:
            kind = entry[0]

            if kind == "MMSignal":
                c_sig = entry[1]
                samples = [c_sig.samples[i] for i in range(c_sig.numberOfSamples)]

                py_sig = MMSignal()
                py_sig.insert_values(
                    numberOfSamples=c_sig.numberOfSamples,
                    samples=samples,
                    area=c_sig.area,
                    mean=c_sig.mean,
                    localExtrema=None
                )
                restored.append(py_sig)

            elif kind == "array":
                arr, length = entry[1], entry[2]
                restored.append([arr[i] for i in range(length)])

            else:
                restored.append(None)

        return restored

    def thread_func(self, funcname, q, args):
        lib = loader.load_lib()

        try:
            c_args, meta = self.prepare_args(lib, args)

            func = getattr(lib[0], funcname)
            result_c = func(*c_args)

            result_py = self.restore_result(lib, result_c, meta)
            args_py = self.restore_args(meta)

            q.put([result_py, args_py])  
   

        except Exception as e:
            q.put(e)

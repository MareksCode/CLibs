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

report = TestReport("C Library Test Report – MMS25_26")

def run_test(test_function, value):
    value = value[0]
    exec = Executioner()
    q = multiprocessing.Queue()
    process = exec.run_test_process(test_function, q, value)
    process.join()
    if not q.empty():
        val = q.get()
        logging.info("Ergebniss: %s", val[100])
        return val

    else :
        logging.info("Ergebniss: exit-code: %s", process.exitcode)
        return "exit code %s" % process.exitcode

def todo(func):
    def wrapper(*args, **kwargs):
        raise NotImplementedError(f"funktion {func.__name__} ist unvollständig und funktioniert zur zeit nicht.")
    return wrapper

def test_interpolateLine():
    test_function = "interpolateLine"
    test_values = [[(1.0, 2.0, 3.0, 4.0, 5.0), False],
                   [(1.0, 2.0, 3.0, 4.0, 2.5), True],
                   [(1.0, 2.0, 3.0, 4.0, -0.1), False],
                   [(1.0, 2.0, 1.0, 2.0, 1.0), False],
                   [(1.0, None, 3.0, 4.0, 5.0), False]]
    
    # todo: vergleiche einfügen
    for value in test_values: 
        resp = value[1]
        val = run_test(test_function, value)
        erwartetes_ergebniss_erhalten = True
        abweichung = ""
        if resp:
            py_berechnet = interpolateLine(*value[0])
            # vergleiche ergebnisse mit erlaubter abwechung
            abw = 0.0001
            
            if abs(val[0] - py_berechnet) > abw:
                erwartetes_ergebniss_erhalten = False
                abweichung = f"c: {val}, py: {py_berechnet} (abweichung: {abs(val - py_berechnet)}) zu groß!"
                break
        else: 
            if not str(val).startswith("exit code") and "Error" not in str(val):
                abweichung = "keinen exit code erhalten"
                erwartetes_ergebniss_erhalten = False

        report.add_test(
            name="interpolateLine",
            input_data={
                "x1, y1": (f"({value[0][0]} | {value[0][1]})"),
                "x2, y2": (f"({value[0][2]} | {value[0][3]})"),
                "xb": str(value[0][3])
            },
            expected= "Fehlschlag" if not resp else f"{interpolateLine(*value[0])}",
            output = val,
            passed=erwartetes_ergebniss_erhalten,
            notes = abweichung
        )

        x = "Fehlschlag" if not resp else f"Ergebniss: {interpolateLine(*value[0])}"
        logging.info(f"Erwartet: {x}")
    
def test_scaleValuesInArray():
    numbers = [random.uniform(-1000,1000) for _ in range(20)]

    test_function = "scaleValuesInArray"
    test_values = [[(len(numbers), numbers, 3.0, 4.0), True],
                   [(7, numbers, 3.0, -4.0), True],
                   [(1, numbers, None, 4.0), False],
                   [(len(numbers), [], 1.0, 2.0), True],
                   [(len(numbers), [1,2,3], 1.0, 2.0), True]]
    
    
    # test auf korrektheit
    for value in test_values:
        resp = value[1]
        val = run_test(test_function, value)

        erwartetes_ergebniss_erhalten = True
        abweichung = ""
        py_berechnet = None

        try:
            py_berechnet = scaleValueInArray(*value[0])
        except:
            pass
        
        if "exit code" in str(val) or "Error" in str(val):
            if resp:
                abweichung = "keinen exit code erhalten" 
                erwartetes_ergebniss_erhalten = False
            else:
                abweichung = str(val)
                erwartetes_ergebniss_erhalten = True
        
        elif resp:
            erg_list = val[0].split(",")
            
            # vergleiche ergebnisse mit erlaubter abwechung
            abw = 0.0001
            for i in range(len(py_berechnet)):
                if abs(float(erg_list[i]) - py_berechnet[i]) > abw:
                    erwartetes_ergebniss_erhalten = False
                    abweichung = f"c: {erg_list[i]}, py: {py_berechnet[0]} (abweichung: {abs(float(erg_list[i]) - py_berechnet[0])}) zu groß!"
                    break
        else: 
            abweichung = "keinen exit code erhalten" 
            erwartetes_ergebniss_erhalten = False

        print(type(py_berechnet))

        report.add_test(
            name="scaleValuesInArray",
            input_data={
                "Number of Samples": value[0][0],
                "Werte": value[0][1],
                "Minimum": value[0][2],
                "Scaling Faktor": value[0][3]
            },
            expected= "Fehlschlag" if not resp or py_berechnet == None else f"{[round(float(x), 6) for x in py_berechnet]}",
            output = val if type(val) is not tuple else val[0].split(","),
            passed = erwartetes_ergebniss_erhalten,
            notes = abweichung
        )

        #x = "Fehlschlag" if not resp else f"Ergebniss: {scaleValueInArray(value[0][1], value[0][2], value[0][3])}"
        #logging.info(f"Erwartet: {x}")

# anassen an neuen header
def test_writeCreateArrayFile(): 
    test_function = "createSineSignal"
    test_value = [(700, 200, 2.0)]

    signal, _ = run_test(test_function, test_value)

    run_test("writeSignal", [(signal, "sine_output.txt")])

    #values = [0.0] * test_value[0][0]
    x = c_int(0)
    out = run_test("readArrayFile", [("sine_output.txt", x)])
    
    crash = False
    if "exit code" in str(out) or "Error" in str(out):
        crash = True
    
    periods = test_value[0][0] / test_value[0][1]

    
    plt.title("Sine Signal") 
    plt.plot(np.linspace(0, periods * np.pi, test_value[0][0]), signal.samples)
    plt.savefig(os.path.join(report.image_dir, "sine_wave.png"))
    
    if not crash:
        plt.title("Gelesenes Signal")
        val = out[0].split(",")
        val = [float(x) for x in val]
        plt.plot(np.linspace(0, periods * np.pi, test_value[0][0]), val)
        plt.savefig(os.path.join(report.image_dir, "sine_wave_read.png"))
    
    report.add_test(
            name="createSineSignal",
            input_data={
                "Total Samples": test_value[0][0],
                "Sample per Period": test_value[0][1],
                "Amplitude": test_value[0][2]
            },
            expected = "signal erstellt",
            output = "vergeliche bild" ,
            plots=["sine_wave.png"],
            passed="maby" ,
        )
    
    report.add_test(
            name="Schreiben und Lesen von Signalen",
            input_data={
                "Werte": "übernahme des vorherigen generierten sin-signales",
            },

            expected = "signal geschrieben und gelesen",
            output = "vergeliche bild mit erstem. sollte identisch sein." if not crash else str(out),
            plots=["sine_wave_read.png"] if not crash else None,
            passed="maby" if not crash else False,
        )
    #plt.show()




if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, filename="log.log", filemode="w", format="%(asctime)s - %(levelname)s - %(message)s", encoding="utf-8")
    test_interpolateLine()
    test_scaleValuesInArray()
    test_writeCreateArrayFile()
    report.write()
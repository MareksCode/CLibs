import matplotlib.pyplot as plt
import numpy as np
from report_writer import TestReport
import os

x = np.linspace(0, 2*np.pi, 256)
y = np.sin(3*x)
report = TestReport("C Library Test Report – MMS25_26")
plt.figure()
plt.plot(x, y)
plt.title("Eingangssignal")
plt.savefig(os.path.join(report.image_dir, "signal.png"))
plt.close()




report.add_test(
    name="DFT Sinus-Test",
    input_data={
        "Signal": "sin(2π·3t)",
        "Samples": 256
    },
    expected="Peak bei Frequenzindex 3",
    output="Peak bei Frequenzindex 3",
    passed=True,
    plots=[
        "signal.png",
    ]
)

report.write()

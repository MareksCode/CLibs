import numpy as np
import matplotlib.pyplot as plt

def dft(numberOfValues, realIn, imaginaryIn, realOut, imaginaryOut, Direction):
    for k in range(numberOfValues):
        realOut[k] = 0.0
        imaginaryOut[k] = 0.0
        for n in range(numberOfValues):
            angle = Direction * 2 * np.pi * k * n / numberOfValues
            realOut[k] += realIn[n] * np.cos(angle) + imaginaryIn[n] * np.sin(angle)
            imaginaryOut[k] += -realIn[n] * np.sin(angle) + imaginaryIn[n] * np.cos(angle)

def create_signal(N):
    x = np.linspace(0, 2 * np.pi, N)
    
    y1 = np.sin(x)  
    y2 = 0.3 * np.sin(3*x)  
    y3 = 0.2 * np.sin(5*x)
    y4 = 0.125 * np.sin(7*x)
    
    signal = y1+y2+y3+y4
    return x, signal

def plot_signal_and_spectrum(x, signal, realOut, imaginaryOut, N):
    plt.figure(figsize=(12, 6))
    plt.subplot(1, 2, 1)
    plt.plot(x, signal, label="Überlagertes Signal")
    plt.title("Eingangssignal (Zeitbereich)")
    plt.xlabel("Zeit")
    plt.ylabel("Amplitude")
    plt.grid(True)
    
    plt.subplot(1, 2, 2)
    plt.plot(range(N // 2), np.abs(realOut[:N // 2]), label="Magnitude")
    plt.title("Spektrum des Signals (Frequenzbereich)")
    plt.xlabel("Frequenz (Hz)")
    plt.ylabel("Amplitude")
    plt.grid(True)
    
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    N = 300
    x, signal = create_signal(N)
    
    realIn = signal
    imaginaryIn = [0.0] * N
    realOut = [0.0] * N
    imaginaryOut = [0.0] * N
    
    dft(N, realIn, imaginaryIn, realOut, imaginaryOut, Direction=1)
    plot_signal_and_spectrum(x, signal, realOut, imaginaryOut, N)

    #dft(N, realOut, imaginaryOut, realIn, imaginaryIn, Direction=-1)
    #plot_signal_and_spectrum(x, signal, realIn, imaginaryIn, N)


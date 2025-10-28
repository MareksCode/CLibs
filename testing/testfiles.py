from ctypes import *
import os

os.system("gcc -fPIC -shared -o ./library.co ./library.c")

file = "./libary.c"
fnc = CDLL("./library.co")

fnc.lerp.argtypes = [c_double, c_double, c_double]
fnc.lerp.restype = c_double
print(fnc.lerp(10,20,0.5))
print(fnc.lerp(20,10,0.5))
print(fnc.lerp(-10,20,0.5))
print(fnc.lerp(10,-20,0.5))
print(fnc.lerp(10,20,-15))
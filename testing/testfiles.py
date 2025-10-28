from ctypes import *
import os

class Testing():
    def __init__(self, filename:str):
        """creates an co (coperative object) file from a c file\n
        @filename: name of the c file in current working directory
        """
        os.system(f"gcc -fPIC -shared -o ./{filename}o ./filename")
        self.filename = filename
        self.sharedfilename = f"{filename}o"

    def test_lerp():
        """tests the lerp function in library.co"""
        fnc = CDLL("./library.co")

        fnc.lerp.argtypes = [c_double, c_double, c_double]
        fnc.lerp.restype = c_double
        print(fnc.lerp(10,20,0.5))
        print(fnc.lerp(20,10,0.5))
        print(fnc.lerp(-10,20,0.5))
        print(fnc.lerp(10,-20,0.5))
        print(fnc.lerp(10,20,-15))

    def __del__(self):
        os.remove(self.sharedfilename)    

if __name__ == "__main__":
    test = Testing("library.c")
    test.test_lerp()
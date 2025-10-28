from ctypes import *
import os, unittest

class Testing():
    def __init__(self, filename:str):
        """creates an co (coperative object) file from a c file\n
        @filename: name of the c file in current working directory
        """
        os.system(f"gcc -fPIC -shared -o ./{filename}o ./{filename}")
        self.filename = filename
        self.sharedfilename = f"{filename}o"

    def test_lerp(self):
        """tests the lerp function in library.co"""
        fnc = CDLL("./"+self.sharedfilename)

        fnc.lerp.argtypes = [c_double, c_double, c_double]
        fnc.lerp.restype = c_double
        print(fnc.lerp(10,20,0.5))
        print(fnc.lerp(20,10,0.5))
        print(fnc.lerp(-10,20,0.5))
        print(fnc.lerp(10,-20,0.5))
        print(fnc.lerp(10,20,-15))

    def test_unify(self):
        """tests the unify function in library.co"""
        fnc = CDLL("./"+self.sharedfilename)
        fnc.scaled.argtypes = [
            POINTER(c_double),
            c_int, 
            c_double,                 
            c_double          
        ]

        fnc.scaled.restype = POINTER(c_double)

        numbers = [1.0, 2.0, 3.0, 4.0]
        arr_type = c_double * len(numbers)
        c_array = arr_type(*numbers)
        
        minimum = 0
        alpha = 2.2
        length = len(numbers)
        
        result_ptr = fnc.scaled(c_array, length, minimum, alpha)
        
        result = [result_ptr[i] for i in range(length)]
        print("Result:", result)

if __name__ == "__main__":
    test = Testing("library.c")
    #test.test_lerp()
    test.test_unify()
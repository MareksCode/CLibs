from ctypes import *
import os, math, random

class Testing():
    def __init__(self, filename:str):
        """creates an co (coperative object) file from a c file\n
        @filename: name of the c file in current working directory
        """
        os.system(f"gcc -fPIC -shared -o ./{filename}o ./{filename}")
        self.filename = filename
        self.sharedfilename = f"{filename}o"

    def test_all(self):
        """runs all tests"""
        print("###################################\n")
        print("testing lerp...")
        self.test_lerp()
        print("\n###################################\n")
        print("testing unify...")
        self.test_unify()
        print("\n###################################\n")
        print("testing createSineArray...")
        print (f"test für sinus ist {self.test_createSineArray()}") 
        print("\n###################################\n")
        print("testing create and read array file...")
        self.test_create_read()
        print("\n###################################")

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

    def test_createSineArray(self):
        fnc = CDLL("./"+self.sharedfilename)
        fnc.scaled.argtypes = [
            c_double,
            c_double,
            ]

        fnc.createSineArray.restype = POINTER(c_double)

        samplingRate = c_double(10)
        amplitude = c_double(1.0)
        stepsize = (2*math.pi) / samplingRate.value

        result_ptr = fnc.createSineArray(samplingRate, amplitude)
        isValid = True
        averageAbweichung = 0.0
        for i in range (int(samplingRate.value)):
            sinval = math.sin(stepsize * i) * amplitude.value
            abweichung = abs(result_ptr[i] - sinval)
            averageAbweichung += abweichung
            averageAbweichung /= 2
            if abweichung > 0.002: 
                isValid = False
                print(f"{i}: {result_ptr[i]},  (abweichung: {abweichung}) zu groß!")
        print(f"average abweichung: {averageAbweichung}")
        return isValid
    
    def test_create_read(self):
        """tests the createArrayFile and readSavedArrayFile functions
        prints out number of generated values, sucess of writing and if generated
        values match read values."""
        fnc = CDLL("./"+self.sharedfilename)
        fnc.createArrayFile.argtypes = [
            POINTER(c_double),
            c_int
            ]
        fnc.createArrayFile.restype = int

        #x ist liste mit nummern die getestet werden, ggf, auch über sinarray erstellbar
        x = list()
        for _ in range(random.randint(1000,10000)):
            x.append(random.randint(-10000,10000))
        
        numbers = list(x)
        length = len(numbers)
        arr_type = c_double * len(numbers)
        c_array = arr_type(*numbers)

        result = fnc.createArrayFile(c_array, length)
        print(f"Created array with {length} numbers. writing output: {result}")

        if result == 1:
            fnc.readSavedArrayFile.argtypes = []
            fnc.readSavedArrayFile.restype = POINTER(c_double)
            result_ptr = fnc.readSavedArrayFile()
            result = [result_ptr[i] for i in range(length)]
            print("Reading matches original:", bool(result == x))


if __name__ == "__main__":
    test = Testing("library.c")
    test.test_all()   
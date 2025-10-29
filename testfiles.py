from ctypes import *
import os, math, random

class Testing():
    def __init__(self, filename:str, iterations:int=50):
        """creates an co (coperative object) file from a c file\n
        @filename: name of the c file in current working directory
        """
        os.system(f"gcc -fPIC -shared -o ./{filename}o ./{filename}")
        self.filename = filename
        self.sharedfilename = f"{filename}o"
        self.iterations = iterations

    def lerp(self, a, b, t):
        return a + t * (b - a)
    
    def lerp_2d(self, p1, p2, t):
        x = self.lerp(p1[0], p2[0], t)
        y = self.lerp(p1[1], p2[1], t)
        return (x, y)

    def test_all(self):
        """runs all tests"""
        print("\n###################################\n")
        print(f"start test with {self.iterations} iterations per unit")
        print("###################################\n")
        print("testing lerp...")
        self.test_lerp()
        print("\n###################################\n")
        print("testing lerp2D...")
        self.test_lerp_2d()
        print("\n###################################\n")
        print("testing unify...")
        self.test_unify()
        print("\n###################################\n")
        print("testing createSineArray...")
        self.test_createSineArray()
        print("\n###################################\n")
        print("testing create and read array file...")
        self.test_create_read()
        print("\n###################################")

    def test_lerp(self):
        """tests the lerp function in library.co"""
        fnc = CDLL("./"+self.sharedfilename)

        fnc.interpolateDigitsByAlpha.argtypes = [c_double, c_double, c_double]
        fnc.interpolateDigitsByAlpha.restype = c_double
        print(fnc.interpolateDigitsByAlpha(10,20,0.5))
        print(fnc.interpolateDigitsByAlpha(20,10,0.5))
        print(fnc.interpolateDigitsByAlpha(-10,20,0.5))
        print(fnc.interpolateDigitsByAlpha(10,-20,0.5))
        #print(fnc.interpolateDigitsByAlpha(10,20,-15))

    def test_lerp_2d(self):
        fnc = CDLL("./"+self.sharedfilename)
        fnc.interpolate2DPointsByAlpha.argtypes = [
            POINTER(c_double),
            POINTER(c_double), 
            c_double          
        ]
        fnc.interpolate2DPointsByAlpha.restype = POINTER(c_double)

        for i in range(self.iterations):
            a = (random.uniform(-100,100), random.uniform(-100,100))
            b = (random.uniform(-100,100), random.uniform(-100,100))
            t = random.uniform(0,1)

            a_arr = (c_double * 2)(a[0], a[1])
            b_arr = (c_double * 2)(b[0], b[1])
            
            res_ptr = fnc.interpolate2DPointsByAlpha(a_arr, b_arr, t)
            res_c = (res_ptr[0], res_ptr[1])
            res_py = self.lerp_2d(a, b, t)

            # Vergleich
            diff_x = abs(res_c[0] - res_py[0])
            diff_y = abs(res_c[1] - res_py[1])
            tol = 0.005
            assert diff_x < tol and diff_y < tol, (
                f"Fehler bei Test {i}: "
                f"C={res_c}, Python={res_py}, Δx={diff_x}, Δy={diff_y}"
                f"{a}, {b}, t={t}"
            )
        
        print("test completed successfully")


    def test_unify(self):
        """tests the unify function in library.co"""
        fnc = CDLL("./"+self.sharedfilename)
        fnc.scaleNumbersInArray.argtypes = [
            POINTER(c_double),
            c_int, 
            c_double,                 
            c_double          
        ]

        fnc.scaleNumbersInArray.restype = POINTER(c_double)

        numbers = [1.0, 2.0, 3.0, 4.0]
        arr_type = c_double * len(numbers)
        c_array = arr_type(*numbers)
        
        minimum = 0
        alpha = 2.2
        length = len(numbers)
        
        result_ptr = fnc.scaleNumbersInArray(c_array, length, minimum, alpha)
        
        result = [result_ptr[i] for i in range(length)]
        print("Result:", result)

    def test_createSineArray(self):
        fnc = CDLL("./"+self.sharedfilename)
        fnc.createSineArray.argtypes = [
            c_double,
            c_double,
            ]

        fnc.createSineArray.restype = POINTER(c_double)

        samplingRate = c_double(10)
        amplitude = c_double(1.0)
        stepsize = (2*math.pi) / samplingRate.value

        result_ptr = fnc.createSineArray(samplingRate, amplitude)
        averageAbweichung = 0.0
        for i in range (int(samplingRate.value)):
            sinval = math.sin(stepsize * i) * amplitude.value
            abweichung = abs(result_ptr[i] - sinval)
            averageAbweichung += abweichung
            averageAbweichung /= 2
            assert abweichung < 0.002, (
                f"{i}: {result_ptr[i]},  (abweichung: {abweichung}) zu groß!"
            )
        
        print(f"average abweichung: {averageAbweichung}\nTest completed successfully.")


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
            x.append(float(random.uniform(-10000,10000).__format__('.5f')))
        
        numbers = list(x)
        length = len(numbers)
        arr_type = c_double * len(numbers)
        c_array = arr_type(*numbers)

        result = fnc.createArrayFile(c_array, length)
        print(f"Created array with {length} numbers. writing output: {result}")
        
        assert result == 1, "writing failed!"
        
        fnc.readSavedArrayFile.argtypes = []
        fnc.readSavedArrayFile.restype = POINTER(c_double)
        result_ptr = fnc.readSavedArrayFile()
        result = [result_ptr[i] for i in range(length)]

        assert result==x, "file is not what should be in there!"
        print("test completed successfully")


if __name__ == "__main__":
    test = Testing("library.c")
    test.test_all()
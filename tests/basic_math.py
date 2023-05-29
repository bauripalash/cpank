import utils
import unittest

ch = utils.CpankHandler()
ch.cpank_exe = "zig-out/bin/pankti"

class BasicMathTesting(unittest.TestCase):
    def test_addition(self):
        inputs = {"show 1+2" : "3" , "show 3 * 3" : "9"}
        for s , r in inputs.items():
            self.assertEqual(ch.get(s) , r)
    def test_substraction(self):
        inputs = {"show 1-2" : "-1" , "show 3 - 3" : "0"}
        for s , r in inputs.items():
            self.assertEqual(ch.get(s) , r)
    def test_multiplication(self):
        inputs = {"show 9 * 9" : "81" , "show 2 * 2 * (3+4)" : "28"}
        for s , r in inputs.items():
            self.assertEqual(ch.get(s) , r)
    def test_division(self):
        inputs = {"show 1/2" : "0.5" , "show 100/10" : "10"}
        for s , r in inputs.items():
            self.assertEqual(ch.get(s) , r)




if __name__ == "__main__":
    unittest.main()
            

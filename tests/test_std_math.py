from .utils import CpankHandler
import unittest
import math

ch = CpankHandler()

math_import = 'import m \\"math\\"'

class TestStdMath(unittest.TestCase):
    def test_add(self):
        inputs = {'show(m.add(99 , 99))' : str(99+99)}
        for s, r in inputs.items():
            self.assertEqual(ch.get(math_import + s), r)

    def test_gcd(self):
        inputs = {'show(m.gcd(33,3))' : str(math.gcd(33,3)),
                  'show(m.gcd(1,100))' : str(math.gcd(1,100))}
        for s, r in inputs.items():
            self.assertEqual(ch.get(math_import + s), r)

    def test_lcm(self):
        inputs = {'show(m.lcm(33,22))' : str(math.lcm(33,22))}
        for s, r in inputs.items():
            self.assertEqual(ch.get(math_import + s), r)

    def test_sqrt(self):
        inputs = {'show(m.sqrt(33))' : "5.74456" , 'show(m.sqrt(100))' : "10"}
        for s, r in inputs.items():
            self.assertEqual(ch.get(math_import + s), r)


if __name__ == "__main__":
    unittest.main()


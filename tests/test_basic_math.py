from .utils import CpankHandler
import unittest

ch = CpankHandler()

function_addition = """
fun add(a , b)
    return (a + b) * 99
end

show(add(99 , 99))
"""


class TestBasicMath(unittest.TestCase):
    def test_addition(self):
        inputs = {
            "show(1+2)": "3",
            "let number = 100 + 200;show(number + number)": "600",
            function_addition: str((99 + 99) * 99),
            "show((3 * 3) + 100)": "109",
        }
        for s, r in inputs.items():
            self.assertEqual(ch.get(s), r)

    def test_substraction(self):
        inputs = {"show(1-2)": "-1", "show(3 - 3)": "0"}
        for s, r in inputs.items():
            self.assertEqual(ch.get(s), r)

    def test_multiplication(self):
        inputs = {"show(9 * 9)": "81", "show(2 * 2 * (3+4))": "28"}
        for s, r in inputs.items():
            self.assertEqual(ch.get(s), r)

    def test_division(self):
        inputs = {"show(1/2)": "0.5", "show(100/10)": "10"}
        for s, r in inputs.items():
            self.assertEqual(ch.get(s), r)


if __name__ == "__main__":
    unittest.main()

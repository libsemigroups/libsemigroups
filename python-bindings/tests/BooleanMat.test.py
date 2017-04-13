import unittest
import sys
import os

path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
if not path in sys.path:
    sys.path.insert(1, path)
del path

from semigroups import BooleanMat

class TestPartialPerm(unittest.TestCase):
    def test_init(self):
        BooleanMat([True, True], [False, False])
        BooleanMat([False, True, True], [True, True, False], [False, False, False])
        BooleanMat([True])
        BooleanMat([True, False], [False, True])

    def test_init_fail(self):
        with self.assertRaises(TypeError):
            BooleanMat(True)
            BooleanMat(set([True, False]), set([False, True]))
            BooleanMat(26)
            BooleanMat([1, 0], [0, 1])
            BooleanMat([True, False], ["i", range(10)])

        with self.assertRaises(ValueError):
            BooleanMat([True], [False])
            BooleanMat([True, False], [False, False], [True, True])
            BooleanMat([True, True, False], [False, False])        

if __name__ == '__main__':
    unittest.main()

import unittest
import sys
import os

path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
if not path in sys.path:
    sys.path.insert(1, path)
del path

from semigroups import Transformation, PartialPerm
import semigroups


class TestTransformation(unittest.TestCase):
    def test_init(self):
        Transformation([0, 1, 2, 3])
        Transformation([1, 1, 3, 2, 4, 3])
        Transformation([9, 3, 1, 2, 0, 8, 1, 2, 0, 5])

    def test_init_fail(self):
        with self.assertRaises(ValueError):
            Transformation([1,5,26])
            Transformation([1])

        with self.assertRaises(TypeError):
            Transformation(26)

    def test_mul(self):
        self.assertEqual(Transformation([1, 3, 2, 1]) * Transformation([0, 3, 2, 2]), Transformation([3, 2, 2, 3]))
        self.assertEqual(Transformation([2, 2, 2]) * Transformation([1, 0, 1]), Transformation([1, 1, 1]))
        self.assertEqual(Transformation([0, 1, 2, 3, 4, 5]) * Transformation([3, 2, 2, 3, 1, 4]), Transformation([3, 2, 2, 3, 1, 4]))

        with self.assertRaises(AssertionError):
            Transformation([0, 2, 1]) * PartialPerm([0, 1], [1, 2], 3)
            Transformation([0, 2, 1]) * Transformation([1, 2, 3, 0])



if __name__ == '__main__':
    unittest.main()

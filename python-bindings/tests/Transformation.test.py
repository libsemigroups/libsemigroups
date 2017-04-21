import unittest
import sys
import os

path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
if not path in sys.path:
    sys.path.insert(1, path)
del path

from semigroups import Transformation, PartialPerm, Bipartition
import semigroups

class TestTransformation(unittest.TestCase):
    def test_init(self):
        Transformation([0, 1, 2, 3])
        Transformation([1, 1, 3, 2, 4, 3])
        Transformation([9, 3, 1, 2, 0, 8, 1, 2, 0, 5])

    def test_init_fail(self):
        with self.assertRaises(ValueError):
            Transformation([1, 5, 26])
        with self.assertRaises(ValueError):
            Transformation([1])

        with self.assertRaises(TypeError):
            Transformation(26)
        with self.assertRaises(TypeError):
            Transformation(['a', 'b'])
        with self.assertRaises(TypeError):
            Transformation([0.1, 1.0])

    def test_richcmp(self):
        assert Transformation([1, 2, 2, 0]) == Transformation([1, 2, 2, 0])
        assert not Transformation([1, 2, 2, 0]) == Transformation([1, 2, 1, 3])
        assert Transformation([1, 2, 2, 0]) != Transformation([1, 2, 1, 3])
        assert not Transformation([2, 2, 1]) < Transformation([0, 1, 2])
        assert Transformation([2, 2, 1]) > Transformation([0, 1, 2])
        assert Transformation([2, 2, 1]) <= Transformation([2, 2, 2])
        assert Transformation([2, 2, 2]) <= Transformation([2, 2, 2])
        assert Transformation([2, 2, 2]) >= Transformation([2, 2, 0])
        assert Transformation([3, 2, 3, 0]) >= Transformation([3, 2, 0, 1])

        with self.assertRaises(TypeError):
            Transformation([2, 2, 0]) == Bipartition([1, -1], [2, 3, -2], [-3])
        with self.assertRaises(TypeError):
            Bipartition([1, -1], [2, -2]) < Transformation([0, 1])
        with self.assertRaises(TypeError):
            Bipartition([1, -1], [2, -2]) != Transformation([0, 1])

    def test_mul(self):
        self.assertEqual(Transformation([1, 3, 2, 1]) * \
                         Transformation([0, 3, 2, 2]), 
                         Transformation([3, 2, 2, 3]))
        self.assertEqual(Transformation([2, 2, 2]) * Transformation([1, 0, 1]),
                         Transformation([1, 1, 1]))
        self.assertEqual(Transformation([0, 1, 2, 3, 4, 5]) * \
                         Transformation([3, 2, 2, 3, 1, 4]), 
                         Transformation([3, 2, 2, 3, 1, 4]))

        with self.assertRaises(TypeError):
            Transformation([0, 2, 1]) * PartialPerm([0, 1], [1, 2], 3)
        with self.assertRaises(TypeError):
            Transformation([0, 2, 1]) * Bipartition([1, -1], [2, 3, -3], [-2])
        with self.assertRaises(TypeError):
            Transformation([0, 1, 2, 3, 4, 5]) * 8

        with self.assertRaises(ValueError):
            Transformation([0, 2, 1]) * Transformation([1, 2, 3, 0])

    def test_pow(self):
        self.assertEqual(Transformation([9, 3, 1, 2, 0, 8, 1, 2, 0, 5]) ** 6,
                         Transformation([5, 1, 2, 3, 9, 0, 2, 3, 9, 8]))
        self.assertEqual(Transformation([2, 2, 2]) ** 30, 
                         Transformation([2, 2, 2]))
        self.assertEqual(Transformation([1, 2, 3, 3]) ** 8, 
                         Transformation([3, 3, 3, 3]))
 
        with self.assertRaises(ValueError):
            Transformation([1, 2, 3, 0]) ** -1
        with self.assertRaises(ValueError):
            Transformation([1, 1, 3, 2, 4, 3]) ** 0

        with self.assertRaises(TypeError):
            Transformation([1, 0, 1, 2]) ** 1.5
        with self.assertRaises(TypeError):
            Transformation([3, 2, 0, 0]) ** 'l'

    def test_dealloc(self):
        U, V = Transformation([1, 0, 1, 2]), Transformation([1, 1, 3, 2, 4, 3])
        del U, V
        with self.assertRaises(NameError):
            V
        with self.assertRaises(NameError):
            U

    def test_identity(self):
        self.assertEqual(Transformation([9, 3, 1, 2, 0, 
                                         8, 1, 2, 0, 5]).identity(),
                         Transformation([0, 1, 2, 3, 4, 5, 6, 7, 8, 9]))
        self.assertEqual(Transformation([2, 2, 2]).identity(), 
                         Transformation([0, 1, 2]))
        self.assertEqual(Transformation([1, 2, 3, 3]).identity(), 
                         Transformation([0, 1, 2, 3]))

    def test_degree(self):
        self.assertEqual(Transformation([9, 3, 1, 2, 0, 
                                         8, 1, 2, 0, 5]).degree(), 10)
        self.assertEqual(Transformation([2, 2, 2]).degree(), 3)
        self.assertEqual(Transformation([1, 2, 3, 3]).degree(), 4)

    def test_repr(self):
        self.assertEqual(eval(Transformation([9, 3, 1, 2, 0, 
                                         8, 1, 2, 0, 5]).__repr__()),
                         Transformation([9, 3, 1, 2, 0, 
                                         8, 1, 2, 0, 5]))
        self.assertEqual(eval(Transformation([2, 2, 2]).__repr__()),
                         Transformation([2, 2, 2]))
        self.assertEqual(eval(Transformation([1, 2, 3, 3]).__repr__()),
                         Transformation([1, 2, 3, 3]))

if __name__ == '__main__':
    unittest.main()

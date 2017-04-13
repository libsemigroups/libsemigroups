import unittest
import sys
import os

path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
if not path in sys.path:
    sys.path.insert(1, path)
del path

from semigroups import BooleanMat, Bipartition, Transformation, PartialPerm

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

    def test_richcmp(self):
        assert BooleanMat([True, False], [False, True]) == BooleanMat([True, False], [False, True])
        assert not BooleanMat([True, False], [False, True]) != BooleanMat([True, False], [False, True])
        assert not BooleanMat([True, False], [False, True]) == BooleanMat([False, False], [False, True])
        assert BooleanMat([False]) < BooleanMat([True])
        assert not BooleanMat([False, True, True], [True, True, False], [False, False, False]) < BooleanMat([False, True, False], [True, False, False], [False, False, True])
        assert BooleanMat([False]) <= BooleanMat([False])
        assert BooleanMat([True, False], [False, True]) > BooleanMat([True, False], [False, False])
        assert not BooleanMat([True, False], [False, True]) > BooleanMat([True, False], [False, True])
        assert BooleanMat([False]) >= BooleanMat([False])

        with self.assertRaises(TypeError):
            Bipartition([1, -2], [-1, 2]) >= BooleanMat([False, False], [True, False])
            BooleanMat([False, False], [True, False]) < Transformation([0, 1])
            BooleanMat([True, False], [False, True]) == PartialPerm([0], [1], 2)

    def test_mul(self):
        self.assertEqual(BooleanMat([True, False], [False, True]) * BooleanMat([False, False], [False, True]), BooleanMat([False, False], [False, True]))
        self.assertEqual(BooleanMat([False]) * BooleanMat([True]), BooleanMat([False]))
        self.assertEqual(BooleanMat([False, True, True], [True, True, False], [False, False, False]) * BooleanMat([False, True, False], [True, False, False], [False, False, True]), BooleanMat([True, False, True], [True, True, False], [False, False, False]))

        with self.assertRaises(TypeError):
            BooleanMat([True, True], [False, False]) * Transformation([1, 1])
            BooleanMat([False, True, True], [True, True, False], [False, False, False]) * PartialPerm([0, 1], [1, 2], 3)
            BooleanMat([True]) * [True]
            BooleanMat([True, False], [False, True]) * Bipartition([1, 2], [-1], [-2])

        with self.assertRaises(ValueError):
            BooleanMat([False, True, True], [True, True, False], [False, False, False]) * BooleanMat([True, False], [False, True])

    def test_pow(self):
        self.assertEqual(BooleanMat([True, False], [False, True]) ** 30, BooleanMat([True, False], [False, True]))
        self.assertEqual(BooleanMat([True, False], [True, True]) ** 7, BooleanMat([True, False], [True, True]))
        self.assertEqual(BooleanMat([True]) ** 26, BooleanMat([True]))

        with self.assertRaises(TypeError):
            BooleanMat([True, False], [True, True]) ** 'i'
            BooleanMat([True]) ** range(10)
            BooleanMat([True]) ** BooleanMat([True])

        with self.assertRaises(ValueError):
            BooleanMat([True, False], [True, True]) ** 0
            BooleanMat([False, True, True], [True, True, False], [False, False, False]) ** -7

    def test_dealloc(self):
        A, B = BooleanMat([True, False], [True, True]), BooleanMat([False, False], [False, True])
        del A, B
        assert not 'A' in globals()
        assert not 'B' in globals()        

if __name__ == '__main__':
    unittest.main()

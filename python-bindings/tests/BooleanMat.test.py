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

if __name__ == '__main__':
    unittest.main()

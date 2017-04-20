import unittest
import sys
import os

path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
if not path in sys.path:
    sys.path.insert(1, path)
del path

from semigroups import PBR, BooleanMat, Bipartition, Transformation, PartialPerm

class TestPBR(unittest.TestCase):
    def test_init(self):
        PBR([[1, -1]], [[1]])
        PBR([[1, -1], [-2, -1]], [[1], [-2, -1]])
        PBR([[1, -1], [-2, -1, 2]], [[2], [-2]])
        PBR([[1, -1, 3], [-2, -1, 2], [3, -2]], [[2], [-2], [1, -1, 2]])
        PBR([[1, 2], [-2, 1, 2], [3, -3]], [[2, 1], [-2, 2, 1], [1, -1]])

    def test_init_fail(self):
        with self.assertRaises(TypeError):
            PBR([[1, -1]], [[1]], [[1]])
            PBR(set([(1, -1)]), set([(1)]))
            PBR('a', 2)

        with self.assertRaises(ValueError):
            PBR([[1, -1], [2, -1]], [[1]])
            PBR([[1, -1], [-2, -1]], [[0], [-2, -1]])
            PBR([[1, -1], [-2, -1, 2]], [[3], [-2]])
            PBR([[1, -1, 3], [-2, -1, 2], [3, 3, -2]], [[2], [-2], [1, -1, 2]])

    def test_richcmp(self):
        assert PBR([[1, -1], [-2, -1, 2]], [[2], [-2]]) == \
               PBR([[1, -1], [-2, -1, 2]], [[2], [-2]])
        assert not PBR([[1, -1], [-2, -1, 2]], [[2], [-2]]) != \
                   PBR([[1, -1], [-2, -1, 2]], [[2], [-2]])
        assert PBR([[1, -1], [-2, -1, 2]], [[2], [-2]]) != \
               PBR([[1, 2], [-2, 1, 2], [3, -3]], [[2, 1], [-2, 2, 1], [1, -1]])
        assert PBR([[1], [1, 2, -1]], [[1], [2, -1, 1]]) < \
               PBR([[1], [2]], [[-1], [-2]])
        assert PBR([[1, -1, 3], [-2, -1, 2], [3, -2]], 
                   [[2], [-2], [1, -1, 2]]) > \
               PBR([[1, -1], [-2, -1, 2]], [[2], [-2]])
        assert PBR([[1, -1, 3], [-2, -1, 2], [3, -2]], 
                   [[2], [-2], [1, -1, 2]]) >= \
               PBR([[1, -1], [-2, -1, 2]], [[2], [-2]])
        assert PBR([[1, -1], [-2, -1, 2]], [[2], [-2]]) <= \
               PBR([[1, -1], [-2, -1, 2]], [[2], [-2]])

        with self.assertRaises(TypeError):
            PBR([[1, -1, 3], [-2, -1, 2], [3, -2]], 
                [[2], [-2], [1, -1, 2]]) == \
            Bipartition([1, -1], [2, 3, -2], [-3])
            PBR([[1, -1], [-2, -1]], [[1], [-2, -1]]) < Transformation([0, 1])
            PBR([[1, -1], [-2, -1]], [[1], [-2, -1]]) != \
            PartialPerm([0, 1], [1, 0], 2)
            PBR([[1, -1]], [[1]]) < 3

if __name__ == '__main__':
    unittest.main()

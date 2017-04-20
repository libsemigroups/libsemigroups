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

if __name__ == '__main__':
    unittest.main()

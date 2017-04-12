import unittest
import sys
import os

path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
if not path in sys.path:
    sys.path.insert(1, path)
del path

from semigroups import Bipartition

class TestPartialPerm(unittest.TestCase):
    def test_init(self):
        Bipartition([-1, -2], [2, -3], [1, 3])
        Bipartition([-7, -6, -5, -4], [3, 2, 1], [-3, -2, -1, 4, 5, 6, 7])
        Bipartition([1, -1, 2, -2])

    def test_init_fail(self):
        with self.assertRaises(ValueError):
            Bipartition([1, -1, 2])
            Bipartition([1, 2, 3], [-3, -2])

        with self.assertRaises(TypeError):
            Bipartition([1, 2, 3], (-1, -2, -3))
            Bipartition(1, 2, -1, -2)

if __name__ == '__main__':
    unittest.main()

import unittest
import sys
import os

path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
if not path in sys.path:
    sys.path.insert(1, path)
del path

from semigroups import Transformation
import semigroups


class TestTransformation(unittest.TestCase):
    def test_init(self):
        Transformation([0, 1, 2, 3])
        Transformation([1,1,3,2,4,3])
        Transformation([9,3,1,2,0,8,1,2,0,5])







if __name__ == '__main__':
    unittest.main()

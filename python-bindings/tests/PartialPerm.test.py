import unittest
import sys
import os

path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
if not path in sys.path:
    sys.path.insert(1, path)
del path

from semigroups import PartialPerm


class TestPartialPerm(unittest.TestCase):
    def test_init(self):
        PartialPerm([1,0,2],[2,0,1],3)
        PartialPerm([1,0],[0,1],5)
        PartialPerm([0,3,4,5,8,20,23373],[1,2,34,23423,233,432,26],26260)


    def test_init_fail(self):
        with self.assertRaises(ValueError):
            PartialPerm([1,3],[0,1],3)
            PartialPerm([1,2],[3,2],3)
            PartialPerm([-2,2],[0,1],3)
            PartialPerm([1,2],[-1,2],3)
            PartialPerm([1,2],[2,2],3)
            PartialPerm([1,1],[0,2],3)

        with self.assertRaises(TypeError):
            PartialPerm([1,2],[0,'i'],3)
            PartialPerm([1,[0]],[1,2],3)

        with self.assertRaises(IndexError):
            PartialPerm([1,2],[0,1,2],3)


if __name__ == '__main__':
    unittest.main()

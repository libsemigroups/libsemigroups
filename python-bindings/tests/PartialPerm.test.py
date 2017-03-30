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


    def test_mul(self):
        self.assertEquals( PartialPerm([0, 1], [0, 1], 2)*PartialPerm([0, 1], [0, 1], 2), PartialPerm([0, 1], [0, 1], 2))
        self.assertEquals( PartialPerm([1, 2, 4, 6, 7, 23], [0, 5, 2, 4, 6, 7], 26)*PartialPerm([2, 4, 3, 5, 0, 19], [7, 8, 2, 3, 23, 0], 26), PartialPerm([1, 2, 4, 6], [23, 3, 7, 8], 26))
        self.assertEquals( PartialPerm([0, 3, 7, 2], [5, 7, 1, 3], 8)*PartialPerm([4, 7, 3, 6], [5, 0, 3, 2], 8), PartialPerm([2, 3], [3, 0], 8))





if __name__ == '__main__':
    unittest.main()

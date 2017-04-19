
import unittest
import sys
import os

path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
if not path in sys.path:
    sys.path.insert(1, path)
del path

import semigroups

class TestFpSemigroups(unittest.TestCase):

    def test_init(self):
        FpSemigroup(1, [])
        FpSemigroup(1, [[[0], [0,0]]])
        FpSemigroup(2, [[[1], [0,0]]])

    def order_test(self):
        self.assertEqual(FpSemigroup(1, [[[0], [0,0]]]), FpSemigroup(1, [[[0,0], [0]]]))


if __name__ == '__main__':
    unittest.main()

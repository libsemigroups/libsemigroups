import unittest
import sys
import os

path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
if not path in sys.path:
    sys.path.insert(1, path)
del path

from semigroups import Bipartition, Transformation, PartialPerm

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

    def test_mul(self):
        self.assertEqual(Bipartition([1, -1, 2, -2]) * Bipartition([1, -1, 2, -2]), Bipartition([1, 2, -1, -2]))
        self.assertEqual(Bipartition([1, 2], [-1], [-2]) * Bipartition([1, -1], [2, -2]), Bipartition([1, 2], [-1], [-2]))
        self.assertEqual(Bipartition([1, -1], [2, 3, -2], [-3]) * Bipartition([1, 3, 2, -3], [-2], [-1]), Bipartition([1, 2, 3, -3], [-1], [-2]))

        with self.assertRaises(TypeError):
            Bipartition([1, -1], [2, 3, -3], [-2]) * PartialPerm([0, 1], [1, 2], 3)
            Transformation([0, 2, 1]) * Bipartition([1, -1], [2, 3, -3], [-2])
            Bipartition([1, -1], [2, 3, -3], [-2]) * 26

        with self.assertRaises(ValueError):
            Bipartition([1, -1], [2, 3, -3], [-2]) * Bipartition([1, -1, 2, -2])

    def test_pow(self):
        self.assertEqual(Bipartition([-7, -6, -5, -4], [3, 2, 1], [-3, -2, -1, 4, 5, 6, 7]) ** 20, Bipartition([1, 2, 3], [4, 5, 6, 7], [-1, -2, -3], [-4, -5, -6, -7]))
        self.assertEqual(Bipartition([1, -1, 2, -2]) ** 26, Bipartition([1, 2, -1, -2]))
        self.assertEqual(Bipartition([-1, -2], [2, -3], [1, 3]) ** 3, Bipartition([1, 3], [2, -3], [-1, -2]))
 
        with self.assertRaises(ValueError):
            Bipartition([1, -1, 2, -2]) ** -26
            Bipartition([1], [-1, 2, -2]) ** 0

        with self.assertRaises(TypeError):
            Bipartition([1, 2], [-1], [-2]) ** 3.1415926535897932384626433832795028841971693993751058209
            Bipartition([1, 2], [-1], [-2]) ** 'c'

    def test_dealloc(self):
        A, B = Bipartition([1, -1, 2, -2]), Bipartition([-7, -6, -5, -4], [3, 2, 1], [-3, -2, -1, 4, 5, 6, 7])
        del A, B
        with self.assertRaises(NameError):
            A; B

    def test_blocks(self):
        self.assertEqual(Bipartition([1,2],[-2,-1]).blocks(), [[1, 2], [-2, -1]])
        self.assertEqual(Bipartition([-7, -6, -5, -4], [3, 2, 1], [-3, -2, -1, 4, 5, 6, 7]).blocks(), [[-7, -6, -5, -4], [3, 2, 1], [-3, -2, -1, 4, 5, 6, 7]])
        self.assertEqual(Bipartition([-1, -2], [2, -3], [1, 3]).blocks(), [[-1, -2], [2, -3], [1, 3]])

    def test_numberOfBlocks(self):
        self.assertEqual(Bipartition([1, 2], [-2, -1]).numberOfBlocks(), 2)
        self.assertEqual(Bipartition([-7, -6, -5, -4], [3, 2, 1], [-3, -2, -1, 4, 5, 6, 7]).numberOfBlocks(), 3)
        self.assertEqual(Bipartition([-1, -2], [2, -3], [1, 3]).numberOfBlocks(), 3)

    def test_degree(self):
        self.assertEqual(Bipartition([1, 2], [-2, -1]).degree(), 2)
        self.assertEqual(Bipartition([-7, -6, -5, -4], [3, 2, 1], [-3, -2, -1, 4, 5, 6, 7]).degree(), 7)
        self.assertEqual(Bipartition([-1, -2], [2, -3], [1, 3]).degree(), 3)

    def test_block(self):
        self.assertEqual(Bipartition([1, 2], [-2, -1]).block(-2), 1)
        self.assertEqual(Bipartition([-7, -6, -5, -4], [3, 2, 1], [-3, -2, -1, 4, 5, 6, 7]).block(3), 0)
        self.assertEqual(Bipartition([-1, -2], [2, -3], [1, 3]).block(-1), 2)

if __name__ == '__main__':
    unittest.main()

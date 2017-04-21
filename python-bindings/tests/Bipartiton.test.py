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
        with self.assertRaises(ValueError):
            Bipartition([1, 2, 3], [-3, -2])

        with self.assertRaises(TypeError):
            Bipartition([1, 2, 3], (-1, -2, -3))
        with self.assertRaises(TypeError):
            Bipartition(1, 2, -1, -2)

    def test_richcmp(self):
        assert Bipartition([3, -4, -1], [2, -3], [4, -2], [1]) == \
               Bipartition([4, -2], [3, -4, -1], [1], [2, -3])
        assert not Bipartition([3, -4, -1], [2, -3], [4, -2], [1]) != \
                   Bipartition([4, -2], [3, -4, -1], [1], [2, -3])
        assert Bipartition([1, -1, 3], [-3, 2, -2]) <= \
               Bipartition([1, -1], [2, 3, -2], [-3])
        assert Bipartition([1, -1], [2, -2]) < Bipartition([1, -2], [2, -1])
        assert Bipartition([1, -1], [2, -2]) <= Bipartition([1, -2], [2, -1])
        assert Bipartition([1, -1, 3], [-3, 2, -2]) >= \
               Bipartition([1, -2], [2, -1])
        assert not Bipartition([1, -1, 3], [-3, 2, -2]) > \
                   Bipartition([1, -1], [2, 3, -2], [-3])

        with self.assertRaises(TypeError):
            PartialPerm([1, 2], [2, 1], 3) == \
            Bipartition([1, -1], [2, 3, -2], [-3])
        with self.assertRaises(TypeError):
            Bipartition([1, -1], [2, -2]) < Transformation([0, 1])
        with self.assertRaises(TypeError):
            Bipartition([1, -1], [2, -2]) != Transformation([0, 1])

    def test_mul(self):
        self.assertEqual(Bipartition([1, -1, 2, -2]) * \
                         Bipartition([1, -1, 2, -2]), 
                         Bipartition([1, 2, -1, -2]))
        self.assertEqual(Bipartition([1, 2], [-1], [-2]) * \
                         Bipartition([1, -1], [2, -2]),
                         Bipartition([1, 2], [-1], [-2]))
        self.assertEqual(Bipartition([1, -1], [2, 3, -2], [-3]) * \
                         Bipartition([1, 3, 2, -3], [-2], [-1]),
                         Bipartition([1, 2, 3, -3], [-1], [-2]))

        with self.assertRaises(TypeError):
            Bipartition([1, -1], [2, 3, -3], [-2]) * \
            PartialPerm([0, 1], [1, 2], 3)
        with self.assertRaises(TypeError):
            Transformation([0, 2, 1]) * Bipartition([1, -1], [2, 3, -3], [-2])
        with self.assertRaises(TypeError):
            Bipartition([1, -1], [2, 3, -3], [-2]) * 26

        with self.assertRaises(ValueError):
            Bipartition([1, -1], [2, 3, -3], [-2]) * \
            Bipartition([1, -1, 2, -2])

    def test_pow(self):
        self.assertEqual(Bipartition([-7, -6, -5, -4], [3, 2, 1], 
                                     [-3, -2, -1, 4, 5, 6, 7]) ** 20, 
                         Bipartition([1, 2, 3], [4, 5, 6, 7], [-1, -2, -3],
                                     [-4, -5, -6, -7]))
        self.assertEqual(Bipartition([1, -1, 2, -2]) ** 26, 
                         Bipartition([1, 2, -1, -2]))
        self.assertEqual(Bipartition([-1, -2], [2, -3], [1, 3]) ** 3, 
                         Bipartition([1, 3], [2, -3], [-1, -2]))
 
        with self.assertRaises(ValueError):
            Bipartition([1, -1, 2, -2]) ** -26
        with self.assertRaises(ValueError):
            Bipartition([1], [-1, 2, -2]) ** 0

        with self.assertRaises(TypeError):
            Bipartition([1, 2], [-1], [-2]) ** 3.141592653589793238462643383279
        with self.assertRaises(TypeError):
            Bipartition([1, 2], [-1], [-2]) ** 'c'

    def test_dealloc(self):
        A = Bipartition([1, -1, 2, -2])
        B = Bipartition([-7, -6, -5, -4], [3, 2, 1], [-3, -2, -1, 4, 5, 6, 7])
        del A, B
        with self.assertRaises(NameError):
            A
        with self.assertRaises(NameError):
            B

    def test_blocks(self):
        self.assertEqual(Bipartition([1,2],[-2,-1]).blocks(), 
                         [[1, 2], [-2, -1]])
        self.assertEqual(Bipartition([-7, -6, -5, -4], [3, 2, 1], 
                                     [-3, -2, -1, 4, 5, 6, 7]).blocks(), 
                         [[-7, -6, -5, -4], [3, 2, 1], 
                          [-3, -2, -1, 4, 5, 6, 7]])
        self.assertEqual(Bipartition([-1, -2], [2, -3], [1, 3]).blocks(),
                         [[-1, -2], [2, -3], [1, 3]])

    def test_numberOfBlocks(self):
        self.assertEqual(Bipartition([1, 2], [-2, -1]).numberOfBlocks(), 2)
        self.assertEqual(Bipartition([-7, -6, -5, -4], [3, 2, 1], [-3, -2, 
                                      -1, 4, 5, 6, 7]).numberOfBlocks(), 3)
        self.assertEqual(Bipartition([-1, -2], [2, -3], 
                                     [1, 3]).numberOfBlocks(), 3)

    def test_degree(self):
        self.assertEqual(Bipartition([1, 2], [-2, -1]).degree(), 2)
        self.assertEqual(Bipartition([-7, -6, -5, -4], [3, 2, 1], 
                                     [-3, -2, -1, 4, 5, 6, 7]).degree(), 7)
        self.assertEqual(Bipartition([-1, -2], [2, -3], [1, 3]).degree(), 3)

    def test_block(self):
        self.assertEqual(Bipartition([1, 2], [-2, -1]).block(-2), 1)
        self.assertEqual(Bipartition([-7, -6, -5, -4], [3, 2, 1], 
                                     [-3, -2, -1, 4, 5, 6, 7]).block(3), 0)
        self.assertEqual(Bipartition([-1, -2], [2, -3], [1, 3]).block(-1), 2)

        with self.assertRaises(ValueError):
            Bipartition([1, 2], [-2, -1]).block(-3)
        with self.assertRaises(ValueError):
            Bipartition([-7, -6, -5, -4], [3, 2, 1], 
                        [-3, -2, -1, 4, 5, 6, 7]).block(26)
        with self.assertRaises(ValueError):
            Bipartition([-1, -2], [2, -3], [1, 3]).block('a')

    def test_isTransverseBlock(self):
        self.assertEqual(Bipartition([1, 2], 
                                     [-2, -1]).isTransverseBlock(1), False)
        self.assertEqual(Bipartition([-7, -6, -5, -4], [3, 2, 1], [-3, -2, -1,
                                      4, 5, 6, 7]).isTransverseBlock(1), True)
        self.assertEqual(Bipartition([-1, -2], [2, -3], 
                                     [1, 3]).isTransverseBlock(1), True)

        with self.assertRaises(IndexError):
            Bipartition([1, 2], [-2, -1]).isTransverseBlock(-3)
        with self.assertRaises(IndexError):
            Bipartition([-7, -6, -5, -4], [3, 2, 1], 
                        [-3, -2, -1, 4, 5, 6, 7]).isTransverseBlock(26)

        with self.assertRaises(TypeError):
            Bipartition([-1, -2], [2, -3], [1, 3]).isTransverseBlock('a')
        with self.assertRaises(TypeError):
            Bipartition([-1, -2], [2, -3], [1, 3]).isTransverseBlock([7, 26])

    def test_identity(self):
        self.assertEqual(Bipartition([1, 2], [-2, -1]).identity(), 
                         Bipartition([1, -1], [2, -2]))
        self.assertEqual(Bipartition([-7, -6, -5, -4], [3, 2, 1], 
                                     [-3, -2, -1, 4, 5, 6, 7]).identity(),
                         Bipartition([1, -1], [2, -2], [3, -3], [4, -4], 
                                     [5, -5], [6, -6], [7, -7]))
        self.assertEqual(Bipartition([-1, -2], [2, -3], [1, 3]).identity(), 
                         Bipartition([1, -1], [2, -2], [3, -3]))

    def test_repr(self):
        self.assertEqual(eval(Bipartition([1, 2], [-2, -1]).__repr__()),
                         Bipartition([1, 2], [-2, -1]))
        self.assertEqual(eval(Bipartition([-1, -2], [2, -3], 
                                          [1, 3]).__repr__()),
                         Bipartition([-1, -2], [2, -3], [1, 3]))
        self.assertEqual(eval(Bipartition([-7, -6, -5, -4], [3, 2, 1], 
                                     [-3, -2, -1, 4, 5, 6, 7]).__repr__()),
                         Bipartition([-7, -6, -5, -4], [3, 2, 1], 
                                     [-3, -2, -1, 4, 5, 6, 7]))

    def test_generator(self):
        self.assertEqual(list(Bipartition([1, 2], [-2, -1])._generator()),
                         list([0, 0, 1, 1]))
        self.assertEqual(list(Bipartition([-1, -2], [2, -3], 
                                          [1, 3])._generator()),
                         list([0, 1, 0, 2, 2, 1]))
        self.assertEqual(list(Bipartition([-7, -6, -5, -4], [3, 2, 1], 
                                     [-3, -2, -1, 4, 5, 6, 7])._generator()),
                         list([0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2]))

if __name__ == '__main__':
    unittest.main()

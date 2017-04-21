import unittest
import sys
import os

path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
if not path in sys.path:
    sys.path.insert(1, path)
del path

from semigroups import PartialPerm, Transformation, Bipartition

class TestPartialPerm(unittest.TestCase):
    def test_init(self):
        PartialPerm([1, 0, 2], [2, 0, 1], 3)
        PartialPerm([1, 0], [0, 1], 5)
        PartialPerm([0, 3, 4, 5, 8, 20, 23373], 
                    [1, 2, 34, 23423, 233, 432, 26], 26260)

    def test_init_fail(self):
        with self.assertRaises(ValueError):
            PartialPerm([1, 3], [0, 1], 3)
        with self.assertRaises(ValueError):
            PartialPerm([1, 2], [3, 2], 3)
        with self.assertRaises(ValueError):
            PartialPerm([-2, 2], [0, 1], 3)
        with self.assertRaises(ValueError):
            PartialPerm([1, 2], [-1, 2], 3)
        with self.assertRaises(ValueError):
            PartialPerm([1, 2], [2, 2], 3)
        with self.assertRaises(ValueError):
            PartialPerm([1, 1], [0, 2], 3)
        with self.assertRaises(ValueError):
            PartialPerm([], [], -1)
        with self.assertRaises(ValueError):
            PartialPerm([1, 2], [0, 1, 2], 3)

        with self.assertRaises(TypeError):
            PartialPerm([1, 2], [0, 'i'], 3)
        with self.assertRaises(TypeError):
            PartialPerm([1, [0]], [1, 2], 3)
        with self.assertRaises(TypeError):
            PartialPerm([0, 1], [2, 3], [4])
        with self.assertRaises(TypeError):
            PartialPerm([0, 1], [2, 3], 4.3)

    def test_richcmp(self):
        assert PartialPerm([1, 2, 3], [2, 1, 0], 5) == \
               PartialPerm([1, 2, 3], [2, 1, 0], 5)
        assert not PartialPerm([1, 2, 3], [2, 1, 0], 5) != \
                   PartialPerm([1, 2, 3], [2, 1, 0], 5)
        assert not PartialPerm([1, 2, 4], [2, 1, 0], 5) == \
                   PartialPerm([1, 2, 3], [2, 3, 0], 5)
        assert PartialPerm([1, 2, 4], [2, 1, 0], 5) != \
               PartialPerm([1, 2, 3], [2, 3, 0], 5)
        assert not PartialPerm([1, 2, 4], [2, 1, 0], 5) < \
                   PartialPerm([1, 2, 3], [2, 3, 0], 5)
        assert PartialPerm([1, 2], [0, 1], 3) < \
               PartialPerm([2, 0], [0, 1], 3)
        assert not PartialPerm([1, 2], [0, 1], 3) > \
                   PartialPerm([2, 0], [0, 1], 3)
        assert PartialPerm([1, 2], [1, 2], 3) > \
               PartialPerm([1, 2], [0, 1], 3)
        assert PartialPerm([1, 2], [1, 2], 3) >= \
               PartialPerm([1, 2], [0, 1], 3)
        assert PartialPerm([1, 2, 3], [2, 1, 0], 5) <= \
               PartialPerm([1, 2, 3], [2, 1, 0], 5)

        with self.assertRaises(TypeError):
            PartialPerm([1, 2], [2, 1], 3) == \
            Bipartition([1, -1], [2, 3, -2], [-3])
        with self.assertRaises(TypeError):
            PartialPerm([0, 1], [0, 1], 2) < Transformation([0, 1])
        with self.assertRaises(TypeError):
            PartialPerm([0, 1], [0, 1], 2) != Transformation([0, 1])

    def test_mul(self):
        self.assertEqual(PartialPerm([0, 1], [0, 1], 2) * \
                         PartialPerm([0, 1], [0, 1], 2), 
                         PartialPerm([0, 1], [0, 1], 2))
        self.assertEqual(PartialPerm([1, 2, 4, 6, 7, 23], 
                                     [0, 5, 2, 4, 6, 7], 26) * \
                         PartialPerm([2, 4, 3, 5, 0, 19], 
                                     [7, 8, 2, 3, 23, 0], 26), 
                         PartialPerm([1, 2, 4, 6], [23, 3, 7, 8], 26))
        self.assertEqual(PartialPerm([0, 3, 7, 2], [5, 7, 1, 3], 8) * \
                         PartialPerm([4, 7, 3, 6], [5, 0, 3, 2], 8), 
                         PartialPerm([2, 3], [3, 0], 8))

        with self.assertRaises(TypeError):
            PartialPerm([0, 1], [0, 1], 2) * Transformation([0, 1])
        with self.assertRaises(TypeError):
            PartialPerm([0, 1], [0, 1], 2) * Bipartition([-2, 1], [-1, 2])

        with self.assertRaises(ValueError):
            PartialPerm([1, 2], [0, 1], 3) * PartialPerm([1, 2], [0, 1], 4)

    def test_pow(self):
        self.assertEqual(PartialPerm([0, 1], [0, 1], 2) ** 20, 
                         PartialPerm([0, 1], [0, 1], 2))
        self.assertEqual(PartialPerm([1, 2, 4, 6, 7, 23], 
                                     [0, 5, 2, 4, 6, 7], 26) ** 5, 
                         PartialPerm([4, 6, 7, 23], [5, 2, 4, 6], 26))
        self.assertEqual(PartialPerm([0, 3, 7, 2], [5, 7, 1, 3], 8) ** 10, 
                         PartialPerm([], [], 8))

        with self.assertRaises(ValueError):
            PartialPerm([1,2],[0,1],3)**-1
        with self.assertRaises(ValueError):
            PartialPerm([1, 2, 4, 6, 7, 23], [0, 5, 2, 4, 6, 7], 26) ** 0

        with self.assertRaises(TypeError):
            PartialPerm([0, 1], [0, 1], 2) ** 1.5
        with self.assertRaises(TypeError):
            PartialPerm([1, 4, 2], [2, 3, 4], 6) ** 'a'

    def test_rank(self):
        self.assertEqual(PartialPerm([1, 4, 2], [2, 3, 4], 6).rank(), 3)
        self.assertEqual(PartialPerm([1, 2, 4, 6, 7, 23], 
                                     [0, 5, 2, 4, 6, 7], 26).rank(), 6)
        
        with self.assertRaises(TypeError):
            PartialPerm([1, 2], [0, 1], 3).rank(2)

    def test_domain(self):
        self.assertEqual(set(PartialPerm([1, 4, 2], [2, 3, 4], 6).domain()),
                         set([1, 4, 2]))
        self.assertEqual(set(PartialPerm([7, 26, 3, 5], 
                                         [23, 13, 19, 11], 29).domain()),
                         set([7, 26, 3, 5]))

        with self.assertRaises(TypeError):
            PartialPerm([1, 2], [0, 1], 3).domain('a')

    def test_range(self):
        self.assertEqual(set(PartialPerm([1, 4, 2], [2, 3, 4], 6).range()),
                         set([2, 3, 4]))
        self.assertEqual(set(PartialPerm([7, 26, 3, 5], 
                                         [23, 13, 19, 11], 29).range()),
                         set([23, 13, 19, 11]))

        with self.assertRaises(TypeError):
            PartialPerm([1, 2], [0, 1], 3).range([3])

    def test_degree(self):
        self.assertEqual(PartialPerm([1, 4, 2], [2, 3, 4], 6).degree(), 6)
        self.assertEqual(PartialPerm([7, 26, 3, 5], 
                                     [23, 13, 19, 11], 29).degree(), 29)

        with self.assertRaises(TypeError):
            PartialPerm([1, 2], [0, 1], 3).degree(8.5)

    def test_dealloc(self):
        t = PartialPerm([0, 1], [1, 0], 2)
        del t
        with self.assertRaises(NameError):
            t

    def test_identity(self):
        self.assertEqual(PartialPerm([0, 1], [1, 0], 2).identity(), 
                         PartialPerm([0, 1], [0, 1], 2))
        self.assertEqual(PartialPerm([1, 2, 4, 6, 7, 3], 
                                     [0, 5, 2, 4, 6, 7], 8).identity(), 
                         PartialPerm([0, 1, 2, 3, 4, 5, 6, 7], 
                                     [0, 1, 2, 3, 4, 5, 6, 7], 8))
        self.assertEqual(PartialPerm([0, 3, 4, 2], [2, 4, 1, 3], 5).identity(),
                         PartialPerm([0, 1, 2, 3, 4], [0, 1, 2, 3, 4], 5))

    def test_repr(self):
        self.assertEqual(eval(PartialPerm([0, 3, 4, 2], 
                                          [2, 4, 1, 3], 5).__repr__()),
                         PartialPerm([0, 3, 4, 2], [2, 4, 1, 3], 5))
        self.assertEqual(eval(PartialPerm([1, 4, 2], [2, 3, 4], 6).__repr__()),
                         PartialPerm([1, 4, 2], [2, 3, 4], 6))
        self.assertEqual(eval(PartialPerm([1, 2, 3], [2, 1, 0], 5).__repr__()),
                         PartialPerm([1, 2, 3], [2, 1, 0], 5))

if __name__ == '__main__':
    unittest.main()

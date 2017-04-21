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
        with self.assertRaises(TypeError):
            PBR(set([(1, -1)]), set([(1)]))
        with self.assertRaises(TypeError):
            PBR('a', 2)

        with self.assertRaises(ValueError):
            PBR([[1, -1], [2, -1]], [[1]])
        with self.assertRaises(ValueError):
            PBR([[1, -1], [-2, -1]], [[0], [-2, -1]])
        with self.assertRaises(ValueError):
            PBR([[1, -1], [-2, -1, 2]], [[3], [-2]])
        with self.assertRaises(ValueError):
            PBR([[1, -1, 3], [-2, -1, 2], [3, 3, -2]], [[2], [-2], [1, -1, 2]])

    def test_richcmp(self):
        assert PBR([[1, -1], [-2, -1, 2]], [[2], [-2]]) == \
               PBR([[1, -1], [-2, -1, 2]], [[2], [-2]])
        assert not PBR([[1, -1], [-2, -1, 2]], [[2], [-2]]) != \
                   PBR([[1, -1], [-2, -1, 2]], [[2], [-2]])
        assert PBR([[1, -1], [-2, -1, 2]], [[2], [-2]]) != \
               PBR([[1, 2], [-2, 1, 2], [3, -3]], 
                   [[2, 1], [-2, 2, 1], [1, -1]])
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
        with self.assertRaises(TypeError):
            PBR([[1, -1], [-2, -1]], [[1], [-2, -1]]) < Transformation([0, 1])
        with self.assertRaises(TypeError):
            PBR([[1, -1], [-2, -1]], [[1], [-2, -1]]) != \
            PartialPerm([0, 1], [1, 0], 2)
        with self.assertRaises(TypeError):
            PBR([[1, -1]], [[1]]) < 3

    def test_mul(self):
        self.assertEqual(PBR([[1, -1]], [[1]]) * PBR([[1, -1]], [[1]]),
                         PBR([[1, -1]], [[1]]))
        self.assertEqual(PBR([[1, -1], [-2, -1]], [[1], [-2, -1]]) * \
                         PBR([[-1, 1, 2], [2]], [[-1, 1], [-2, 2]]),
                         PBR([[1, -1, -2], [1, -1, -2]], [[1], [1, -1, -2]]))
        self.assertEqual(PBR([[-1, 1, 2], [2]], [[-1, 1], [-2, 2]]) * \
                         PBR([[1, -1], [-2, -1]], [[1], [-2, -1]]),
                         PBR([[1, 2, -1], [-1, -2]], [[1, 2], [2, -1, -2]]))
        self.assertEqual(PBR([[1, -1, 3], [-2, -1, 2], [3, -2]], 
                             [[2], [-2], [1, -1, 2]]) * \
                         PBR([[1, -1, 3], [-2, -1, 2], [3, -3, -2]], 
                             [[1, 2], [-2, 3, -3], [1, -1]]),
                         PBR([[1, 2, 3, -1, -2, -3], 
                              [1, 2, 3, -1, -2, -3], [3, -2, -3]], 
                              [[2], [3, -2, -3], [1, 2, 3, -2, -3]]))

        with self.assertRaises(TypeError):
            Transformation([0, 2, 1]) * \
            PBR([[1, -1, 3], [-2, -1, 2], [3, -2]], [[2], [-2], [1, -1, 2]])
        with self.assertRaises(TypeError):
            PBR([[1, -1, 3], [-2, -1, 2], [3, -2]], 
                [[2], [-2], [1, -1, 2]]) * \
            Bipartition([1, -1], [2, 3, -3], [-2])
        with self.assertRaises(TypeError):
            PBR([[1, -1]], [[1]]) * 0.142857

        with self.assertRaises(ValueError):
            PBR([[1, -1, 3], [-2, -1, 2], [3, -2]], 
                [[2], [-2], [1, -1, 2]]) * \
            PBR([[1, -1]], [[1]])

    def test_pow(self):
        self.assertEqual(PBR([[1, -1, 3], [-2, -1, 2], [3, -2]],
                            [[2], [-2], [1, -1, 2]]) ** 5,
                         PBR([[1, 2, 3, -1, -2], [2, -1, -2], [3, -2]],
                             [[2], [-2], [1, 2, -2]]))
        self.assertEqual(PBR([[1, -1]], [[1]]) ** 26, PBR([[1, -1]], [[1]]))
        self.assertEqual(PBR([[-1, 1, 2], [2]], [[-1, 1], [-2, 2]]) ** 4,
                         PBR([[1, 2, -1], [2]], [[1, 2, -1], [2, -2]]))
 
        with self.assertRaises(ValueError):
            PBR([[1, -1, 3], [-2, -1, 2], [3, -2]],
                            [[2], [-2], [1, -1, 2]]) ** -26
        with self.assertRaises(ValueError):
            PBR([[1, 2, 3, -1, -2, -3], 
                              [1, 2, 3, -1, -2, -3], [3, -2, -3]], 
                              [[2], [3, -2, -3], [1, 2, 3, -2, -3]]) ** 0

        with self.assertRaises(TypeError):
            PBR([[1, -1]], [[1]]) ** 0.21813
        with self.assertRaises(TypeError):
            PBR([[1, -1]], [[1]]) ** 'a'

    def test_dealloc(self):
        A = PBR([[1, -1]], [[1]]), 
        B = PBR([[1, -1, 3], [-2, -1, 2], [3, -2]], [[2], [-2], [1, -1, 2]])
        del A, B
        assert not 'A' in globals()
        assert not 'B' in globals()

    def test_degree(self):
        self.assertEqual(PBR([[1, -1]], [[1]]).degree(), 1)
        self.assertEqual(PBR([[1, 2, 3, -1, -2, -3], 
                              [1, 2, 3, -1, -2, -3], [3, -2, -3]], 
                              [[2], [3, -2, -3], [1, 2, 3, -2, -3]]).degree(), 3)
        self.assertEqual(PBR([[1, -1], [-2, -1, 2]], [[2], [-2]]).degree(), 2)

    def test_identity(self):
        self.assertEqual(PBR([[-1, 1, 2], [2]], [[-1, 1], [-2, 2]]).identity(),
                         PBR([[1], [2]], [[-1], [-2]]))

        self.assertEqual(PBR([[1, 2, 3, -1, -2, -3], 
                              [1, 2, 3, -1, -2, -3], [3, -2, -3]], 
                              [[2], [3, -2, -3], [1, 2, 3, -2, -3]]).identity(), 
                         PBR([[1], [2], [3]], [[-1], [-2], [-3]]))
        self.assertEqual(PBR([[1, -1]], [[1]]).identity(), PBR([[1]], [[-1]]))

    def test_repr(self):
        self.assertEqual(eval(PBR([[-1, 1, 2], [2]], [[-1, 1], [-2, 2]]).__repr__()),
                         PBR([[-1, 1, 2], [2]], [[-1, 1], [-2, 2]]))

        self.assertEqual(eval(PBR([[1, 2, 3, -1, -2, -3], 
                              [1, 2, 3, -1, -2, -3], [3, -2, -3]], 
                              [[2], [3, -2, -3], [1, 2, 3, -2, -3]]).__repr__()), 
                         PBR([[1, 2, 3, -1, -2, -3], 
                              [1, 2, 3, -1, -2, -3], [3, -2, -3]], 
                              [[2], [3, -2, -3], [1, 2, 3, -2, -3]]))
        self.assertEqual(eval(PBR([[1, -1]], [[1]]).__repr__()), PBR([[1, -1]], [[1]]))

if __name__ == '__main__':
    unittest.main()

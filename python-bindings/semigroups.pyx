#cython: infer_types=True

from libc.stdint cimport uint16_t
from libcpp.vector cimport vector

cdef extern from "semigroups.h" namespace "semigroupsplusplus":
    cdef cppclass Element:
        pass
    cdef cppclass Transformation[T](Element):
        Transformation(vector[T]) except +
    cdef cppclass Semigroup:
        Semigroup(vector[Element*]) except +
        int size()

def f():
    cdef vector[Element *] gens;
    gens.push_back(new Transformation[uint16_t]([0, 1, 0]))
    gens.push_back(new Transformation[uint16_t]([0, 1, 2]))

    S = new Semigroup(gens);
    return S.size()

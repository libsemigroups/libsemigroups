from libc.stdint cimport uint16_t
from libcpp.vector cimport vector

cdef extern from "semigroups.h" namespace "semigroupsplusplus":
    cdef cppclass Element:
        pass
    cdef cppclass Transformation[T](Element):
        Transformation(vector[T]) except +
        vector[T] _vector
    cdef cppclass Semigroup:
        Semigroup(vector[Element*]) except +
        int size()


# Declarations of C++ classes and methods we will call from Cython
# Could possibly be automatically generated

# We use the strategy proposed in
# https://stackoverflow.com/questions/10114710/how-to-keep-a-c-class-name-unmodified-with-cython
# to use the same names for the C++ classes and their Cython wrappers

from libc.stdint cimport uint16_t
from libcpp.vector cimport vector

cdef extern from "semigroups/semigroups.h" namespace "libsemigroups":
    cdef cppclass Element:
        Element* identity()
        void redefine(Element *, Element *)
    cdef cppclass Transformation[T](Element):
        Transformation(vector[T]) except +
        vector[T] _vector
        vector[T].iterator begin()
        vector[T].iterator end()
    cdef cppclass Semigroup:
        Semigroup(vector[Element*]) except +
        int size()

cdef extern from "semigroups_cpp.h" namespace "libsemigroups":
    cdef cppclass PythonElement(Element):
        object get_value()
        PythonElement(value) except +

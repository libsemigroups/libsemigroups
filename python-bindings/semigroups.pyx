#cython: infer_types=True

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

def f():
    cdef vector[Element *] gens
    gens.push_back(new Transformation[uint16_t]([0, 1, 0]))
    gens.push_back(new Transformation[uint16_t]([0, 1, 2]))

    S = new Semigroup(gens);
    return S.size()

cdef class Converter:
    def is_a(self, t):
        pass

    cdef Element* convert_from(self, t):
        pass

    cdef convert_to(self, Element* t):
        pass

cdef class ListConverter(Converter):
    def is_a(self, t):
        return isinstance(t, (list, tuple))

    cdef Element* convert_from(self, t):
        return new Transformation[uint16_t](t)

    cdef convert_to(self, Element * element):
        cdef Transformation[uint16_t]* t = <Transformation[uint16_t]*> element
        return [] # list((t[0]._vector)[0]) # TODO: Fixed that

converters = [ListConverter()]

cdef class PySemigroup:
    cdef Semigroup* c_semigroup      # holds a pointer to the C++ instance which we're wrapping
    cdef Converter converter         # object that contains pointers to conversion functions
    def __cinit__(self, generators):
        # Type checking on the generators to decide on whether to build Transformation's, PartialTransformation, ...
        # Add some plugins so that Sage can configure other converters
        cdef vector[Element *] gens
        cdef converter
        for conv in converters:
            converter = <Converter>(conv)
            if all(converter.is_a(g) for g in generators):
                self.converter=converter
                break
        for g in generators:
            gens.push_back(self.converter.convert_from(g))
        self.c_semigroup = new Semigroup(gens)

    def __dealloc__(self):
        del self.c_semigroup

    def size(self):
        """
        Return the size of this semigroup

        EXAMPLES::

            sage: from semigroups import PySemigroup
            sage: S = PySemigroup([[1,1,4,5,4,5],[2,3,2,3,5,5]])
            sage: S.size()
            5
        """
        return self.c_semigroup.size()

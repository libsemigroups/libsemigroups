#cython: infer_types=True

from libc.stdint cimport uint16_t
from libcpp.vector cimport vector
cimport semigroups_cpp as cpp

cdef class Element:
    cdef cpp.Element* cpp_element

    def __dealloc__(self):
        del self.cpp_element

cdef class Transformation(Element):
    def __cinit__(self, iterable):
        self.cpp_element = new cpp.Transformation[uint16_t](iterable)

    def __iter__(self):
        """
        Return an iterator over `self`

        EXAMPLES::

            >>> from semigroups import Transformation
            >>> list(Transformation([1,2,0]))
            [1, 2, 0]
        """
        cdef cpp.Element* e = self.cpp_element
        e2 = <cpp.Transformation[uint16_t] *>e
        for x in e2[0]:
            yield x

    def __repr__(self):
        """
        Return a string representation of `self`

        EXAMPLES::

            >>> from semigroups import Transformation
            >>> Transformation([1,2,0])
            [1, 2, 0]
        """
        return str(list(self))


cdef class Semigroup:
    cdef cpp.Semigroup* cpp_semigroup      # holds a pointer to the C++ instance which we're wrapping
    def __cinit__(self, generators):
        cdef vector[cpp.Element *] gens
        for g in generators:
            gens.push_back((<Element>g).cpp_element)
        self.cpp_semigroup = new cpp.Semigroup(gens)

    def __dealloc__(self):
        del self.cpp_semigroup

    def size(self):
        """
        Return the size of this semigroup

        EXAMPLES::

            >>> from semigroups import Semigroup, Transformation
            >>> S = Semigroup([Transformation([1,1,4,5,4,5]),Transformation([2,3,2,3,5,5])])
            >>> S.size()
            5
        """
        return self.cpp_semigroup.size()

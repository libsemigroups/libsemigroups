#cython: infer_types=True

from libc.stdint cimport uint16_t
from libcpp.vector cimport vector
cimport semigroups_cpp as cpp

cdef class Converter:
    def is_a(self, t):
        pass

    cdef cpp.Element* convert_from(self, t):
        pass

    cdef convert_to(self, cpp.Element* t):
        pass

cdef class ListConverter(Converter):
    def is_a(self, t):
        return isinstance(t, (list, tuple))

    cdef cpp.Element* convert_from(self, t):
        return new cpp.Transformation[uint16_t](t)

    cdef convert_to(self, cpp.Element * element):
        cdef cpp.Transformation[uint16_t]* t = <cpp.Transformation[uint16_t]*> element
        return [] # list((t[0]._vector)[0]) # TODO: Fixed that

converters = [ListConverter()]

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
    cdef Converter converter         # object that contains pointers to conversion functions
    def __cinit__(self, generators):
        # Type checking on the generators to decide on whether to build Transformation's, PartialTransformation, ...
        # Add some plugins so that Sage can configure other converters
        cdef vector[cpp.Element *] gens
        cdef converter
        for conv in converters:
            converter = <Converter>(conv)
            if all(converter.is_a(g) for g in generators):
                self.converter=converter
                break
        for g in generators:
            gens.push_back(self.converter.convert_from(g))
        self.cpp_semigroup = new cpp.Semigroup(gens)

    def __dealloc__(self):
        del self.cpp_semigroup

    def size(self):
        """
        Return the size of this semigroup

        EXAMPLES::

            sage: from semigroups import Semigroup
            sage: S = Semigroup([[1,1,4,5,4,5],[2,3,2,3,5,5]])
            sage: S.size()
            5
        """
        return self.cpp_semigroup.size()

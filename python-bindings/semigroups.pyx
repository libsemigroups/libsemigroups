#cython: infer_types=True

"""

>>> from semigroups import Semigroup
>>> Semigroup([0,1,-1])


"""
from libc.stdint cimport uint16_t
from libcpp.vector cimport vector
cimport semigroups_cpp as cpp

#cdef class MyCppElement(cpp.Element):
#    pass


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

cdef class PythonElement(Element):
    """
    A class for semigroup element wrapping Python elements

    For now, it's actually a wrapper for C int's ...

    EXAMPLE::

        >>> from semigroups import Semigroup, PythonElement
        >>> x = PythonElement(-1)
        <semigroups.PythonElement at ...>

    Testing deallocation:

        >>> x = 3

        >>> Semigroup([PythonElement(-1)]).size()
        2
        >>> Semigroup([PythonElement(1)]).size()
        1
        >>> Semigroup([PythonElement(0)]).size()
        1
        >>> Semigroup([PythonElement(0), PythonElement(-1)]).size()
        3

        x = [PythonElement(-1)]
        x = 2
    """
    def __cinit__(self):
        self.cpp_element = NULL

    def __dealloc__(self):
        """
        TESTS::

            >>> from semigroups import Semigroup, PythonElement
            >>> x = PythonElement(-1)
            <semigroups.PythonElement at ...>

        Testing deallocation:
            >>> x = 3
        """
        pass
        # TODO: why activating the del below causes a seg fault
        #del self.cpp_element

    def __init__(self, value):
        self.cpp_element = new cpp.PythonElement(value)

    def get_value(self):
        """

        """
        return (<cpp.PythonElement *>self.cpp_element).get_value()

    def __repr__(self):
        return repr(self.get_value())

    def __mul__(self, other):
        pass
        #product = cpp.PythonElement(None)
        #product.redefine(self. , other.)

cdef class Semigroup:
    cdef cpp.Semigroup* cpp_semigroup      # holds a pointer to the C++ instance which we're wrapping
    def __cinit__(self, generators):
        ## Jeroen: Move this to __init__
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
        # Plausibly wrap in sig_off / sig_on
        return self.cpp_semigroup.size()

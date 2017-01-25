#cython: infer_types=True, embedsignature=True
"""

    >>> from semigroups import Semigroup
    >>> S = Semigroup([0,-1])
    >>> S.size()
    3

"""
from libc.stdint cimport uint16_t
from libcpp.vector cimport vector
cimport semigroups_cpp as cpp

#cdef class MyCppElement(cpp.Element):
#    pass


cdef class Element:
    """
    An abstract base class for handles to libsemigroups elements.

    Any subclass shall implement an ``__init__`` method which
    initializes _handle.

    .. WARNING::

        For now, the ``__init__`` method should also accept to be
        called with ``None`` as argument, in which case it *should
        not* initialize the handle.

        This is used by ``new_from_handle``.

    .. TODO::

        Find a better protocol to create an instance from a class and
        a handle.
    """
    cdef cpp.Element* _handle

    def __cinit__(self):
        self._handle = NULL

    def __dealloc__(self):
        """
        Deallocate the handle of ``self``.

        TESTS::

            >>> from semigroups import Semigroup, PythonElement, Transformation
            >>> x = PythonElement(-1)
            >>> x = 3

            >>> x = Transformation([1,2,0])
            >>> del x
        """
        if self._handle != NULL:
            del self._handle

    def __mul__(Element self, Element other):
        """
        Return the product of ``self`` and ``other``.

        EXAMPLES::

            >>> from semigroups import Semigroup, PythonElement, Transformation
            >>> x = Transformation([2,1,1])
            >>> y = Transformation([2,1,0])
            >>> x * y
            [0, 1, 1]
            >>> y * x
            [1, 1, 2]
        """
        cdef cpp.Element* product = self._handle.identity()
        product.redefine(self._handle, other._handle)
        return self.new_from_handle(product)

    cdef new_from_handle(self, cpp.Element* handle):
        """
        Construct a new element from a specified handle and with the
        same class as ``self``.
        """
        cdef Element result = self.__class__(None)
        result._handle = handle
        return result

cdef class Transformation(Element):
    """
    A class for handles to libsemigroups transformations.

    EXAMPLES::

        >>> from semigroups import Semigroup, PythonElement, Transformation
        >>> Transformation([2,1,1])
        [2, 1, 1]
    """
    def __init__(self, iterable):
        if iterable is not None:
            self._handle = new cpp.Transformation[uint16_t](iterable)

    def __iter__(self):
        """
        Return an iterator over ``self``

        EXAMPLES::

            >>> from semigroups import Transformation
            >>> list(Transformation([1,2,0]))
            [1, 2, 0]
        """
        cdef cpp.Element* e = self._handle
        e2 = <cpp.Transformation[uint16_t] *>e
        for x in e2[0]:
            yield x

    def __repr__(self):
        """
        Return a string representation of `self`.

        EXAMPLES::

            >>> from semigroups import Transformation
            >>> Transformation([1,2,0])
            [1, 2, 0]
        """
        return str(list(self))

cdef class PythonElement(Element):
    """
    A class for handles to libsemigroups elements that themselves wrap
    back a Python element

    EXAMPLE::

        >>> from semigroups import Semigroup, PythonElement
        >>> x = PythonElement(-1); x
        -1

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

        sage: W = SymmetricGroup(4)
        sage: pi = W.simple_projections()
        sage: F = FiniteSetMaps(W)
        sage: S = Semigroup([PythonElement(F(p)) for p in pi])
        sage: S.size()
        Thread #0: Semigroup::enumerate: limit = 18446744073709551615
        Thread #0: Semigroup::enumerate: found 14 elements, 6 rules, max word length 3, so far
        Thread #0: Semigroup::enumerate: found 19 elements, 7 rules, max word length 4, so far
        Thread #0: Semigroup::enumerate: found 22 elements, 7 rules, max word length 5, so far
        Thread #0: Semigroup::enumerate: found 23 elements, 7 rules, max word length 6, so far
        Thread #0: Semigroup::enumerate: found 23 elements, 7 rules, max word length 6, finished!
        Thread #0: Semigroup::enumerate: elapsed time = 308μs
        23

    TESTS::

        Testing reference counting::

            >>> s = "UN NOUVEL OBJET"
            >>> sys.getrefcount(s)
            2
            >>> x = PythonElement(s)
            >>> sys.getrefcount(s)
            3
            >>> del x
            >>> sys.getrefcount(s)
            2
    """
    def __init__(self, value):
        if value is not None:
            self._handle = new cpp.PythonElement(value)

    def get_value(self):
        """

        """
        return (<cpp.PythonElement *>self._handle).get_value()

    def __repr__(self):
        return repr(self.get_value())

cdef class Semigroup:
    """
    A class for handles to libsemigroups semigroups

    EXAMPLES:

    We construct the symmetric group::

        >>> from semigroups import Semigroup, Transformation
        >>> S = Semigroup([Transformation([1,2,0]),Transformation([2,1,0])])
        >>> S.size()
        6
    """
    cdef cpp.Semigroup* _handle      # holds a pointer to the C++ instance which we're wrapping

    def __cinit__(self):
        self._handle = NULL

    def __init__(self, generators):
        """
        TESTS::

            >>> Semigroup([1, Transformation([1,0])])
            ...
            TypeError: all generators should have the same type
        """
        generators = [g if isinstance(g, Element) else PythonElement(g)
                      for g in generators]
        if not len({type(g) for g in generators}) <= 1:
            raise TypeError("all generators should have the same type")
        cdef vector[cpp.Element *] gens
        for g in generators:
            gens.push_back((<Element>g)._handle)
        self._handle = new cpp.Semigroup(gens)

    def __dealloc__(self):
        del self._handle

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
        return self._handle.size()

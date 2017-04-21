# Declarations of C++ classes and methods we will call from Cython
# Could possibly be automatically generated

# We use the strategy proposed in
# https://stackoverflow.com/questions/10114710/how-to-keep-a-c-class-name-unmodified-with-cython
# to use the same names for the C++ classes and their Cython wrappers

from libc.stdint cimport uint16_t
from libc.stdint cimport uint32_t
from libc.stdint cimport uint64_t
from libcpp.vector cimport vector
from libcpp.pair cimport pair
from libcpp cimport bool
from libcpp.string cimport string

cdef extern from "<libsemigroups/semigroups.h>" namespace "libsemigroups":
    cdef cppclass Element:
        Element* identity()
        void redefine(Element *, Element *)
        bool operator==(Element&)
        bool operator<(Element&)
        Element* really_copy()
        void really_delete()
        int degree()
    cdef cppclass Transformation[T](Element):
        Transformation(vector[T]) except +
        vector[T] _vector
        vector[T].iterator begin()
        vector[T].iterator end()
    cdef cppclass PartialPerm[T](Element):
        PartialPerm(vector[T]) except +
        vector[T] _vector
        vector[T].iterator begin()
        vector[T].iterator end()
    cdef cppclass Bipartition(Element):
        Bipartition(vector[uint32_t]) except +
        vector[uint32_t] _vector
    cdef cppclass Semigroup:
        # ctypedef pos_t # can't declare it here; this is private!
        Semigroup(vector[Element*]) except +
        int size()
        int nridempotents()
        Element* at(size_t pos)  # pos_t
        void set_report(bool val)
        int current_max_word_length()
        int current_position(Element* x)
        int position(Element* x)
        bool is_done()
        bool is_begun()
        bool test_membership(Element* x)
        vector[size_t]* factorisation(size_t pos)
        void enumerate(size_t limit)

cdef extern from "libsemigroups/cong.h" namespace "libsemigroups":
    cdef cppclass Congruence:
        Congruence(string, size_t, vector[pair[vector[uint64_t],vector[uint64_t]]],
                        vector[pair[vector[uint64_t],vector[uint64_t]]]) except +
        int nr_classes()
        int word_to_class_index(vector[uint64_t] word)
        void set_report(bool val)
        void set_max_threads(size_t nr_threads)
        

cdef extern from "libsemigroups/rws.h" namespace "libsemigroups":
    cdef cppclass RWS:
        RWS(vector[pair[vector[uint64_t],vector[uint64_t]]]) except +
        bool is_confluent()
      

cdef extern from "semigroups_cpp.h" namespace "libsemigroups":
    cdef cppclass PythonElement(Element):
        object get_value()
        PythonElement(value) except +

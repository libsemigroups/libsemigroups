.. Copyright (c) 2019, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

Congruences
===========

``libsemigroups`` implements serveral algorithms for computing a congruence of
a semigroup or monoid. The main algorithms implemented are Todd-Coxeter,
Knuth-Bendix, and brute-force enumeration; see the links below for further
details. As a convenience, ``libsemigroups`` also has a class
:cpp:type:`libsemigroups::Congruence` that runs some predetermined variants of
Todd-Coxeter, Knuth-Bendix, and the brute-force enumeration in parallel.  This
class is, at present, not very customisable, and lacks some of the fine grained
control offered by the classes implementing individual algorithms, such as
:cpp:type:`congruence::ToddCoxeter` and :cpp:type:`congruence::KnuthBendix`.

All of the classes for congruences in ``libsemigroups`` can be used
"interactively", in the sense that they can be run for a particular amount of
time, or until some condition is met; for further details see, for example,
:cpp:func:`libsemigroups::Runner::run_for` and
:cpp:func:`libsemigroups::Runner::run_until`.

The "handedness" of a congruence is determined by:

.. doxygenenum:: libsemigroups::congruence_type
   :project: libsemigroups

The classes in ``libsemigroups`` for congruences are:

.. toctree::
   :maxdepth: 1

   _generated/libsemigroups__congruenceinterface
   _generated/libsemigroups__congruence
   _generated/libsemigroups__congruencebypairs 
   _generated/libsemigroups__congruence__toddcoxeter 
   _generated/libsemigroups__congruence__knuthbendix
   _generated/libsemigroups__knuthbendixcongruencebypairs 

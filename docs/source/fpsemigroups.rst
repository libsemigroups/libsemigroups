.. Copyright (c) 2019, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

Finitely presented semigroups
=============================

``libsemigroups`` implements several algorithms for computing finitely
presented semigroups and monoids. The main algorithms implemented are
Todd-Coxeter, Knuth-Bendix, algorithms for C(4) small overlap monoids (by
Kambites and Tsalakou), and brute-force enumeration; see the links below for
further details. As a convenience, ``libsemigroups`` also has a class
:cpp:type:`libsemigroups::FpSemigroup` that runs some predetermined variants of
Todd-Coxeter, Knuth-Bendix, and the brute-force enumeration in parallel.  This
class is, at present, not very customisable, and lacks some of the fine grained
control offered by the classes implementing individual algorithms, such as
:cpp:type:`libsemigroups::fpsemigroup::ToddCoxeter` and
:cpp:type:`libsemigroups::fpsemigroup::KnuthBendix`.

All of the classes for finitely presented semigroups and monoids in
``libsemigroups`` can be used "interactively", in the sense that they can be
run for a particular amount of time, or until some condition is met; for
further details see, for example,
:cpp:func:`libsemigroups::Runner::run_for` and
:cpp:func:`libsemigroups::Runner::run_until`.

The classes in ``libsemigroups`` for finitely presented semigroups
and monoids are:

.. toctree::
   :maxdepth: 1

   _generated/libsemigroups__fpsemigroupinterface
   _generated/libsemigroups__congruencewrapper
   _generated/libsemigroups__fpsemigroup
   _generated/libsemigroups__fpsemigroupbypairs
   _generated/libsemigroups__fpsemigroup__kambites
   _generated/libsemigroups__fpsemigroup__knuthbendix
   _generated/libsemigroups__fpsemigroup__toddcoxeter
   present

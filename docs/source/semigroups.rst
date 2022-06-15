.. Copyright (c) 2019-2021, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

Semigroups
==========

.. cpp:namespace:: libsemigroups

``libsemigroups`` implements several algorithms for computing the structure of
a finite semigroup or monoid. The main implementations are of the Froidure-Pin,
Konieczny, and Schreier-Sims (for permutation groups) algorithms; see the links
below for further details. 

The implementations of these algorithms are generic, and can be easily adapted
for user-defined types.  In other words, it is possible to directly apply these
algorithms to any type that satisfy some minimal prerequisites, via traits
classes and/or the adapters described here.

Many of the classes for semigroups in ``libsemigroups`` can be used
"interactively", in the sense that they can be run for a particular amount of
time, or until some condition is met; for further details see, for example, 
:cpp:any:`void Runner::run_for(std::chrono::nanoseconds)` and
:cpp:func:`Runner::run_until`.

The classes for semigroups and monoids in ``libsemigroups`` are:

.. toctree::
   :maxdepth: 1

   _generated/libsemigroups__froidurepinbase
   _generated/libsemigroups__froidurepin
   _generated/libsemigroups__froidurepintraits
   konieczny
   _generated/libsemigroups__schreiersims
   _generated/libsemigroups__schreiersimstraits
   api/schreiersims-helper

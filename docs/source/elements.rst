.. Copyright (c) 2019, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

Elements
========

This page contains an overview summary of the functionality in
``libsemigroups`` for defining elements of semigroups. 

If ``libsemigroups`` is compiled with HPCombi_ support enabled (see
:ref:`Installation` for details), then the types defined in HPCombi_ (which use
the SSE and AVX instruction sets for very fast manipulation of these types) can
be used with the algorithms in ``libsemigroups``; for further details see the
HPCombi_ documentation.

.. toctree::
   :maxdepth: 1

   bipart
   ptransf
   transf
   perm
   pperm
   pbr
  
Adapters
~~~~~~~~

.. toctree::
   :maxdepth: 1

   hpcombi-adapters
   adapters

Helpers
~~~~~~~

The following helpers:

.. toctree::
   :maxdepth: 1

   _generated/libsemigroups__leastperm
   _generated/libsemigroups__leastpperm
   _generated/libsemigroups__leasttransf

can be used to get the smallest/fastest type representing a permutation,
partial permutation, or transformation, and can be used independently of
whether or not ``libsemigroups`` is compiled with HPCombi_ support enabled. 



.. _HPCombi: https://github.com/hivert/HPCombi

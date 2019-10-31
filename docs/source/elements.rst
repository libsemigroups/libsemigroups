.. Copyright (c) 2019, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

Elements
========

This page contains an overview summary of the functionality in
``libsemigroups`` for defining elements of semigroups. 

HPCombi
~~~~~~~

If ``libsemigroups`` is compiled with HPCombi_ support enabled (see
:ref:`Installation` for details), then the types defined in HPCombi_ (which use the
SSE and AVX instruction sets for very fast manipulation of these types) can be
used with the algorithms in ``libsemigroups``; for further details see
the HPCombi_ documentation.

The struct templates: 

- :cpp:any:`libsemigroups::BMatHelper`
- :cpp:any:`libsemigroups::PermHelper`
- :cpp:any:`libsemigroups::PPermHelper`
- :cpp:any:`libsemigroups::TransfHelper`

can be used to get the smallest/fastest type representing a boolean matrix,
permutation, partial permutation, or transformation, and can be used
independently of whether or not ``libsemigroups`` is compiled with HPCombi_
support enabled. 

.. _HPCombi: https://github.com/hivert/HPCombi

.. toctree::
   :maxdepth: 1

   hpcombi-adapters

Partial transformations
~~~~~~~~~~~~~~~~~~~~~~~

.. toctree::
   :maxdepth: 1

   _generated/libsemigroups__permhelper
   _generated/libsemigroups__ppermhelper
   _generated/libsemigroups__transfhelper
   api/pperm
   api/ptrans
   api/perm
   api/transf
   
Matrices
~~~~~~~~

.. toctree::
   :maxdepth: 1

   bmat
   api/projmaxplus
   api/mosr

Partitioned binary relations and bipartitions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. toctree::
   :maxdepth: 1

   api/pbr
   api/bipart
   api/blocks

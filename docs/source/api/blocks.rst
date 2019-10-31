.. Copyright (c) 2019, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

Blocks
======

Declared in ``blocks.hpp``.

This file contains the documentation for the Blocks class.  Blocks is a class
representing signed partitions of the set :math:`\{0, \ldots, n - 1\}`, for use
with Bipartition. 

Overview
--------

Constructors
~~~~~~~~~~~~

* :cpp:func:`libsemigroups::Blocks::Blocks`

Other member functions
~~~~~~~~~~~~~~~~~~~~~~

* :cpp:func:`libsemigroups::Blocks::block`
* :cpp:func:`libsemigroups::Blocks::degree`
* :cpp:func:`libsemigroups::Blocks::is_transverse_block`
* :cpp:func:`libsemigroups::Blocks::lookup`
* :cpp:func:`libsemigroups::Blocks::nr_blocks`
* :cpp:func:`libsemigroups::Blocks::rank`
* :cpp:func:`libsemigroups::Blocks::hash_value`
* :cpp:func:`libsemigroups::Blocks::cbegin`
* :cpp:func:`libsemigroups::Blocks::cend`
  
Full API
--------

.. doxygenclass:: libsemigroups::Blocks
   :project: libsemigroups
   :members:

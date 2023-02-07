.. Copyright (c) 2022, M. T. Whyte

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

   This file only exists because Breathe always displays all members when
   documenting a namespace, and this is nicer for now.

.. cpp:namespace:: libsemigroups

Word functions
--------

This page contains the documentation for functions which may be useful for manipulating words and strings.

Contents
~~~~~~~~

.. cpp:namespace:: libsemigroups::fpsemigroup

.. list-table::
   :widths: 50 50
   :header-rows: 0

   * - :cpp:any:`operator<<`
     - Concatenate two words or strings.

   * - :cpp:any:`pow`
     - Take a power of a word.

.. cpp:namespace-pop::

Full API
~~~~~~~~

.. doxygenfunction:: libsemigroups::operator<<
   :project: libsemigroups

.. doxygenfunction:: template<T> libsemigroups::pow
   :project: libsemigroups

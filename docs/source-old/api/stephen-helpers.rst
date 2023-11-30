.. Copyright (c) 2022, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

   This file only exists because Breathe always displays all members when
   documenting a namespace, and this is nicer for now.

.. cpp:namespace:: libsemigroups

Stephen helpers
---------------

Defined in ``stephen.hpp``.

This page contains the documentation for various helper functions for
manipulating :cpp:type:`Stephen` objects. All such functions are contained in
the namespace ``stephen``.

Contents
~~~~~~~~

.. cpp:namespace:: libsemigroups::stephen

.. list-table::
   :widths: 50 50
   :header-rows: 0

   * - :cpp:any:`const_iterator_words_accepted`
     - The return type of :cpp:any:`cbegin_words_accepted` and
       :cpp:any:`cend_words_accepted`.
   * - :cpp:any:`const_iterator_left_factors`
     - The return type of :cpp:any:`cbegin_left_factors` and
       :cpp:any:`cend_left_factors`.
   * - :cpp:any:`accepts`
     - Check if a word is equivalent to :cpp:any:`Stephen::word`.
   * - :cpp:any:`is_left_factor`
     - Check if a word is a left factor of :cpp:any:`Stephen::word`.
   * - :cpp:any:`cbegin_words_accepted`
     - Returns an iterator pointing at the first word equivalent to
       :cpp:any:`Stephen::word` in short-lex order.
   * - :cpp:any:`cend_words_accepted`
     - Returns an iterator pointing one past the last word equivalent to
       :cpp:any:`Stephen::word`.
   * - :cpp:any:`cbegin_left_factors`
     - Returns an iterator pointing at the first word (in short-lex order)
       that is a left factor of :cpp:any:`Stephen::word`.
   * - :cpp:any:`cend_left_factors`
     - Returns an iterator pointing one past the last word that is a left
       factor of :cpp:any:`Stephen::word`.
   * - :cpp:any:`number_of_words_accepted`
     - Returns the number of words accepted with length in given range.
   * - :cpp:any:`number_of_left_factors`
     - Returns the number of left factors with length in given range.

.. cpp:namespace-pop::

Full API
~~~~~~~~

.. doxygennamespace:: libsemigroups::stephen
   :project: libsemigroups

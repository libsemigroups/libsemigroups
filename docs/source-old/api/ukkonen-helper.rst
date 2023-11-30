.. Copyright (c) 2023, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

   This file only exists because Breathe always displays all members when
   documenting a namespace, and this is nicer for now.

.. cpp:namespace:: libsemigroups

Ukkonen helper functions
------------------------

Defined in ``ukkonen.hpp``.

This page contains the documentation for various helper functions for
manipulating :cpp:type:`Ukkonen` objects. All such functions are contained in
the namespace ``ukkonen``.

Contents
~~~~~~~~

.. cpp:namespace:: libsemigroups::ukkonen

.. list-table::
   :widths: 50 50
   :header-rows: 0

   * - :cpp:any:`add_words_no_checks`
     - Add all words in a ``std::vector`` to a :cpp:any:`Ukkonen` object.
   * - :cpp:any:`add_words`
     - Add all words in a range to a :cpp:any:`Ukkonen` object.
   * - :cpp:any:`dfs`
     - Perform a depth first search in a suffix tree.
   * - :cpp:any:`dot`
     - Returns a string containing a `GraphViz <https://graphviz.org>`_
       representation of a suffix tree.
   * - :cpp:any:`is_piece_no_checks`
     - Check if a word is a piece.
   * - :cpp:any:`is_piece`
     - Check if a word is a piece.
   * - :cpp:any:`is_subword_no_checks`
     - Check if a word is a subword of any word in a suffix tree.
   * - :cpp:any:`is_subword`
     - Check if a word is a subword of any word in a suffix tree.
   * - :cpp:any:`is_suffix_no_checks`
     - Check if a word is a suffix of any word in a suffix tree.
   * - :cpp:any:`is_suffix`
     - Check if a word is a suffix of any word in a suffix tree.
   * - :cpp:any:`length_maximal_piece_prefix_no_checks`
     - Find the length of the maximal piece prefix of a word.
   * - :cpp:any:`length_maximal_piece_prefix`
     - Find the length of the maximal piece prefix of a word.
   * - :cpp:any:`length_maximal_piece_suffix_no_checks`
     - Find the length of the maximal piece suffix of a word.
   * - :cpp:any:`length_maximal_piece_suffix`
     - Find the length of the maximal piece suffix of a word.
   * - :cpp:any:`maximal_piece_prefix_no_checks`
     - Find the maximal piece prefix of a word.
   * - :cpp:any:`maximal_piece_prefix`
     - Find the maximal piece prefix of a word.
   * - :cpp:any:`maximal_piece_suffix_no_checks`
     - Find the maximal piece suffix of a word.
   * - :cpp:any:`maximal_piece_suffix`
     - Find the maximal piece suffix of a word.
   * - :cpp:any:`number_of_distinct_subwords`
     - Returns the number of distinct subwords of the words in a suffix tree.
   * - :cpp:any:`number_of_pieces_no_checks`
     - Find the number of pieces in a decomposition of a word (if any).
   * - :cpp:any:`number_of_pieces`
     - Find the number of pieces in a decomposition of a word (if any).
   * - :cpp:any:`pieces_no_checks`
     - Find the pieces in a decomposition of a word (if any).
   * - :cpp:any:`pieces`
     - Find the pieces in a decomposition of a word (if any).
   * - :cpp:any:`traverse`
     - Traverse the suffix tree from the root.

.. cpp:namespace-pop::

Full API
~~~~~~~~

.. doxygennamespace:: libsemigroups::ukkonen
   :project: libsemigroups
   :content-only:

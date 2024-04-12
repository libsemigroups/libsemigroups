.. Copyright (c) 2022, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

   This file only exists because Breathe always displays all members when
   documenting a namespace, and this is nicer for now.

.. cpp:namespace:: libsemigroups

Presentation helpers
--------------------

Defined in ``present.hpp`` and ``knuth-bendix.hpp``.

This page contains the documentation for various helper functions for
manipulating :cpp:type:`Presentation` objects. All such functions are contained in
the namespace ``presentation``.

These functions are available in the ``present.hpp`` file except
:cpp:any:`redundant_rule` which is in ``knuth-bendix.hpp``.

Contents
~~~~~~~~

.. cpp:namespace:: libsemigroups::presentation

.. list-table::
   :widths: 50 50
   :header-rows: 0

   * - :cpp:any:`word_type operator+(word_type const& u, word_type const& w)`
     - Concatenate two words or strings.

   * - :cpp:any:`void operator+=(word_type& u, word_type const& w)`
     - Concatenate a word/string with another word/string in-place.

   * - :cpp:any:`template \<typename T, typename = std::enable_if_t\<IsWord\<T>::value>> T pow(T const&, size_t)`
     - Take a power of a word or string.

   * - :cpp:any:`template \<typename T, typename = std::enable_if_t\<IsWord\<T>::value>> void pow_inplace(T&, size_t)`
     - Take a power of a word or string.

   * - :cpp:any:`template \<typename T, typename = std::enable_if_t\<IsWord\<T>::value>> T prod(T const&, size_t, size_t, int)`
     - Take a product from a collection of letters.

   * - :cpp:any:`add_rule`
     - Add a rule to the presentation.

   * - :cpp:any:`add_rule_and_check`
     - Add a rule to the presentation and check.

   * - :cpp:any:`add_identity_rules`
     - Add rules for an identity element.

   * - :cpp:any:`add_inverse_rules`
     - Add rules for inverses.

   * - :cpp:any:`add_zero_rules`
     - Add rules for a zero element.

   * - :cpp:any:`are_rules_sorted`
     - Check if the rules :math:`u_1 = v_1, \ldots, u_n = v_n` satisfy
       :math:`u_1v_1 < \cdots < u_nv_n` where :math:`<` is the shortlex order.

   * - :cpp:any:`change_alphabet`
     - Change or re-order the alphabet.

   * - :cpp:any:`character`
     - Return a ``char`` by index (ordered for readability).

   * - :cpp:any:`first_unused_letter`
     - Returns the first letter **not** in the alphabet of a presentation.

   * - :cpp:any:`greedy_reduce_length`
     - Greedily reduce the length of the presentation using
       :cpp:any:`longest_common_subword`.

   * - :cpp:any:`is_strongly_compressible`
     - Returns ``true`` if the 1-relation presentation can be strongly
       compressed.

   * - :cpp:any:`length`
     - Return the sum of the lengths of the rules.

   * - :cpp:any:`letter`
     - Return a possible letter by index.

   * - :cpp:any:`longest_common_subword`
     - Returns the longest common subword of the rules.

   * - :cpp:any:`longest_rule`
     - Returns an iterator pointing at the left hand side of the first rule of
       maximal length.

   * - :cpp:any:`longest_rule_length`
     - Returns the maximum length of a rule.

   * - :cpp:any:`make_semigroup`
     - Convert a monoid presentation to a semigroup presentation.

   * - :cpp:any:`normalize_alphabet`
     - Modify the presentation so that the alphabet is :math:`\{0, \ldots, n -
       1\}` (or equivalent) and rewrites the rules.

   * - :cpp:any:`reduce_complements`
     - If there are rules :math:`u = v` and :math:`v = w` where
       :math:`|w| < |v| `, then replace :math:`u = v` by :math:`u = w`.

   * - :cpp:any:`reduce_to_2_generators`
     - Reduce the number of generators in a \f$1\f$-relation presentation to `2`.

   * - :cpp:any:`redundant_rule`
     - Return an iterator pointing at the left hand side of a redundant rule.

   * - :cpp:any:`remove_duplicate_rules`
     - Remove duplicate rules.

   * - :cpp:any:`remove_redundant_generators`
     - Remove any trivially redundant generators.

   * - :cpp:any:`remove_trivial_rules`
     - Remove rules consisting of identical words.

   * - :cpp:any:`replace_subword`
     - Replace non-overlapping instances of a subword.

   * - :cpp:any:`replace_word`
     - Replace instances of a word occupying either side of a rule.

   * - :cpp:any:`reverse`
     - Reverse every word in every rule.

   * - :cpp:any:`shortest_rule`
     - Returns an iterator pointing at the left hand side of the first rule of
       minimal length.

   * - :cpp:any:`shortest_rule_length`
     - Returns the minimum length of a rule.

   * - :cpp:any:`sort_each_rule`
     - Sort each rule :math:`u = v` so that the left hand side is shortlex greater
       than the right hand side.

   * - :cpp:any:`sort_rules`
     - Sort the rules :math:`u_1 = v_1, \ldots, u_n = v_n` so that
       :math:`u_1v_1 < \cdots < u_nv_n`.

   * - :cpp:any:`strongly_compress`
     - Strongly compress a 1-relation presentation.

.. cpp:namespace-pop::

Full API
~~~~~~~~

.. doxygennamespace:: libsemigroups::presentation
   :project: libsemigroups

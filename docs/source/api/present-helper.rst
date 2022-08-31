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

   * - :cpp:any:`redundant_rule`
     - Return an iterator pointing at the left hand side of a redundant rule.

   * - :cpp:any:`add_rule`
     - Add a rule to the presentation.

   * - :cpp:any:`add_rule_and_check`
     - Add a rule to the presentation and check.

   * - :cpp:any:`add_identity_rules`
     - Add rules for an identity element.

   * - :cpp:any:`add_inverse_rules`
     - Add rules for inverses.

   * - :cpp:any:`remove_duplicate_rules`
     - Remove duplicate rules.

   * - :cpp:any:`remove_trivial_rules`
     - Remove rules consisting of identical words.

   * - :cpp:any:`reduce_complements`
     - If there are rules :math:`u = v` and :math:`v = w` where :math:`|w| <
       |v|`, then replace :math:`u = v` by :math:`u = w`.

   * - :cpp:any:`sort_each_rule`
     - Sort each rule :math:`u = v` so that the left hand side is shortlex greater
       than the right hand side.

   * - :cpp:any:`sort_rules`
     - Sort the rules :math:`u_1 = v_1, \ldots, u_n = v_n` so that
       :math:`u_1v_1 < \cdots < u_nv_n`.

   * - :cpp:any:`longest_common_subword`
     - Returns the longest common subword of the rules.

   * - :cpp:any:`replace_subword`
     - Replace non-overlapping instances of a subword.

   * - :cpp:any:`length`
     - Return the sum of the lengths of the rules.

   * - :cpp:any:`reverse`
     - Reverse every word in every rule.

   * - :cpp:any:`normalize_alphabet`
     - Modify the presentation so that the alphabet is :math:`\{0, \ldots, n -
       1\}` (or equivalent) and rewrites the rules to use this alphabet.

   * - :cpp:any:`longest_rule`
     - Returns an iterator pointing at the left hand side of the first rule of
       maximal length.

   * - :cpp:any:`shortest_rule`
     - Returns an iterator pointing at the left hand side of the first rule of
       minimal length.

   * - :cpp:any:`longest_rule_length`
     - Returns the maximum length of a rule.

   * - :cpp:any:`shortest_rule_length`
     - Returns the minimum length of a rule.

   * - :cpp:any:`remove_redundant_generators`
     - Remove any trivially redundant generators.

.. cpp:namespace-pop::

Full API
~~~~~~~~

.. doxygennamespace:: libsemigroups::presentation
   :project: libsemigroups

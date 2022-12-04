.. Copyright (c) 2022, M. T. Whyte

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

   This file only exists because Breathe always displays all members when
   documenting a namespace, and this is nicer for now.

.. cpp:namespace:: libsemigroups

Examples
--------

This page contains the documentation for various examples of presentations of
finitely presented semigroups and monoids.

Presentations from the following sources are implemented: :cite:`Gay1999aa`;
:cite:`Godelle2009aa`; :cite:`East2022aa`; :cite:`Maltcev2007aa`;
:cite:`Arthur2000aa`; :cite:`Kudryavtseva2006aa`; :cite:`Burnside2012aa`;
:cite:`Ganyushkin2009aa`; :cite:`Cassaigne2001aa`; :cite:`Campbell1994aa`;
:cite:`Abram2022aa`; :cite:`Easdown2007aa`; :cite:`FitzGerald2003aa`;
:cite:`East2011aa`; :cite:`Ayik2000aa`; :cite:`Ruskuc1995aa`;
:cite:`Aizenstat1958aa`; :cite:`Coxeter1979aa`; :cite:`Knuth1970aa`;
:cite:`Lascoux1981aa`; :cite:`Moore1897aa`; :cite:`Aizenstat1962aa`;
:cite:`Fernandes2022aa`.

.. cpp:type:: libsemigroups::fpsemigroup::author

   The values in this enum class are used to specify the authors of a
   presentation. Where there are different presentations by different authors,
   values of this type can be passed as an argument to disambiguate which
   presentation is wanted.

.. doxygenfunction:: libsemigroups::fpsemigroup::operator+(author, author)
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::operator<<(std::ostringstream&, author)
   :project: libsemigroups

Contents
~~~~~~~~

.. cpp:namespace:: libsemigroups::fpsemigroup

.. list-table::
   :widths: 50 50
   :header-rows: 0

   * - :cpp:any:`symmetric_group`
     - A presentation for the symmetric group.

   * - :cpp:any:`alternating_group`
     - A presentation for the alternating group.

   * - :cpp:any:`full_transformation_monoid`
     - A presentation for the full transformation monoid.

   * - :cpp:any:`partial_transformation_monoid`
     - A presentation for the partial transformation monoid.

   * - :cpp:any:`symmetric_inverse_monoid`
     - A presentation for the symmetric inverse monoid.

   * - :cpp:any:`dual_symmetric_inverse_monoid`
     - A presentation for the dual symmetric inverse monoid.

   * - :cpp:any:`uniform_block_bijection_monoid`
     - A presentation for the uniform block bijection monoid.

   * - :cpp:any:`partition_monoid`
     - A presentation for the partition monoid.

   * - :cpp:any:`brauer_monoid`
     - A presentation for the Brauer monoid.

   * - :cpp:any:`rectangular_band`
     - A presentation for a rectangular band.

   * - :cpp:any:`stellar_monoid`
     - A presentation for the stellar monoid.

   * - :cpp:any:`chinese_monoid`
     - A presentation for the Chinese monoid.

   * - :cpp:any:`monogenic_semigroup`
     - A presentation for a monogenic semigroup.

   * - :cpp:any:`plactic_monoid`
     - A presentation for the plactic monoid.

   * - :cpp:any:`stylic_monoid`
     - A presentation for the stylic monoid.

   * - :cpp:any:`fibonacci_semigroup`
     - A presentation for a Fibonacci semigroup.

   * - :cpp:any:`temperley_lieb_monoid`
     - A presentation for the Temperley-Lieb monoid.

   * - :cpp:any:`singular_brauer_monoid`
     - A presentation for the singular part of the Brauer monoid.

   * - :cpp:any:`orientation_preserving_monoid`
     - A presentation for the monoid of orientation preserving
       mappings.

   * - :cpp:any:`orientation_reversing_monoid`
     - A presentation for the monoid of orientation reversing
       mappings.

   * - :cpp:any:`order_preserving_monoid`
     - A presentation for the monoid of order preserving
       mappings.

   * - :cpp:any:`cyclic_inverse_monoid`
     - A presentation for the cyclic inverse monoid.

   * - :cpp:any:`order_preserving_cyclic_inverse_monoid`
     - A presentation for the order-preserving part of the cyclic inverse monoid.

   * - :cpp:any:`not_symmetric_group`
     - A non-presentation for the symmetric group.
.. cpp:namespace-pop::

Full API
~~~~~~~~

.. doxygenfunction:: libsemigroups::fpsemigroup::symmetric_group
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::alternating_group
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::full_transformation_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::partial_transformation_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::symmetric_inverse_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::dual_symmetric_inverse_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::uniform_block_bijection_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::partition_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::brauer_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::rectangular_band
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::stellar_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::chinese_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::monogenic_semigroup
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::plactic_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::stylic_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::fibonacci_semigroup
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::temperley_lieb_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::singular_brauer_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::orientation_preserving_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::orientation_reversing_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::order_preserving_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::cyclic_inverse_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::order_preserving_cyclic_inverse_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fpsemigroup::not_symmetric_group
   :project: libsemigroups

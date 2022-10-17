.. Copyright (c) 2022, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

   This file only exists because Breathe always displays all members when
   documenting a namespace, and this is nicer for now.

.. cpp:namespace:: libsemigroups

Examples
--------------------

Defined in ``present.hpp`` and ``knuth-bendix.hpp``.

This page contains the documentation for various helper functions for
manipulating :cpp:type:`Presentation` objects. All such functions are contained in
the namespace ``presentation``.

These functions are available in the ``present.hpp`` file except
:cpp:any:`redundant_rule` which is in ``knuth-bendix.hpp``.

Presentations from the following sources are implemented: :cite:`Gay1999aa`; :cite:`Godelle2009aa`; :cite:`East2022aa`; :cite:`Maltcev2007aa`;
:cite:`Arthur2000aa`; :cite:`Kudryavtseva2006aa`; :cite:`Burnside2012aa`; :cite:`Ganyushkin2009aa`; :cite:`Cassaigne2001aa`; :cite:`Campbell1994aa`;
:cite:`Abram2022aa`; :cite:`Easdown2007aa`; :cite:`FitzGerald2003aa`; :cite:`East2011aa`; :cite:`Ayik2000aa`.

Contents
~~~~~~~~

.. cpp:namespace:: libsemigroups

.. list-table::
   :widths: 50 50
   :header-rows: 0

   * - :cpp:any:`symmetric_group`
     - Construct a presentation for the symmetric group.

   * - :cpp:any:`full_transformation_monoid`
     - Construct a presentation for the full transformation monoid.

   * - :cpp:any:`partial_transformation_monoid`
     - Construct a presentation for the partial transformation monoid.

   * - :cpp:any:`symmetric_inverse_monoid`
     - Construct a presentation for the symmetric inverse monoid.

   * - :cpp:any:`dual_symmetric_inverse_monoid`
     - Construct a presentation for the dual symmetric inverse monoid.

   * - :cpp:any:`uniform_block_bijection_monoid`
     - Construct a presentation for the uniform block bijection monoid.

   * - :cpp:any:`partition_monoid`
     - Construct a presentation for the partition monoid.

   * - :cpp:any:`brauer_monoid`
     - Construct a presentation for the Brauer monoid.

   * - :cpp:any:`rectangular_band`
     - Construct a presentation for a rectangular band.

   * - :cpp:any:`stellar_monoid`
     - Construct a presentation for the stellar monoid.

   * - :cpp:any:`chinese_monoid`
     - Construct a presentation for the Chinese monoid.

   * - :cpp:any:`monogenic_semigroup`
     - Construct a presentation for a monogenic semigroup.

   * - :cpp:any:`plactic_monoid`
     - Construct a presentation for the plactic monoid.

   * - :cpp:any:`stylic_monoid`
     - Construct a presentation for the stylic monoid.

   * - :cpp:any:`fibonacci_semigroup`
     - Construct a presentation for a Fibonacci semigroup.

   * - :cpp:any:`temperley_lieb_monoid`
     - Construct a presentation for the Temperley-Lieb monoid.

   * - :cpp:any:`singular_brauer_monoid`
     - Construct a presentation for the singular part of the Brauer monoid.

   * - :cpp:any:`orientation_preserving_monoid`
     - Construct a presentation for the monoid of orientation preserving mappings.

   * - :cpp:any:`orientation_reversing_monoid`
     - Construct a presentation for the monoid of orientation reversing mappings.
.. cpp:namespace-pop::

Full API
~~~~~~~~

.. doxygenfunction:: libsemigroups::symmetric_group
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::full_transformation_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::partial_transformation_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::symmetric_inverse_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::dual_symmetric_inverse_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::uniform_block_bijection_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::partition_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::brauer_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::rectangular_band
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::stellar_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::chinese_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::monogenic_semigroup
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::plactic_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::stylic_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::fibonacci_semigroup
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::temperley_lieb_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::singular_brauer_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::orientation_preserving_monoid
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::orientation_reversing_monoid
   :project: libsemigroups

Changelog - version 2
=====================

v2.3.2 (released 28/10/2022)
----------------------------

In this release a number of bugs and other minor issues were resolved.

* Fix inaccurate exception information for ``replace_subword`` by @MTWhyte in
  https://github.com/libsemigroups/libsemigroups/pull/394
* Fix broken link in `CONTRIBUTING.rst` by @MTWhyte in
  https://github.com/libsemigroups/libsemigroups/pull/396
* matrix: add case for some Apple clang weirdness by @james-d-mitchell in
  https://github.com/libsemigroups/libsemigroups/pull/401 This might be a bug
  in clang versions 13 and 14, which caused the ``identity`` static member
  function of the ``StaticMatrix`` class template to not properly initialise the
  matrix being constructed.
* string-view: fix bug in append method by @james-d-mitchell in
  https://github.com/libsemigroups/libsemigroups/pull/399
  This bug sometimes caused the ``normal_form`` member function of the
  ``Kambites`` class template to return incorrect results, or sometimes to
  cause a crash.

v2.3.1 (released 10/10/2022)
----------------------------

In this release a number of corner cases in the ``Sims1`` class are
fixed: 
* sometimes ``number_of_congruences`` erroneously returned ``0`` for the number
of congruences with 1 class;
* if an empty presentation (no generators or relations) was used to
initialise ``short_rules``, then the ``Sims1::for_each`` and
``Sims1::find_if`` could crash;
* a more serious bug which could cause an invalid read beyond the end of a
container (caused by the erroneous assumption that the containers were always
of even length). Thanks to `J. James` for reporting this issue.

v2.3.0 (released 28/09/2022)
----------------------------

In this release a number of minor issues a fixed, some improvements are made,
and the `Sims1` class has been redesigned and improved to implement a parallel
version of the low index congruences algorithm.

v2.2.3 (released 22/09/2022)
----------------------------

This is a minor release fixing a number of minor issues, and including some
minor improvements.

v2.2.2 (released 12/09/2022)
----------------------------

This is a minor release fixing a number of minor issues, and including some
minor improvements.

v2.2.1 (released 12/08/2022)
----------------------------

This is a minor release fixing a data race (probably) caused by setting the
global locale, that sometimes caused a crash.

v2.2.0 (released 01/07/2022)
----------------------------

This release adds two new features to libsemigroups:

* Sims Low-Index Congruence Algorithm by @james-d-mitchell in
  https://github.com/libsemigroups/libsemigroups/pull/349
* Add permutation group intersections for SchreierSims by @reiniscirpons in
  https://github.com/libsemigroups/libsemigroups/pull/351

v2.1.5 (released 21/04/2022)
----------------------------

This is a minor release with a few minor changes:

* schreier-sims: fix issue #343 by @james-d-mitchell in https://github.com/libsemigroups/libsemigroups/pull/344
* Replace -march=native by -march=avx by @fingolfin in https://github.com/libsemigroups/libsemigroups/pull/345

@fingolfin made their first contribution in https://github.com/libsemigroups/libsemigroups/pull/345 welcome @fingolfin!

v2.1.4 (released 04/03/2022)
----------------------------

This is a minor release with a few minor changes:

* it is now possible to create the ``quotient_froidure_pin`` of a congruence
  with infinitely many classes;
* some exception messages were corrected for ``Bipartition`` objects;
* compilation of v2.1.0 to v2.1.3 did not work in cygwin for Windows;
* there was a file missing in the archive, which meant that it wasn't possible
  to build the documentation.

v2.1.3 (released 16/12/2021)
----------------------------

This is a minor release with some performance improvements in:
``ActionDigraph::number_of_paths`` (`eigen`_ is used in some circumstances when
available); the suffix tree implementation (used by ``Kambites``); and in
``KnuthBendix``.

v2.1.2 (released 30/11/2021)
----------------------------

This is another very minor release resolving an issue in the last release
on 32-bit systems raised by Jan Engelhardt.

v2.1.1 (released 28/11/2021)
----------------------------

This is a very minor release resolving an issue in the last release pointed out
by Jan Engelhardt.

v2.1.0 (released 28/11/2021)
----------------------------

The following new features, improvements, and fixes are included in this
release:

* new implementations of the linear time algorithm by Kambites for equality
  checking and the algorithm by Mitchell and Tsalakou
  for finding normal forms in small overlap monoids in the class ``Kambites``
  (@james-d-mitchell and @mariatsalakou
  https://github.com/libsemigroups/libsemigroups/pull/278)
* a performance improvement in the implementation of Konieczny algorithm, for
  determining the structure of a finite semigroup or monoid (@flsmith
  https://github.com/libsemigroups/libsemigroups/pull/297
  https://github.com/libsemigroups/libsemigroups/pull/304)
* an implement of the linear time algorithm by Radoszewski and Rytter for
  checking equality of words in free bands in the function
  ``freeband_equal_to`` (@reiniscirpons
  https://github.com/libsemigroups/libsemigroups/pull/298)
* a major revision, expansion of the settings, and improvement in the
  performance of the implementation of the Todd-Coxeter algorithm in the
  class ``ToddCoxeter`` (@james-d-mitchell
  https://github.com/libsemigroups/libsemigroups/pull/313
  https://github.com/libsemigroups/libsemigroups/pull/318)
* some other minor issues in the class templates
  ``FroidurePin`` and ``ActionDigraph`` were resolved
  (@james-d-mitchell https://github.com/libsemigroups/libsemigroups/pull/299
  https://github.com/libsemigroups/libsemigroups/pull/300)

**Full Changelog**: https://github.com/libsemigroups/libsemigroups/compare/v2.0.3...v2.1.0

v2.0.3 (released 11/11/2021)
----------------------------

This release contains a minor change to the ``FroidurePin`` class template that
resolves an issue with the `python binding <https://github.com/libsemigroups/libsemigroups_pybind11>`_ highlighted by `Maria Tsalakou`_ and Chinmaya Nagpal.

v2.0.2 (released 20/09/2021)
----------------------------

This release includes some missing operators, adapters, and doc were added.

v2.0.1 (released 28/08/2021)
----------------------------

This release includes some improvements and minor fixes including:

- some remnants of C++11 in the code and docs were removed
- the class ``Runner`` was given proper copy and move constructors
- some performance improvements were made to ``Konieczny`` by `Finn Smith`_
- some minor updates were made to ``FroidurePin`` and its documentation
- some missing adapters for ``KBE`` and ``TCE`` were implemented.

v2.0.0 (released 26/05/2021)
----------------------------

This release includes a major rewrite of the functionality in ``libsemigroups``
for matrices, transformations, and other elements of semigroups.
``libsemigroups`` has been updated to use C++14 (from C++11), and some
simplifications were made as a consequence.  The documentation has been
improved, some deprecated functionality was remove, and other minor
improvements, and simplifications were made. The included version of `eigen`_
was updated from version 3.3.7 to 3.3.9.

.. _eigen: http://eigen.tuxfamily.org/

`J. James`_ contributed some improvements to the build system, and some
additional updates were made which arose from warnings issued by newer versions
of autotools.

The following function was added:

- ``Action::cache_scc_multipliers``

The following deprecated functionality was removed:

- ``FroidurePin::FroidurePin(std::vector<element_type> const*)``
- ``FroidurePin::reset_next_relation``
- ``FroidurePin::next_relation``
- the ``Element`` class was removed
- the ``UFOld`` class was removed (replaced by ``Suf`` and ``Duf`` in v1.3.4)

The following backwards incompatible changes have been made:

- every function including ``_nr_`` has been renamed using ``_number_of_`` for
  consistency
- ``const`` has been removed from all function parameters passed by value
- ``congruence_type`` was renamed ``congruence_kind`` for consistency
- ``CongruenceByPairsHelper`` was renamed ``CongruenceByPairs``
- ``Congruence::policy`` was renamed ``Congruence::options``
- ``FroidurePin::copy_closure`` returns by value instead of returning a pointer
- ``FroidurePin::copy_add_generators`` returns by value instead of returning a
  pointer
- ``FroidurePin::letter_to_pos`` was renamed ``FroidurePin::current_position``
- ``FroidurePin::length_const`` was renamed ``FroidurePin::length``
- ``FroidurePin::length_non_const`` was renamed ``FroidurePin::current_length``
  (again for consistency)
- ``FroidurePin::word_to_pos`` was renamed ``FroidurePin::current_position``
- ``KnuthBendix::policy`` was renamed ``KnuthBendix::options``
- ``ToddCoxeter::policy`` was renamed ``ToddCoxeter::options``

The following files were renamed:

- ``libsemigroups-config.hpp`` was renamed ``config.hpp``
- ``libsemigroups-debug.hpp`` was renamed ``debug.hpp``
- ``libsemigroups-exception.hpp`` was renamed ``exception.hpp``

Changelog - version 1
=====================

.. toctree::

   changelog-v1

.. _J. James: http://www.jamezone.org/
.. _Isuru Fernando: https://github.com/isuruf
.. _Bernhard M. Wiedemann: https://lizards.opensuse.org/author/bmwiedemann/
.. _Semigroups: https://github.com/semigroups/Semigroups/
.. _Finn Smith: https://flsmith.github.io
.. _Maria Tsalakou: https://mariatsalakou.github.io/
.. _Reinis Cirpons: https://reinisc.id.lv
.. _Finn Smith: https://flsmith.github.io

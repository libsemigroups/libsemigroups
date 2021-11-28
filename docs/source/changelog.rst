Changelog - version 2
=====================

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
.. _libsemigroups_cppyy: https://github.com/libsemigroups/libsemigroups_cppyy
.. _Bernhard M. Wiedemann: https://lizards.opensuse.org/author/bmwiedemann/
.. _Semigroups: https://github.com/semigroups/Semigroups/
.. _Finn Smith: https://flsmith.github.io
.. _Maria Tsalakou: https://mariatsalakou.github.io/
.. _Reinis Cirpons:
.. _Finn Smith: https://flsmith.github.io

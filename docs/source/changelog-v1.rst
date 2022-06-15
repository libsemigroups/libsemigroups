Changelog - version 1
=====================

v1.3.7 (released 28/02/2021)
----------------------------

This is a minor release for with a bug in ``Konieczny`` fixed by
`Finn Smith`_, and a limit added to ``CongruenceByPairs`` to try to resolve 
https://github.com/semigroups/Semigroups/issues/634 (again).

v1.3.6 (released 04/02/2021)
----------------------------

This is a minor release for compatibility with the GAP package `Semigroups`_.

v1.3.5 (released 28/01/2021)
----------------------------

This is a minor release fixing a bug in the ``ToddCoxeter`` class (the
``contains`` member function could return false negatives in some cases after
``shrink_to_fit`` was called).

v1.3.4 (released 27/11/2020)
----------------------------

This is a minor release resolving the following issues:

* the tests failed when the hardware concurrency was limited to 1 (reported by
  `Bernhard M. Wiedemann`_)
* compilation failed when in debug mode
* there were some bugs in the string formatting in exceptions on 32-bit
  systems
* the disjoint sets data structures were refactored completely, and their
  performance improved. 

v1.3.3 (released 07/10/2020)
----------------------------

This is a minor release resolving an issue with `make install` including a copy
of `eigen3` when it should not have. Reported by Antonio Rojas.

v1.3.2 (released 02/10/2020)
----------------------------

This is a minor release resolving some issues in the build system, and some
issues related to PBRs.

The issues resolved are:

* eigen support is enabled if the configure flag `--with-external-eigen` is
  given; Resolved by `J. James`_.
* the order the compile flags related to HPCombi were appended was changed, so
  that they do not override user-provided `CFLAGS`. Reported by Jan Engelhardt
  and fixed by `J. D. Mitchell`_
* the macro ``LIBSEMIGROUPS_FMT_ENABLED`` was incorrectly used in ``report.cpp``
  and some weirdness ensued when ``libsemigroups`` was system installed (via
  ``make install``). Resolved by `J. D. Mitchell`_.
* the configuration options ``--disable-fmt`` and ``--enable-fmt`` did the same
  thing. Resolved by `J. D. Mitchell`_.
* it was possible to create invalid ``PBR`` objects, which caused some
  incorrect values to be returned by ``FroidurePin<PBR>`` instances. The
  ``to_string`` implementation for ``PBR``'s also didn't work properly in all
  cases.  Resolved by `Finn Smith`_.

v1.3.1 (released 28/08/2020)
----------------------------

This is a tediously minor release to fix some issues in the last release.

v1.3.0 (released 28/08/2020)
----------------------------

In this release some new features have been added and some issues have been
resolved. The new features in this version of ``libsemigroups`` were implemented
by `J. D. Mitchell`_, `Reinis Cirpons`_, and `Finn Smith`_.

.. _J. D. Mitchell: https://jdbm.me
.. _Reinis Cirpons: https://reinisc.id.lv
.. _Finn Smith: https://flsmith.github.io

The major new feature in this release is:

* an implementation of the Konieczny-Lallement-McFadden Algorithm
  :cite:`Konieczny1994aa`, :cite:`Lallement1990aa` for computing
  finite semigroups (`Finn Smith`_ and `J. D. Mitchell`_).

The major improvements in this release are:

* an improvement to the `is_obviously_infinite` function for congruences, and
  finitely presented semigroups and monoids. When ``libsemigroups`` is compiled
  with ``eigen`` enabled (as it is by default), then an integer matrix can be
  associated to a finite presentation, if this matrix does not have full rank,
  then the associated finitely presented semigroup or monoid is
  infinite. ``eigen`` is used to perform the computation of the rank of this
  matrix if available (`Reinis Cirpons`_ and `J. D. Mitchell`_).

* the time it takes to compile ``libsemigroups`` has be reduced. 

v1.2.1 (released 28/06/2020)
----------------------------

This is an exceedingly minor release removing some extraneous files from, and
adding one essential file to, the distro.

v1.2.0 (released 28/06/2020)
----------------------------

In this release some new features have been added and some bugs have been
fixed. The new features in this version of ``libsemigroups`` were implemented
by `J. D. Mitchell`_ and `M. Tsalakou`_.

.. _J. D. Mitchell: https://jdbm.me
.. _M. Tsalakou: https://mariatsalakou.github.io/

The major new features in this release are:

* an implementation of Gilman's Algorithm :cite:`Gilman1979` to construct an
  automaton that allows for counting, and iterating through, the normal forms
  of strings in a ``KnuthBendix`` instance. This automaton is accessible via
  the member function ``gilman_digraph``.  Using this approach significantly
  improves the performance of the ``size`` member function of ``KnuthBendix``
  and allows a ``KnuthBendix`` instance to know whether or not it is infinite. 

* improvements to the algorithm used by the ``number_of_paths`` member function
  of the class template ``ActionDigraph``, and the ability to specify the
  algorithm to be used.

* the class template ``ActionDigraph`` gets new member functions: ``number_of_edges``
  for a node; ``number_of_paths`` with a single node as argument; 
  ``number_of_paths_algorithm`` which returns a value in an enum describing the
  algorithm used by ``number_of_paths`` by default.

* the functions ``topological_sort`` and ``add_cycle`` in the namespace
  ``libsemigroups::action_digraph_helper``

* the function ``number_of_words`` for counting the number of words with length
  in a given range over an alphabet of specified size.

* the class template ``ActionDigraph`` gets new static member functions
  ``random`` for outputing a random action digraph with a given number of
  edges; and ``random_acyclic``.

There were also several further minor improvements and bug fixes implemented in
this version, many of which arose while developing `libsemigroups_cppyy`_.
Thanks to Murray Whyte for pointing out several of these bugs. 

One major bug was also resolved: sometimes a ``KnuthBendix`` instance refusing
to run even though the rules it contained were not reduced (but were
confluent). 

v1.1.0 (released 28/05/2020)
----------------------------

This is a minor release which adds some new features and fixes some bugs. 

The more major new features in this release are:

* iterators for words and strings with respect to lexicographic and short-lex
  orders (``cbegin_wilo``, ``cbegin_wislo``, ``cbegin_silo``, ``cbegin_sislo``)
* the class template ``FroidurePin`` now has member functions for iterating
  through its rules (``cbegin_rules``)
* the namespace ``action_digraph_helper`` is introduced. This namespace
  contains the helper functions ``follow_path``, ``is_acyclic``, 
  ``is_reachable``, ``validate_node``, and ``validate_label`` for the class
  template ``ActionDigraph``.  
* the class template ``ActionDigraph`` has new member functions for iterating
  through nodes in reverse (``crbegin_nodes``); for iterating through paths
  (``cbegin_panilo``, ``cbegin_panislo``, ``cbegin_pilo``, ``cbegin_pislo``,
  ``cbegin_pstilo``, and ``cbegin_pstislo``); and for counting the number of
  paths starting at a source (and optionally ending at a target) node
  (``number_of_paths``).

The more major bugs fixed are:

* the class ``FpSemigroup`` member functions ``number_of_rules``, ``run_until``, and
  ``run_for`` now behave as expected
* a fix so that ``libsemigroups`` can be used with ``fmt`` version 6.2.0 was
  made.
* a bug was resolved in ``FroidurePin::word_to_pos`` which resulted in
  incorrect results for instances with duplicate generators.

There were also several further minor improvements and bug fixes implemented in
this version, many of which arose while developing `libsemigroups_cppyy`_.
Thanks to Murray Whyte for pointing out several of these bugs. 

v1.0.9 (released 20/04/2020)
----------------------------

This is an exceedingly minor release removing some extraneous files from the
distro. 

v1.0.8 (released 20/04/2020)
----------------------------

This is another minor release fixing some minor issues, again mostly
highlighted from `libsemigroups_cppyy`_

v1.0.7 (released 17/03/2020)
----------------------------

This is a minor release fixing some minor issues, mostly highlighted from
`libsemigroups_cppyy`_

v1.0.6 (released 09/02/2020)
----------------------------

This is a minor release fixing some bugs.

v1.0.5 (released 13/01/2020)
----------------------------

This is a minor release with some minor changes to the documentation and build
system. The non-trivial changes in this release were made by `Isuru Fernando`_

v1.0.4 (released 11/01/2020)
----------------------------

This is a minor release with several improvements to the build system. It is
now possible to build ``libsemigroups`` using an external installation of ``fmt`` 
with the configure option ``--with-external-fmt``, and it should also be
possible to build ``libsemigroups`` on non-x86 architectures.

v1.0.3 (released 07/12/2019)
----------------------------

This is a minor release that includes some fixes that caused tests to fail on
32-bit systems. The non-trivial changes in this release were made by 
`J.  James`_

v1.0.2 (released 30/11/2019)
----------------------------

This is a very minor release to fix a bad test that failed in the conda package
continuous integration.

v1.0.1 (released 29/11/2019)
----------------------------

This is a minor release to resolve some issues with the conda package, and to
remove some compiler warnings.

v1.0.0 (released 27/11/2019)
----------------------------

This is a major release that dramatically expands the scope of
``libsemigroups``.  In v1.0.0, ``libsemigroups`` has been almost completely
rewritten, generalised, and some new features have been added. 

.. _J. James: http://www.jamezone.org/
.. _Isuru Fernando: https://github.com/isuruf
.. _libsemigroups_cppyy: https://github.com/libsemigroups/libsemigroups_cppyy
.. _Bernhard M. Wiedemann: https://lizards.opensuse.org/author/bmwiedemann/
.. _Semigroups: https://github.com/semigroups/Semigroups/

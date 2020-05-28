Changelog
=========

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

* the class ``FpSemigroup`` member functions ``nr_rules``, ``run_until``, and
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

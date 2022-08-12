.. Copyright (c) 2019-2022, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

libsemigroups - Version 2.2.1
=============================

.. image:: https://readthedocs.org/projects/libsemigroups/badge/?version=master
    :target: https://libsemigroups.readthedocs.io/en/devel/?badge=master
    :alt: Documentation Status

.. image:: https://mybinder.org/badge_logo.svg
    :target: https://mybinder.org/v2/gh/libsemigroups/libsemigroups/master
    :alt: Launch Binder

.. image:: https://codecov.io/gh/libsemigroups/libsemigroups/branch/master/graph/badge.svg
  :target: https://codecov.io/gh/libsemigroups/libsemigroups

.. image:: https://img.shields.io/conda/dn/conda-forge/libsemigroups
  :target: https://github.com/conda-forge/libsemigroups-feedstock

.. image:: https://anaconda.org/conda-forge/libsemigroups/badges/installer/conda.svg
  :target: https://conda.anaconda.org/conda-forge

.. image:: https://zenodo.org/badge/DOI/10.5281/zenodo.1437752.svg
  :target: https://doi.org/10.5281/zenodo.1437752

.. image:: https://anaconda.org/conda-forge/libsemigroups/badges/license.svg
  :target: https://anaconda.org/conda-forge/libsemigroups

.. image:: https://anaconda.org/conda-forge/libsemigroups/badges/platforms.svg
  :target: https://anaconda.org/conda-forge/libsemigroups

C++ library for semigroups and monoids
--------------------------------------

What is ``libsemigroups``?
~~~~~~~~~~~~~~~~~~~~~~~~~~

``libsemigroups``  is a C++14 library containing implementations of several
algorithms for computing finite, and finitely presented, semigroups and
monoids. Namely:

- the `Froidure-Pin algorithm`_ for computing finite semigroups;
- the `Todd-Coxeter algorithm`_ for finitely presented semigroups and monoids;
  see also `this paper <https://arxiv.org/abs/2203.11148>`__;
- the `Knuth-Bendix algorithm`_ for finitely presented semigroups and monoids;
- the `Schreier-Sims algorithm`_ for permutation groups;
- a preliminary implementation of the `Konieczny`_ and
  `Lallement-McFadden`_ algorithm for computing finite
  semigroups which act on sets;
- an implementation of the `Radoszewski-Rytter`_
  algorithm for testing equivalence of words in free bands;
- an implementation of the algorithm for solving the word problem
  for small overlap monoids, and for computing normal forms in such monoids;
  see `Kambites <https://doi.org/10.1016/j.jalgebra.2008.09.038>`__,
  `Kambites <https://doi.org/10.1016/j.jalgebra.2008.12.028>`__, and
  `Mitchell-Tsalakou
  <http://arxiv.org/abs/2105.12125>`__
- a version of Sims low index subgroup algorithm for computing one-sided
  congruences of a semigroup or monoid.

.. _Froidure-Pin algorithm: https://www.irif.fr/~jep/PDF/Rio.pdf
.. _Todd-Coxeter algorithm: https://en.wikipedia.org/wiki/Todd%E2%80%93Coxeter_algorithm
.. _Knuth-Bendix algorithm: https://en.wikipedia.org/wiki/Knuth%E2%80%93Bendix_completion_algorithm
.. _Schreier-Sims algorithm: https://en.wikipedia.org/wiki/Schreier%E2%80%93Sims_algorithm
.. _Konieczny: https://link.springer.com/article/10.1007/BF02573672
.. _Lallement-McFadden: https://www.sciencedirect.com/science/article/pii/S0747717108800570
.. _Radoszewski-Rytter: https://link.springer.com/chapter/10.1007/978-3-642-11266-9_55

``libsemigroups`` is partly based on `Algorithms for computing finite
semigroups`_, `Expository Slides`_, and `Semigroupe 2.01`_ by `Jean-Eric Pin`_.

.. _Algorithms for computing finite semigroups: https://www.irif.fr/~jep/PDF/Rio.pdf
.. _Expository slides: https://www.irif.fr/~jep/PDF/Exposes/StAndrews.pdf
.. _Semigroupe 2.01: https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html
.. _Jean-Eric Pin: https://www.irif.fr/~jep/

``libsemigroups`` is used in the `Semigroups package for GAP`_,  and it is
possible to use ``libsemigroups`` directly in Python 3 via the package
`libsemigroups_pybind11`_. The development version of ``libsemigroups`` is
available on github_, and some related projects are here_.

.. _github: https://github.com/libsemigroups/libsemigroups
.. _here: https://github.com/libsemigroups
.. _libsemigroups_pybind11: https://libsemigroups.github.io/libsemigroups_pybind11/
.. _Semigroups package for GAP: https://semigroups.github.io/Semigroups

The main classes in ``libsemigroups`` are named after the algorithms they
implement; see, for example,  ``libsemigroups::FroidurePin``,
``libsemigroups::Konieczny``,
``libsemigroups::congruence::ToddCoxeter``,
``libsemigroups::fpsemigroup::Kambites``,
``libsemigroups::fpsemigroup::KnuthBendix``, and
``libsemigroups::SchreierSims``.

The implementations in ``libsemigroups::FroidurePin``,
``libsemigroups::Konieczny``, and ``libsemigroups::SchreierSims``
are generic and easily adapted to user-defined types.

``libsemigroups`` uses: `HPCombi`_ which uses the SSE and AVX instruction sets
for very fast manipulation of transformations, partial permutations,
permutations, and boolean matrices of small size;  `catch`_ for tests;
`fmt`_ for reporting; and `eigen`_ for some linear algebra computations.

.. _HPCombi: https://github.com/hivert/HPCombi
.. _catch: https://github.com/catchorg/Catch2
.. _fmt: https://github.com/fmtlib/fmt
.. _eigen: http://eigen.tuxfamily.org/

How to use it
~~~~~~~~~~~~~

See the documentation https://libsemigroups.readthedocs.io/en/latest/

Installation instructions are here https://libsemigroups.readthedocs.io/en/latest/install.html

Issues
~~~~~~

If you find any problems with ``libsemigroups``, or have any suggestions for
features that you'd like to see, please use the `issue tracker`_.

.. _issue tracker: https://github.com/libsemigroups/libsemigroups/issues

Author
~~~~~~~

`J. D. Mitchell`_

.. _J. D. Mitchell: https://jdbm.me

Contributors
~~~~~~~~~~~~

- `R. Cirpons`_ contributed to ``IsObviouslyInfinite``, to integrating
  ``eigen``, contributed an implementation of the `Radoszewski-Rytter`_
  algorithm for testing equivalence of words in free bands; and 
  the intersection of permutation groups in ``SchreierSims``. 
- `F. Hivert`_ contributed many helpful ideas to ``libsemigroups``, an
  allocator implementation (to be included in a future version), and
  ``HPCombi``.
- `J. James`_ contributed some bugfixes.
- `J. Jonušas`_ contributed to the implementation of the Froidure-Pin
  algorithm.
- `D. V. Pasechnik`_ contributed to the build system.
- `F. Smith`_ contributed the implementation of the Konieczny and
  Lallement-McFadden algorithm, to the Todd-Coxeter implementation, and to
  BMat8s.
- `N. Thiery`_ contributed to the build system, packaging ``libsemigroups`` via
  conda, the python bindings and many helpful conversations and suggestions.
- `M. Torpey`_ contributed to the congruences code in the v0.0.1 to v0.6.7.
- `M. Tsalakou`_ contributed to the Knuth-Bendix implementation, related
  algorithms for the class `ActionDigraph`, to the implementation
  of the `Kambites` class, and to the `Sims1` class.

.. _R. Cirpons: https://reinisc.id.lv
.. _F. Hivert: https://www.lri.fr/~hivert/
.. _J. James: http://www.jamezone.org/
.. _J. Jonušas: http://julius.jonusas.work/
.. _D. V. Pasechnik:  http://users.ox.ac.uk/~coml0531
.. _F. Smith: https://flsmith.github.io
.. _N. Thiery: http://nicolas.thiery.name/
.. _M. Torpey: https://mtorpey.github.io/
.. _M. Tsalakou: https://mariatsalakou.github.io/

Acknowledgements
~~~~~~~~~~~~~~~~

We acknowledge financial support from the OpenDreamKit_ Horizon 2020
European Research Infrastructures project (#676541) (primarily for the
python bindings).

We thank the `Carnegie Trust for the Universities of Scotland`_ for funding
the PhD scholarship of `J. Jonušas`_ when he worked on this project.

We thank the `Engineering and Physical Sciences Research Council (EPSRC)`_ for
funding the PhD scholarships of `M. Torpey`_ and `F. Smith`_ when they worked
on this project (EP/M506631/1, EP/N509759/1).

.. _OpenDreamKit: https://opendreamkit.org/
.. _Carnegie Trust for the Universities of Scotland: https://www.carnegie-trust.org/
.. _Engineering and Physical Sciences Research Council (EPSRC): https://epsrc.ukri.org/

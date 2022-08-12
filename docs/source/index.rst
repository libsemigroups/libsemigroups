libsemigroups - Version 2.2.1
=============================

C++ library for semigroups and monoids
--------------------------------------

What is ``libsemigroups``?
~~~~~~~~~~~~~~~~~~~~~~~~~~

``libsemigroups``  is a C++14 library containing implementations of several
algorithms for computing finite, and finitely presented, semigroups and
monoids. Namely:

- the `Froidure-Pin algorithm`_ for computing finite semigroups
  :cite:`Froidure1997aa`;
- the `Todd-Coxeter algorithm`_ for finitely presented semigroups and monoids;
  see also `this paper <https://arxiv.org/abs/2203.11148>`_ :cite:`Coleman2022aa`;
- the `Knuth-Bendix algorithm`_ for finitely presented semigroups and monoids;
- the `Schreier-Sims algorithm`_ for permutation groups;
- a preliminary implementation of the `Konieczny`_ :cite:`Konieczny1994aa` and
  `Lallement-McFadden`_ :cite:`Lallement1990aa` algorithm for computing finite
  semigroups which act on sets;
- an implementation of the `Radoszewski-Rytter`_ :cite:`Radoszewski2010aa`
  algorithm for testing equivalence of words in free bands;
- an implementation of the algorithm for solving the word problem
  for small overlap monoids, and for computing normal forms in such monoids;
  see `Kambites <https://doi.org/10.1016/j.jalgebra.2008.09.038>`__
  :cite:`Kambites2009aa`,
  `Kambites <https://doi.org/10.1016/j.jalgebra.2008.12.028>`__
  :cite:`Kambites2009ab`, and `Mitchell-Tsalakou
  <http://arxiv.org/abs/2105.12125>`__ :cite:`Mitchell2021aa`;
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
implement; see, for example,  :cpp:any:`libsemigroups::FroidurePin`,
:cpp:any:`libsemigroups::Konieczny`,
:cpp:any:`libsemigroups::congruence::ToddCoxeter`,
:cpp:any:`libsemigroups::fpsemigroup::Kambites`,
:cpp:any:`libsemigroups::fpsemigroup::KnuthBendix`,
:cpp:any:`libsemigroups::SchreierSims`, or
:cpp:any:`libsemigroups::Sims1`.

The implementations in :cpp:any:`libsemigroups::FroidurePin`,
:cpp:any:`libsemigroups::Konieczny`, and :cpp:any:`libsemigroups::SchreierSims`
are generic and easily adapted to user-defined types.

``libsemigroups`` uses: `HPCombi`_ which uses the SSE and AVX instruction sets
for very fast manipulation of transformations, partial permutations,
permutations, and boolean matrices of small size;  `catch`_ for tests;
`fmt`_ for reporting; and `eigen`_ for some linear algebra computations.

.. _HPCombi: https://github.com/hivert/HPCombi
.. _catch: https://github.com/catchorg/Catch2
.. _fmt: https://github.com/fmtlib/fmt
.. _eigen: http://eigen.tuxfamily.org/

.. toctree::
   :maxdepth: 1
   :caption: Installation and changelog

   install
   changelog

.. toctree::
   :caption: API REFERENCE
   :maxdepth: 1

   action
   congruences
   digraphs
   elements
   fpsemigroups
   freeband
   matrix/index
   misc
   order
   semigroups
   words

.. toctree::
   :caption: Bibliography

   biblio

.. toctree::
   :caption: Further info
   :maxdepth: 1

   semigroupe
   authors

.. _Semigroupe: https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html

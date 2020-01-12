libsemigroups - Version 1.0.5
=============================

C++ library for semigroups and monoids
--------------------------------------

What is ``libsemigroups``?
~~~~~~~~~~~~~~~~~~~~~~~~~~

``libsemigroups``  is a C++11 library containing implementations of several
algorithms for computing finite and finitely presented semigroups. 
Namely:

- the `Froidure-Pin algorithm`_ for computing finite semigroups
  :cite:`Froidure1997aa`;
- the `Todd-Coxeter algorithm`_ for finitely presented semigroups and monoids;
- the `Knuth-Bendix algorithm`_ for finitely presented semigroups and monoids;
- the `Schreier-Sims algorithm`_ for permutation groups.

.. _Froidure-Pin algorithm: https://www.irif.fr/~jep/PDF/Rio.pdf
.. _Todd-Coxeter algorithm: https://en.wikipedia.org/wiki/Todd%E2%80%93Coxeter_algorithm
.. _Knuth-Bendix algorithm: https://en.wikipedia.org/wiki/Knuth%E2%80%93Bendix_completion_algorithm
.. _Schreier-Sims algorithm: https://en.wikipedia.org/wiki/Schreier%E2%80%93Sims_algorithm

The development version of ``libsemigroups`` is available on github_, and some related projects are here_.

.. _github: https://github.com/libsemigroups/libsemigroups
.. _here: https://github.com/libsemigroups 

The main classes in ``libsemigroups`` are named after the algorithms they
implement; see, for example,  :cpp:any:`libsemigroups::FroidurePin`,
:cpp:any:`libsemigroups::congruence::ToddCoxeter`, 
:cpp:any:`libsemigroups::fpsemigroup::KnuthBendix`, and
:cpp:any:`libsemigroups::SchreierSims`.

The implementations in :cpp:any:`libsemigroups::FroidurePin` and
:cpp:any:`libsemigroups::SchreierSims` are generic and easily adapted to
user-defined types.

``libsemigroups`` uses: `HPCombi`_ which uses the SSE and AVX instruction sets
for very fast manipulation of transformations, partial permutations,
permutations, and boolean matrices of small size;  `catch`_ for tests; 
and `fmt`_ for reporting.

.. _HPCombi: https://github.com/hivert/HPCombi
.. _catch: https://github.com/catchorg/Catch2
.. _fmt: https://github.com/fmtlib/fmt

How to use it
~~~~~~~~~~~~~

See the installation instructions:

.. toctree::
   :maxdepth: 1
  
   install
   changelog

``libsemigroups`` is used in the `Semigroups package for GAP`_, and the
development version is available on `Github`. There were `python bindings`_
(for Python 2 and 3) for earlier versions of ``libsemigroups``, and there will
be in the future too, but presently they don't work. 

.. _Github: https://github.com/libsemigroups/libsemigroups 
.. _python bindings: https://github.com/libsemigroups/libsemigroups-python-bindings/
.. _Semigroups package for GAP: https://gap-packages.github.io/Semigroups 

.. toctree::
   :caption: API REFERENCE
   :hidden:

   action
   congruences
   digraphs
   elements
   fpsemigroups
   semigroups
   semiring
   misc

.. toctree::
   :caption: Bibliography
   :hidden:

   biblio

For the authors, contributors, and acknowledgements and a
comparison with Semigroupe_ by Jean-Eric Pin see below.

.. toctree::
   :caption: Further info
   :maxdepth: 1
  
   semigroupe
   authors

.. _Semigroupe: https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html 

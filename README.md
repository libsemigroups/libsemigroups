<!--
Distributed under the terms of the GPL license version 3.

The full license is in the file LICENSE, distributed with this
software.
-->

# libsemigroups 

[![image](https://img.shields.io/conda/dn/conda-forge/libsemigroups)](https://github.com/conda-forge/libsemigroups-feedstock)
[![image](https://zenodo.org/badge/DOI/10.5281/zenodo.1437752.svg)](https://doi.org/10.5281/zenodo.1437752)
[![image](https://anaconda.org/conda-forge/libsemigroups/badges/license.svg)](https://anaconda.org/conda-forge/libsemigroups)
[![image](https://anaconda.org/conda-forge/libsemigroups/badges/platforms.svg)](https://anaconda.org/conda-forge/libsemigroups)
[![codecov](https://codecov.io/gh/libsemigroups/libsemigroups/graph/badge.svg?token=lLBnFR6mq5)](https://codecov.io/gh/libsemigroups/libsemigroups)

## What is libsemigroups?

`libsemigroups` is a C++17 library containing implementations of several
algorithms for computing finite, and finitely presented, semigroups and
monoids. The main algorithms implemented in `libsemigroups` are:

- the [Froidure-Pin algorithm][] for computing semigroups and monoids defined
  by a generating set consisting of elements whose multiplication and equality is
  decidable (such as [transformations, partial permutations, permutations][],
  [bipartitions][], and [matrices over a semiring][]) in the
  `libsemigroups::FroidurePin` class template; 
- Kambites' algorithm for solving the word problem in small overlap monoids
  from ["Small overlap monoids I: The word problem"][], and the algorithm from
  ["An explicit algorithm for normal forms in small overlap monoids"][] in the
  class template [`libsemigroups::Kambites`][];
- the [Knuth-Bendix algorithm] for finitely presented semigroups and monoids 
  in the class template [`libsemigroups::KnuthBendix`][];
- a version of Sims' low index subgroup algorithm for computing congruences of a
  semigroup or monoid from 
  ["Computing finite index congruences of finitely presented semigroups and monoids"][] 
  in the classes `libsemigroups::Sims1` and `libsemigroups::Sims2`;
- a generalized version of the algorithms described in 
  ["Green's equivalences in finite semigroups of binary relations"][] by 
  Konieczny, and 
  ["On the determination of Green's relations in finite transformation semigroups"][] 
  by Lallement and Mcfadden for computing finite semigroups and monoids
  admitting a pair of actions with particular properties, in the class template
  `libsemigroups::Konieczny`;
- the algorithm from ["Efficient Testing of Equivalence of Words in a Free Idempotent Semigroup"][] 
  by Radoszewski and Rytter in the function `libsemigroups::freeband_equal_to`;
- a non-random version of the [Schreier-Sims algorithm][]
  for permutation groups in the class template `libsemigroups::SchreierSims`;
- a version of Stephen's procedure from 
  ["Applications of automata theory to presentations of monoids and inverse monoids"][]
  for finitely presented inverse semigroups and monoids (for a given word \\(w\\) this
  procedure is for determining words equivalent to \\(w\\) or that are
  left divisors of \\(w\\)) in the class template `libsemigroups::Stephen`;
- the [Todd-Coxeter algorithm][] for finitely presented semigroups and monoids; 
  in the class template [`libsemigroups::ToddCoxeter`][]; see also 
  ["The Todd–Coxeter algorithm for semigroups and monoids"][].

`libsemigroups` is partly based on 
[Algorithms for computing finite semigroups][Froidure-Pin algorithm], 
[Expository Slides][], and [Semigroupe 2.01][] by [Jean-Eric Pin][].

[Froidure-Pin algorithm]: https://www.irif.fr/~jep/PDF/Rio.pdf
[transformations, partial permutations, permutations]: group__transf__group.html
[bipartitions]: group__bipart__group.html
[matrices over a semiring]: group__matrix__group.html
["Small overlap monoids I: The word problem"]: https://doi.org/10.1016/j.jalgebra.2008.09.038
["An explicit algorithm for normal forms in small overlap monoids"]: https://doi.org/10.1016/j.jalgebra.2023.04.019
[Knuth-Bendix algorithm]: https://en.wikipedia.org/wiki/Knuth%E2%80%93Bendix_completion_algorithm
["Computing finite index congruences of finitely presented semigroups and monoids"]: https://arxiv.org/abs/2302.06295
["Green's equivalences in finite semigroups of binary relations"]: https://link.springer.com/article/10.1007/BF02573672
["On the determination of Green's relations in finite transformation semigroups"]: https://www.sciencedirect.com/science/article/pii/S0747717108800570
["Efficient Testing of Equivalence of Words in a Free Idempotent Semigroup"]: https://link.springer.com/chapter/10.1007/978-3-642-11266-9_55
["Applications of automata theory to presentations of monoids and inverse monoids"]: https://digitalcommons.unl.edu/dissertations/AAI8803771/
[Todd-Coxeter algorithm]: https://en.wikipedia.org/wiki/Todd%E2%80%93Coxeter_algorithm
["The Todd–Coxeter algorithm for semigroups and monoids"]: https://doi.org/10.1007/s00233-024-10431-z
[Schreier-Sims algorithm]: https://en.wikipedia.org/wiki/Schreier%E2%80%93Sims_algorithm
[`libsemigroups::ToddCoxeter`]: group__todd__coxeter__group.html
[Expository Slides]: https://www.irif.fr/~jep/PDF/Exposes/StAndrews.pdf
[Semigroupe 2.01]: https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html
[Jean-Eric Pin]: https://www.irif.fr/~jep/

`libsemigroups` is used in the [Semigroups package for
GAP](https://semigroups.github.io/Semigroups), and it is possible to use
`libsemigroups` directly in Python 3 via the package
[libsemigroups_pybind11](https://libsemigroups.github.io/libsemigroups_pybind11/).
The development version of `libsemigroups` is available on
[github](https://github.com/libsemigroups/libsemigroups), and some
related projects are [here](https://github.com/libsemigroups).

## Design principles

The main classes in `libsemigroups` are named after the algorithms they
implement; see, for example, `libsemigroups::FroidurePin`,
`libsemigroups::Konieczny`, [`libsemigroups::ToddCoxeter`][],
[`libsemigroups::Kambites`][], [`libsemigroups::KnuthBendix`][],
`libsemigroups::SchreierSims`, `libsemigroups::Sims1`, `libsemigroups::Sims2`,
or `libsemigroups::Stephen`.

`libsemigroups` is a modern open source C++ library designed to be:

-  **state of the art**: the mathematical underpinnings of several of the
   algorithms in `libsemigroups` originate from the authors and contributors; and the
   other algorithms are adapted from the literature (see above for details);
-  **extensible**: `libsemigroups::FroidurePin`, `libsemigroups::Konieczny`, and
   `libsemigroups::SchreierSims` are generic and can be adapted to user-defined
   types;
-  **adaptable**: the behaviour of many algorithms implementing in `libsemigroups` 
   can be fine-tuned via many settings, and used interactively via the functions
  `libsemigroups::Runner::run_for` and `libsemigroups::Runner::run_until`;
-  **easy to use**: converting between different types of `libsemigroups`
   objects (see [the `to` function][]) is easy; there are many
   helper functions to streamline common tasks; high quality exception messages
   are implemented throughout the code base (although you don't have to use these
   if you don't want to); long running algorithms can provide detailed feedback on
   their progress (see `libsemigroups::ReportGuard`); many data structures can be
   visualised using graphviz (see `libsemigroups::Dot` and
   [Visualisation][]);
   and there are hundreds of examples in the [tests][] directory.
-  **fast**: `libsemigroups` is designed with performance in mind; several
  classes implement parallel algorithms (`libsemigroups::Sims1`,
  `libsemigroups::Sims2`, [`libsemigroups::Congruence`][]; 
  we provide some "winner takes all" mechanisms for running algorithms
  concurrently (see [`libsemigroups::Congruence`][]; there are
  `_no_checks` versions of most functions if performance is critical.

[`libsemigroups::Congruence`]: group__congruence__group.html
[`libsemigroups::Kambites`]: group__kambites__group.html
[`libsemigroups::KnuthBendix`]: group__knuth_bendix__group.html
[`libsemigroups::ToddCoxeter`]: group__todd__coxeter__group.html
[tests]: https://github.com/libsemigroups/libsemigroups/tree/main/tests
[the `to` function]: group__to__group.html
[Visualisation]: group__dot__group.html

## Thanks

`libsemigroups` uses: 

-  [Catch2](https://github.com/catchorg/Catch2) for tests and benchmarks; 
-  [eigen](http://eigen.tuxfamily.org/) for some linear algebra computations;
-  [fmt](https://github.com/fmtlib/fmt) for string formatting;
-  [HPCombi](https://github.com/libsemigroups/HPCombi) which uses the SSE and AVX
   instruction sets for very fast manipulation of transformations, partial
   permutations, permutations, and boolean matrices of small size; 
-  [backward-cpp](https://github.com/bombela/backward-cpp) for beautiful
   backtraces;
-  [magic_enum](https://github.com/Neargye/magic_enum) for static reflection for
   enums;
-  [rx-ranges](https://github.com/simonask/rx-ranges) the minimalist ranges
   library for C++17.

We'd like to thank the authors and contributors to these excellent projects!

## How to use it

See the documentation <https://libsemigroups.github.io/libsemigroups/>.

## Issues

If you find any problems with `libsemigroups`, or have any suggestions
for features that you\'d like to see, please use the [issue
tracker](https://github.com/libsemigroups/libsemigroups/issues).

## Authors

[Reinis Cirpons][] (<rc234@st-andrews.ac.uk>)
[Joseph Edwards][] (<jde1@st-andrews.ac.uk>) 
[James Mitchell](https://jdbm.me) (<jdm3@st-andrews.ac.uk>)

## Contributors

-   [Luna Elliott](https://le27.github.io/L-Elliott/) 
    (<luna.elliott142857@gmail.com>)
    contributed to the Schreier-Sims implementation.
-   Jan Engelhardt (<jengelh@inai.de>) contributed some bug fixes to the
    build system, and a number of helpful issues.
-   Ilya Finkelshteyn (<ilyaf@appveyor.com>) contributed to the
    continuous integration in AppVeyor.
-   Isuru Fernando (<isuruf@gmail.com>) contributed to the build system.
-   [Florent Hivert](https://www.lri.fr/~hivert/)
    (<Florent.Hivert@lri.fr>) contributed many helpful ideas to
    `libsemigroups`, and `HPCombi`.
-   [Max Horn](https://math.rptu.de/en/wgs/agag/people/head/prof-dr-max-horn)
    (<max@quendi.de>) contributed some fixes.
-   [Jerry James](http://www.jamezone.org/) (<loganjerry@gmail.com>)
    contributed some bugfixes.
-   [Julius Jonušas][] contributed to the implementation of the Froidure-Pin
    algorithm.
-   [Samuel Lelièvre][] (<samuel.lelievre@gmail.com>) contributed a number of
    fixes to the docs.
-   Alex Levine (<A.Levine@uea.ac.uk>) contributed to the Schreier-Sims
    implementation.
-   [Dima Pasechnik](http://users.ox.ac.uk/~coml0531)
    (<dimpase@gmail.com>) contributed to the build system.
-   Chris Russell contributed some tests for finitely presented
    semigroups.
-   [Finn Smith][] (<fls3@st-andrews.ac.uk>)
    contributed the implementation of the Konieczny and
    Lallement-McFadden algorithm, to the Todd-Coxeter implementation,
    and to BMat8s.
-   [Nicolas Thiéry](http://nicolas.thiery.name/)
    (<nthiery@users.sf.net>) contributed to the build system, packaging
    `libsemigroups` via conda, the python bindings and many helpful
    conversations and suggestions.
-   [Maria Tsalakou][] (<mt200@st-andrews.ac.uk>) contributed to the Knuth-Bendix
    implementation, related algorithms for the class `WordGraph`,
    and to the implementation of the `Ukkonen` and `Kambites` classes.
-   [Wilf Wilson](https://wilf.me) (<wilf@wilf-wilson.net>) contributed some fixes.
-   Murray Whyte (<mw231@st-andrews.ac.uk>) contributed many examples of
    finitely presented semigroups and monoids, to the documentation and reported a
    number of bugs.
-   [Michael Young][] (<mct25@st-andrews.ac.uk>) contributed to the congruences
    code in the v0.0.1 to v0.6.7.

## Installation

See [installation page](install.md) for more info.

## Acknowledgements

We thank:

* [OpenDreamKit](https://opendreamkit.org/) Horizon 2020 European Research
  Infrastructures project (#676541);
* the [Carnegie Trust for the Universities of Scotland][] for funding the PhD scholarship of
  [Julius Jonušas][];
* the [EPSRC](https://epsrc.ukri.org/) for funding the PhD scholarships of
  [Michael Young][], [Finn Smith][], and [Reinis Cirpons][] (EP/M506631/1,
  EP/N509759/1);
* the [School of Mathematics and Statistics, University of St Andrews][] for
  funding the PhD scholarships of [Maria Tsalakou][], [Joseph Edwards][], and
  Murray Whyte;
* the Cyprus State Scholarship Foundation for their financial support for
  [Maria Tsalakou][].

[Reinis Cirpons]: https://reinisc.id.lv
[Joseph Edwards]: https://github.com/Joseph-Edwards
[Julius Jonušas]: http://julius.jonusas.work/
[Finn Smith]: https://flsmith.github.io
[Maria Tsalakou]: https://mariatsalakou.github.io/
[Michael Young]: https://mtorpey.github.io/
[Samuel Lelièvre]: https://lelievre.perso.math.cnrs.fr/index-en.html

[Carnegie Trust for the Universities of Scotland]: https://www.carnegie-trust.org/
[School of Mathematics and Statistics, University of St Andrews]: https://www.st-andrews.ac.uk/mathematics-statistics/

<!--
Distributed under the terms of the GPL license version 3.

The full license is in the file LICENSE, distributed with this
software.
-->

# libsemigroups 

[![Documentation Status](https://readthedocs.org/projects/libsemigroups/badge/?version=master)](https://libsemigroups.readthedocs.io/en/devel/?badge=master)
[![Launch Binder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gh/libsemigroups/libsemigroups/master)
[![image](https://img.shields.io/conda/dn/conda-forge/libsemigroups)](https://github.com/conda-forge/libsemigroups-feedstock)
[![image](https://zenodo.org/badge/DOI/10.5281/zenodo.1437752.svg)](https://doi.org/10.5281/zenodo.1437752)
[![image](https://anaconda.org/conda-forge/libsemigroups/badges/license.svg)](https://anaconda.org/conda-forge/libsemigroups)
[![image](https://anaconda.org/conda-forge/libsemigroups/badges/platforms.svg)](https://anaconda.org/conda-forge/libsemigroups)

## What is libsemigroups?

`libsemigroups` is a C++14 library containing implementations of several
algorithms for computing finite, and finitely presented, semigroups and
monoids. Namely:

-   the [Froidure-Pin algorithm](https://www.irif.fr/~jep/PDF/Rio.pdf)
    for computing finite semigroups;
-   the [Todd-Coxeter
    algorithm](https://en.wikipedia.org/wiki/Todd%E2%80%93Coxeter_algorithm)
    for finitely presented semigroups and monoids; see also [this
    paper](https://arxiv.org/abs/2203.11148);
-   the [Knuth-Bendix
    algorithm](https://en.wikipedia.org/wiki/Knuth%E2%80%93Bendix_completion_algorithm)
    for finitely presented semigroups and monoids;
-   the [Schreier-Sims
    algorithm](https://en.wikipedia.org/wiki/Schreier%E2%80%93Sims_algorithm)
    for permutation groups;
-   a preliminary implementation of the
    [Konieczny](https://link.springer.com/article/10.1007/BF02573672)
    and
    [Lallement-McFadden](https://www.sciencedirect.com/science/article/pii/S0747717108800570)
    algorithm for computing finite semigroups which act on sets;
-   an implementation of the
    [Radoszewski-Rytter](https://link.springer.com/chapter/10.1007/978-3-642-11266-9_55)
    algorithm for testing equivalence of words in free bands;
-   an implementation of the algorithm for solving the word problem for
    small overlap monoids, and for computing normal forms in such
    monoids; see
    [Kambites](https://doi.org/10.1016/j.jalgebra.2008.09.038),
    [Kambites](https://doi.org/10.1016/j.jalgebra.2008.12.028), and
    [Mitchell-Tsalakou](http://arxiv.org/abs/2105.12125)
-   a version of Sims low index subgroup algorithm for computing
    one-sided congruences of a semigroup or monoid; see [this
    paper](https://arxiv.org/abs/2302.06295) for details.
-   a version of [Stephen's procedure](https://rb.gy/brsuvc) for
    finitely presented semigroups and monoids (for a given word \\(w\\) this
    procedure is for determining words equivalent to $w$ or that are
    left divisors of $w$).

`libsemigroups` is partly based on [Algorithms for computing finite
semigroups](https://www.irif.fr/~jep/PDF/Rio.pdf), [Expository
Slides](https://www.irif.fr/~jep/PDF/Exposes/StAndrews.pdf), and
[Semigroupe
2.01](https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html)
by [Jean-Eric Pin](https://www.irif.fr/~jep/).

`libsemigroups` is used in the [Semigroups package for
GAP](https://semigroups.github.io/Semigroups), and it is possible to use
`libsemigroups` directly in Python 3 via the package
[libsemigroups_pybind11](https://libsemigroups.github.io/libsemigroups_pybind11/).
The development version of `libsemigroups` is available on
[github](https://github.com/libsemigroups/libsemigroups), and some
related projects are [here](https://github.com/libsemigroups).

The main classes in `libsemigroups` are named after the algorithms they
implement; see, for example, `libsemigroups::FroidurePin`,
`libsemigroups::Konieczny`, `libsemigroups::congruence::ToddCoxeter`,
`libsemigroups::fpsemigroup::Kambites`,
`libsemigroups::fpsemigroup::KnuthBendix`, and
`libsemigroups::SchreierSims`, `libsemigroups::Sims1`, or
`libsemigroups::Stephen`.

The implementations in `libsemigroups::FroidurePin`,
`libsemigroups::Konieczny`, and `libsemigroups::SchreierSims` are
generic and easily adapted to user-defined types.

`libsemigroups` uses: [HPCombi](https://github.com/hivert/HPCombi) which
uses the SSE and AVX instruction sets for very fast manipulation of
transformations, partial permutations, permutations, and boolean
matrices of small size; [catch](https://github.com/catchorg/Catch2) for
tests; [fmt](https://github.com/fmtlib/fmt) for reporting; and
[eigen](http://eigen.tuxfamily.org/) for some linear algebra
computations.

## How to use it

See the documentation <https://libsemigroups.readthedocs.io/en/latest/>

## Issues

If you find any problems with `libsemigroups`, or have any suggestions
for features that you\'d like to see, please use the [issue
tracker](https://github.com/libsemigroups/libsemigroups/issues).

## Author

[James Mitchell](https://jdbm.me) (<jdm3@st-andrews.ac.uk>)

## Contributors

-   [Reinis Cirpons](https://reinisc.id.lv) (<rc234@st-andrews.ac.uk>)
    contributed to `IsObviouslyInfinite`, to integrating `eigen`, and
    contributed an implementation of the Radoszewski-Rytter algorithm
    for testing equivalence of words in free bands.
-   Joseph Edwards (<jde1@st-andrews.ac.uk>) contributed the container
    `StaticTriVector2`.
-   Luke Elliott (<le27@st-andrews.ac.uk>) contributed to the
    Schreier-Sims implementation.
-   Jan Engelhardt (<jengelh@inai.de>) contributed some bug fixes to the
    build system, and a number of helpful issues.
-   Ilya Finkelshteyn (<ilyaf@appveyor.com>) contributed to the
    continuous integration in AppVeyor.
-   Isuru Fernando (<isuruf@gmail.com>) contributed to the build system.
-   [Florent Hivert](https://www.lri.fr/~hivert/)
    (<Florent.Hivert@lri.fr>) contributed many helpful ideas to
    `libsemigroups`, an allocator implementation (to be included in a
    future version), and `HPCombi`.
-   Max Horn (<max@quendi.de>) contributed some fixes.
-   [Jerry James](http://www.jamezone.org/) (<loganjerry@gmail.com>)
    contributed some bugfixes.
-   [Julius Jonušas](http://julius.jonusas.work/) contributed to the
    implementation of the Froidure-Pin algorithm.
-   [Alex Levine]{.title-ref} contributed to the Schreier-Sims
    implementation.
-   [Dima Pasechnik](http://users.ox.ac.uk/~coml0531)
    (<dimpase@gmail.com>) contributed to the build system.
-   Chris Russell contributed some tests for finitely presented
    semigroups.
-   [Finn Smith](https://flsmith.github.io) (<fls3@st-andrews.ac.uk>)
    contributed the implementation of the Konieczny and
    Lallement-McFadden algorithm, to the Todd-Coxeter implementation,
    and to BMat8s.
-   [Nicolas Thiéry](http://nicolas.thiery.name/)
    (<nthiery@users.sf.net>) contributed to the build system, packaging
    `libsemigroups` via conda, the python bindings and many helpful
    conversations and suggestions.
-   [Maria Tsalakou](https://mariatsalakou.github.io/)
    (<mt200@st-andrews.ac.uk>) contributed to the Knuth-Bendix
    implementation, related algorithms for the class `ActionDigraph`,
    and to the implementation of the `Kambites` class.
-   Wilf Wilson (<wilf@wilf-wilson.net>) contributed some fixes.
-   Murray Whyte (<mw231@st-andrews.ac.uk>) contributed to the
    documentation and reported a number of bugs.
-   [Michael Young](https://mtorpey.github.io/)
    (<mct25@st-andrews.ac.uk>) contributed to the congruences code in
    the v0.0.1 to v0.6.7.

## Installation
See [the other page](install.md) for more info.

## Acknowledgements

We acknowledge financial support from the
[OpenDreamKit](https://opendreamkit.org/) Horizon 2020 European Research
Infrastructures project (#676541) (primarily for the python bindings).

We thank the [Carnegie Trust for the Universities of
Scotland](https://www.carnegie-trust.org/) for funding the PhD
scholarship of [Julius Jonušas](http://julius.jonusas.work/) when he
worked on this project.

We thank the [Engineering and Physical Sciences Research Council
(EPSRC)](https://epsrc.ukri.org/) for funding the PhD scholarships of
[Michael Young](https://mtorpey.github.io/) and [Finn
Smith](https://flsmith.github.io) when they worked on this project
(EP/M506631/1, EP/N509759/1).


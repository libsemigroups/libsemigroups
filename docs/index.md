# libsemigroups - Version 2.7.1
## C++ library for semigroups and monoids
### What is `libsemigroups`?

`libsemigroups` is a C++17 library containing implementations of several
algorithms for computing finite, and finitely presented, semigroups and
monoids. Namely:

-   the `Froidure-Pin algorithm`\_ for computing finite semigroups
    `` `Froidure1997aa ``{.interpreted-text role="cite"}\`;
-   the `Todd-Coxeter algorithm`\_ for finitely presented semigroups and
    monoids; see also `this paper <https://arxiv.org/abs/2203.11148>`\_
    `` `Coleman2022aa ``{.interpreted-text role="cite"}\`;
-   the `Knuth-Bendix algorithm`\_ for finitely presented semigroups and
    monoids;
-   the `Schreier-Sims algorithm`\_ for permutation groups;
-   a preliminary implementation of the `Konieczny`\_
    `` `Konieczny1994aa ``{.interpreted-text role="cite"}[ and
    ]{.title-ref}[Lallement-McFadden]{.title-ref}[\_
    :cite:]{.title-ref}[Lallement1990aa]{.title-ref}\` algorithm for
    computing finite semigroups which act on sets;
-   an implementation of the `Radoszewski-Rytter`\_
    `` `Radoszewski2010aa ``{.interpreted-text role="cite"}\` algorithm
    for testing equivalence of words in free bands;
-   an implementation of the algorithm for solving the word problem for
    small overlap monoids, and for computing normal forms in such
    monoids; see
    `Kambites <https://doi.org/10.1016/j.jalgebra.2008.09.038>`**:cite:\`\`Kambites2009aa\`\`,
    \`\`Kambites
    \<https://doi.org/10.1016/j.jalgebra.2008.12.028\>\`\`**
    `` `Kambites2009ab ``{.interpreted-text role="cite"}[, and
    ]{.title-ref}[Mitchell-Tsalakou
    \<http://arxiv.org/abs/2105.12125\>]{.title-ref}[\_\_
    :cite:]{.title-ref}[Mitchell2021aa]{.title-ref}\`;
-   a version of Sims low index subgroup algorithm for computing
    one-sided congruences of a semigroup or monoid;
-   a version of `Stephen's procedure`\_ for finitely presented
    semigroups and monoids (for a given word $`w$[ this procedure is for
    determining words equivalent to :math:]{.title-ref}[w]{.title-ref}[
    or that are left divisors of :math:]{.title-ref}[w]{.title-ref}\`).

\_Todd-Coxeter algorithm:
<https://en.wikipedia.org/wiki/Todd%E2%80%93Coxeter_algorithm> ..
\_Knuth-Bendix algorithm:
<https://en.wikipedia.org/wiki/Knuth%E2%80%93Bendix_completion_algorithm>
.. \_Schreier-Sims algorithm:
<https://en.wikipedia.org/wiki/Schreier%E2%80%93Sims_algorithm> ..
\_Konieczny: <https://link.springer.com/article/10.1007/BF02573672> ..
\_Lallement-McFadden:
<https://www.sciencedirect.com/science/article/pii/S0747717108800570> ..
\_Radoszewski-Rytter:
<https://link.springer.com/chapter/10.1007/978-3-642-11266-9_55> ..
\_Stephen's procedure: <https://rb.gy/brsuvc>

`libsemigroups` is partly based on
`Algorithms for computing finite semigroups`*, \`\`Expository
Slides\`\`*, and `Semigroupe 2.01`\_ by `Jean-Eric Pin`\_.

<https://www.irif.fr/~jep/PDF/Rio.pdf> .. \_Expository slides:
<https://www.irif.fr/~jep/PDF/Exposes/StAndrews.pdf> .. \_Semigroupe
2.01:
<https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html> ..
\_Jean-Eric Pin: <https://www.irif.fr/~jep/>

`libsemigroups` is used in the `Semigroups package for GAP`*, and it is
possible to use \`\`libsemigroups\`\` directly in Python 3 via the
package \`\`libsemigroups_pybind11\`\`*. The development version of
`libsemigroups` is available on github\_, and some related projects are
here\_.

<https://github.com/libsemigroups> .. \_libsemigroups_pybind11:
<https://libsemigroups.github.io/libsemigroups_pybind11/> ..
\_Semigroups package for GAP: <https://semigroups.github.io/Semigroups>

The main classes in `libsemigroups` are named after the algorithms they
implement; see, for example,
`` `libsemigroups::FroidurePin ``{.interpreted-text role="cpp:any"}[,
:cpp:any:]{.title-ref}[libsemigroups::Konieczny]{.title-ref}[,
:cpp:any:]{.title-ref}[libsemigroups::congruence::ToddCoxeter]{.title-ref}[,
:cpp:any:]{.title-ref}[libsemigroups::fpsemigroup::Kambites]{.title-ref}[,
:cpp:any:]{.title-ref}[libsemigroups::fpsemigroup::KnuthBendix]{.title-ref}[,
:cpp:any:]{.title-ref}[libsemigroups::SchreierSims]{.title-ref}[,
:cpp:any:]{.title-ref}[libsemigroups::Sims1]{.title-ref}[, or
:cpp:any:]{.title-ref}[libsemigroups::Stephen]{.title-ref}\`.

The implementations in
`` `libsemigroups::FroidurePin ``{.interpreted-text role="cpp:any"}[,
:cpp:any:]{.title-ref}[libsemigroups::Konieczny]{.title-ref}[, and
:cpp:any:]{.title-ref}[libsemigroups::SchreierSims]{.title-ref}\` are
generic and easily adapted to user-defined types.

`libsemigroups` uses: `HPCombi`\_ which uses the SSE and AVX instruction
sets for very fast manipulation of transformations, partial
permutations, permutations, and boolean matrices of small size;
`catch`\_ for tests; `fmt`\_ for reporting; and `eigen`\_ for some
linear algebra computations.

<https://github.com/catchorg/Catch2> .. \_fmt:
<https://github.com/fmtlib/fmt> .. \_eigen:
<http://eigen.tuxfamily.org/>

::: toctree

maxdepth

:   1 :caption: Installation and changelog
:::

install changelog

::: toctree

caption

:   API REFERENCE :maxdepth: 1
:::

action congruences digraphs elements fpsemigroups freeband matrix/index
misc order semigroups words

::: toctree

caption

:   Bibliography
:::

biblio

::: toctree

caption

:   Further info :maxdepth: 1
:::

semigroupe authors

<https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html>

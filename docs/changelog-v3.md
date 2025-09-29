# Changelog - version 3

## v3.2.0 (released 30/09/2025)

This release contains a number of improvements, bug fixes, and a small number
of new features:

- Balance presentation by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/785
- `is_obviously_infinite(Kambites)` respects generating pairs by [\@Joseph-Edwards] in https://github.com/libsemigroups/libsemigroups/pull/791
- Check `is_obviously_infinite` before running in `Congruence::number_of_classes` by [\@Joseph-Edwards] in https://github.com/libsemigroups/libsemigroups/pull/790
- Bipartition benchmarks by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/759
- Add an option to compile with external backward by [\@Joseph-Edwards] in https://github.com/libsemigroups/libsemigroups/pull/798
- Add CI jobs for OpenBSD and FreeBSD by [\@Joseph-Edwards] in https://github.com/libsemigroups/libsemigroups/pull/799
- Use the latest version of backward by [\@Joseph-Edwards] in https://github.com/libsemigroups/libsemigroups/pull/797
- Update the contributing guide by [\@Joseph-Edwards] in https://github.com/libsemigroups/libsemigroups/pull/802
- Fix some issues in the doc by [\@Joseph-Edwards] in https://github.com/libsemigroups/libsemigroups/pull/807
- Improve reporting for Todd-Coxeter by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/754
- todd-coxeter: fix bug in reduce by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/808
- todd-coxeter: fix some reporting issues by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/813
- word-graph: improve standardization performance by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/811
- todd-coxeter: fix typo in doc by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/818
- to-presentation: add Kambites -> Presentation by [\@jswent] in https://github.com/libsemigroups/libsemigroups/pull/812

## New Contributors

- [\@jswent] made their first contribution in https://github.com/libsemigroups/libsemigroups/pull/812

## v3.1.3 (released 12/08/2025)

This is a minor release that does the following:

- Add a `to_human_readable_repr` for the `Action` class by [\@james-d-mitchell] in
  https://github.com/libsemigroups/libsemigroups/pull/777
- Update the `to_human_readable_repr` of the `Congruence` class to be more
  consistent with the `KnuthBendix` and `ToddCoxeter` classes by
  [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/778
- Mark `IsBipartition`, `IsPBR`, `IsPresentation`, `IsInversePresentation` and
  `IsWordGraph` as deprecated, to be favoured by `is_specialization_of` by
  [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/771

## v3.1.2 (released 29/07/2025)

This is a minor release fixing some more issues that were introduced in v3.1.0.
Namely:

- Remove remaining occurrences of `std::basic_string<uint8_t>` by
  [\@joseph-edwards] in
  https://github.com/libsemigroups/libsemigroups/pull/772

## v3.1.1 (released 28/07/2025)

This is a minor release fixing some issues in the previous release. It is
currently only possible to construct `KnuthBendix` objects using presentations
with at most `128` generators. This is properly checked, and a meaningful
exception is thrown.

## v3.1.0 (released 19/07/2025)

This release contains a number of bugfixes, and other improvements. In
particular, the performance of the Todd-Coxeter and Knuth-Bendix algorithms has
been improved.

- Add presentation for the `abacus_jones_monoid` by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/711
- Fix issue 557 by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/717
- Alphabetise Low Index menu section by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/719
- Throw if bad degree by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/723
- word-graph: group digits in WordGraph repr by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/725
- word-graph: fix last_node_on_path doc by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/727
- Fix transf corner cases by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/724
- Make `check_doxygen_line_breaks.py` compatible with Python 3.9 by [\@Joseph-Edwards] in https://github.com/libsemigroups/libsemigroups/pull/728
- Fix greedy presentation issue by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/726
- action: fix add_seed issue by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/729
- Update doxygen-awesome by [\@Joseph-Edwards] in https://github.com/libsemigroups/libsemigroups/pull/732
- build: properly set `FMT_HEADER_ONLY` by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/733
- to: fix `to<Presentation>(kb)` by [\@Joseph-Edwards] in https://github.com/libsemigroups/libsemigroups/pull/731
- Add more `Sims` benchmarks and improve benchmarking infrastructure by [\@reiniscirpons] in https://github.com/libsemigroups/libsemigroups/pull/715
- ToddCoxeter fixes by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/730
- Prevent bad accesses of empty containers by [\@Joseph-Edwards] in https://github.com/libsemigroups/libsemigroups/pull/737
- exception: default visibility by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/736
- konieczny: reference returned by add_generator/s by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/738
- Fix return types by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/739
- Use group_digits in all to_human_readable_repr by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/741
- kbe: make it easier to use in python bindings by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/740
- Update my coordinates in README.md by \@dimpase in https://github.com/libsemigroups/libsemigroups/pull/742
- Update the v2 changelog for v2.7.4 by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/744
- Look behind by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/745
- todd-coxeter: doc fixes by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/746
- Todd coxeter redefine by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/747
- print: split into separate file by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/748
- multi-view: implement from MultiStringView by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/749
- Presentation updates by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/751
- Template `ToWord` by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/750
- Add AhoCorasickImpl with strong alphabet constraints by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/752
- Knuth bendix performance improvements by [\@james-d-mitchell] in https://github.com/libsemigroups/libsemigroups/pull/753
- Reorganise the doc by [\@Joseph-Edwards] in https://github.com/libsemigroups/libsemigroups/pull/755

## v3.0.3 (released 09/04/2025)

This is a minor release fixing some issues in the previous v3 releases. Namely:

- failure to compile with (the not yet released) gcc-15
  https://github.com/libsemigroups/libsemigroups/issues/557

## v3.0.2 (released 01/04/2025)

This is a minor release fixing some issues in the previous v3 releases. Namely:

- Fix issue in `SimsRefinerIdeals` by [\@james-d-mitchell] in
  https://github.com/libsemigroups/libsemigroups/pull/710
- Konieczny: fix typos in `to_human_readable_repr` by [\@james-d-mitchell] in
  https://github.com/libsemigroups/libsemigroups/pull/714

## v3.0.1 (released 26/03/2025)

This is a minor release to fix some issues in the last release, namely:

- the library version number was not updated to 3 (thanks to Jan Engelhardt for
  pointing this out);
- the include directories for `make install` were not correct (thanks to
  Antonio Rojas for pointing this out);
- the release archive contained an extra directory named
  `libsemigroups-3.0.0/libsemigroups-3.0.0` when unpacked under some
  circumstances, which prevented the conda feedstock from updating correctly;
- we made some minor improvements to the doc + `README.md`.

## v3.0.0 (released 21/03/2025)

This release includes a major refactor of the interfaces of the main
algorithms in `libsemigroups` with the view to making them easier to use,
more uniform, simpler, and more composable. The extent of the refactor means
that it is likely that code written using earlier versions of `libsemigroups`
will not compile.

Perhaps the biggest changes are:

- reporting during a computation has been greatly improved (both in its
  appearance and the mechanism used to ensure that feedback is produced at
  regular intervals);

- class interfaces were reduced in size, with member functions only calling
  other public member functions being moved into a helper namespace. Helper
  functions for `SomeThing` are in the namespace `some_thing`;

- most classes now use iterators exclusively in their interfaces (rather than
  \ref libsemigroups::word_type or `std::string` objects as in version 2);

- all of the classes and class templates for computing congruences over
  finitely presented semigroups and monoids (\ref congruence_group , \ref
  kambites_group, \ref knuth_bendix_group, \ref todd_coxeter_group,
  \ref libsemigroups::Sims1, \ref libsemigroups::Sims2, \ref libsemigroups::Stephen):
  - have a common set of constructors, primarily from \ref libsemigroups::Presentation objects;
  - can be instantiated with any type of words (usually \ref libsemigroups::word_type or
    `std::string`);
  - it is now possible to use \ref knuth_bendix_group to compute one-sided congruences;
  - it is now possible to use \ref todd_coxeter_group to compute monoid as well as
    semigroup presentations;
  - there is a variant of \ref knuth_bendix_group that uses a trie for rewriting;
  - a significant amount of functionality has been added to \ref
    libsemigroups::Sims1 (for one-sided low index congruences), and \ref
    libsemigroups::Sims2 (for two-sided low index congruences) has also been
    added.
  - the class template \ref libsemigroups::InversePresentation for finitely
    presented inverse semigroups and monoids was introduced, and can be used as
    the input to \ref libsemigroups::Stephen, which, in this case, implements
    Stephen's procedure for inverse semigroups.

- the enum \ref libsemigroups::congruence_kind used to contain three values
  `left`, `right`, and `twosided`. Essentially every algorithm in the library for
  computing `left` congruences simply reversed the relations in the input
  presentation. However, the output was not "reversed", which led to some
  inconsistencies and opportunities for confusion. As a consequence, there are
  only two values in \ref libsemigroups::congruence_kind `onesided` and
  `twosided`, where `onesided` always means right. Left congruences can be
  computed as right congruences of the dual semigroup or monoid;

- the class `ActionDigraph` has been renamed `WordGraph` for consistency with
  the mathematical literature;

- some functionality that used to be in `ActionDigraph` has been move
  into the classes \ref libsemigroups::Gabow and \ref libsemigroups::Paths for ease
  of use;

- the class \ref libsemigroups::Dot facilitates the creation and rendering of
  graph descriptions in the DOT language of the Graphviz graph drawing software;

- every constructor of every class in `libsemigroups` (except the copy/move
  constructor and the copy/move assignment operators) has a corresponding `init`
  function. These functions put the object into the same state as if they had
  been newly constructed, but they do not necessarily relinquish any memory allocated;

- throughout the library, functions with the suffix `_no_checks` do not check
  their arguments are valid; while those without this suffix will throw an
  exception if their arguments are invalid;

- conversion from one `libsemigroups` type to another is now exclusively via
  the function template \ref to_group "to". Previously, these were entangled in
  some classes;

- many constructors for classes where it is expected that many instances will
  be created (such as \ref libsemigroups::Transf, for example) do not perform any
  checks on their arguments. Alternative versions of these constructors, that
  do perform checks, are available using the \ref make_group "make" function.
  The \ref make_group "make" function also provides some alternate convenience
  constructors that only use the public member functions of the underlying
  class;

- the documentation is now generated solely by [Doxygen] and
  [doxygen-awesome-css];

- compiling with [fmt](https://github.com/fmtlib/fmt) enabled is now required;

- `libsemigroups` now vendors and uses copies of:
  - [magic_enum](https://github.com/Neargye/magic_enum) for static reflection for
    enums;
  - [rx-ranges](https://github.com/simonask/rx-ranges) the minimalist ranges
    library for C++17;

- some configuration flags have been withdrawn (`--enable-stats` and
  `--enable-verbose`);

- range objects are used fairly extensively throughout the library.

`libsemigroups` has been updated to use C++17 (from C++14), and some
simplifications were made as a consequence. The documentation has been
improved, some deprecated (or now unnecessary) functionality was removed.

[doxygen]: https://www.doxygen.nl
[doxygen-awesome-css]: https://github.com/jothepro/doxygen-awesome-css
[\@james-d-mitchell]: https://github.com/james-d-mitchell
[\@joseph-edwards]: https://github.com/joseph-edwards
[\@reiniscirpons]: https://github.com/reiniscirpons

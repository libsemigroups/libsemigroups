# Changelog - version 3

## v3.0.3 (released 09/04/2025)

This is a minor release fixing some issues in the previous v3 releases. Namely:

* failure to compile with (the not yet released) gcc-15
  https://github.com/libsemigroups/libsemigroups/issues/557

## v3.0.2 (released 01/04/2025)

This is a minor release fixing some issues in the previous v3 releases. Namely:

* Fix issue in `SimsRefinerIdeals` by @james-d-mitchell in
  https://github.com/libsemigroups/libsemigroups/pull/710
* Konieczny: fix typos in `to_human_readable_repr` by @james-d-mitchell in
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
  - can be instantiated with any type of words (usually \ref libsemigroups::word_type  or
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

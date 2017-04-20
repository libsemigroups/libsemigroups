# libsemigroups - C++ library for semigroups and monoids - Version 0.2.2
### by [J. D. Mitchell](http://www-groups.mcs.st-andrews.ac.uk/~jamesm/) with contributions by [J. Jonusas](http://www-circa.mcs.st-andrews.ac.uk/~julius/) and [M. Torpey](http://www-circa.mcs.st-andrews.ac.uk/~mct25/)

[![Build Status](https://travis-ci.org/james-d-mitchell/libsemigroups.svg?branch=master)](https://travis-ci.org/james-d-mitchell/libsemigroups)
[![Build Status](https://ci.appveyor.com/api/projects/status/github/james-d-mitchell/libsemigroups)](https://ci.appveyor.com/api/projects/status/github/james-d-mitchell/libsemigroups) 

The current version of libsemigroups is 0.2.2.
libsemigroups requires a C++ compiler supporting the c++11 standard.

libsemigroups is a C++ library for semigroups and monoids; it is partly based on 
[Algorithms for computing finite semigroups](https://www.irif.fr/~jep/PDF/Rio.pdf), 
[Expository Slides](https://www.irif.fr/~jep/PDF/Exposes/StAndrews.pdf), and 
[Semigroupe 2.01](https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html) by Jean-Eric Pin.

The libsemigroups library is used in the 
[Semigroups package for GAP](http://gap-packages.github.io/Semigroups/).
The development version is available on 
[Github](https://github.com/james-d-mitchell/libsemigroups).

Some of the features of 
[Semigroupe 2.01](https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html) 
are not yet implemented in libsemigroups, this is a work in
progress. Missing features include those for:

* Green's relations, or classes
* finding a zero
* minimal ideal, principal left/right ideals, or indeed any ideals
* inverses
* local submonoids
* the kernel
* variety tests.

These will be included in a future version. 

libsemigroups performs roughly the same as 
[Semigroupe 2.01](https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html)
when there is a known upper bound on the size of the semigroup being
enumerated, and this is used to initialise the data structures for the
semigroup; see libsemigroups::Semigroup::reserve for more
details. Note that in 
[Semigroupe 2.01](https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html)
it is always necessary to provide such an upper bound, but in libsemigroups
it is not. 

libsemigroups also has some advantages over 
[Semigroupe 2.01](https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html):

* there is a (hopefully) convenient C++ API, which makes it relatively easy to
  create, and manipulate semigroups and monoids
* there are some multithreaded methods for semigroups and their congruences
* you do not have to know/guess the size of a semigroup or monoid before you
  begin
* libsemigroups supports more types of elements than 
[Semigroupe 2.01](https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html)
* it is relatively straightforward to add support for further types of elements
  and semigroups
* it is possible to enumerate a certain number of elements of a semigroup or
  monoid (say if you are looking for an element with a particular property), to
  stop, and then start the enumeration again at a later point
* you can instantiate as many semigroups and monoids as you can fit in memory
* it is possible to add more generators after a semigroup or monoid has been
  constructed, without losing or having to recompute any information that was
  previously known
* libsemigroups contains a rudimentary implementations of the
  [Todd-Coxeter](https://en.wikipedia.org/wiki/Todd–Coxeter_algorithm) and
  [Knuth-Bendix](https://en.wikipedia.org/wiki/Knuth–Bendix_completion_algorithm)
  algorithms for finitely presented semigroups, which can also be used to
  compute congruence of a (not necessarily finitely presented) semigroup or
  monoid.

## Issues
If you find any problems with libsemigroups or have any suggestions for features
that you'd like to see please use the 
[issue tracker](https://github.com/james-d-mitchell/libsemigroups/issues).

## Documentation
The documentation is generated using
[doxygen](https://www.doxygen.org) and is available
[here](http://james-d-mitchell.github.io/libsemigroups/).
This documentation can be compiled by running `make doc` in the `libsemigroups`
directory, and the tests can be run by doing `make test` in the `libsemigroups`
directory.



#<cldoc:index>

Semigroups++

# Semigroups++ Version 0.0.3
### by [J. D. Mitchell](http://www-groups.mcs.st-andrews.ac.uk/~jamesm/) with contributions by [J. Jonusas](http://www-circa.mcs.st-andrews.ac.uk/~julius/) and [M. Torpey](http://www-circa.mcs.st-andrews.ac.uk/~mct25/)

This is the source code documentation for Semigroups++ produced using
[cldoc](https://github.com/jessevdk/cldoc). This documentation can be
compiled by running `make doc` in the `semigroupsplusplus`
directory, and the tests can be run by doing `make test` in the
`semigroupsplusplus` directory.

The algorithms and data structures in this version of Semigroups++ are
based on 
[Algorithms for computing finite semigroups](https://www.irif.fr/~jep/PDF/Rio.pdf), 
[Expository Slides](https://www.irif.fr/~jep/PDF/Exposes/StAndrews.pdf), and 
[Semigroupe 2.01](https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html) 
by Jean-Eric Pin.

The Semigroups++ library is used in the 
[Semigroups package for GAP](http://gap-packages.github.io/Semigroups/).
The development version is available on 
[Github](https://github.com/james-d-mitchell/semigroupsplusplus).

Some of the features of 
[Semigroupe 2.01](https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html) 
are not yet implemented in the Semigroups++ library, this is a work in
progress. Missing features include those for:

* Green's relations, or classes
* finding a zero
* minimal ideal, principal left/right ideals, or indeed any ideals
* inverses
* local submonoids
* the kernel
* variety tests.

These will be included in a future version. 

The Semigroups++ library performs roughly the same as 
[Semigroupe 2.01](https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html)
when there is a known upper bound on the size of the semigroup being
enumerated, and this is used to initialise the data structures for the
semigroup; see <semigroupsplusplus::Semigroup::set_batch_size> for more
details. Note that in 
[Semigroupe 2.01](https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html)
it is always necessary to provide such an upper bound, but in the Semigroups++
library it is not. 

The Semigroups++ library also has some advantages over 
[Semigroupe 2.01](https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html):

* there is a (hopefully) convenient C++ API, which makes it relatively easy to
  create, and manipulate semigroups and monoids
* there are some multithreaded methods for semigroups and their congruences
* you do not have to know/guess the size of a semigroup or monoid before you
  begin
* Semigroups++ supports more types of elements than 
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
* Semigroups++ contains a rudimentary implementation of the 
  [Todd-Coxeter algorithm](https://en.wikipedia.org/wiki/Todd–Coxeter_algorithm)
  for finitely presented semigroups, which can also be used to
  compute congruence of a (not necessarily finitely presented) semigroup or
  monoid.

If you find any problems with Semigroups++ or have any suggestions for features
that you'd like to see please use the 
[issue tracker](https://github.com/james-d-mitchell/semigroupsplusplus/issues).

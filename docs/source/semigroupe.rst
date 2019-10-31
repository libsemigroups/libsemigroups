Semigroupe
==========

``libsemigroups`` is partly based on `Algorithms for computing finite
semigroups <https://www.irif.fr/~jep/PDF/Rio.pdf>`__, `Expository
Slides <https://www.irif.fr/~jep/PDF/Exposes/StAndrews.pdf>`__, and
`Semigroupe
2.01 <https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html>`__
by Jean-Eric Pin. 

Some of the features of `Semigroupe
2.01 <https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html>`__
are not yet implemented in ``libsemigroups``, this is a work in progress.
Missing features include those for:

-  Green’s relations, or classes
-  finding a zero
-  minimal ideal, principal left/right ideals, or indeed any ideals
-  inverses
-  local submonoids
-  the kernel
-  variety tests.

These may be included in a future version.

``libsemigroups`` performs roughly the same as `Semigroupe
2.01 <https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html>`__
when there is a known upper bound on the size of the semigroup being
enumerated, and this is used to initialise the data structures for the
semigroup; see libsemigroups::Semigroup::reserve for more details. Note
that in `Semigroupe
2.01 <https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html>`__
it is always necessary to provide such an upper bound, but in
``libsemigroups`` it is not.

``libsemigroups`` also has some advantages over `Semigroupe
2.01 <https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html>`__

-  there is a (hopefully) convenient C++ API, which makes it relatively
   easy to create and manipulate semigroups and monoids
-  there are some multithreaded methods for semigroups and their
   congruences
-  you do not have to know/guess the size of a semigroup or monoid
   before you begin
-  ``libsemigroups`` supports more types of elements than `Semigroupe
   2.01 <https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html>`__
-  it is relatively straightforward to add support for further types of
   elements and semigroups
-  it is possible to enumerate a certain number of elements of a
   semigroup or monoid (say if you are looking for an element with a
   particular property), to stop, and then to start the enumeration
   again at a later point
-  you can instantiate as many semigroups and monoids as you can fit in
   memory
-  it is possible to add more generators after a semigroup or monoid has
   been constructed, without losing or having to recompute any
   information that was previously known
-  ``libsemigroups`` contains implementations of the Todd-Coxeter and
   Knuth-Bendix algorithms for finitely presented semigroups, which can
   also be used to compute congruences of a (not necessarily finitely
   presented) semigroup or monoid.

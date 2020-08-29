Semigroupe
==========

The algorithm implemented in the class ``FroidurePin``
is based on `Algorithms for computing finite
semigroups <https://www.irif.fr/~jep/PDF/Rio.pdf>`__, `Expository
Slides <https://www.irif.fr/~jep/PDF/Exposes/StAndrews.pdf>`__, and
`Semigroupe
2.01 <https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html>`__
by Jean-Eric Pin. 

Some of the features of `Semigroupe
2.01 <https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html>`__
are not yet implemented in ``FroidurePin``, this is a work in progress.
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
semigroup; see ``libsemigroups::FroidurePin::reserve`` for more details. Note
that in `Semigroupe
2.01 <https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html>`__
it is always necessary to provide such an upper bound, but in
``libsemigroups`` it is not.

The ``FroidurePin`` class has some advantages over `Semigroupe
2.01 <https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html>`__

-  there is a (hopefully) convenient C++ API, which makes it relatively
   easy to create and manipulate semigroups and monoids
-  more types of elements are supported
-  it is relatively straightforward to add support for further types of
   elements
-  it is possible to enumerate a certain number of elements of a
   semigroup or monoid (say if you are looking for an element with a
   particular property), to stop, and then to start the enumeration
   again at a later point
-  it is possible to add more generators after a semigroup or monoid has
   been constructed, without losing or having to recompute any
   information that was previously known

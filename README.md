[![Build Status](https://travis-ci.org/james-d-mitchell/semigroupsplusplus.svg?branch=master)](https://travis-ci.org/james-d-mitchell/semigroupsplusplus)
[![Build Status](https://ci.appveyor.com/api/projects/status/github/james-d-mitchell/semigroupsplusplus)](https://ci.appveyor.com/api/projects/status/github/james-d-mitchell/semigroupsplusplus) 

# Semigroups++ - A C++ library for computing finite semigroups
### by [J. D. Mitchell](http://www-groups.mcs.st-andrews.ac.uk/~jamesm/) with contributions by [J. Jonusas](http://www-circa.mcs.st-andrews.ac.uk/~julius/) and [M. Torpey](http://www-circa.mcs.st-andrews.ac.uk/~mct25/)

## Introduction
Semigroups++ is a C/C++ library for computing finite semigroups; it is based on 
[Algorithms for computing finite semigroups](https://www.irif.fr/~jep/PDF/Rio.pdf), 
[Expository Slides](https://www.irif.fr/~jep/PDF/Exposes/StAndrews.pdf), and 
[Semigroupe 2.01](https://www.irif.fr/~jep/Logiciels/Semigroupe2.0/semigroupe2.html) by Jean-Eric Pin.

The Semigroups++ library is used in the [Semigroups package for GAP](https://gap-packages.github.io/Semigroups/).

The current version of Semigroups++ is 0.0.3 from 14/12/2016.

## Documentation
The C/C++ library API is generated using [cldoc](https://github.com/jessevdk/cldoc) and available [here](http://james-d-mitchell.github.io/semigroupsplusplus/).

## Dependencies

Semigroups++ requires a C++ compiler supporting the c++11 standard.

## Using with cling (EXPERIMENTAL!!!)

After autoconfiscation ...

      ./configure
      make
      sudo make install
      cling -L /usr/local/lib
      [cling]$ .L semigroups
      [cling]$ #include "../semigroups.h"
      [cling]$ using namespace semigroupsplusplus;
      [cling]$   std::vector<Element*> gens = {new Transformation<u_int16_t>({0, 1, 0}),
      [cling]$                                 new Transformation<u_int16_t>({0, 1, 2})};
      [cling]$   Semigroup S = Semigroup(gens);
      [cling]$ S.size(false)
      2

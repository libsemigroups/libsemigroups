//****************************************************************************//
//     Copyright (C) 2016-2024 Florent Hivert <Florent.Hivert@lisn.fr>,       //
//                                                                            //
//  This file is part of HP-Combi <https://github.com/libsemigroups/HPCombi>  //
//                                                                            //
//  HP-Combi is free software: you can redistribute it and/or modify it       //
//  under the terms of the GNU General Public License as published by the     //
//  Free Software Foundation, either version 3 of the License, or             //
//  (at your option) any later version.                                       //
//                                                                            //
//  HP-Combi is distributed in the hope that it will be useful, but WITHOUT   //
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     //
//  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License      //
//  for  more details.                                                        //
//                                                                            //
//  You should have received a copy of the GNU General Public License along   //
//  with HP-Combi. If not, see <https://www.gnu.org/licenses/>.               //
//****************************************************************************//

/** @file
@brief Main entry point; includes the API files: bmat8.hpp, perm16.hpp, etc
and also debug.hpp, epu8.hpp, etc.*/

#ifndef HPCOMBI_HPCOMBI_HPP_
#define HPCOMBI_HPCOMBI_HPP_

#include "bmat8.hpp"
#include "bmat16.hpp"
#include "debug.hpp"
#include "epu8.hpp"
#include "perm16.hpp"
#include "perm_generic.hpp"
#include "power.hpp"
#include "vect16.hpp"
#include "vect_generic.hpp"

#endif  // HPCOMBI_HPCOMBI_HPP_

/*! \mainpage HPCombi

\section readme_sec Readme

You might want to have a look at [the Readme in the
sources](https://github.com/libsemigroups/HPCombi/blob/main/README.md).

\section sec_philo Philosophy
This library provides high performance computations in combinatorics (hence its
name). In practice we observe large speedups in several enumeration problems.

The main idea of the library is a way to encode data as a small sequence of
small integers, that can be handled efficiently by a creative use of vector
instructions. For example, on the current x86 machines, small permutations (N ≤
16) are very well handled. Indeed thanks to machine instructions such as PSHUFB
(Packed SHUFfle Bytes), applying a permutation on a vector only takes a few CPU
cycles.

Further ideas are:
- Vectorization (MMX, SSE, AVX instructions sets) and careful memory alignment,
- Careful memory management: avoid all dynamic allocation during the
computation,
- Avoid all unnecessary copies (it is often needed to rewrite the containers),
- Due to combinatorial explosion, sets often don’t fit in memory or disk and are
enumerated on the fly.

Here are some examples,
the speedup is in comparison to an implementation without vector instructions:

Operation |   Speedup
----------|-----------
Inverting a permutation | 1.28
Sorting a list of bytes | 21.3
Number of cycles of a permutation |  41.5
Number of inversions of a permutation  | 9.39
Cycle type of a permutation | 8.94



\section sec_tips Tips to the user

Note that memory access can become a problem.
If your algorithm stores many things, most of the time will be spent in fetching
from RAM, not computing. The data structures your client code uses should
preserve locality. You might want to compute some stats on data structure usage
(eg avg size of buckets used, lengths of lists, lifetime of objects, etc.)
and write custom data structure optimized for your usage profile.

This lib is implemented with speed in mind, not code safety.
Eg. there are no checks when building a permutation, which could be invalid
(like non injective).

We suggest having a look, in the menus above, at Classes → [Class
list](annotated.html), esp. at the classes HPCombi::Perm16 and HPCombi::BMat8.

\section Parallelism
There is no parallelisation here. To use parallelism with this lib, see for
instance:
- Florent Hivert, High Performance Computing Experiments in Enumerative and
Algebraic Combinatorics
([pdf](https://plouffe.fr/OEIS/citations/3115936.3115938.pdf),
[DOI](https://dx.doi.org/10.1145/3115936.3115938)).
- [OpenCilk](https://github.com/OpenCilk/) or look for another work stealing
framework.

Cilk is based on C++ and essentially adds the keywords `spawn` and `sync` to
ease parallelism. Intel decided not to maintain Cilk anymore so it is
deprecated. [OpencilK](https://github.com/OpenCilk/) is an open source project
to continue it.

We tested OpenMP and it was 2 orders of magnitude slower.

OpencilK adds the keyword `spawn`,
which adds a special tag to the stack and launches a recursive call.
If a thread finishes its work, it will look at other threads' stacks and steal
their work. The value of Cilk is that recursive calls cost only 4 or 5 times
more, much faster than launching true threads (which would take 6-7 orders of
magnitude more time to create, measured in μs).

OpencilK provides some primitives for concurrent access to data.
It guarantees the semantics of serial execution.

*/

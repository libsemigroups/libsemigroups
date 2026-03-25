# HPCombi

High Performance Combinatorics in C++ using vector instructions v1.1.2

HPCombi is a C++17 header-only library using the SSE and AVX instruction sets,
and some equivalents, for very fast manipulation of small combinatorial objects
such as transformations, permutations, and boolean matrices. HPCombi implements
new algorithms and benchmarks them on various compilers and architectures.

HPCombi was initially designed using the SSE and AVX instruction sets, and did
not work on machines without these instructions (such as ARM). From v1
HPCombi supports processors with other instruction sets also, via
[SIMD Everywhere][]. It might be the case that the greatest performance gains
are achieved on processors supporting the SSE and AVX instruction sets, but the
HPCombi benchmarks indicate that there are also still significant gains on
other processors too.

<!-- TODO add link to HPCombi wiki with benchmark graphs -->

## Authors

- Florent Hivert <florent.hivert@lisn.fr>
- James Mitchell <jdm3@st-andrews.ac.uk>

## Contributors

- Victorin Brunel <victorin.brunel@universite-paris-saclay.fr> : wrote the
  BMat16 code
- Reinis Cirpons <rc234@st-andrews.ac.uk> : CI
- Jean-Baptiste Rouquier <jrouquie@gmail.com> : improvements to the doc
- Joe Edwards <jde1@st-andrews.ac.uk> : improvements to the CI
- Finn Smith <fls3@st-andrews.ac.uk> : discussions + BMat8 reference code
- Viviane Pons <viviane.pons@lri.fr> : algorithms discussions
- Daniel Vanzo <daniel.vanzo@lri.fr> : GPU experiments

## Documentation

- See the [Doxygen generated doc](https://libsemigroups.github.io/HPCombi/)

## Thanks

- The development of HPCombi was partly funded by the [OpenDreamKit][] Horizon
  2020 European Research Infrastructure project (#676541), which the authors
  acknowledge with thanks.
- Thanks also to the [SIMD everywhere][] and [catch2][] authors and
  contributors for their excellent libraries!

[SIMD everywhere]: https://github.com/simd-everywhere/simde
[OpenDreamKit]: https://opendreamkit.org/
[catch2]: https://github.com/catchorg/Catch2

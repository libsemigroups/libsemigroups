# Building HPCombi

Note that HPCombi is a C++17 header-only library, and as such does not need to
be built. The instructions below are only for those who wish to either run the
tests, experiments, examples, or benchmarks.

## Build prerequisites:

- CMake 3.8 or later

- A recent C++ compiler implementing the C++17 standard. We routinely test
  HPCombi using:
  * gcc 9 to 12; and
  * clang 11 to 15
  on both x86 and arm processors.

- Your machine must support a small number of builtin functions such as `__builtin_popcnt`.

- [optional] : Google `sparsehash/dense_hash_map` and/or `sparsehash/dense_hash_set`.

- [optional] Doxygen for generating the API documentation (in [build/doc/html/index.html](build/doc/html/index.html)).

## Building

Using Make:

    mkdir build
    cd build
    cmake ..
    make

If you want to build the tests:

    mkdir build
    cd build
    cmake -DBUILD_TESTING=1 ..
    make
    make test

By default, cmake compile in debug mode (no optimisation, assert are on). To
compile in release mode:

    cmake -DCMAKE_BUILD_TYPE=Release ..

As a consequence for benchmark you write 

    cmake -DBUILD_TESTING=1 -DCMAKE_BUILD_TYPE=Release ..

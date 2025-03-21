# Installation

## Installing with conda

This installation method assumes that you have a working version of `conda`.
See the [getting started](https://docs.conda.io/projects/conda/en/latest/user-guide/getting-started.html)
on the [conda](https://conda.io/) website.

Activate the [conda-forge](https://conda-forge.org/) package
repository:

    conda config --add channels conda-forge

Install `libsemigroups`:

    conda install libsemigroups

## From the sources

Building `libsemigroups` from the source files requires a C++ compiler
supporting the C++17 standard, `autoconf`, and `automake`. Building the
documentation from source has some additional requirements that are
detailed in the section [Building the documentation](index.md#building-the-documentation).

### From the github repo

To build `libsemigroups` from the github repository:

    git clone https://github.com/libsemigroups/libsemigroups
    cd libsemigroups
    ./autogen.sh && ./configure && make -j8 && sudo make install

### From a release archive

To build `libsemigroups` from a release archive:

    curl -L -O https://github.com/libsemigroups/libsemigroups/releases/latest/download/libsemigroups-3.0.0.tar.gz
    tar -xf libsemigroups-3.0.0.tar.gz
    rm -f libsemigroups-3.0.0.tar.gz
    cd libsemigroups-3.0.0
    ./configure && make -j8 && sudo make install

### Docker

If you have [Docker](https://www.docker.com) installed, you can download an
[x86 docker image](https://hub.docker.com/repository/docker/libsemigroups/libsemigroups-docker)
for `libsemigroups` as follows:

    docker pull libsemigroups/libsemigroups-docker

or an [arm docker image](https://hub.docker.com/repository/docker/libsemigroups/libsemigroups-docker-arm)
as follows

    docker pull libsemigroups/libsemigroups-docker-arm

and run it by doing

    docker run --rm -it libsemigroups/libsemigroups-docker

or

    docker run --rm -it libsemigroups/libsemigroups-docker-arm

If you want to use a specific version of `libsemigroups`, such as 1.0.9, then
use:

    docker pull libsemigroups/libsemigroups-docker:version-1.0.9
    docker run --rm -it libsemigroups/libsemigroups-docker:version-1.0.9

or, for the latest version, use:

    docker pull libsemigroups/libsemigroups-docker:latest
    docker run --rm -it libsemigroups/libsemigroups-docker:latest

## Configuration options

In addition to the usual `autoconf` configuration options, the following
configuration options are available for `libsemigroups`:

| Option                     | Description                                        |
| -------------------------- | -------------------------------------------------- |
| \--enable-code-coverage    | enable code coverage support (default=no)          |
| \--enable-compile-warnings | enable compiler warnings (default=no)              |
| \--enable-debug            | enable debug mode (default=no)                     |
| \--enable-eigen            | enable `eigen` (default=yes)                       |
| \--enable-hpcombi          | enable `HPCombi` (default=yes)                     |
| \--with-external-fmt       | do not use the included copy of fmt (default=no)   |
| \--with-external-eigen     | do not use the included copy of eigen (default=no) |
| \--disable-popcnt          | do not use \_\_builtin_popcountl (default=yes)     |
| \--disable-clzll           | do not use \_\_builtin_clzll (default=yes)         |

Debug mode significantly degrades the performance of `libsemigroups`. Note that
the flags `--enable-eigen` and `--with-external-eigen` are independent of each
other, and so both flags should be included to enable `eigen` and use an
external `eigen`.

## Make install

By default, `make install` installs the library and its `pkg-config`
configuration into `/usr/local`. One can also specify another install
location, say, `/foo/bar`, by calling `./configure` with parameter
`--prefix`, e.g. `./configure --prefix=/foo/bar`. This might be useful
if `sudo` is not available.

If you have `pkg-config` installed, it may be called to get
`libsemigroups`'s version, location, etc (see its docs for details; this
facility is used in GAP package Semigroups, which may be configured to
use the external `libsemigroups`). For example,

    pkg-config --modversion libsemigroups

will print the version of the installed `libsemigroups`, provided it is
scanning configurations in the install location. For installation
location unknown to `pkg-config`, one needs to add it to
`PKG_CONFIG_PATH` (an environment variable). E.g. if it is `/foo/bar` as
above then

    PKG_CONFIG_PATH=/foo/bar/lib/pkgconfig pkg-config --modversion libsemigroups

will print the version of the installed `libsemigroups`. (As usual,
`PKG_CONFIG_PATH` may be exported, added to shell configuration, etc.).

## Building the documentation

To build the documentation, a version of `doxygen` is required. Instructions on
how to do this can be found on Doxygen's
[Installation page](https://www.doxygen.nl/manual/install.html).

Then, it ought to be possible to just run `make doc` in the
`libsemigroups` directory, and the documentation will be generated.

## Issues

If you find any problems with `libsemigroups`, or have any suggestions
for features that you'd like to see, please use the
[issue tracker](https://github.com/libsemigroups/libsemigroups/issues).

.. |libsemigroups-version| replace:: 2.2.1

.. _Installation:

Installation
============

Installing with conda
---------------------

This installation method assumes that you have anaconda or miniconda installed.
See the `getting started`_ and `miniconda download page`_ on the conda_
website.

.. _getting started: http://bit.ly/33B0Vfs
.. _miniconda download page: https://conda.io/miniconda.html
.. _conda: https://conda.io/

Activate the `conda-forge <https://conda-forge.github.io/>`__ package
repository:

::

   conda config --add channels conda-forge

Install ``libsemigroups``:

::

   conda install libsemigroups

From the sources
----------------

Building ``libsemigroups`` from the source files requires a C++ compiler
supporting the C++14 standard, ``autoconf``, and ``automake``.
Building the documentation from source has some additional requirements that
are detailed `here <Building the documentation from source>`_.

From the github repo
~~~~~~~~~~~~~~~~~~~~

To build ``libsemigroups`` from the github repository:

::

   git clone https://github.com/libsemigroups/libsemigroups
   cd libsemigroups
   ./autogen.sh && ./configure && make -j8 && sudo make install

From a release archive
~~~~~~~~~~~~~~~~~~~~~~

To build ``libsemigroups`` from a release archive:

.. parsed-literal::

   curl -L -O https://github.com/libsemigroups/libsemigroups/releases/latest/download/libsemigroups-|libsemigroups-version|.tar.gz
   tar -xf libsemigroups-|libsemigroups-version|.tar.gz
   rm -f libsemigroups-|libsemigroups-version|.tar.gz
   cd libsemigroups-|libsemigroups-version|
   ./configure && make -j8 && sudo make install

Docker
~~~~~~

If you have Docker_ installed, you can download an `x86 docker image`_ for
``libsemigroups`` as follows:

.. parsed-literal::
   docker pull libsemigroups/libsemigroups-docker

or an `arm docker image`_ as follows

.. parsed-literal::
   docker pull libsemigroups/libsemigroups-docker-arm

and run it by doing

.. parsed-literal::
   docker run --rm -it libsemigroups/libsemigroups-docker
   docker run --rm -it libsemigroups/libsemigroups-docker-arm

If you want to use a specific version of ``libsemigroups``, then use:

.. parsed-literal::
   docker pull libsemigroups/libsemigroups-docker:version-1.0.9
   docker run --rm -it libsemigroups/libsemigroups-docker:version-1.0.9

or, for the latest version, use:

.. parsed-literal::
  docker pull libsemigroups/libsemigroups-docker:latest
  docker run --rm -it libsemigroups/libsemigroups-docker:latest

.. _Docker: https://www.docker.com
.. _x86 docker image: https://hub.docker.com/repository/docker/libsemigroups/libsemigroups-docker
.. _arm docker image: https://hub.docker.com/repository/docker/libsemigroups/libsemigroups-docker-arm

Configuration options
---------------------

In addition to the usual ``autoconf`` configuration options, the following
configuration options are available for ``libsemigroups``:

==========================  ==================================================
Option
--------------------------  --------------------------------------------------
--enable-code-coverage      enable code coverage support (default=no)
--enable-compile-warnings   enable compiler warnings (default=no)
--enable-debug              enable debug mode (default=no)
--enable-eigen              enable ``eigen`` (default=yes)
--enable-fmt                enable fmt (default=no)
--enable-hpcombi            enable ``HPCombi`` (default=yes)
--enable-stats              enable statistics mode in :cpp:any:`ToddCoxeter` (default=yes)
--enable-verbose            enable verbose mode (default=no)
--with-external-fmt         do not use the included copy of fmt (default=no)
--with-external-eigen       do not use the included copy of eigen (default=no)
--disable-popcnt            do not use __builtin_popcountl (default=yes)
--disable-clzll             do not use __builtin_ctzll (default=yes)
==========================  ==================================================

Debug mode and verbose mode significantly degrade the performance of
``libsemigroups``. Compiling with ``fmt`` enabled increases build times
significantly. Note that the flags ``--enable-fmt`` and ``--with-external-fmt``
are indendent of each other, and so both flags should be included to enable
``fmt`` and use an external ``fmt``.

Make install
------------

By default, ``make install`` installs the library and its ``pkg-config``
configuration into ``/usr/local``. One can also specify another install
location, say, ``/foo/bar``, by calling ``./configure`` with parameter
``--prefix``, e.g. \ ``./configure --prefix=/foo/bar``. This might be
useful if ``sudo`` is not available.

If you have ``pkg-config`` installed, it may be called to get
``libsemigroups``\ ’s version, location, etc (see its docs for details;
this facility is used in GAP package Semigroups, which may be configured
to use the external ``libsemigroups``). For example,

::

   pkg-config --modversion libsemigroups

will print the version of the installed ``libsemigroups``, provided it is
scanning configurations in the install location. For installation
location unknown to ``pkg-config``, one needs to add it to
``PKG_CONFIG_PATH`` (an environment variable). E.g. if it is
``/foo/bar`` as above then

::

   PKG_CONFIG_PATH=/foo/bar/lib/pkgconfig pkg-config --modversion libsemigroups

will print the version of the installed ``libsemigroups``. (As usual,
``PKG_CONFIG_PATH`` may be exported, added to shell configuration, etc.)

Building the documentation
--------------------------

The following are required to be able to build the documentation:

1. ``python3``
2. ``doxygen``
3. the python packages: ``sphinx bs4 lxml breathe pyyaml sphinx_rtd_theme sphinx_copybutton sphinxcontrib-bibtex``

Assuming you already have ``python3`` install, on Mac OSX you can install all of
the above by doing:

::

    brew install doxygen sphinx
    pip3 install -r docs/requirements

.. TODO add ubuntu instructions

Then it ought to be possible to just run ``make doc`` in the ``libsemigroups``
directory.

Issues
------

If you find any problems with ``libsemigroups``, or have any suggestions for
features that you’d like to see, please use the `issue
tracker <https://github.com/libsemigroups/libsemigroups/issues>`__.

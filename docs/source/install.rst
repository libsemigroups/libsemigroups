.. |libsemigroups-version| replace:: 1.0.5

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
supporting the C++11 standard, ``autoconf``, and ``automake``.
Building the documentation from source has some additional requirements that
are detailed `here <Building the documentation from source>`_.

From the github repo
~~~~~~~~~~~~~~~~~~~~

To build ``libsemigroups`` from the github repository:

::

   git clone https://github.com/libsemigroups/libsemigroups
   cd libsemigroups/extern
   curl -L -O https://github.com/fmtlib/fmt/archive/5.3.0.tar.gz
   tar -xzf 5.3.0.tar.gz && rm -f 5.3.0.tar.gz && cd ..

Optionally, if you want to use ``HPCombi``, you should also do:

::

   cd extern
   curl -L -O https://github.com/hivert/HPCombi/archive/v0.0.3.zip
   tar -xzf v0.0.3.zip && rm -f v0.0.3.zip && cd ..

Then actually build ``libsemigroups`` by doing:

::

   ./autogen.sh && ./configure && make -j8 && sudo make install

.. A bash script is available: TODO the script is etc/install-libsemigroups.sh

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

If you have Docker_ installed, you can download a `docker container`_ for
``libsemigroups`` as follows:

.. parsed-literal::
   docker pull libsemigroups/libsemigroups

and run it by doing

.. parsed-literal::
   docker run --rm -it libsemigroups/libsemigroups

.. _Docker: https://www.docker.com
.. _docker container: https://hub.docker.com/repository/docker/libsemigroups/libsemigroups

Configuration options
---------------------

In addition to the usual ``autoconf`` configuration options, the following
configuration options are available for ``libsemigroups``:

==========================  ===================================
Option 
--------------------------  -----------------------------------
--enable-code-coverage      enable code coverage support
--enable-compile-warnings   enable compiler warnings
--enable-debug              enable debug mode
--enable-hpcombi            enable ``HPCombi``
--enable-verbose            enable verbose mode
--with-external-fmt         do not use the included copy of fmt
==========================  ===================================

Debug mode and verbose mode significantly degrade the performance of
``libsemigroups``.

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

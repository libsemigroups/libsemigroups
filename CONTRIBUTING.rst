Information for developers
==========================

Building the documentation from source
--------------------------------------

If you are checking out the ``libsemigroups`` git repo, then the following steps
are required to build the documentation:

1. Install doxygen (using homebrew or apt-get)
2. Check out the doxygen-awesome-css/ submodule of ``libsemigroups`` using:

.. code-block:: console

  git submodule update --init --recursive

3. Type ``make doc``

Naming conventions
------------------

====================  ============================
Object                Convention
--------------------  ----------------------------
class                 NamedLikeThis
struct                NamedLikeThis
function              named\_like\_this
enum class            namedlikethis
enum class member     named_like_this
stateful type alias   named\_like\_this\_type
stateless type alias  LikeThis
iterator              iterator or const\_iterator
global variable       NAMED\_LIKE\_THIS
====================  ============================

Try to ensure that any new functionality added to ``libsemigroups`` satisfies
the strong exception guarantee, and if not that this is documented.

Some more conventions:

* If a function does not check its arguments (including ``*this`` for member
  functions), then it must have the suffix ``_no_checks`` if there is any
  possibility that the function will fail. This indicates that anything may
  happen if the function is called on a non-valid object or for non-valid
  arguments.

  In the python bindings ``libsemigroups_pybind11`` it should not be possible
  to construct or modify an object so that it is invalid. For example, it
  shouldn't be possible to create an invalid ``Bipartition`` or to modify a
  valid ``Bipartition`` so that it is invalid. So, it is safe to call the
  ``Bipartition::rank_no_checks`` member function as if it was the
  ``Bipartition::rank`` function. Hence in the python bindings we bind a
  functions called ``Bipartition.rank`` to the C++ function
  ``Bipartition::rank_no_checks``.

* All class helper functions (i.e. those free functions taking a class as an
  argument, and using public member functions of that class) should be in a
  helper namespace with the same name as the class (but in lower case). I.e.
  ``bipartition::one``, or ``ptransf::one``. Exceptions are:

  - operators such as ``operator*`` etc. This is because these are more or less
    difficult to use if they are in a nested namespace.
  - functions that are (or should be) implemented for all of the public facing
    classes in ``libsemigroups``, such as, for example
    ``to_human_readable_repr``

* Functions that construct an object must be of one of three types;
  constructors, ``make`` functions, or ``to`` functions:

  * A constructor should be used to construct an object without checking its
    arguments.
  * A ``make`` function should be a free function template in the libsemigroups
    namespace that is used to construct an object from non-libsemigroups
    objects such as containers, integers and strings. It should check its
    arguments.
  * A ``to`` function should a free function template in the libsemigroups
    namespace that is used to convert from one ``libsemigroups`` type to
    another. A typical signature might look something like
    ``to<ToddCoxeter>(Presentation<word_type> p)``. It should check its
    arguments.

* The documentation for each function may contain the following section
  indicators in the following order:

  #. ``\tparam``
  #. ``\param``
  #. ``\return``
  #. ``\exceptions`` or ``\throws``
  #. ``\complexity``
  #. ``\note``
  #. ``\warning``
  #. ``\sa``
  
  This can be checked by running ``etc/check_doc_order.py``.

Debugging and valgrinding
-------------------------

To run ``lldb``:

.. code-block:: console

    ./configure --enable-debug && make test_all
    libtool --mode=execute lldb test_all

``test_all`` is the name of the check program produced by ``make check``.
Similarly to run valgrind you have to do:

.. code-block:: console

    ./configure --enable-debug --disable-hpcombi && make test_all
    libtool --mode=execute valgrind --leak-check=full ./test_all [quick] 2>&1 | tee --append valgrind.txt

Adding new test cases
---------------------

Any new tests should be tagged with one of the following:

========  =======
Tag       Runtime
--------  -------
quick     < 200ms
standard  < 3s
extreme   > 3s
========  =======

They should be declared using

.. code-block:: cpp

    LIBSEMIGROUPS_TEST_CASE(classname, number, message, tags)

"tags" should include "[FilePrefix]" where "FilePrefix" would be
"cong-pair" in the file "tests/cong-pair.test.cc", if the file prefix is the
same as "classname", then it should not be included. Tags are case insensitive.

Making a release
----------------

A ***bugfix release*** is one of the form ``x.y.z -> x.y.z+1``, and
a ***non-bugfix release*** is one of the form ``x.y.z -> x+1.y.z`` or
``x.y+1.z``.

Tools for developers
--------------------

The following tools may be useful for developers of ``libsemigroups``. Their use
is explained in the following sections, and all of them cen be installed in a
conda/mamba environment by running

.. code-block:: console

  source etc/make-dev-environment.sh

This script requires a conda-flavoured package manager. A good choice is
``mamba``, and instructions on how to install it can be found on the
`mamba installation page <https://mamba.readthedocs.io/en/latest/installation/mamba-installation.html>`_.

``clang-format``
~~~~~~~~~~~~~~~~

The codebase of ``libsemigroups`` is formatted using using version ``15`` of
`clang-format <https://releases.llvm.org/15.0.0/tools/clang/docs/ClangFormat.html>`_,
and adherence to this standard is checked as part of all pull requests.

To install this locally on a Debian-like system with root access, you may be
able to run:

.. code-block:: console

  apt install clang-format-15

In some cases, it may be desirable to disable automatic formatting; for example,
if you want to use two lines to construct a vector that represents a 2x2 matrix,
even though it would fit on one line. Automatic formatting can be disabled using
the comment

.. code-block:: cpp

  // clang-format off

and then, importantly, re-enabled using the comment

.. code-block:: cpp

  // clang-format on

This should be done sparingly and with good reason.

``cpplint``
~~~~~~~~~~~

As well as checking formatting, the codebase of ``libsemigroups`` is linted
using `cpplint <https://github.com/cpplint/cpplint>`_ as part of every pull
request. Instructions on how to install locally can be found on their
`installation page <https://github.com/cpplint/cpplint?tab=readme-ov-file#installation>`_.

Certain lines can be spared from the linter in the following way

.. code-block:: cpp

  void some_offending_line_of_code(); // NOLINT(category)

  // NOLINTNEXTLINE(category)
  class SomeOtherOffendingLineOfCode;

  // NOLINTBEGIN(category)
  auto some_offending_thing({
    "that", "spans",
    "several", "lines"
  });
  // NOLINTEND

where ``category`` is the type of linter error to ignore. The names of these
categories can be found in the
`source code <https://github.com/cpplint/cpplint/blob/e50a4ae01985273a1b15efd1d4540f764c878976/cpplint.py#L300>`_,
but it is probably easier to try linting, and then quote the error that is spat
out.

As with ``clang-format``, disabling the linter should be done sparingly and with
good reason.

``codespell``
~~~~~~~~~~~~~

As part of every pull request, the codebase of ``libsemigroups`` is checked for
spelling mistakes using
`codespell <https://github.com/codespell-project/codespell>`_. Instructions on how
to install this locally can be found on their `installation page <https://github.com/codespell-project/codespell?tab=readme-ov-file#installation>`_.

Sometimes, false positives may get thrown, especially when writing maths or
names. Therefore, ``codespell`` can be disabled using

.. code-block:: cpp

  // codespell:begin-ignore

and then re-enabled using

.. code-block:: cpp

  // codespell:end-ignore

If the issue is more widespread than a few lines of code, then the offending
word(s) can be added to the ``ignore-words-list`` of the ``.codespellrc`` file.

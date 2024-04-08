Information for developers
==========================

Building the documentation from source
--------------------------------------

If you are checking out the libsemigroups git repo, then the following steps
are required to build the documentation:

1. Install doxygen (using homebrew or apt-get)
2. Check out the doxygen-awesome-css/ submodule using:

   git submodule update --init --recursive

3. Type `make doc`

See the `manual <https://libsemigroups.readthedocs.io/en/latest/install.html>`_
for details of how to do this.

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

Debugging and valgrinding
-------------------------

To run `lldb`: 

.. code-block:: bash

    ./configure --enable-debug && make test_all 
    libtool --mode=execute lldb test_all 

`test_all` is the name of the check program produced by `make check`. Similarly
to run valgrind you have to do:

.. code-block:: bash

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

A ***bugfix release*** is one of the form `x.y.z -> x.y.z+1`, and                
a ***non-bugfix release*** is one of the form `x.y.z -> x+1.y.z` or `x.y+1.z`. 

Use the script `etc/release-libsemigroups.py`.

References
----------

- <https://conda.io/docs/build_tutorials.html>
- <https://github.com/conda/conda-recipes>

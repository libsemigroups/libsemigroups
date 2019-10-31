.. Copyright (c) 2019, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

Constants
=========

``libsemigroups`` contains certain constant values, which are described in this
section.  The values below correspond to integer values (in some sense).  All
of the constants in this section can be implicitly converted to any integral
type, and have a value specified by 

.. code-block:: cpp 

    std::numeric_limits<T>::max/min()

minus a compile-time offset (where :code:`T` is a member function template, and
can hence be any integral type), and we overload :code:`operator==`,
:code:`operator!=`, :code:`operator<`, and :code:`operator>` the constants
defined below and for certain integral type. So, the constants can be used as
if it was an integral type (say, :code:`-1`, although the precise value is not
defined), for the purposes of storing it in a data member and for comparisons.
This allows us to avoid duplicating code for constants like
:cpp:any:`libsemigroups::UNDEFINED`, and makes
:cpp:any:`libsemigroups::UNDEFINED` more straightforward to use.

This approach also has some drawbacks, if, for example,
:cpp:any:`libsemigroups::UNDEFINED` is implicitly converted to two different
integral types and then these are compared, this will yield :code:`false`, and
so :code:`==` of :cpp:any:`libsemigroups::UNDEFINED` and itself is not
transitive, so beware.

.. code-block:: cpp 

    using namespace libsemigroups;
    UNDEFINED != 0;                        // true
    POSITIVE_INFINITY > 0;                 // true
    POSITIVE_INFINITY > NEGATIVE_INFINITY; // true

.. doxygenvariable:: libsemigroups::LIMIT_MAX
   :project: libsemigroups

.. doxygenvariable:: libsemigroups::UNDEFINED
   :project: libsemigroups

.. doxygenvariable:: libsemigroups::NEGATIVE_INFINITY
   :project: libsemigroups

.. doxygenvariable:: libsemigroups::POSITIVE_INFINITY
   :project: libsemigroups

.. doxygenvariable:: libsemigroups::FOREVER
   :project: libsemigroups

.. Copyright (c) 2019, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

Actions
=======

.. cpp:namespace:: libsemigroups

This page contains an overview summary of the functionality in
``libsemigroups`` for finding actions of semigroups, or groups, on sets.  The
notion of an "action" in the context of ``libsemigroups`` is analogous to the
notion of an orbit of a group.  

You are unlikely to want to use :cpp:class:`Action` directly, but rather via
the more convenient aliases :cpp:any:`RightAction` and :cpp:any:`LeftAction`.
To use :cpp:any:`RightAction` and :cpp:any:`LeftAction` with custom types,
actions, and so on, see :cpp:class:`ActionTraits`.
See also :cpp:any:`ImageLeftAction` and :cpp:any:`ImageRightAction`.

.. code-block:: cpp 

    using namespace libsemigroups;
    RightAction<PPerm<16>, PPerm<16>, ImageRightAction<PPerm<16>, PPerm<16>>> o;
    o.add_seed(PPerm<16>::identity(16));
    o.add_generator(
        PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0},
                  16));
    o.add_generator(
        PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  {1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  16));
    o.add_generator(
        PPerm<16>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
                  16));
    o.add_generator(
        PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
                  {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  16));
    o.reserve(70000);
    o.size(); // returns 65536


The classes in ``libsemigroups`` for actions are:

.. toctree::
   :maxdepth: 1

   _generated/libsemigroups__action.rst
   _generated/libsemigroups__actiontraits
   api/left-action
   api/right-action
   _generated/libsemigroups__side.rst

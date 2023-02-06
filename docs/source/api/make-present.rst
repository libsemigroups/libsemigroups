.. Copyright (c) 2022, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

Making presentations from other objects
=======================================

Defined in ``make-present.hpp``.

This page describes the functionality for creating semigroup and
monoid presentations in ``libsemigroups`` from other types of objects.

.. doxygenfunction:: make(FroidurePinBase&)
   :project: libsemigroups

.. doxygenfunction:: make(FroidurePinBase &, std::string const &)
   :project: libsemigroups

.. doxygenfunction:: make(Presentation<W> const &, F&&)
   :project: libsemigroups

.. cpp:function:: template <typename S, typename W> \
   S make(Presentation<W> const & p)

   Make a presentation from a different type of presentation.

   Returns a presentation equivalent to the input presentation but of a
   different type (for example, can be used to convert from ``std::string`` to
   ``word_type``).

   :tparam S:
     the type of the returned presentation, must be a type of
     :cpp:any:`Presentation`
   :tparam W: the type of the words in the input presentation
   :param p: the input presentation

   :returns: A value of type ``S``.
   :throws LibsemigroupsException: if ``p.validate()`` throws.

.. doxygenfunction:: make(Presentation<W> const &, std::string const &)
   :project: libsemigroups

.. Copyright (c) 2021, J. D. Mitchell
                       T. D. Conti-Leslie
                       M. T. Whyte
                       R. Cirpons

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

Free Bands
===========

.. cpp:namespace:: libsemigroups

``libsemigroups`` implements algorithms for computing in free bands.
See :cite:`Radoszewski2010aa` for more details.

The functions in ``libsemigroups`` for free bands are:

.. cpp:namespace-pop::

.. doxygenfunction:: libsemigroups::freeband_equal_to(word_type&&, word_type&&)
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::freeband_equal_to(T, T)
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::freeband_equal_to(T, T, T, T)
   :project: libsemigroups

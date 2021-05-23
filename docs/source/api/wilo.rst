.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

Words in lexicographic order (wilo)
===================================

The functions :cpp:any:`libsemigroups::cbegin_wilo` and
:cpp:any:`libsemigroups::cend_wilo` can be used to iterate through words in
lexicographic order in some range. 

.. doxygenfunction:: libsemigroups::cbegin_wilo(size_t, size_t, word_type&&, word_type&&)
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::cend_wilo(size_t, size_t, word_type&&, word_type&&)
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::cbegin_wilo(size_t, size_t, word_type const&, word_type const&)
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::cend_wilo(size_t, size_t, word_type const&, word_type const&)
   :project: libsemigroups

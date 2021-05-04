.. Copyright (c) 2019-20, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _dynamicrowview_comparison_ops:

Comparison operators
====================

.. _dynamicrowview_operator_equals:
.. cpp:function:: template<typename U> \
                  bool operator==(U const& that) const

   Equality operator.

   :tparam U: either :cpp:any:`Row` or :cpp:any:`RowView`

   :param that: :cpp:any:`Row` or :cpp:any:`RowView` object for comparison.
  
   :returns:
     Returns ``true`` if the first :cpp:any:`size()` entries of ``that`` equal
     the entries of ``this``, and ``false`` if not.

   :complexity:
     At worst :math:`O(n)` where :math:`n` is the size of the row view.


.. _dynamicrowview_operator_not_equals:
.. cpp:function:: template<typename U> \
                  bool operator!=(U const& that) const

   Inequality operator.

   :tparam U: either :cpp:any:`Row` or :cpp:any:`RowView`

   :param that: :cpp:any:`Row` or :cpp:any:`RowView` object for comparison.
  
   :returns:
     Returns ``true`` if the first :cpp:any:`size()` entries of ``that`` do not
     equal the entries of ``this``, and ``false`` otherwise.

   :complexity:
     At worst :math:`O(n)` where :math:`n` is the size of the row view.


.. _dynamicrowview_operator_less:
.. cpp:function:: template<typename U> \
                  bool operator<(U const& that) const

   Lexicographical comparison of rows.

   :tparam U: either :cpp:any:`Row` or :cpp:any:`RowView`

   :param that: :cpp:any:`Row` or :cpp:any:`RowView` object for comparison.
  
   :returns:
     Returns ``true`` if ``this`` is lex less than that ``that`` and ``false``
     otherwise.

   :complexity:
     At worst :math:`O(n)` where :math:`n` is the size of the row view.


.. _dynamicrowview_operator_more:
.. cpp:function:: template<typename U> \
                  bool operator>(U const& that) const

   Lexicographical comparison of rows.

   :tparam U: either :cpp:any:`Row` or :cpp:any:`RowView`

   :param that: :cpp:any:`Row` or :cpp:any:`RowView` object for comparison.
  
   :returns:
     Returns ``true`` if ``this`` is lex greater than that ``that`` and
     ``false`` otherwise.

   :complexity:
     At worst :math:`O(n)` where :math:`n` is the size of the row view.


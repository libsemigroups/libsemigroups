.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _dynamicrowview_constructors:

Constructors
============

This page contains information about the constructors for the
:cpp:any:`DynamicRowView` class.

Note that unless copying an existing ``DynamicRowView`` it's most likely that
you will obtain a ``DynamicRowView`` from :cpp:any:`DynamicMatrix::row` or
:cpp:any:`DynamicMatrix::rows`.


.. cpp:namespace:: libsemigroups

Default constructors
--------------------

.. cpp:function:: DynamicRowView() = default
   
   Default constructor.

.. cpp:function:: DynamicRowView(DynamicRowView const&) = default
   
   Default copy constructor.

.. cpp:function:: DynamicRowView(DynamicRowView&&) = default
   
   Default move constructor.

.. cpp:function:: DynamicRowView& operator=(DynamicRowView const&) = default

   Default copy assignment operator.

.. cpp:function:: DynamicRowView& operator=(DynamicRowView&&) = default

   Default move assignment operator.

Constructor from Row
--------------------

.. cpp:function:: explicit DynamicRowView(Row const& r)

   Construct a row view from a Row.
   
   :param r: the row.

   :exceptions: 
     This function guarantees not to throw a :cpp:any:`LibsemigroupsException`. 
    
   :complexity: 
     Constant.

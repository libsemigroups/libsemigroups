.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _staticrowview_constructors:

StaticRowView
=============

.. cpp:namespace:: libsemigroups::StaticRowView<PlusOp, ProdOp, ZeroOp, OneOp, C, Scalar>

This page contains information about the constructors for the
:cpp:any:`StaticRowView` class.

Note that unless copying an existing ``StaticRowView`` it's most likely that
you will obtain a ``StaticRowView`` from :cpp:any:`StaticMatrix::row` or
:cpp:any:`StaticMatrix::rows`.

Default constructors
--------------------

.. cpp:function:: StaticRowView() = default
   
   Default constructor.

.. cpp:function:: StaticRowView(StaticRowView const&) = default
   
   Default copy constructor.

.. cpp:function:: StaticRowView(StaticRowView&&) = default
   
   Default move constructor.

.. cpp:function:: StaticRowView& operator=(StaticRowView const&) = default

   Default copy assignment operator.

.. cpp:function:: StaticRowView& operator=(StaticRowView&&) = default

   Default move assignment operator.

Constructor from Row
--------------------

.. cpp:function:: explicit StaticRowView(Row const& r)

   Construct a row view from a Row.
   
   :param r: the row.

   :exceptions: 
     This function guarantees not to throw a :cpp:any:`LibsemigroupsException`. 
    
   :complexity: 
     Constant.

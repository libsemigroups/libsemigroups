.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

Helper functions for ActionDigraph
==================================

Overview
--------

Defined in ``action-digraph-helper.hpp``.

This page contains the documentation for helper function for the class
:cpp:type:`libsemigroups::ActionDigraph`.

Full API
--------

.. doxygenfunction:: libsemigroups::operator<<(std::ostream&, ActionDigraph<T> const&)
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::action_digraph_helper::follow_path
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::action_digraph_helper::follow_path_nc(ActionDigraph<T> const&, node_type<T> const, word_type const&) noexcept
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::action_digraph_helper::follow_path_nc(ActionDigraph<T> const&, node_type<T> const, S, S) noexcept
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::action_digraph_helper::last_node_on_path_nc
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::action_digraph_helper::is_acyclic(ActionDigraph<T> const&)
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::action_digraph_helper::is_acyclic(ActionDigraph<T> const&, node_type<T>)
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::action_digraph_helper::is_reachable
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::action_digraph_helper::topological_sort(ActionDigraph<T> const&)
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::action_digraph_helper::topological_sort(ActionDigraph<T> const&, node_type<T>)
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::action_digraph_helper::add_cycle(ActionDigraph<T>&, U, U)
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::action_digraph_helper::add_cycle(ActionDigraph<T>&, size_t)
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::action_digraph_helper::make(size_t, std::initializer_list<std::initializer_list<T>>)
   :project: libsemigroups

.. doxygenfunction:: libsemigroups::action_digraph_helper::is_connected(ActionDigraph<T> const&)
   :project: libsemigroups

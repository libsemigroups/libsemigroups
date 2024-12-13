//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 James D. Mitchell
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

// This file contains the implementation for a class to manage nodes for a
// ToddCoxeterDigraph instance.

////////////////////////////////////////////////////////////////////////////////
//
// We use these two vectors to implement a doubly-linked list of nodes. There
// are two types of node, those that are "active" and those that are "free".
//
// If c is a node, then
//   * _forwd[c] is the node that comes after c in the list,
//     _forwd[the last node in the list] = UNDEFINED
//
//   * _bckwd[c] is the node that comes before c in the list,
//     _bckwd[_id_node] = _id_node
//
// If c is an active node, then _ident[c] = c.
//
// If c is a free node, then _ident[c] != c.
//
// We also store some special locations in the list:
//
//   * _id_node:  the first node, this never changes.
//
//   * _current:   is the node which we are currently using for something in a
//     loop somewhere, the member functions of this class guarantee that
//     _current always points to an active node, even if the value changes
//     during a function call.
//
//   * _current_la: is similar to _current, and can be used independently of
//     _current.
//
//   * _last_active_node points to the final active node in the list.
//
//   * _first_free_node points to the first free node in the list, if there
//     are any, or is set to UNDEFINED if there aren't any. Otherwise,
//     _first_free_node == _forwd[_last_active_node].
//
////////////////////////////////////////////////////////////////////////////////

namespace libsemigroups {
  namespace detail {

    ////////////////////////////////////////////////////////////////////////
    // Typedefs/aliases
    ////////////////////////////////////////////////////////////////////////

    template <typename NodeType>
    constexpr NodeType NodeManager<NodeType>::_id_node;

    ////////////////////////////////////////////////////////////////////////
    // Helper free function
    ////////////////////////////////////////////////////////////////////////

    template <typename NodeType>
    static inline NodeType ff(NodeType c, NodeType d, NodeType r) {
      return (r == c ? d : (r == d ? c : r));
      // return (r == c) * d + (r == d) * c + (r != c && r != d) * r;
    }

    ////////////////////////////////////////////////////////////////////////
    // NodeManager - constructors - public
    ////////////////////////////////////////////////////////////////////////
    template <typename NodeType>
    NodeManager<NodeType>::NodeManager()
        :  // protected
          _current(0),
          _current_la(0),
          // private - stats
          _active(1),
          _defined(1),
          _nodes_killed(0),
          // private - settings
          _growth_factor(2.0),
          // private - data
          _bckwd(1, 0),
          _first_free_node(UNDEFINED),
          _forwd(1, static_cast<NodeType>(UNDEFINED)),
          _ident(1, 0),
          _last_active_node(0) {}

    template <typename NodeType>
    NodeManager<NodeType>::~NodeManager() = default;

    template <typename NodeType>
    NodeManager<NodeType>& NodeManager<NodeType>::growth_factor(float val) {
      if (val < 1.0) {
        LIBSEMIGROUPS_EXCEPTION("expected a value of at least 1.0, found %f",
                                val);
      }
      _growth_factor = val;
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // NodeManager - member functions - protected
    ////////////////////////////////////////////////////////////////////////

    template <typename NodeType>
    void NodeManager<NodeType>::add_active_nodes(size_t n) {
      if (n > (node_capacity() - number_of_nodes_active())) {
        size_t const m = n - (node_capacity() - number_of_nodes_active());
        add_free_nodes(m);
        // add_free_nodes adds new free nodes to the start of the free list
        _last_active_node = _forwd.size() - 1;
        _first_free_node  = _forwd[_last_active_node];
        std::iota(_ident.begin() + (_ident.size() - m),
                  _ident.end(),
                  _ident.size() - m);
        _active += m;
        _defined += m;
        n -= m;
      }
      _active += n;
      _defined += n;
      for (; n > 0; --n) {
        _bckwd[_first_free_node]  = _last_active_node;
        _last_active_node         = _first_free_node;
        _first_free_node          = _forwd[_last_active_node];
        _ident[_last_active_node] = _last_active_node;
      }
    }

    template <typename NodeType>
    void NodeManager<NodeType>::add_free_nodes(size_t n) {
      // We add n new free nodes at the end of the current list, and link them
      // in as follows:
      //
      // 0 <-> ... <-> _last_active_node <-> old_capacity <-> new free node 1
      //   <-> ... <-> new free node n   <-> old_first_free_node
      //   <-> remaining old free nodes
      size_t const   old_capacity        = _forwd.size();
      NodeType const old_first_free_node = _first_free_node;

      _forwd.resize(_forwd.size() + n, UNDEFINED);
      std::iota(
          _forwd.begin() + old_capacity, _forwd.end() - 1, old_capacity + 1);

      _bckwd.resize(_bckwd.size() + n, 0);
      std::iota(_bckwd.begin() + old_capacity + 1, _bckwd.end(), old_capacity);

      _ident.resize(_ident.size() + n, 0);

      _first_free_node          = old_capacity;
      _forwd[_last_active_node] = _first_free_node;
      _bckwd[_first_free_node]  = _last_active_node;

      if (old_first_free_node != UNDEFINED) {
        _forwd[_forwd.size() - 1]   = old_first_free_node;
        _bckwd[old_first_free_node] = _forwd.size() - 1;
      }
#ifdef LIBSEMIGROUPS_DEBUG
      debug_validate_forwd_bckwd();
#endif
    }

    template <typename NodeType>
    void NodeManager<NodeType>::erase_free_nodes() {
#ifdef LIBSEMIGROUPS_DEBUG
      size_t sum = 0;
      for (NodeType c = _id_node; c != _first_free_node; c = _forwd[c]) {
        LIBSEMIGROUPS_ASSERT(c < number_of_nodes_active());
        sum += c;
      }
      LIBSEMIGROUPS_ASSERT(
          sum == number_of_nodes_active() * (number_of_nodes_active() - 1) / 2);
      std::vector<NodeType> copy(_forwd.cbegin(),
                                 _forwd.cbegin() + number_of_nodes_active());
      std::sort(copy.begin(), copy.end());
      LIBSEMIGROUPS_ASSERT(std::unique(copy.begin(), copy.end()) == copy.end());
#endif
      _first_free_node = UNDEFINED;
      _forwd.erase(_forwd.begin() + number_of_nodes_active(), _forwd.end());
      _forwd[_last_active_node] = UNDEFINED;
      _forwd.shrink_to_fit();
      _bckwd.erase(_bckwd.begin() + number_of_nodes_active(), _bckwd.end());
      _bckwd.shrink_to_fit();
      _ident.erase(_ident.begin() + number_of_nodes_active(), _ident.end());
      _ident.shrink_to_fit();
#ifdef LIBSEMIGROUPS_DEBUG
      debug_validate_forwd_bckwd();
#endif
    }

    template <typename NodeType>
    NodeType NodeManager<NodeType>::new_active_node() {
      if (_first_free_node == UNDEFINED) {
        // There are no free nodes to recycle: make new ones.
        // It seems to be marginally faster to make lots like this, than to
        // just make 1, in some examples, notably ToddCoxeterBase 040 (Walker 3).
        add_free_nodes(growth_factor() * node_capacity());
      }
      add_active_nodes(1);
      return _last_active_node;
    }

    template <typename NodeType>
    void NodeManager<NodeType>::switch_nodes(NodeType c, NodeType d) {
      LIBSEMIGROUPS_ASSERT(is_active_node(c) || is_active_node(d));
      NodeType fc = _forwd[c], fd = _forwd[d], bc = _bckwd[c], bd = _bckwd[d];

      if (fc != d) {
        _forwd[d]  = fc;
        _bckwd[c]  = bd;
        _forwd[bd] = c;
        if (fc != UNDEFINED) {
          _bckwd[fc] = d;
        }
      } else {
        _forwd[d] = c;
        _bckwd[c] = d;
      }

      if (fd != c) {
        _forwd[c]  = fd;
        _bckwd[d]  = bc;
        _forwd[bc] = d;
        if (fd != UNDEFINED) {
          _bckwd[fd] = c;
        }
      } else {
        _forwd[c] = d;
        _bckwd[d] = c;
      }

      if (!is_active_node(c)) {
        _ident[d] = 0;
        _ident[c] = c;
      } else if (!is_active_node(d)) {
        _ident[c] = 0;
        _ident[d] = d;
      }

      _current          = ff(c, d, _current);
      _last_active_node = ff(c, d, _last_active_node);
      _first_free_node  = ff(c, d, _first_free_node);
      // This is never called from lookahead so we don't have to update
      // _current_la, also it might be that we are calling this after
      // everything is finished and so _current may not be active.

      LIBSEMIGROUPS_ASSERT(is_active_node(_last_active_node));
      LIBSEMIGROUPS_ASSERT(!is_active_node(_first_free_node));
    }

    // The permutation p ^ -1 must map the active nodes to the [0, ..
    // , number_of_nodes_active())
    template <typename NodeType>
    void NodeManager<NodeType>::apply_permutation(NodeManager::Perm p) {
      size_t const n = p.size();
      for (NodeType i = 0; i < n; ++i) {
        NodeType current = i;
        while (i != p[current]) {
          size_t next = p[current];
          switch_nodes(current, next);
          p[current] = current;
          current    = next;
        }
        p[current] = current;
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // NodeManager - member functions - private
    ////////////////////////////////////////////////////////////////////////

    template <typename NodeType>
    void NodeManager<NodeType>::free_node(NodeType c) {
      _active--;
      _nodes_killed++;
      LIBSEMIGROUPS_ASSERT(is_active_node(c));
      // If any "controls" point to <c>, move them back one in the list
      LIBSEMIGROUPS_ASSERT(_current < _bckwd.size()
                           || _current == _first_free_node);
      _current = (c == _current ? _bckwd[_current] : _current);
      LIBSEMIGROUPS_ASSERT(_current_la < _bckwd.size()
                           || _current_la == _first_free_node);
      _current_la = (c == _current_la ? _bckwd[_current_la] : _current_la);

      if (c == _last_active_node) {
        // Simply move the start of the free list back by 1
        LIBSEMIGROUPS_ASSERT(_last_active_node < _bckwd.size());
        _last_active_node = _bckwd[_last_active_node];
      } else {
        LIBSEMIGROUPS_ASSERT(_forwd[c] != UNDEFINED);
        // Remove <c> from the active list
        _bckwd[_forwd[c]] = _bckwd[c];
        _forwd[_bckwd[c]] = _forwd[c];
        // Add <c> to the start of the free list
        _forwd[c] = _first_free_node;
        LIBSEMIGROUPS_ASSERT(_last_active_node < _forwd.size());
        if (_first_free_node != UNDEFINED) {
          _bckwd[_first_free_node] = c;
        }
        _forwd[_last_active_node] = c;
      }
      _bckwd[c]        = _last_active_node;
      _first_free_node = c;
      _ident[c]        = _id_node;
    }

    // Basically free all nodes
    template <typename NodeType>
    void NodeManager<NodeType>::clear() {
      _nodes_killed += (_active - 1);
      _active = 1;
      std::iota(_forwd.begin(), _forwd.end() - 1, 1);
      _forwd.back() = UNDEFINED;
      std::iota(_bckwd.begin() + 1, _bckwd.end(), 0);
      std::fill(_ident.begin(), _ident.end(), 0);
      if (_forwd.size() > 1) {
        _first_free_node = 1;
      } else {
        _first_free_node = UNDEFINED;
      }
      _last_active_node = 0;
      _current          = _id_node;
      _current_la       = _id_node;
    }

#ifdef LIBSEMIGROUPS_DEBUG
    ////////////////////////////////////////////////////////////////////////
    // NodeManager - member functions (debug only) - protected
    ////////////////////////////////////////////////////////////////////////

    template <typename NodeType>
    void NodeManager<NodeType>::debug_validate_forwd_bckwd() const {
      LIBSEMIGROUPS_ASSERT(_forwd.size() == _bckwd.size());
      LIBSEMIGROUPS_ASSERT(_bckwd.size() == _ident.size());
      NodeType number_of_nodes = 0;
      NodeType e               = _id_node;
      while (e != _first_free_node) {
        LIBSEMIGROUPS_ASSERT(e == _id_node || _forwd[_bckwd[e]] == e);
        LIBSEMIGROUPS_ASSERT(_forwd[e] == _first_free_node
                             || _bckwd[_forwd[e]] == e);
        number_of_nodes++;
        LIBSEMIGROUPS_ASSERT(is_active_node(_forwd[e])
                             || _forwd[e] == _first_free_node);
        e = _forwd[e];
      }
      while (e != UNDEFINED) {
        LIBSEMIGROUPS_ASSERT(!is_active_node(e));
        LIBSEMIGROUPS_ASSERT(_forwd[_bckwd[e]] == e);
        LIBSEMIGROUPS_ASSERT(_forwd[e] == UNDEFINED || _bckwd[_forwd[e]] == e);
        number_of_nodes++;
        e = _forwd[e];
      }
      LIBSEMIGROUPS_ASSERT(number_of_nodes == _forwd.size());
      LIBSEMIGROUPS_ASSERT(number_of_nodes == _bckwd.size());
      LIBSEMIGROUPS_ASSERT(number_of_nodes == _ident.size());
    }
#endif
  }  // namespace detail
}  // namespace libsemigroups

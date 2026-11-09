//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 James D. Mitchell
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

// This file contains the declaration for a class to manage nodes for
// NodeManagedGraph instances.
//
// TODO(later) combine NodeManager and NodeManagedGraph

#ifndef LIBSEMIGROUPS_DETAIL_NODE_MANAGER_HPP_
#define LIBSEMIGROUPS_DETAIL_NODE_MANAGER_HPP_

#include <cstddef>      // for size_t
#include <cstdint>      // for uint32_t
#include <numeric>      // for iota
#include <type_traits>  // for true_type
#include <vector>       // for vector

#include "libsemigroups/config.hpp"     // for LIBSEMIGROUPS_DEBUG
#include "libsemigroups/constants.hpp"  // for UNDEFINED
#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT/DEBUG
#include "libsemigroups/exception.hpp"  // for LISEMIGROUPS_EXCEPTION
#include "libsemigroups/ranges.hpp"     // for rx::is_input_range

namespace libsemigroups {
  namespace detail {
    template <typename Node>
    class NodeManager {
     public:
      ////////////////////////////////////////////////////////////////////////
      // NodeManager - typedefs - public
      ////////////////////////////////////////////////////////////////////////

      using node_type = Node;

      ////////////////////////////////////////////////////////////////////////
      // NodeManager - data - private
      ////////////////////////////////////////////////////////////////////////
     private:
      struct Stats;             // forward decl
      struct ActiveNodesRange;  // forward decl

      static constexpr node_type _id_node = 0;

      std::vector<node_type>         _bckwd;
      node_type                      _current;
      node_type                      _current_la;
      node_type                      _first_free_node;
      std::vector<node_type>         _forwd;
      mutable std::vector<node_type> _ident;
      node_type                      _last_active_node;
      Stats                          _stats;

     public:
      ////////////////////////////////////////////////////////////////////////
      // NodeManager - constructors + destructor - public
      ////////////////////////////////////////////////////////////////////////

      NodeManager();
      NodeManager& init();

      NodeManager(NodeManager const& that) = default;
      NodeManager(NodeManager&&)           = default;

      NodeManager& operator=(NodeManager const& that) = default;
      NodeManager& operator=(NodeManager&&)           = default;

      ~NodeManager();

      ////////////////////////////////////////////////////////////////////////
      // NodeManager - member functions - public
      ////////////////////////////////////////////////////////////////////////

      [[nodiscard]] node_type& cursor() {
        return _current;
      }

      [[nodiscard]] node_type& lookahead_cursor() {
        return _current_la;
      }

      // std::vector::size is noexcept
      [[nodiscard]] inline size_t node_capacity() const noexcept {
        return _forwd.size();
      }

      [[nodiscard]] inline node_type first_free_node() const noexcept {
        return _first_free_node;
      }

      [[nodiscard]] inline bool has_free_nodes() const noexcept {
        return _first_free_node != UNDEFINED;
      }

      // not noexcept since std::vector::operator[] isn't.
      [[nodiscard]] inline bool is_active_node(node_type c) const {
        LIBSEMIGROUPS_ASSERT(c < _ident.size() || c == UNDEFINED);
        return c != UNDEFINED && _ident[c] == c;
      }

      [[nodiscard]] size_t position_of_node(node_type n) const;

      [[nodiscard]] inline bool is_valid_node(node_type c) const noexcept {
        return c < _forwd.size();
      }

      // not noexcept since std::vector::operator[] isn't.
      [[nodiscard]] inline node_type next_active_node(node_type c) const {
        return _forwd[c];
      }

      // Note that these iterators are invalidated by any changes to the
      // container, i.e. they can't be used as a substitute for cursors.
      [[nodiscard]] auto cbegin_active_nodes() const {
        return rx::begin(ActiveNodesRange(this));
      }

      // Note that these iterators are invalidated by any changes to the
      // container, i.e. they can't be used as a substitute for cursors.
      [[nodiscard]] auto cend_active_nodes() const {
        return rx::end(ActiveNodesRange(this));
      }

      [[nodiscard]] inline uint64_t number_of_nodes_active() const noexcept {
        return _stats.num_nodes_active;
      }

      [[nodiscard]] inline uint64_t number_of_nodes_defined() const noexcept {
        return _stats.num_nodes_defined;
      }

      [[nodiscard]] inline uint64_t number_of_nodes_killed() const noexcept {
        return _stats.num_nodes_killed;
      }

      [[nodiscard]] ActiveNodesRange active_nodes() const {
        return ActiveNodesRange(this);
      }

      [[nodiscard]] constexpr node_type initial_node() const noexcept {
        return _id_node;
      }

      [[nodiscard]] node_type max_active_node() const noexcept;

     protected:
      ////////////////////////////////////////////////////////////////////////
      // NodeManager - member functions - protected
      ////////////////////////////////////////////////////////////////////////

      // not noexcept because free_node isn't, and std::vector::operator[]
      // isn't.

      inline void union_nodes(node_type min, node_type max) {
        LIBSEMIGROUPS_ASSERT(is_active_node(min));
        LIBSEMIGROUPS_ASSERT(is_active_node(max));
        LIBSEMIGROUPS_ASSERT(max > min);
        free_node(max);
        // Leave a "forwarding address" so we know what <max> was identified
        // with
        _ident[max] = min;
      }

      // not noexcept since std::vector::operator[] isn't.

      [[nodiscard]] inline node_type find_node(node_type c) const {
        LIBSEMIGROUPS_ASSERT(is_valid_node(c));
        while (true) {
          node_type d = _ident[c];
          if (d == c) {
            return d;
          }
          node_type e = _ident[d];
          if (d == e) {
            return d;
          }
          _ident[c] = e;
          c         = e;
        }
      }

      void                    add_active_nodes(size_t);
      void                    add_free_nodes(size_t);
      void                    erase_free_nodes();
      [[nodiscard]] node_type new_active_node();

      void clear();
      void compact();

      // not noexcept since std::vector::operator[] isn't.
      void free_node(node_type const);
#ifdef LIBSEMIGROUPS_DEBUG

      ////////////////////////////////////////////////////////////////////////
      // NodeManager - member functions (debug only) - protected
      ////////////////////////////////////////////////////////////////////////
      void debug_validate_forwd_bckwd() const;
#endif

     private:
      void compact(size_t N);
    };

  }  // namespace detail
}  // namespace libsemigroups

#include "node-manager.tpp"

#endif  // LIBSEMIGROUPS_DETAIL_NODE_MANAGER_HPP_

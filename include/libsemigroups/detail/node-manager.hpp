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
// ToddCoxeterDigraph instances.

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
#include "libsemigroups/ranges.hpp"

namespace libsemigroups {
  namespace detail {
    template <typename NodeType>
    class NodeManager {
     public:
      ////////////////////////////////////////////////////////////////////////
      // NodeManager - typedefs - public
      ////////////////////////////////////////////////////////////////////////

      //! Type of nodes
      using node_type = NodeType;
      using Perm      = std::vector<node_type>;

      ////////////////////////////////////////////////////////////////////////
      // NodeManager - constructors + destructor - public
      ////////////////////////////////////////////////////////////////////////

      NodeManager();

      NodeManager(NodeManager const&) = default;
      NodeManager(NodeManager&&)      = default;

      NodeManager& operator=(NodeManager const&) = default;
      NodeManager& operator=(NodeManager&&)      = default;

      ~NodeManager();

      ////////////////////////////////////////////////////////////////////////
      // NodeManager - member functions - public
      ////////////////////////////////////////////////////////////////////////

      node_type& cursor() {
        return _current;
      }

      node_type& lookahead_cursor() {
        return _current_la;
      }

      //! Returns the current node capacity of the graph.
      //!
      //! \returns A value of type \c size_t.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant
      //!
      //! \par Parameters
      //! (None)
      // std::vector::size is noexcept
      inline size_t node_capacity() const noexcept {
        return _forwd.size();
      }

      //! Returns the first inactive node.
      //!
      //! \returns A value of type NodeManager::node_type
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant
      //!
      //! \par Parameters
      //! (None)
      inline node_type first_free_node() const noexcept {
        return _first_free_node;
      }

      //! Check if there are any inactive nodes.
      //!
      //! \returns A value of type \c bool.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant
      //!
      //! \par Parameters
      //! (None)
      inline bool has_free_nodes() const noexcept {
        return _first_free_node != UNDEFINED;
      }

      //! Check if the given node is active or not.
      //!
      //! \param c the to check.
      //!
      //! \returns A value of type \c bool.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \complexity
      //! Constant
      // not noexcept since std::vector::operator[] isn't.
      inline bool is_active_node(node_type c) const {
        LIBSEMIGROUPS_ASSERT(c < _ident.size() || c == UNDEFINED);
        return c != UNDEFINED && _ident[c] == c;
      }

      //! Check if the given node is valid.
      //!
      //! \param c the node to check.
      //!
      //! \returns A value of type \c bool.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant
      inline bool is_valid_node(node_type c) const noexcept {
        return c < _forwd.size();
      }

      //! Returns the next active node after the given node.
      //!
      //! \param c the node.
      //!
      //! \returns A value of type NodeManager::node_type
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \complexity
      //! Constant
      // not noexcept since std::vector::operator[] isn't.
      inline node_type next_active_node(node_type c) const {
        LIBSEMIGROUPS_ASSERT(is_active_node(c));
        return _forwd[c];
      }

      // Note that these iterators are invalidated by any changes to the
      // container, i.e. they can't be used as a substitute for cursors.
      auto cbegin_active_nodes() const {
        return rx::begin(ActiveNodesRange(this));
      }

      // Note that these iterators are invalidated by any changes to the
      // container, i.e. they can't be used as a substitute for cursors.
      auto cend_active_nodes() const {
        return rx::end(ActiveNodesRange(this));
      }

      //! Returns the number of active nodes.
      //!
      //! \returns A value of type \c size_t.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant
      //!
      //! \par Parameters
      //! (None)
      inline size_t number_of_nodes_active() const noexcept {
        return _active;
      }

      //! Returns the total number of nodes defined so far.
      //!
      //! \returns A value of type \c size_t.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant
      //!
      //! \par Parameters
      //! (None)
      inline size_t number_of_nodes_defined() const noexcept {
        return _defined;
      }

      //! Returns the total number of nodes that have been killed so far.
      //!
      //! \returns A value of type \c size_t.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant
      //!
      //! \par Parameters
      //! (None)
      inline size_t number_of_nodes_killed() const noexcept {
        return _nodes_killed;
      }

      //! Set the value of the growth factor setting.
      //!
      //! This setting is used to determine the factor by which the number of
      //! nodes in the table is increased, when more nodes are required.
      //!
      //! The default value of this setting is \c 2.0.
      //!
      //! \param val the new value of the setting.
      //!
      //! \returns A reference to \c *this.
      //!
      //! \throws LibsemigroupsException if \p val is less than \c 1.0.
      //!
      //! \complexity
      //! Constant
      NodeManager& growth_factor(float val);

      //! The current value of the growth factor setting.
      //!
      //! \returns A value of type \c float.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant
      float growth_factor() const noexcept {
        return _growth_factor;
      }

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

      inline node_type find_node(node_type c) const {
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
        LIBSEMIGROUPS_ASSERT(is_active_node(c));
        return c;
      }

      void      add_active_nodes(size_t);
      void      add_free_nodes(size_t);
      void      erase_free_nodes();
      node_type new_active_node();
      // TODO rename -> swap_nodes_no_checks
      void switch_nodes(node_type const, node_type const);
      // TODO rename -> permute_nodes_no_checks
      void apply_permutation(Perm p);

      // not noexcept since std::vector::operator[] isn't.
      void free_node(node_type const);

      void clear();

      ////////////////////////////////////////////////////////////////////////
      // NodeManager - data - protected
      ////////////////////////////////////////////////////////////////////////

      constexpr node_type initial_node() const noexcept {
        return _id_node;
      }

      static constexpr node_type _id_node = 0;
      node_type                  _current;
      node_type                  _current_la;

     private:
      struct ActiveNodesRange {
        using output_type = node_type;

        NodeManager const* _node_manager;
        output_type        _current;

        ActiveNodesRange() : _node_manager(nullptr), _current(UNDEFINED) {}

        explicit ActiveNodesRange(NodeManager const* nm)
            : _node_manager(nm), _current(nm->initial_node()) {}

        output_type get() const noexcept {
          return _current;
        }

        void next() noexcept {
          _current = _node_manager->next_active_node(_current);
        }

        bool at_end() const noexcept {
          return _node_manager == nullptr
                 || _current == _node_manager->first_free_node();
        }

        size_t size_hint() const noexcept {
          return _node_manager->number_of_nodes_active();
        }

        static constexpr bool is_finite     = true;
        static constexpr bool is_idempotent = true;
      };

     public:
      ActiveNodesRange active_nodes() const {
        return ActiveNodesRange(this);
      }

     private:
      ////////////////////////////////////////////////////////////////////////
      // NodeManager - data - private
      ////////////////////////////////////////////////////////////////////////

      // Stats
      size_t _active;
      size_t _defined;
      size_t _nodes_killed;

      // Settings
      float _growth_factor;

      // Data
      std::vector<node_type>         _bckwd;
      node_type                      _first_free_node;
      std::vector<node_type>         _forwd;
      mutable std::vector<node_type> _ident;
      node_type                      _last_active_node;

#ifdef LIBSEMIGROUPS_DEBUG

     protected:
      ////////////////////////////////////////////////////////////////////////
      // NodeManager - member functions (debug only) - protected
      ////////////////////////////////////////////////////////////////////////
      void debug_validate_forwd_bckwd() const;
#endif
    };

  }  // namespace detail
}  // namespace libsemigroups

#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
// TODO replace uint32_t by Node
template <>
struct rx::is_input_range<
    typename libsemigroups::detail::NodeManager<uint32_t>::ActiveNodesRange>
    : std::true_type {};
#endif
#include "node-manager.tpp"

#endif  // LIBSEMIGROUPS_DETAIL_NODE_MANAGER_HPP_

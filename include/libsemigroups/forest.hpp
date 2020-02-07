//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 Finn Smith
// Copyright (C) 2019 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_FOREST_HPP_
#define LIBSEMIGROUPS_FOREST_HPP_

#include <cstddef>  // for size_t
#include <vector>   // for vector

#include "constants.hpp"                // for UNDEFINED
#include "libsemigroups-exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION

namespace libsemigroups {
  //! Defined in ``forest.hpp``.
  //!
  //! This class represents the collection of spanning trees of the strongly
  //! connected components of a digraph.
  class Forest final {
   public:
    //! Alias for the type of nodes in a forest
    using node_type = size_t;

    //! Alias for the type of edge labels in a forest.
    using label_type = size_t;

    //! Constructs a forest with \p n nodes.
    //!
    //! The Forest is initialised so that the parent() and label() of every
    //! node is libsemigroups::UNDEFINED.
    //!
    //! \param n the number of nodes, defaults to \c 0.
    explicit Forest(size_t n = 0)
        : _edge_label(n, static_cast<size_t>(UNDEFINED)),
          _parent(n, static_cast<size_t>(UNDEFINED)) {}

    //! Default copy constructor
    Forest(Forest const&) = default;

    //! Default move constructor
    Forest(Forest&&) = default;

    //! Default copy assignment constructor
    Forest& operator=(Forest const&) = default;

    //! Default move assignment constructor
    Forest& operator=(Forest&&) = default;

    ~Forest() = default;

    //! Add nodes to the Forest.
    //!
    //! \param n the number of nodes to add.
    //!
    //! \returns
    //! (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //! \strong_guarantee
    //!
    //! \complexity
    //! At most linear in `nr_nodes() + n`.
    //!
    //! \iterator_validity
    //! \iterator_invalid
    void add_nodes(size_t n) {
      size_t const old_nr_nodes = nr_nodes();
      try {
        _edge_label.insert(
            _edge_label.cend(), n, static_cast<size_t>(UNDEFINED));
        _parent.insert(_parent.cend(), n, static_cast<size_t>(UNDEFINED));
      } catch (...) {
        _edge_label.resize(old_nr_nodes);
        _parent.resize(old_nr_nodes);
        throw;
      }
    }

    //! Removes all nodes from the forest.
    //!
    //! \returns
    //! (None)
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Linear in nr_nodes().
    //!
    //! \iterator_validity
    //! \iterator_invalid
    //!
    //! \par Parameters
    //! (None)
    // std::vector::clear is noexcept
    void clear() noexcept {
      _edge_label.clear();
      _parent.clear();
    }

    //! Set the parent and edge label for a node.
    //!
    //! \param node   the node whose parent and label to set.
    //! \param parent the parent node
    //! \param gen    the label of the edge from \p parent to \p node.
    //!
    //! \returns
    //! (None)
    //!
    //! \throws LibsemigroupsException if \p node or \p parent exceeds
    //! nr_nodes().
    //!
    //! \complexity
    //! Constant
    // not noexcept because std::vector::operator[] isn't.
    void set(node_type node, node_type parent, label_type gen) {
      validate_node(node);
      validate_node(parent);
      _parent[node]     = parent;
      _edge_label[node] = gen;
    }

    //! Returns the number of nodes in the forest.
    //!
    //! \returns
    //! A `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant
    //!
    //! \par Parameters
    //! (None)
    size_t nr_nodes() const noexcept {
      return _parent.size();
    }

    //! Returns the parent of a node.
    //!
    //! \param i the node whose parent is sought.
    //!
    //! \returns
    //! A Forest::node_type.
    //!
    //! \throws LibsemigroupsException if \p i exceeds \p nr_nodes().
    //!
    //! \complexity
    //! Constant
    // not noexcept because std::vector::operator[] isn't.
    node_type parent(node_type i) const {
      validate_node(i);
      return _parent[i];
    }

    //! Returns the label of the edge from a node to its parent.
    //!
    //! \param i the node whose label is sought.
    //!
    //! \returns
    //! A Forest::label_type.
    //!
    //! \throws LibsemigroupsException if \p i exceeds \p nr_nodes().
    //!
    //! \complexity
    //! Constant
    // not noexcept because std::vector::operator[] isn't.
    label_type label(node_type i) const {
      validate_node(i);
      return _edge_label[i];
    }

    //! Returns an iterator pointing to the parent of the first node.
    //!
    //! \returns
    //! A std::vector<node_type>::const_iterator.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    std::vector<size_t>::const_iterator cbegin_parent() const noexcept {
      return _parent.cbegin();
    }

    //! Returns an iterator pointing one-past the parent of the last node.
    //!
    //! \returns
    //! A std::vector<node_type>::const_iterator.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    std::vector<size_t>::const_iterator cend_parent() const noexcept {
      return _parent.cend();
    }

   private:
    void validate_node(node_type v) const {
      if (v >= nr_nodes()) {
        LIBSEMIGROUPS_EXCEPTION("node value out of bounds, expected value in "
                                "the range [0, %d), got %d",
                                nr_nodes(),
                                v);
      }
    }

    std::vector<size_t> _edge_label;
    std::vector<size_t> _parent;
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_FOREST_HPP_

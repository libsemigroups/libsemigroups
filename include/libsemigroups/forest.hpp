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

#include <algorithm>         // for fill
#include <cstddef>           // for size_t
#include <initializer_list>  // for initializer_list
#include <iosfwd>            // for ostream
#include <iterator>          // for begin, end
#include <vector>            // for vector, allocator, operator==

#include "constants.hpp"  // for Undefined, Max, UNDEFINED, operator!=
#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "types.hpp"      // for word_type

namespace libsemigroups {
  //! Defined in ``forest.hpp``.
  //!
  //! This class represents the collection of spanning trees of the strongly
  //! connected components of a digraph.
  class Forest {
   public:
    //! Alias for the type of nodes in a forest
    using node_type = size_t;

    //! Alias for the type of edge labels in a forest.
    using label_type = size_t;

    //! Constructs a forest with \p n nodes.
    //!
    //! The Forest is initialised so that the parent() and label() of every
    //! node is UNDEFINED.
    //!
    //! \param n the number of nodes, defaults to \c 0.
    explicit Forest(size_t n = 0)
        : _edge_label(n, static_cast<size_t>(UNDEFINED)),
          _parent(n, static_cast<size_t>(UNDEFINED)) {}

    void init(size_t n = 0) {
      _edge_label.resize(n);
      std::fill(std::begin(_edge_label),
                std::end(_edge_label),
                static_cast<size_t>(UNDEFINED));
      _parent.resize(n);
      std::fill(std::begin(_parent),
                std::end(_parent),
                static_cast<size_t>(UNDEFINED));
    }

    bool operator==(Forest const& that) const {
      return _parent == that._parent && _edge_label == that._edge_label;
    }

    bool operator!=(Forest const& that) const {
      return !(*this == that);
    }

    //! Default copy constructor
    Forest(Forest const&) = default;

    //! Default move constructor
    Forest(Forest&&) = default;

    //! Default copy assignment constructor
    Forest& operator=(Forest const&) = default;

    //! Default move assignment constructor
    Forest& operator=(Forest&&) = default;

    ~Forest();

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
    //! At most linear in `number_of_nodes() + n`.
    //!
    //! \iterator_validity
    //! \iterator_invalid
    void add_nodes(size_t n) {
      size_t const old_nr_nodes = number_of_nodes();
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
    //! Linear in number_of_nodes().
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

    //! Check if there are any nodes in the forest.
    //!
    //! \returns
    //! A value of type `bool`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant
    //!
    //! \par Parameters
    //! (None)
    [[nodiscard]] bool empty() const noexcept {
      return _parent.empty();
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
    //! number_of_nodes().
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
    [[nodiscard]] size_t number_of_nodes() const noexcept {
      return _parent.size();
    }

    //! Returns the parent of a node.
    //!
    //! \param i the node whose parent is sought.
    //!
    //! \returns
    //! A \ref node_type.
    //!
    //! \throws LibsemigroupsException if \p i exceeds \p number_of_nodes().
    //!
    //! \complexity
    //! Constant
    // not noexcept because std::vector::operator[] isn't.
    [[nodiscard]] node_type parent(node_type i) const {
      validate_node(i);
      return _parent[i];
    }

    [[nodiscard]] node_type parent_no_checks(node_type i) const {
      return _parent[i];
    }

    //! Returns the label of the edge from a node to its parent.
    //!
    //! \param i the node whose label is sought.
    //!
    //! \returns
    //! A \ref label_type.
    //!
    //! \throws LibsemigroupsException if \p i exceeds \p number_of_nodes().
    //!
    //! \complexity
    //! Constant
    // not noexcept because std::vector::operator[] isn't.
    [[nodiscard]] label_type label(node_type i) const {
      validate_node(i);
      return _edge_label[i];
    }

    [[nodiscard]] label_type label_no_checks(node_type i) const {
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

    [[nodiscard]] auto const& parents() const noexcept {
      return _parent;
    }

    [[nodiscard]] auto const& labels() const noexcept {
      return _edge_label;
    }

    void path_to_root_no_checks(word_type& w, node_type i) const {
      while (parent_no_checks(i) != UNDEFINED) {
        w.push_back(label_no_checks(i));
        LIBSEMIGROUPS_ASSERT(i != parent_no_checks(i));
        i = parent_no_checks(i);
      }
    }

    [[nodiscard]] word_type path_to_root_no_checks(node_type i) const {
      word_type w;
      path_to_root_no_checks(w, i);
      return w;
    }

    void path_to_root(word_type& w, node_type i) const {
      validate_node(i);
      path_to_root_no_checks(w, i);
    }

    [[nodiscard]] word_type path_to_root(node_type i) const {
      word_type w;
      path_to_root(w, i);
      return w;
    }

   private:
    void validate_node(node_type v) const {
      if (v >= number_of_nodes()) {
        LIBSEMIGROUPS_EXCEPTION("node value out of bounds, expected value in "
                                "the range [0, {}), got {}",
                                number_of_nodes(),
                                v);
      }
    }
    // TODO(later) combine into 1 using a struct
    std::vector<size_t> _edge_label;
    std::vector<size_t> _parent;
  };

  [[nodiscard]] Forest to_forest(std::initializer_list<size_t> parent,
                                 std::initializer_list<size_t> edge_label);

  std::ostream& operator<<(std::ostream& os, Forest const& f);

  // namespace forest {

  //! Returns a \ref normal_form_iterator pointing at the first normal
  //! form.
  //!
  //! Returns a const iterator pointing to the normal form of the first
  //! class of the congruence represented by an instance of ToddCoxeter.
  //! The order of the classes, and the normal form, that is returned are
  //! controlled by standardize(order).
  //!
  //! \parameters
  //! (None)
  //!
  //! \returns A value of type \ref normal_form_iterator.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  // TODO(refactor): update the doc
  // TODO This is probably not very efficient, better to have a custom range
  // where the word_type is cached, and next just deletes the end of the word
  // and adds whatever's appropriate, rather than retracing the entire path
  // every time
  //
  // inline auto paths(Forest const& f) {
  //   rx::seq<Forest::node_type>() | rx::transform([&f](auto i) {
  //     word_type w;
  //     while (f.parent_no_checks(i) != UNDEFINED) {
  //       w.push_back(f.label_no_checks(i));
  //       i = f.parent_no_checks(i);
  //     }
  //     return w;
  //   });
  // }
  //}  // namespace forest

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_FOREST_HPP_

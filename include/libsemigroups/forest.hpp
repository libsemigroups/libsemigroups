//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 Finn Smith + James Mitchell
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

#include <cstddef>           // for size_t, ptrdiff_t
#include <cstdint>           // for uint32_t
#include <initializer_list>  // for initializer_list
#include <iterator>          // for forward_iterator_tag
#include <string>            // for string, basic_string
#include <vector>            // for vector, operator==

#include "constants.hpp"  // for UNDEFINED, operator!=, operator==
#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "dot.hpp"        // for Dot
#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "types.hpp"      // for word_type, enable_if_is_same

#include "detail/fmt.hpp"  // for formatter, string_view

namespace libsemigroups {
  //! \ingroup word_graph_group
  //!
  //! \brief Class representing a collection of spanning trees of a word graph.
  //!
  //! Defined in `forest.hpp`.
  //!
  //! This class represents the collection of spanning trees of the strongly
  //! connected components of a word graph.
  class Forest {
    std::vector<uint32_t> _edge_label;
    std::vector<uint32_t> _parent;

   public:
    //! Alias for the type of nodes in a forest
    using node_type = uint32_t;

    //! Alias for the type of edge labels in a forest.
    using label_type = uint32_t;

   private:
    void throw_if_not_acyclic(node_type node, node_type parent) const;

   public:
    //! \brief Constructs a forest with \p n nodes.
    //!
    //! Constructs a forest with \p n nodes, that is initialised so that the
    //! parent() and label() of every node is UNDEFINED.
    //!
    //! \param n the number of nodes, defaults to \c 0.
    explicit Forest(size_t n = 0)
        : _edge_label(n, static_cast<uint32_t>(UNDEFINED)),
          _parent(n, static_cast<uint32_t>(UNDEFINED)) {}

    //! \brief Reinitialize an existing Forest object.
    //!
    //! This function reinitializes an existing Forest object so that it is in
    //! the same state as if it had just be constructed as `Forest(n)`.
    //!
    //! \param n the number of nodes, defaults to \c 0.
    //!
    //! \returns A reference to \c this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    Forest& init(size_t n = 0);

    //! Default copy constructor
    Forest(Forest const&) = default;

    //! Default move constructor
    Forest(Forest&&) = default;

    //! Default copy assignment constructor
    Forest& operator=(Forest const&) = default;

    //! Default move assignment constructor
    Forest& operator=(Forest&&) = default;

    ~Forest();

    //! \brief Throw an exception if the Forest is not acyclic.
    //!
    //! This function throws a descriptive exception if a Forest instance is not
    //! well-defined, i.e. if it is not acyclic. See \ref forest::is_forest for
    //! more details.
    void throw_if_not_acyclic() const {
      throw_if_not_acyclic(UNDEFINED, UNDEFINED);
    }

    //! \brief Compare Forest objects for equality.
    //!
    //! This function returns \c true if and only if the parent and label of
    //! every node in \c this coincides with the parent and label in \p that.
    //!
    //! \param that the Forest for comparison.
    //!
    //! \returns whether or not the Forest objects are equal.
    [[nodiscard]] bool operator==(Forest const& that) const {
      return _parent == that._parent && _edge_label == that._edge_label;
    }

    //! \brief Compare Forest objects for inequality.
    //!
    //! This function returns the negation of `operator==(that)`
    //!
    //! \param that the Forest for comparison.
    //!
    //! \returns whether or not the Forest objects are not equal.
    [[nodiscard]] bool operator!=(Forest const& that) const {
      return !(*this == that);
    }

    //! \brief Add nodes to the Forest.
    //!
    //! This function adds \p n nodes to the forest, but no edges.
    //!
    //! \param n the number of nodes to add.
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
    Forest& add_nodes(size_t n);

    //! \brief Check if there are any nodes in the forest.
    //!
    //! This function returns \c true if there are 0 nodes in the forest, and
    //! \c false otherwise.
    //!
    //! \returns
    //! A value of type `bool`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant
    [[nodiscard]] bool empty() const noexcept {
      return _parent.empty();
    }

    //! \brief Set the parent and edge label for a node.
    //!
    //! This function defines the parent and the label of the edge from that
    //! parent to the node \p node to be \p parent and \p gen respectively.
    //!
    //! \param node   the node whose parent and label to set.
    //! \param parent the parent node.
    //! \param gen    the label of the edge from \p parent to \p node.
    //!
    //! \throws LibsemigroupsException if \p node or \p parent exceeds
    //! number_of_nodes().
    //!
    //! \complexity
    //! Constant
    //!
    //! \note The value of \p gen can be arbitrary, and so this argument is
    //! never checked.
    //!
    //! \warning No checks are performed on the arguments of this function. In
    //! particular, if \p node or \p parent is greater than or equal to
    //! \ref Forest::number_of_nodes, then bad things may happen.
    Forest& set_parent_and_label_no_checks(node_type  node,
                                           node_type  parent,
                                           label_type gen) {
      LIBSEMIGROUPS_ASSERT(node < number_of_nodes() || node == UNDEFINED);
      LIBSEMIGROUPS_ASSERT(parent < number_of_nodes() || parent == UNDEFINED);

      _parent[node]     = parent;
      _edge_label[node] = gen;
      return *this;
    }

    //! \brief Set the parent and edge label for a node.
    //!
    //! This function sets the parent of \p node to be \p parent, and the
    //! associated edge-label to be \p gen.
    //!
    //! \param node   the node whose parent and label to set.
    //! \param parent the parent node.
    //! \param gen    the label of the edge from \p parent to \p node.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p node or \p parent exceeds
    //! number_of_nodes().
    //! \throws LibsemigroupsException if \p node equals \p parent.
    //! \throws LibsemigroupsException if adding \p parent as the parent of
    //! \p node would result in a cycle.
    //!
    //! \complexity
    //! Constant
    //!
    //! \note The value of \p gen can be arbitrary, and so this argument is
    //! never checked.
    Forest& set_parent_and_label(node_type  node,
                                 node_type  parent,
                                 label_type gen);

    //! \brief Returns the number of nodes in the forest.
    //!
    //! This function returns the number of nodes in the forest.
    //!
    //! \returns
    //! The number of nodes.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant
    [[nodiscard]] size_t number_of_nodes() const noexcept {
      return _parent.size();
    }

    //! \brief Returns the parent of a node.
    //!
    //! This function returns the parent of the node \p i in the forest. The
    //! value \ref UNDEFINED is returned if \p i has no parent, i.e. when \p i
    //! is a root node.
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
    [[nodiscard]] node_type parent(node_type i) const {
      throw_if_node_out_of_bounds(i);
      return _parent[i];
    }

    //! \brief Returns the parent of a node.
    //!
    //! This function returns the parent of the node \p i in the forest. The
    //! value \ref UNDEFINED is returned if \p i has no parent, i.e. when \p i
    //! is a root node.
    //!
    //! \param i the node whose parent is sought.
    //!
    //! \returns
    //! A \ref node_type.
    //!
    //! \complexity
    //! Constant
    //!
    //! \warning No checks are performed on the arguments of this function.
    // not noexcept because std::vector::operator[] isn't.
    [[nodiscard]] node_type parent_no_checks(node_type i) const {
      return _parent[i];
    }

    //! \brief Returns the label of the edge from a node to its parent.
    //!
    //! This function returns the label of the edge from the parent of \p i to
    //! the node \p i. The value \ref UNDEFINED is returned if \p i has no
    //! parent, i.e. when \p i is a root node.
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
      throw_if_node_out_of_bounds(i);
      return _edge_label[i];
    }

    //! \brief Returns the label of the edge from a node to its parent.
    //!
    //! This function returns the label of the edge from the parent of \p i to
    //! the node \p i. The value \ref UNDEFINED is returned if \p i has no
    //! parent, i.e. when \p i is a root node.
    //!
    //! \param i the node whose label is sought.
    //!
    //! \returns
    //! A \ref label_type.
    //!
    //! \complexity
    //! Constant
    //!
    //! \warning No checks are performed on the arguments of this function.
    [[nodiscard]] label_type label_no_checks(node_type i) const {
      return _edge_label[i];
    }

    //! \brief Returns a const reference to the vector of parents.
    //!
    //! This function returns a const reference to the vector of parents in the
    //! Forest. The value in position \c i of this vector is the parent of node
    //! \c i. If the parent equals UNDEFINED, then node \c i is a root node.
    //!
    //! \returns
    //! A std::vector<node_type>.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] std::vector<node_type> const& parents() const noexcept {
      return _parent;
    }

    //! \brief Returns a const reference to the vector of edge labels.
    //!
    //! This function returns a const reference to the vector of edge labels in
    //! the Forest. The value in position \c i of this vector is the label of
    //! the edge from the parent of node \c i to \c i. If the parent equals
    //! UNDEFINED, then node \c i is a root node.
    //!
    //! \returns
    //! A std::vector<label_type>.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] std::vector<label_type> const& labels() const noexcept {
      return _edge_label;
    }

    //! \brief Store the labels of the edges on the path to a root node from
    //! a given node.
    //!
    //! This function writes labels of the edges on the path to a root node
    //! from node \p i to the iterator \p d_first.
    //!
    //! \tparam Iterator The type of the parameter, and the return type.
    //!
    //! \param d_first the output iterator.
    //! \param i the node.
    //!
    //! \returns An \c Iterator pointing one beyond the last letter inserted
    //! into \p d_first.
    //!
    //! \warning No checks are performed on the arguments of this function.
    //
    //! \sa forest::PathsToRoots
    template <typename Iterator>
    Iterator path_to_root_no_checks(Iterator d_first, node_type i) const;

    //! \brief Store the labels of the edges on the path from a root node to
    //! a given node.
    //!
    //! This function writes labels of the edges on the path from a root node
    //! to node \p i to the iterator \p d_first.
    //!
    //! \tparam Iterator The type of the parameter, and the return type.
    //!
    //! \param d_first the output iterator.
    //! \param i the node.
    //!
    //! \returns An \c Iterator pointing one beyond the last letter inserted
    //! into \p d_first.
    //!
    //! \warning No checks are performed on the arguments of this function.
    //!
    //! \sa forest::PathsFromRoots
    template <typename Iterator>
    Iterator path_from_root_no_checks(Iterator d_first, node_type i) const;

    //! \brief Throw an exception if a node is out of bound.
    //!
    //! This function throws an exception if the node \p v is out of points.
    //!
    //! \param v the node.
    void throw_if_node_out_of_bounds(node_type v) const;

   private:
    class const_iterator_path {
     private:
      node_type     _current_node;
      Forest const* _forest;

     public:
      using iterator_category = std::forward_iterator_tag;
      using value_type        = label_type;
      using difference_type   = std::ptrdiff_t;
      using pointer           = label_type*;
      using reference         = label_type&;

      const_iterator_path(Forest const* f, node_type n)
          : _current_node(n), _forest(f) {}

      const_iterator_path() = delete;

      const_iterator_path(const_iterator_path const& that)            = default;
      const_iterator_path& operator=(const_iterator_path const& that) = default;
      const_iterator_path(const_iterator_path&& that) noexcept        = default;
      const_iterator_path& operator=(const_iterator_path&& that) noexcept
          = default;

      ~const_iterator_path() = default;

      [[nodiscard]] value_type operator*() const;

      // pointer operator->() const {
      //   return TODO;
      // }

      // Pre-increment
      const_iterator_path& operator++();

      // Post-increment
      const_iterator_path operator++(int) {
        const_iterator_path tmp = *this;
        ++(*this);
        return tmp;
      }

      [[nodiscard]] bool operator==(const_iterator_path const& that) const;

      [[nodiscard]] bool operator!=(const_iterator_path const& that) const {
        return !(*this == that);
      }
    };

   public:
    //! \brief Returns a const iterator pointing at the first letter of a word
    //! from a given node to its root.
    //!
    //! This function returns a const iterator point at the first letter of the
    //! word from the node \p n to the root of the subtree of the Forest object
    //! containing \p n. The letters in this word correspond to labels of edges
    //! in the Forest, and not nodes.
    //!
    //! \param n the node.
    //!
    //! \returns A const iterator.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \warning No checks are performed on the arguments of this function. In
    //! particular, it is not checked whether or not \p n is less than
    //! \ref number_of_nodes.
    [[nodiscard]] const_iterator_path
    cbegin_path_to_root_no_checks(node_type n) const noexcept {
      return const_iterator_path(this, n);
    }

    //! \brief Returns a const iterator pointing one beyond the last letter in
    //! a word from a given node to its root.
    //!
    //! This function returns a const iterator point at one beyond the last
    //! letter of the word from the node \p n to the root of the subtree of the
    //! Forest object containing \p n. The letters in this word correspond to
    //! labels of edges in the Forest, and not nodes.
    //!
    //! \param n the node.
    //!
    //! \returns A const iterator.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \warning No checks are performed on the arguments of this function. In
    //! particular, it is not checked whether or not \p n is less than
    //! \ref number_of_nodes.
    [[nodiscard]] const_iterator_path
    cend_path_to_root_no_checks(node_type n) const noexcept {
      (void) n;
      return const_iterator_path(this, UNDEFINED);
    }

    //! \brief Returns a const iterator pointing at the first letter of a word
    //! from a given node to its root.
    //!
    //! This function returns a const iterator point at the first letter of the
    //! word from the node \p n to the root of the subtree of the Forest object
    //! containing \p n. The letters in this word correspond to labels of edges
    //! in the Forest, and not nodes.
    //!
    //! \param n the node.
    //!
    //! \returns A const iterator.
    //!
    //! \throws LibsemigroupsException if \p n is greater than or equal to
    //! \ref Forest::number_of_nodes.
    [[nodiscard]] const_iterator_path cbegin_path_to_root(node_type n) const {
      throw_if_node_out_of_bounds(n);
      return cbegin_path_to_root_no_checks(n);
    }

    //! \brief Returns a const iterator pointing one beyond the last letter in
    //! a word from a given node to its root.
    //!
    //! This function returns a const iterator point at one beyond the last
    //! letter of the word from the node \p n to the root of the subtree of the
    //! Forest object containing \p n. The letters in this word correspond to
    //! labels of edges in the Forest, and not nodes.
    //!
    //! \param n the node.
    //!
    //! \returns A const iterator.
    //!
    //! \throws LibsemigroupsException if \p n is greater than or equal to
    //! \ref Forest::number_of_nodes.
    [[nodiscard]] const_iterator_path cend_path_to_root(node_type n) const {
      throw_if_node_out_of_bounds(n);
      return cend_path_to_root_no_checks(n);
    }
  };  // class Forest

  //! \defgroup make_forest_group make<Forest>
  //! \ingroup word_graph_group
  //!
  //! \brief Safely construct a \ref Forest instance.
  //!
  //! This page contains documentation related to safely constructing a
  //! \ref Forest instance.
  //!
  //! \sa \ref make_group for an overview of possible uses of the `make`
  //! function.

  //! \ingroup make_forest_group
  //!
  //! \brief Construct a Forest from parents and labels.
  //!
  //! This function constructs a Forest from vector of parents and labels.
  //!
  //! \tparam Return the return type. Must satisfy
  //! `std::is_same<Return, Forest>`.
  //!
  //! \param parent the vector of parents of nodes in the Forest.
  //! \param edge_labels the vector of edge labels in the Forest.
  //!
  //! \returns A newly constructed Forest with parents \p parent and edge labels
  //! \p edge_labels.
  //!
  //! \throws LibsemigroupsException if any of the following hold:
  //! * \p parent and \p edge_labels have different sizes;
  //! * \p parent and \p edge_labels do not have the value \ref UNDEFINED in the
  //! same positions (these values indicate where the roots of the trees in the
  //! forest are located and so must coincide).
  //! * Forest::set_parent_and_label throws for `parent[i]` and `edge_labels[i]`
  //! for any value of `i`.
  template <typename Return>
  [[nodiscard]] enable_if_is_same<Return, Forest>
  make(std::vector<uint32_t> const& parent,
       std::vector<uint32_t> const& edge_labels);

  //! \ingroup make_forest_group
  //!
  //! \brief Construct a Forest from parents and labels.
  //!
  //! This function constructs a Forest from initializer lists of parents and
  //! labels.
  //!
  //! \tparam Return the return type. Must satisfy
  //! `std::is_same<Return, Forest>`.
  //!
  //! \param parent the initializer list of parents of nodes in the Forest.
  //! \param edge_labels the initializer list of edge labels in the Forest.
  //!
  //! \returns A newly constructed Forest with parents \p parent and edge labels
  //! \p edge_labels.
  //!
  //! \throws LibsemigroupsException if any of the following hold:
  //! * \p parent and \p edge_labels have different sizes;
  //! * \p parent and \p edge_labels do not have the value \ref UNDEFINED in the
  //! same positions (these values indicate where the roots of the trees in the
  //! forest are located and so must coincide).
  //! * Forest::set_parent_and_label throws for `parent[i]` and `edge_labels[i]`
  //! for any value of `i`.
  template <typename Return>
  [[nodiscard]] enable_if_is_same<Return, Forest>
  make(std::initializer_list<uint32_t> parent,
       std::initializer_list<uint32_t> edge_labels) {
    return make<Forest>(std::vector<uint32_t>(parent),
                        std::vector(edge_labels));
  }

  //! \relates Forest
  //!
  //! \brief Return a human readable representation of a Forest object.
  //!
  //! Return a human readable representation of a Forest object.
  //!
  //! \param f the Forest.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  [[nodiscard]] std::string to_human_readable_repr(Forest const& f);

  //! \ingroup word_graph_group
  //! \brief Helper functions for the Forest class.
  //!
  //! This page contains the documentation of some helper functions for the
  //! Forest class.
  namespace forest {
    //! \brief Modifies \p w to contain the labels of the edges on the path
    //! to a root node from \p n.
    //!
    //! This function modifies its first argument \p w in-place to contain the
    //! labels of the edges on the path to a root node from node \p n.
    //!
    //! \param f the forest.
    //! \param w value to contain the result.
    //! \param n the node.
    //!
    //! \warning No checks are performed on the arguments of this function.
    void path_to_root_no_checks(Forest const&     f,
                                word_type&        w,
                                Forest::node_type n);

    //! \brief Modifies \p w to contain the labels of the edges on the path
    //! from a root node to \p n.
    //!
    //! This function modifies its first argument \p w in-place to contain the
    //! labels of the edges on the path from a root node to the node \p n.
    //!
    //! \param f the forest.
    //! \param w value to contain the result.
    //! \param n the node.
    //!
    //! \warning No checks are performed on the arguments of this function.
    //!
    //! \sa PathsFromRoots
    void path_from_root_no_checks(Forest const&     f,
                                  word_type&        w,
                                  Forest::node_type n);

    //! \brief Returns a word containing the labels of the edges on the path
    //! to a root node from \p n.
    //!
    //! This function returns a word containing the labels of the edges on the
    //! path to a root node from node \p n.
    //!
    //! \param f the forest.
    //! \param n the node.
    //!
    //! \returns The word labelling the path from a root node to \p n.
    //!
    //! \warning No checks are performed on the arguments of this function.
    //!
    //! \sa PathsToRoots
    [[nodiscard]] word_type path_to_root_no_checks(Forest const&     f,
                                                   Forest::node_type n);

    //! \brief Returns a word containing the labels of the edges on the path
    //! from a root node to \p n.
    //!
    //! This function returns a word containing the labels of the edges on the
    //! path from a root node to the node \p n.
    //!
    //! \param f the forest.
    //! \param n the node.
    //!
    //! \returns The word labelling the path from a root node to \p n.
    //!
    //! \warning No checks are performed on the arguments of this function.
    //!
    //! \sa PathsFromRoots
    [[nodiscard]] word_type path_from_root_no_checks(Forest const&     f,
                                                     Forest::node_type n);

    //! \brief Modifies \p w to contain the labels of the edges on the path
    //! to a root node from \p n.
    //!
    //! This function modifies its first argument \p w in-place to contain the
    //! labels of the edges on the path to a root node from node \p n.
    //!
    //! \param f the forest.
    //! \param w value to contain the result.
    //! \param n the node.
    //!
    //! \throws LibsemigroupsException if \p n is greater than or equal to
    //! \ref Forest::number_of_nodes.
    //!
    //! \sa PathsToRoots
    void path_to_root(Forest const& f, word_type& w, Forest::node_type n);

    //! \brief Modifies \p w to contain the labels of the edges on the path
    //! from a root node to \p n.
    //!
    //! This function modifies its first argument \p w in-place to contain the
    //! labels of the edges on the path from a root node to the node \p n.
    //!
    //! \param f the forest.
    //! \param w value to contain the result.
    //! \param n the node.
    //!
    //! \throws LibsemigroupsException if \p n is greater than or equal to
    //! \ref Forest::number_of_nodes.
    //!
    //! \sa PathsFromRoots
    void path_from_root(Forest const& f, word_type& w, Forest::node_type n);

    //! \brief Returns a word containing the labels of the edges on the path
    //! to a root node from \p n.
    //!
    //! This function returns a word containing the labels of the edges on the
    //! path to a root node from node \p n.
    //!
    //! \param f the forest.
    //! \param n the node.
    //!
    //! \returns The word labelling the path from a root node to \p n.
    //!
    //! \throws LibsemigroupsException if \p n is greater than or equal to
    //! \ref Forest::number_of_nodes.
    //!
    //! \sa PathsToRoots
    [[nodiscard]] word_type path_to_root(Forest const& f, Forest::node_type n);

    //! \brief Returns a word containing the labels of the edges on the path
    //! from a root node to \p n.
    //!
    //! This function returns a word containing the labels of the edges on the
    //! path from a root node to the node \p n.
    //!
    //! \param f the forest.
    //! \param n the node.
    //!
    //! \returns The word labelling the path from a root node to \p n.
    //!
    //! \throws LibsemigroupsException if \p n is greater than or equal to
    //! \ref Forest::number_of_nodes.
    //!
    //! \sa PathsFromRoots
    [[nodiscard]] word_type path_from_root(Forest const&     f,
                                           Forest::node_type n);

    //! \brief Returns the depth of a node in the forest, n.e. the distance,
    //! in terms of the number of edges, from a root.
    //!
    //! This function returns the length of the word returned by
    //! \ref path_to_root_no_checks and \ref path_from_root_no_checks.
    //!
    //! \param f the Forest.
    //! \param n the node.
    //!
    //! \returns The depth of \p n.
    //!
    //! \warning No checks are performed on the arguments of this function.
    [[nodiscard]] size_t depth_no_checks(Forest const& f, Forest::node_type n);

    //! \brief Returns the depth of a node in the forest, i.e. the distance,
    //! in terms of the number of edges, from a root.
    //!
    //! This function returns the length of the word returned by
    //! \ref path_to_root_no_checks and \ref path_from_root_no_checks.
    //!
    //! \param f the Forest.
    //! \param n the node.
    //!
    //! \returns The depth of \p n.
    //!
    //! \throws LibsemigroupsException if \p n is out of bounds (i.e. it is
    //! greater than or equal to \ref Forest::number_of_nodes).
    [[nodiscard]] inline size_t depth(Forest const& f, Forest::node_type n) {
      f.throw_if_node_out_of_bounds(n);
      return depth_no_checks(f, n);
    }

    //! \brief Check if a node is the root of any tree in the Forest.
    //!
    //! This function returns \c true if the node \p n in the Forest \p f is a
    //! root node, and \c false if it is not.
    //!
    //! \param f the Forest.
    //! \param n the node.
    //!
    //! \returns Whether or not \p n is a root of \p f.
    //!
    //! \warning No checks are performed on the arguments of this function. In
    //! particular it is not checked whether or not \p n is a node of \p f.
    [[nodiscard]] inline bool is_root_no_checks(Forest const&     f,
                                                Forest::node_type n) {
      return f.parent_no_checks(n) == UNDEFINED;
    }

    //! \brief Check if a node is the root of any tree in the Forest.
    //!
    //! This function returns \c true if the node \p n in the Forest \p f is a
    //! root node, and \c false if it is not.
    //!
    //! \param f the Forest.
    //! \param n the node.
    //!
    //! \returns Whether or not \p n is a root of \p f.
    //!
    //! \throws LibsemigroupsException if \p n is out of bounds (i.e. it is
    //! greater than or equal to \ref Forest::number_of_nodes).
    [[nodiscard]] inline bool is_root(Forest const& f, Forest::node_type n) {
      f.throw_if_node_out_of_bounds(n);
      return is_root_no_checks(f, n);
    }

    //! \brief Returns the maximum label of any edge in a Forest.
    //!
    //! This function returns the maximum label of any edge in the Forest \p f
    //! or \ref UNDEFINED if there are no edges.
    //!
    //! \param f the Forest.
    //!
    //! \returns The maximum label or \ref UNDEFINED.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    [[nodiscard]] Forest::label_type max_label(Forest const& f);

    //! \brief Check whether a Forest object is well-defined.
    //!
    //! This function returns \c true if the Forest \p f is well-defined,
    //! meaning that it contains no cycles. It is not possible to create a
    //! Forest with cycles using \ref Forest::set_parent_and_label (which will
    //! throw if such a cycle is introduced), but it is possible using
    //! Forest with cycles using \ref Forest::set_parent_and_label_no_checks.
    //!
    //! \param f the Forest
    //! \returns Whether or not \p f is valid.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \sa Forest::throw_if_not_acyclic
    [[nodiscard]] bool is_forest(Forest const& f);

    namespace detail {
      //! So that doc is generated
      //!
      //! Generate doc please doxygen.
      class PathsFromToRootsCommon {
       public:
#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
        // We don't document these because we have to redeclare them in
        // PathsFromRoots and PathsToRoots anyway.
        using size_type   = typename std::vector<word_type>::size_type;
        using output_type = word_type const&;
        using node_type   = Forest::node_type;
        static constexpr bool is_finite     = true;
        static constexpr bool is_idempotent = true;
#endif

       protected:
        node_type              _current_node;
        word_type              _current_path;
        std::vector<size_type> _depths;
        Forest const*          _forest;
        word_type              _visited;

        // Lazily compute depth of a node in the tree
        size_type depth(node_type n);

       public:
        //! \brief Deleted.
        //!
        //! A Forest object is required to construct or initialise a
        //! PathsFromToRootsCommon object.
        PathsFromToRootsCommon() = delete;

        //! Default copy constructor.
        PathsFromToRootsCommon(PathsFromToRootsCommon const&) = default;

        //! Default move constructor.
        PathsFromToRootsCommon(PathsFromToRootsCommon&&) = default;

        //! Default copy assignment operator.
        PathsFromToRootsCommon& operator=(PathsFromToRootsCommon const&)
            = default;

        //! Default move assignment operator.
        PathsFromToRootsCommon& operator=(PathsFromToRootsCommon&&) = default;

        //! \brief Construct from a Forest.
        //!
        //! This function constructs a new PathsFromToRootsCommon for the Forest
        //! \p f. The newly constructed object does not copy \p f and is not
        //! valid if \p f is destroyed.
        //!
        //! \param f the Forest.
        //!
        //! \exceptions
        //! \no_libsemigroups_except
        explicit PathsFromToRootsCommon(Forest const& f);

        //! \brief Re-initialize from a Forest.
        //!
        //! This function re-initializes a PathsFromToRootsCommon so that its
        //! underlying Forest object is \p f. This puts the
        //! PathsFromToRootsCommon object back into the same state it would have
        //! been in if it had been newly constructed from \p f.
        //!
        //! \param f the Forest.
        //!
        //! \exceptions
        //! \no_libsemigroups_except
        PathsFromToRootsCommon& init(Forest const& f);

        //! \brief Get the current target of the path.
        //!
        //! This function returns the current target of the path returned by
        //! \ref get.
        //!
        //! \returns The target of the current path.
        //!
        //! \exceptions
        //! \noexcept
        [[nodiscard]] node_type target() const noexcept {
          return _current_node;
        }

        //! \brief Returns a reference to the current path.
        //!
        //! This function returns a const reference to the current path from the
        //! root of the tree containing \ref target to \ref target.
        //!
        //! \returns A const reference to the current path.
        //!
        //! \exceptions
        //! \noexcept
        [[nodiscard]] output_type get() const noexcept {
          return _current_path;
        }

        //! \brief Check if the range is exhausted.
        //!
        //! This function returns \c true if there are no more paths in the
        //! range, and \c false otherwise.
        //!
        //! \returns Whether or not the range is exhausted.
        [[nodiscard]] bool at_end() const noexcept {
          return _forest->empty()
                 || _current_node >= _forest->number_of_nodes();
        }

        //! \brief Get the size of the range.
        //!
        //! This function returns the number of paths in the range. The output
        //! is identical to that of \ref count, and is
        //! included for compatibility with [rx::ranges][].
        //!
        //! [rx::ranges]: https://github.com/simonask/rx-ranges
        //!
        //! \returns the number of paths in the range.
        [[nodiscard]] size_type size_hint() const noexcept {
          if (at_end()) {
            return 0;
          }
          return _forest->number_of_nodes() - _current_node;
        }

        //! \brief Get the size of the range.
        //!
        //! This function returns the number of paths in the range.
        //!
        //! \returns the number of paths in the range.
        [[nodiscard]] size_type count() const noexcept {
          return size_hint();
        }

        //! \brief Returns the underlying Forest object.
        //!
        //! This function returns the Forest object used to constructor or
        //! initialise a PathsFromToRootsCommon object.
        //!
        //! \returns A const reference to a Forest object.
        //!
        //! \exceptions
        //! \noexcept
        [[nodiscard]] Forest const& forest() const noexcept {
          return *_forest;
        }
      };  // class PathsFromToRootsCommonRoots
    }     // namespace detail

    //! \brief Range for iterating through paths in a Forest.
    //!
    //! \hideinheritancegraph
    //!
    //! Defined in `forest.hpp`.
    //!
    //! This class represents a range object that facilitates iterating
    //! through the paths from the roots in a Forest object to every node.
    //! These nodes are taken in numerical order, so the first value returned
    //! by \ref get is the word from a root to node \c 0, after \ref next is
    //! called, \ref get returns the word from a root to node \c 1, and so on.
    //!
    //! The point of this class is to permit more efficient iteration over
    //! many paths in a Forest object than
    //! \ref Forest::path_from_root_no_checks (and its associated helper
    //! functions). \ref Forest::path_from_root_no_checks traverses the Forest
    //! from the given node to the root of its tree. If the path from nodes
    //! \c m and \c n to the root of their tree share a long common prefix, then
    //! this prefix will be traversed twice in two calls to
    //! \ref Forest::path_from_root_no_checks. PathsFromRoots avoids this by
    //! finding the least common ancestor of \c m and \c n, so that the prefix
    //! is not re-traversed. This works best when the nodes in the Forest are
    //! specified in a sensible order (such as via a depth-first or
    //! breadth-first traversal).
    //!
    //! So that the PathsFromRoots class can be used efficiently with the
    //! functionality of [rx::ranges][], the usual naming conventions in
    //! `libsemigroups` are not used for the member functions:
    //! * \ref get
    //! * \ref next
    //! * \ref at_end
    //! * \ref size_hint
    //! * \ref count
    //!
    //! of PathsFromRoots. In particular, none of these member functions check
    //! their arguments, but they do not have the suffix `_no_checks`.
    //!
    //! [rx::ranges]: https://github.com/simonask/rx-ranges
    class PathsFromRoots : public detail::PathsFromToRootsCommon {
      using detail::PathsFromToRootsCommon::depth;

     public:
      //! Alias for the size type.
      using size_type = typename std::vector<word_type>::size_type;

      //! The type of the output of a PathsFromRoots object.
      using output_type = word_type const&;

      //! The type of nodes in the underlying Forest.
      using node_type = Forest::node_type;

      //! Value indicating that the range is finite.
      static constexpr bool is_finite = true;

      //! Value indicating that if get() is called twice on a PathsFromRoots
      //! object that is not changed between the two calls, then the return
      //! value of get() is the same both times.
      static constexpr bool is_idempotent = true;

      using detail::PathsFromToRootsCommon::at_end;
      using detail::PathsFromToRootsCommon::count;
      using detail::PathsFromToRootsCommon::forest;
      using detail::PathsFromToRootsCommon::get;
      using detail::PathsFromToRootsCommon::init;
      using detail::PathsFromToRootsCommon::PathsFromToRootsCommon;
      using detail::PathsFromToRootsCommon::size_hint;
      using detail::PathsFromToRootsCommon::target;

      //! \brief Advance to the next path in the range.
      //!
      //! Advance to the current path in the range. If \ref Paths::at_end
      //! returns \c true, then this function does nothing.
      void next();

      //! \brief Skip a number of paths in the range.
      //!
      //! This function skips \p n paths in the range. If \p n is \c 1, then
      //! this is the same as calling \ref next. If \p n is \c 0, then this
      //! function does nothing.
      //!
      //! \param n the number of paths to skip.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      PathsFromRoots& skip_n(size_type n);

      //! \brief Returns an input iterator pointing to the first word in the
      //! range.
      //!
      //! This function returns an input iterator pointing to the first word
      //! in a PathsFromRoots object.
      //!
      //! \returns An input iterator.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \note The return type of \ref end might be different from the return
      //! type of \ref begin.
      //!
      //! \warning
      //! If using a PathsFromRoots object \c paths in a range-based
      //! for-loop such as:
      //! \code for (auto const& path : paths) {
      //!   ...
      //! }
      //! \endcode
      //! the object \c paths is copied, and so will be constant inside the
      //! for-loop. For example, in
      //! \code
      //! for (auto const& path : paths) {
      //!   auto t = paths.target();
      //! }
      //! \endcode
      //! The value of \c t will be constant, even though the value of \c path
      //! may change.
      //!
      //! \sa \ref end.
      // This can't go in the base class because the base class has no "next"
      // mem fn and so rx::begin/end don't compile.
      [[nodiscard]] auto begin() noexcept {
        return rx::begin(*this);
      }

      //! \brief Returns an input iterator pointing one beyond the last word
      //! in the range.
      //!
      //! This function returns an input iterator pointing one beyond the last
      //! word in a PathsFromRoots object.
      //!
      //! \returns An input iterator.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \note The return type of \ref end might be different from the return
      //! type of \ref begin.
      //!
      //! \warning Please read the warning in \ref begin.
      //!
      //! \sa \ref begin.
      // This can't go in the base class because the base class has no "next"
      // mem fn and so rx::begin/end don't compile.
      [[nodiscard]] auto end() noexcept {
        return rx::end(*this);
      }
    };  // class PathsFromRoots

    //! \brief Range for iterating through paths in a Forest.
    //!
    //! \hideinheritancegraph
    //!
    //! Defined in `forest.hpp`.
    //!
    //! This class represents a range object that facilitates iterating
    //! through the paths from every node to the root of its subtree in a Forest
    //! object. These nodes are taken in numerical order, so the first value
    //! returned by \ref get is the word to a root from node \c 0, after
    //! \ref next is called, \ref get returns the word to a root from node \c 1,
    //! and so on.
    //!
    //! The point of this class is to permit more efficient iteration over many
    //! paths in a Forest object than \ref Forest::path_to_root_no_checks (and
    //! its associated helper functions). \ref Forest::path_to_root_no_checks
    //! traverses the Forest from the given node to the root of its tree. If the
    //! path from nodes \c m and \c n to the root of their tree share a long
    //! common suffix, then this suffix will be traversed twice in two calls to
    //! \ref Forest::path_to_root_no_checks. PathsToRoots avoids this by finding
    //! the least common ancestor of \c m and \c n, so that the suffix is not
    //! re-traversed. This works best when the nodes in the Forest are specified
    //! in a sensible order (such as via a depth-first or breadth-first
    //! traversal).
    //!
    //! So that the PathsToRoots class can be used efficiently with the
    //! functionality of [rx::ranges][], the usual naming conventions in
    //! `libsemigroups` are not used for the member functions:
    //! * \ref get
    //! * \ref next
    //! * \ref at_end
    //! * \ref size_hint
    //! * \ref count
    //!
    //! of PathsToRoots. In particular, none of these member functions check
    //! their arguments, but they do not have the suffix `_no_checks`.
    //!
    //! [rx::ranges]: https://github.com/simonask/rx-ranges
    class PathsToRoots : public detail::PathsFromToRootsCommon {
      using detail::PathsFromToRootsCommon::depth;

     public:
      //! Alias for the size type.
      using size_type = typename std::vector<word_type>::size_type;

      //! The type of the output of a PathsToRoots object.
      using output_type = word_type const&;

      //! The type of nodes in the underlying Forest.
      using node_type = Forest::node_type;

      //! Value indicating that the range is finite.
      static constexpr bool is_finite = true;

      //! Value indicating that if get() is called twice on a PathsToRoots
      //! object that is not changed between the two calls, then the return
      //! value of get() is the same both times.
      static constexpr bool is_idempotent = true;

      using detail::PathsFromToRootsCommon::at_end;
      using detail::PathsFromToRootsCommon::count;
      using detail::PathsFromToRootsCommon::forest;
      using detail::PathsFromToRootsCommon::get;
      using detail::PathsFromToRootsCommon::init;
      using detail::PathsFromToRootsCommon::PathsFromToRootsCommon;
      using detail::PathsFromToRootsCommon::size_hint;
      using detail::PathsFromToRootsCommon::target;

      //! \brief Advance to the next path in the range.
      //!
      //! This function advances to the current path in the range. If
      //! \ref Paths::at_end returns \c true, then this function does nothing.
      void next();

      //! \brief Skip a number of paths in the range.
      //!
      //! This function skips \p n paths in the range. If \p n is \c 1, then
      //! this is the same as calling \ref next. If \p n is \c 0, then this
      //! function does nothing.
      //!
      //! \param n the number of paths to skip.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      PathsToRoots& skip_n(size_type n);

      //! \brief Returns an input iterator pointing to the first word in the
      //! range.
      //!
      //! This function returns an input iterator pointing to the first word
      //! in a PathsToRoots object.
      //!
      //! \returns An input iterator.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \note The return type of \ref end might be different from the return
      //! type of \ref begin.
      //!
      //! \warning
      //! If using a PathsToRoots object \c paths in a range-based
      //! for-loop such as:
      //! \code for (auto const& path : paths) {
      //!   ...
      //! }
      //! \endcode
      //! the object \c paths is copied, and so will be constant inside the
      //! for-loop. For example, in
      //! \code
      //! for (auto const& path : paths) {
      //!   auto t = paths.target();
      //! }
      //! \endcode
      //! The value of \c t will be constant, even though the value of \c path
      //! may change.
      //!
      //! \sa \ref end.
      // This can't go in the base class because the base class has no "next"
      // mem fn and so rx::begin/end don't compile.
      [[nodiscard]] auto begin() noexcept {
        return rx::begin(*this);
      }

      //! \brief Returns an input iterator pointing one beyond the last word
      //! in the range.
      //!
      //! This function returns an input iterator pointing one beyond the last
      //! word in a PathsToRoots object.
      //!
      //! \returns An input iterator.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \note The return type of \ref end might be different from the return
      //! type of \ref begin.
      //!
      //! \warning Please read the warning in \ref begin.
      //!
      //! \sa \ref begin.
      // This can't go in the base class because the base class has no "next"
      // mem fn and so rx::begin/end don't compile.
      [[nodiscard]] auto end() noexcept {
        return rx::end(*this);
      }
    };  // class PathsToRoots

    //! \brief Returns a \ref Dot object representing a Forest.
    //!
    //! This function returns a \ref Dot object representing the Forest
    //! \p f.
    //!
    //! \param f the Forest.
    //!
    //! \returns A \ref Dot object.
    [[nodiscard]] Dot dot(Forest const& f);

    //! \brief Returns a \ref Dot object representing a Forest.
    //!
    //! This function returns a \ref Dot object representing the Forest
    //! \p f. If \p labels is not empty, then each node is labelled with the
    //! path from that node to the root of its tree with each letter replaced
    //! by the string in the corresponding position of \p labels. If \p labels
    //! is empty, then the nodes are not labelled by their paths.
    //!
    //! \param f the Forest.
    //! \param labels substitute for each edge label.
    //!
    //! \returns A \ref Dot object.
    //!
    //! \throws LibsemigroupsException if the size of \p labels is not the
    //! same as the \ref max_label plus one.
    Dot dot(Forest const& f, std::vector<std::string> const& labels);

  }  // namespace forest

  //! \brief Return a human readable representation of a PathsToRoots object.
  //!
  //! Return a human readable representation of a PathsToRoots object.
  //!
  //! \param ptr the PathsToRoots object.
  //! \param sep separator to use between \c forest and \c PathsToRoots
  //! (default: `"::"`).
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  [[nodiscard]] std::string
  to_human_readable_repr(forest::PathsToRoots const& ptr,
                         std::string const&          sep = "::");

  //! \brief Return a human readable representation of a PathsFromRoots object.
  //!
  //! Return a human readable representation of a PathsFromRoots object.
  //!
  //! \param pfr the PathsFromRoots object.
  //! \param sep separator to use between \c forest and \c PathsFromRoots
  //! (default: `"::"`).
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  [[nodiscard]] std::string
  to_human_readable_repr(forest::PathsFromRoots const& pfr,
                         std::string const&            sep = "::");

}  // namespace libsemigroups

namespace fmt {
  template <>
  struct formatter<libsemigroups::Forest> : formatter<std::string> {
    //! \ingroup word_graph_group
    //!
    //! \brief Custom formatter for Forest objects.
    //!
    //! This is the format function of a custom formatter for
    //! [fmt](https://fmt.dev/11.1/) and the libsemigroups::Forest class.
    //!
    //! The intention is to provide a string representation that could be
    //! used to reconstruct the libsemigroups::Forest object. For a more
    //! human readable representation see
    //! \ref libsemigroups::to_human_readable_repr(libsemigroups::Forest
    //! const&) "to_human_readable_repr".
    //!
    //! \tparam FormatContext the type of the context provided in the second
    //! argument.
    //!
    //! \param f the forest to format.
    //! \param ctx the format context.
    //!
    //! \returns The formatted string representation of \p f.
    template <typename FormatContext>
    auto format(libsemigroups::Forest const& f, FormatContext& ctx) const {
      return formatter<string_view>::format(
          fmt::format("{{{}, {}}}", f.parents(), f.labels()), ctx);
    }
  };
}  // namespace fmt

#include "forest.tpp"

#endif  // LIBSEMIGROUPS_FOREST_HPP_

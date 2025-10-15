//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 Nadim Searight
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

// This file contains an implementation of word graph views, a thin layer over
// word graphs exposing a chosen range of nodes

#ifndef LIBSEMIGROUPS_WORD_GRAPH_VIEW_HPP_
#define LIBSEMIGROUPS_WORD_GRAPH_VIEW_HPP_

#include <type_traits>  // for is_integral
#include <utility>      // for pair

#include "detail/containers.hpp"
#include "word-graph.hpp"  // for word_graph pointer

#include "detail/int-range.hpp"  // for IntRange

namespace libsemigroups {
  //! \ingroup word_graph_group
  //!
  //! \brief non-owning view over a specified range of WordGraph nodes.
  //!
  //! Defined in `word-graph-view.hpp`.
  //!
  //! This class provides a wrapper around WordGraph to allow a non-owning view
  //! over a specified range of WordGraph nodes.
  //!
  //! \tparam Node the type of the nodes in the word graph, must be an unsigned
  //! integer type.
  template <typename Node>
  class WordGraphView {
   public:
    static_assert(std::is_integral<Node>(),
                  "the template parameter Node must be an integral type!");
    static_assert(
        std::is_unsigned<Node>(),
        "the template parameter Node must be an unsigned integral type!");

    //! \brief The type of nodes in a word graph.
    using node_type = Node;

    //! \brief The type of edge labels in a word graph.
    using label_type = Node;

    //! \brief Unsigned integer type.
    using size_type = std::size_t;

    //! \brief The type of an iterator pointing to the nodes of the word graph
    //! view.
    using const_iterator_nodes =
        typename detail::IntRange<Node>::const_iterator;

    //! \brief The type of a reverse iterator pointing to the nodes of a word
    //! graph view.
    using const_reverse_iterator_nodes =
        typename detail::IntRange<Node>::const_reverse_iterator;

    //! \brief The type of an iterator pointing to the targets of a node.
    using const_iterator_targets =
        typename detail::DynamicArray2<Node>::const_iterator;

   private:
    WordGraph<Node> const* _graph;
    node_type              _start;
    node_type              _end;

    constexpr node_type to_graph(node_type n) const noexcept;

    constexpr node_type to_view(node_type n) const noexcept;

    constexpr auto
    to_view(rx::iterator_range<const_iterator_targets> it) const {
      return it
             | rx::transform([this](auto elem) { return this->to_view(elem); });
    }

    void to_view(std::pair<label_type, node_type>& in) const {
      // this is designed to operate on pairs of <label, node>
      // so does not modify the first element
      in.second = to_view(in.second);
    }

   public:
    //////////////////////////////////////////////////////////////////////////
    // Constructors + initialisers
    //////////////////////////////////////////////////////////////////////////

    //! \brief Construct from a WordGraph and range of nodes.
    //!
    //! This function is used to construct a WordGraphView from a WordGraph over
    //! the range of nodes from \p start to \p end.
    //!
    //! \param graph underlying WordGraph object.
    //! \param start the first node in the range.
    //! \param end one beyond the last node in the range.
    WordGraphView(WordGraph<Node> const& graph, size_type start, size_type end)
        : _graph(&graph), _start(start), _end(end) {}

    //! \brief Construct from a WordGraph.
    //!
    //! This function is used to construct a WordGraphView from a WordGraph over
    //! all of the nodes in \p graph.
    //!
    //! \param graph underlying WordGraph object.
    explicit WordGraphView(WordGraph<Node> const& graph)
        : _graph(&graph), _start(0), _end(graph.number_of_nodes()) {}

    //! \brief Construct from a WordGraph with another node type.
    //!
    //! This function can be used to construct a WordGraphView<Node> as a copy
    //! of a WordGraphView<OtherNode> so long as `sizeof(OtherNode) <=
    //! sizeof(Node)`.
    //!
    //! \tparam OtherNode the type of the nodes of \p that.
    //!
    //! \param that the word graph view to copy.
    //!
    //! \note Any edge with target \ref UNDEFINED in \p that will have target
    //! `static_cast<Node>(UNDEFINED)` in the constructed word graph.
    template <typename OtherNode>
    WordGraphView(WordGraphView<OtherNode> const& that)
        : WordGraphView(that.number_of_nodes(), that.out_degree()) {
      static_assert(sizeof(OtherNode) <= sizeof(Node));
      init(that);
    }

    //! \brief Construct empty object for future assignment.
    WordGraphView() : _graph(nullptr), _start(), _end() {}

    //! \brief Default copy constructor.
    WordGraphView(WordGraphView<Node> const&) = default;

    //! \brief Default move constructor.
    WordGraphView(WordGraphView<Node>&&) = default;

    ~WordGraphView() = default;

    //! \brief Reshape this view over the same graph.
    //!
    //! This function is used to reshape \c this to be a view over the range
    //! from \p start to \p end.
    //!
    //! \param start the first node in the range.
    //! \param end one beyond the last node in the range.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    WordGraphView& init(size_type start, size_type end) {
      _start = start;
      _end   = end;
      return *this;
    }

    //! \brief Reassign this view to a different graph, keeping the same
    //! shape.
    //!
    //! This function is used to change the underling WordGraph of a
    //! WordGraphView, whilst keeping the same range of nodes.
    //!
    //! \tparam OtherNode the type of the nodes of \p that.
    //!
    //! \param that the new WordGraph.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename OtherNode>
    WordGraphView<OtherNode>& init(WordGraph<OtherNode> const& that) {
      static_assert(sizeof(OtherNode) <= sizeof(Node));
      _graph = &that;
      return this;
    }

    //////////////////////////////////////////////////////////////////////////
    // Member functions
    //////////////////////////////////////////////////////////////////////////

    //! \brief The number of nodes that this view ranges over.
    //!
    //! \returns
    //! The number of nodes in in the view.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] size_type number_of_nodes() const noexcept {
      return _end - _start;
    }

    //! \brief The number of edges in the underlying graph.
    //!
    //! \returns
    //! The number of edges in in the underlying graph.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! The same as \ref WordGraph::number_of_edges
    //!
    //! \sa WordGraph::number_of_edges
    [[nodiscard]] size_type number_of_edges() const noexcept;

    //! \brief The index in the underlying graph of the first node in the view.
    //!
    //! \returns
    //! The index of the node.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] node_type start_node() const noexcept {
      return _start;
    }

    //! \brief The index in the underlying graph of one beyond the final node in
    //! the view.
    //!
    //! \returns
    //! The index of the node.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] node_type end_node() const noexcept {
      return _end;
    }

    //! \brief Returns the out degree.
    //!
    //! This function returns the number of edge labels in the word graph.
    //!
    //! \returns The number of edge labels, type \c size_type.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] size_type out_degree() const noexcept {
      return _graph->out_degree();
    }

    //! \brief Returns a random access iterator pointing at the first node of
    //! the word graph.
    //!
    //! This function returns a random access iterator pointing at the first
    //! in the range of this word graph view.
    //!
    //! \returns
    //! An \ref const_iterator_nodes.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    const_iterator_nodes cbegin_nodes() const noexcept {
      return detail::IntRange<node_type>(0, number_of_nodes()).cbegin();
    }

    //! \brief Returns a random access iterator pointing one past the last node
    //! of the range of this word graph view.
    //!
    //! This function returns a random access iterator pointing one beyond the
    //! last node in the word graph.
    //!
    //! \returns
    //! An \ref const_iterator_nodes.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] const_iterator_nodes cend_nodes() const noexcept {
      return detail::IntRange<node_type>(0, number_of_nodes()).cend();
    }

    //! \brief Returns a random access iterator pointing at the target of
    //! the edge with label \p 0 incident to a given node.
    //!
    //! This function returns a random access iterator pointing at the
    //! target of the edge with label \c 0 incident to the source node
    //! \p source. This target might equal \ref UNDEFINED.
    //!
    //! \param source the source node in the word graph.
    //!
    //! \returns
    //! A \ref const_iterator_targets.
    //!
    //! \throws LibsemigroupsException if \p source is out of range (i.e.
    //! greater than or equal to \ref number_of_nodes).
    //!
    //! \complexity
    //! Constant.
    // Not noexcept because throw_if_node_out_of_bounds isn't
    [[nodiscard]] auto cbegin_targets(node_type source) const {
      throw_if_node_out_of_bounds(source);
      return rx::begin(targets_no_checks(source));
    }

    //! \brief Returns a random access iterator pointing at the target of
    //! the edge with label \p 0 incident to a given node.
    //!
    //! This function returns a random access iterator pointing at the
    //! target of the edge with label \c 0 incident to the source node
    //! \p source. This target might equal \ref UNDEFINED.
    //!
    //! \param source a node in the word graph.
    //!
    //! \returns
    //! A \ref const_iterator_targets.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \warning
    //! No checks whatsoever on the validity of the arguments are performed.
    //!
    //! \sa
    //! \ref cbegin_targets.
    [[nodiscard]] auto
    cbegin_targets_no_checks(node_type source) const noexcept {
      return rx::begin(targets_no_checks(source));
    }

    //! \brief Returns a random access iterator pointing one beyond the target
    //! of the edge with label `out_degree() - 1` incident to a given node.
    //!
    //! This function returns a random access iterator pointing one beyond the
    //! target of the edge with label `out_degree() - 1` incident to the source
    //! node \p source. This target might equal \ref UNDEFINED.
    //!
    //! \param source a node in the word graph.
    //!
    //! \returns
    //! A \ref const_iterator_targets.
    //!
    //! \throws LibsemigroupsException if \p source is out of range (i.e.
    //! greater than or equal to \ref number_of_nodes).
    //!
    //! \complexity
    //! Constant.
    // Not noexcept because throw_if_node_out_of_bounds isn't
    [[nodiscard]] auto cend_targets(node_type source) const {
      throw_if_node_out_of_bounds(source);
      return rx::end(targets_no_checks(source));
    }

    //! \brief Returns a random access iterator pointing one beyond the target
    //! of the edge with label `out_degree() - 1` incident to a given node.
    //!
    //! This function returns a random access iterator pointing one beyond the
    //! target of the edge with label `out_degree() - 1` incident to the source
    //! node \p source. This target might equal \ref UNDEFINED.
    //!
    //! \param source a node in the word graph.
    //!
    //! \returns
    //! A \ref const_iterator_targets.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \warning
    //! No checks whatsoever on the validity of the arguments are performed.
    //!
    //! \sa
    //! \ref cend_targets.
    [[nodiscard]] auto cend_targets_no_checks(node_type source) const noexcept {
      return rx::end(targets_no_checks(source));
    }

    //! \brief Returns a range object containing all nodes in a word graph.
    //!
    //! This function returns a range object containing all the nodes in a
    //! word graph view
    //!
    //! \returns A range object.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] auto nodes() const noexcept {
      return rx::iterator_range(cbegin_nodes(), cend_nodes());
    }

    //! \brief Returns a range object containing all labels of edges in a word
    //! graph.
    //!
    //! This function returns a range object containing all the labels of edges
    //! in a word graph.
    //!
    //! \returns A range object.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] auto labels() const noexcept {
      return _graph->labels();
    }

    //! \brief Returns a range object containing all the targets of edges with
    //! a given source.
    //!
    //! This function returns a range object containing all the targets of
    //! edges with source \p source.
    //!
    //! \param source the source node.
    //!
    //! \returns A range object.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \warning
    //! This function performs no checks whatsoever and assumes that \p source
    //! is a valid node of the word graph (i.e. it is not greater than or equal
    //! to \ref number_of_nodes).
    [[nodiscard]] auto targets_no_checks(node_type source) const noexcept {
      node_type translated = to_graph(source);
      return to_view(_graph->targets_no_checks(translated));
    }

    //! \brief Get the next target of an edge incident to a given node that
    //! doesn't equal \ref UNDEFINED.
    //!
    //! This function returns the next target of an edge with label greater
    //! than or equal to \p a that is incident to the node \p s.
    //!
    //! If `target(s, b)` equals \ref UNDEFINED for every value \c b in the
    //! range \f$[a, n)\f$, where \f$n\f$ is the return value of out_degree()
    //! then \c x.first and \c x.second equal \ref UNDEFINED.
    //!
    //! \param s the node.
    //! \param a the label.
    //!
    //! \returns
    //! Returns a std::pair
    //! \c x where:
    //! 1. \c x.first is adjacent to \p s via an edge labelled
    //!    \c x.second; and
    //! 2. \c x.second is the minimum value in the range \f$[a, n)\f$ such that
    //!    `target(s, x.second)` is not equal to \ref UNDEFINED
    //!    where \f$n\f$ is the return value of out_degree();
    //! If no such value exists, then `{UNDEFINED, UNDEFINED}` is returned.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! At worst \f$O(n)\f$ where \f$n\f$ equals out_degree().
    //!
    //! \warning
    //! This function performs no checks whatsoever and assumes that \p s
    //! is a valid node of the word graph (i.e. it is not greater than or equal
    //! to \ref number_of_nodes).
    [[nodiscard]] std::pair<label_type, node_type>
    next_label_and_target_no_checks(node_type s, label_type a) const;

    //! \brief Get the next target of an edge incident to a given node that
    //! doesn't equal \ref UNDEFINED.
    //!
    //! This function returns the next target of an edge with label greater
    //! than or equal to \p a that is incident to the node \p s.
    //!
    //! If `target(s, b)` equals \ref UNDEFINED for every value \c b in the
    //! range \f$[a, n)\f$, where \f$n\f$ is the return value of out_degree()
    //! then \c x.first and \c x.second equal \ref UNDEFINED.
    //!
    //! \param s the node.
    //! \param a the label.
    //!
    //! \returns
    //! Returns a std::pair
    //! \c x where:
    //! 1. \c x.first is adjacent to \p s via an edge labelled
    //!    \c x.second; and
    //! 2. \c x.second is the minimum value in the range \f$[a, n)\f$ such that
    //!    `target(s, x.second)` is not equal to \ref UNDEFINED
    //!    where \f$n\f$ is the return value of out_degree();
    //! If no such value exists, then `{UNDEFINED, UNDEFINED}` is returned.
    //!
    //! \throws LibsemigroupsException if \p s does not represent a node in
    //! \c this, or \p a is not a valid edge label.
    //!
    //! \complexity
    //! At worst \f$O(n)\f$ where \f$n\f$ equals out_degree().
    //!
    //! \sa next_label_and_target_no_checks.
    // Not noexcept because next_label_and_target_no_checks is not
    [[nodiscard]] std::pair<label_type, node_type>
    next_label_and_target(node_type s, label_type a) const;

    //! \brief Returns a range object containing all the targets of edges with
    //! a given source.
    //!
    //! This function returns a range object containing all the targets of
    //! edges with source \p source.
    //!
    //! \param source the source node.
    //!
    //! \returns A range object.
    //!
    //! \throws LibsemigroupsException if \p source is out of range (i.e. it is
    //! greater than or equal to \ref number_of_nodes).
    [[nodiscard]] auto targets(node_type source) const {
      throw_if_node_out_of_bounds(source);
      node_type translated = to_graph(source);
      return to_view(_graph->targets_no_checks(translated));
    }

    //! \brief Returns a range object containing pairs consisting of edge
    //! labels and target nodes.
    //!
    //! This function returns a range object containing all the edge labels and
    //! targets of edges with source \p source.
    //!
    //! \param source the source node.
    //!
    //! \returns A range object.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \warning
    //! This function performs no checks whatsoever and assumes that \p source
    //! is a valid node of the word graph (i.e. it is not greater than or equal
    //! to \ref number_of_nodes).
    [[nodiscard]] auto
    labels_and_targets_no_checks(node_type source) const noexcept {
      return rx::enumerate(targets_no_checks(source));
    }

    //! \brief Returns a range object containing pairs consisting of edge
    //! labels and target nodes.
    //!
    //! This function returns a range object containing all the edge labels and
    //! targets of edges with source \p source.
    //!
    //! \param source the source node.
    //!
    //! \returns A range object.
    //!
    //! \throws LibsemigroupsException if \p source is out of bounds.
    [[nodiscard]] auto labels_and_targets(node_type source) const {
      throw_if_node_out_of_bounds(source);
      return rx::enumerate(targets_no_checks(source));
    }

    //! \brief Get the target of the edge with given source node and label.
    //!
    //! This function returns the target of the edge with source node \p source
    //! and label \p a.
    //!
    //! \param source the node.
    //! \param a the label.
    //!
    //! \returns the node adjacent to \p source via the edge labelled \p a, or
    //! \ref UNDEFINED; both are values of type \ref node_type.
    //!
    //! \throws LibsemigroupsException if \p source or \p a is not
    //! valid.
    //!
    //! \complexity
    //! Constant.
    // Not noexcept because throw_if_node_out_of_bounds/label aren't
    [[nodiscard]] node_type target(node_type source, label_type a) const {
      throw_if_node_out_of_bounds(source);
      throw_if_label_out_of_bounds(a);
      return target_no_checks(source, a);
    }

    //! \brief Get the target of the edge with given source node and label.
    //!
    //! This function returns the target of the edge with source node \p source
    //! and label \p a.
    //!
    //! \param source the node.
    //! \param a the label.
    //!
    //! \returns
    //! Returns the node adjacent to \p source via the edge labelled \p a, or
    //! \ref UNDEFINED; both are values of type \ref node_type.
    //!
    //! \throws LibsemigroupsException if \p source or \p a is not
    //! valid.
    //!
    //! \complexity
    //! Constant.
    // Not noexcept because throw_if_node_out_of_bounds/label aren't
    [[nodiscard]] node_type target_no_checks(node_type  source,
                                             label_type a) const {
      node_type translated = to_graph(source);
      return to_view(_graph->target_no_checks(translated, a));
    }

    //////////////////////////////////////////////////////////////////////////
    // Operators
    //////////////////////////////////////////////////////////////////////////

    //! \brief Compares two word graph views to see if they are equal.
    //!
    //! This operator compares two views over two (not necessarily the same)
    //! word graph objects to see if the views are equal.
    //!
    //! \param that the WordGraphView to compare with.
    //!
    //! \returns True if this and that have the same number of nodes, out
    //! degree, and range over nodes with identical values and targets.
    [[nodiscard]] bool operator==(WordGraphView const& that) const;

    //! \brief Compares two word graph views to see if they are not equal.
    //!
    //! This operator compares two views over two (not necessarily the same)
    //! word graph objects to see if the views are not equal.
    //!
    //! \param that the WordGraphView to compare with.
    //!
    //! \returns True if this and that are not equal by \c ==.
    [[nodiscard]] bool operator!=(WordGraphView const& that) const {
      return !operator==(that);
    }

    //////////////////////////////////////////////////////////////////////////
    // Validation
    //////////////////////////////////////////////////////////////////////////

    //! \brief Throws if the target of any edge is out of bounds.
    //!
    //! This function throws if any target of any edge is out of
    //! bounds (i.e. is greater than or equal to \ref number_of_nodes,
    //! and not equal to \ref UNDEFINED).
    //!
    //! \throws LibsemigroupsException if any target of any edge is greater than
    //! or equal to \ref number_of_nodes and not equal to \ref UNDEFINED.
    void throw_if_any_target_out_of_bounds() const {
      throw_if_any_target_out_of_bounds(cbegin_nodes(), cend_nodes());
    }

    //! \brief Throws if the target of any edge with source in a given range is
    //! out of bounds.
    //!
    //! This function throws if any target of any edge whose source is
    //! in the range defined by \p first and \p last is out of bounds (i.e. is
    //! greater than or equal to \ref number_of_nodes, and not equal to
    //! \ref UNDEFINED).
    //!
    //! \tparam Iterator the type of the 2nd and 3rd arguments.
    //!
    //! \param first iterator pointing at the first node to check.
    //! \param last iterator pointing one beyond the last node to check.
    //!
    //! \throws LibsemigroupsException if any target of any edge with
    //! source in the range \p first to \p last is greater than or equal to
    //! \ref number_of_nodes and not equal to \ref UNDEFINED.
    //!
    //! \throws LibsemigroupsException if any node in the range \p first to
    //! \p last is out of bounds.
    template <typename Iterator>
    void throw_if_any_target_out_of_bounds(Iterator first, Iterator last) const;

    //! \brief Throws if a label is out of bounds.
    //!
    //! This function throws if the label \p a is out of bounds, i.e. it is
    //! greater than or equal to \ref out_degree.
    //!
    //! \param a the label to check.
    //!
    //! \throws LibsemigroupsException if the label \p a is out of bounds.
    // not noexcept because it throws an exception!
    void
    throw_if_label_out_of_bounds(typename WordGraph<Node>::label_type a) const;

    //! \brief Throws if word contains labels that are out of bounds.
    //!
    //! This function throws if any of the letters in \p word are out of bounds,
    //! i.e. if they are greater than or equal to \ref out_degree.
    //!
    //! \param word the word to check.
    //!
    //! \throws LibsemigroupsException if any value in \p word is out of
    //! bounds.
    void throw_if_label_out_of_bounds(word_type const& word) const {
      throw_if_label_out_of_bounds(word.cbegin(), word.cend());
    }

    //! \brief Throws if range constains labels that are out of bounds.
    //!
    //! This function throws if any of the letters in the word defined by
    //! \p first and \p last is out of bounds, i.e. if they are greater than or
    //! equal to \ref out_degree.
    //!
    //! \tparam Iterator the type of the arguments \p first and \p last.
    //!
    //! \param first iterator pointing at the first letter to check.
    //! \param last iterator pointing one beyond the last letter to check.
    //!
    //! \throws LibsemigroupsException if any value in the word defined by
    //! \p first and \p last is out of bounds.
    template <typename Iterator>
    void throw_if_label_out_of_bounds(Iterator first, Iterator last) const {
      std::for_each(first, last, [this](letter_type a) {
        this->throw_if_label_out_of_bounds(a);
      });
    }

    //! \brief Throws if any of the labels in vector of words is out of bounds.
    //!
    //! This function throws if any of the letters in any of the words in
    //! \p rules are out of bounds, i.e. if they are greater than or
    //! equal to \ref out_degree.
    //!
    //! \param rules the vector of words to check.
    //!
    //! \throws LibsemigroupsException if any value in any of the words in
    //! \p rules are out of bounds.
    void
    throw_if_label_out_of_bounds(std::vector<word_type> const& rules) const {
      std::for_each(rules.cbegin(), rules.cend(), [this](word_type const& w) {
        this->throw_if_label_out_of_bounds(w);
      });
    }

    //! \brief Throws if a node is out of bounds.
    //!
    //! This function throws if the node \p n is out of bounds
    //! i.e. if it is greater than or equal to \ref number_of_nodes.
    //!
    //! \tparam Node2 the type of the node \p n.
    //!
    //! \param n the node to check.
    //!
    //! \throws LibsemigroupsException if \p n is out of bounds.
    // not noexcept because it throws an exception!
    template <typename Node2>
    void throw_if_node_out_of_bounds(Node2 n) const;

    //! \brief Throws if any node in a range is out of bounds.
    //!
    //! This function throws if any node in the range from \p first to
    //! \p last is out of bounds i.e. if they are greater than or equal to
    //! \ref number_of_nodes.
    //!
    //! \tparam Iterator the type of the parameters \p first and \p last.
    //!
    //! \param first an iterator pointing at the first node to check.
    //! \param last an iterator pointing one beyond the last node to check.
    //!
    //! \throws LibsemigroupsException if any node in the range \p first to
    //! \p last is out of bounds.
    // not noexcept because it throws an exception!
    template <typename Iterator1, typename Iterator2>
    void throw_if_node_out_of_bounds(Iterator1 first, Iterator2 last) const {
      for (auto it = first; it != last; ++it) {
        throw_if_node_out_of_bounds(*it);
      }
    }
  };

  namespace word_graph {

    //! \brief Creates a word graph from a corresponding view, copying only the
    //! nodes contained within the view.
    //!
    //! This function creates a word graph from a corresponding view, copying
    //! only the nodes contained within the view.
    //!
    //! \tparam Node the type of node in the WordGraph.
    //!
    //! \param view the WordGraphView to construct the graph from.
    //!
    //! \returns The corresponding WordGraph<Node>.
    //!
    //! \throws LibsemigroupsException if the underlying graph has edges which
    //! crossed the boundaries of the view.
    template <typename Node>
    WordGraph<Node> graph_from_view(WordGraphView<Node> const& view);
  }  // namespace word_graph
}  // namespace libsemigroups
#include "word-graph-view.tpp"

#endif  // LIBSEMIGROUPS_WORD_GRAPH_VIEW_HPP_

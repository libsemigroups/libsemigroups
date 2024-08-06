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

// This file contains the declaration of a class implementing Gabow's algorithm
// for WordGraphs.

#ifndef LIBSEMIGROUPS_GABOW_HPP_
#define LIBSEMIGROUPS_GABOW_HPP_

#include <cstddef>   // for size_t
#include <iterator>  // for pair
#include <queue>     // for queue
#include <stack>     // for stack
#include <string>    // for string
#include <vector>    // for vector

#include "constants.hpp"   // for UNDEFINED, operator!=, Undefined
#include "debug.hpp"       // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"   // for LIBSEMIGROUPS_EXCEPTION
#include "forest.hpp"      // for Forest
#include "ranges.hpp"      // for iterator_range, transform
#include "word-graph.hpp"  // for WordGraph

namespace libsemigroups {

  //! \defgroup gabow_group Gabow
  //!
  //! This page contains information about the implementation in
  //! ``libsemigroups`` of Gabow's algorithm \cite Gab00 for computing the
  //! strongly connected components of a WordGraph.

  //! \ingroup gabow_group
  //!
  //! \brief Class implementing Gabow's algorithm for computing strongly
  //! connected components of a WordGraph.
  //!
  //! Defined in ``gabow.hpp``.
  //!
  //! Instances of this class can be used to compute, and provide information
  //! about, the strongly connected components of the WordGraph used to
  //! construct the instance. The strongly connected components are
  //! lazily evaluated when triggered by a relevant member function. The
  //! complexity of Gabow's algorithm is at most \f$O(mn)\f$ where \c m is
  //! WordGraph::number_of_nodes() and \c n is \ref WordGraph::out_degree().
  //!
  //! \tparam Node the type of the nodes of the underlying
  //! WordGraph.
  template <typename Node>
  class Gabow {
   public:
    //! Type of the nodes in the underlying WordGraph.
    using node_type = Node;

    //! Type of the edge labels in the underlying WordGraph.
    using label_type = typename WordGraph<node_type>::label_type;

    //! Size type used for indices of strongly connected components.
    using size_type = size_t;

   private:
    WordGraph<node_type> const*                 _graph;
    mutable std::vector<std::vector<node_type>> _comps;
    mutable bool                                _finished;
    mutable std::vector<size_type>              _id;
    mutable Forest                              _bckwd_forest;
    mutable bool                                _bckwd_forest_defined;
    mutable Forest                              _forwd_forest;
    mutable bool                                _forwd_forest_defined;

   public:
    //! \brief Deleted.
    //!
    //! To avoid the situation where the underlying WordGraph is not defined, it
    //! is not possible to default construct a Gabow object.
    Gabow() = delete;

    //! \brief Default copy constructor.
    //!
    //! Default copy constructor.
    Gabow(Gabow const&) = default;

    //! \brief Default move constructor.
    //!
    //! Default move constructor.
    Gabow(Gabow&&) = default;

    //! \brief Default copy assignment operator.
    //!
    //! Default copy assignment operator.
    Gabow& operator=(Gabow const&) = default;

    //! \brief Default move assignment operator.
    //!
    //! Default move assignment operator.
    Gabow& operator=(Gabow&&) = default;

    ~Gabow();

    //! \brief Construct from WordGraph
    //!
    //! This function constructs a Gabow object from the WordGraph \p wg.
    //!
    //! \warning The Gabow object only holds a reference to the underlying
    //! WordGraph \p wg, and so that object must outlive the corresponding Gabow
    //! object.
    //!
    //! \note This function does not trigger the computation of the strongly
    //! connected components.
    explicit Gabow(WordGraph<node_type> const& wg) {
      init(wg);
    }

    //! \brief Reinitialize a Gabow object.
    //!
    //! This function re-initializes a Gabow object so that it is in the same
    //! state as if it had just been constructed from \p wg.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning The Gabow object only holds a reference to the underlying
    //! WordGraph \p wg, and so that object must outlive the corresponding Gabow
    //! object.
    //!
    //! \note This function does not trigger the computation of the strongly
    //! connected components.
    Gabow& init(WordGraph<node_type> const& wg);

    //! \brief Get the id of a strongly connected component of a node.
    //!
    //! This function can be used to determine the id-number of a node in the
    //! underlying graph of a Gabow instance.
    //!
    //! \param n the node.
    //!
    //! \returns The id-number of the strongly connected component of \p n.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning This function does not check that its argument \p n is actually
    //! a node of the underlying word graph.
    //!
    //! \note This function triggers the computation of the strongly connected
    //! components (if they are not already known).
    [[nodiscard]] size_type id_no_checks(node_type n) const {
      run();
      return _id[n];
    }

    //! \brief Returns the id-number of the strongly connected component of a
    //! node.
    //!
    //! This function can be used to determine the id-number of a node in the
    //! underlying graph of a Gabow instance.
    //!
    //! \param n the node.
    //!
    //! \returns The id-number of the strongly connected component of \p n.
    //!
    //! \throws LibsemigroupsException if \p n is greater than or equal to
    //! `word_graph().number_of_nodes()`.
    //!
    //! \note This function triggers the computation of the strongly connected
    //! components (if they are not already known).
    // Not noexcept because validate_node isn't
    [[nodiscard]] size_type id(node_type n) const {
      run();
      validate_node(n);
      return id_no_checks(n);
    }

    //! \brief Returns a const reference to a vector of vectors containing the
    //! strongly connected components.
    //!
    //! This function returns a const reference to a vector of vectors
    //! containing all of the strongly connected components of the WordGraph
    //! (\ref word_graph) used to construct the Gabow instance.
    //!
    //! \returns
    //! A vector of vectors of node_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note This function triggers the computation of the strongly connected
    //! components (if they are not already known).
    [[nodiscard]] std::vector<std::vector<node_type>> const&
    components() const {
      run();
      return _comps;
    }

    //! \brief Returns a const reference to a vector containing the strongly
    //! connected component with given index.
    //!
    //! This function returns a const reference to a vector
    //! containing the strongly connected components with index \p i of the
    //! WordGraph (\ref word_graph) used to construct the Gabow instance.
    //!
    //! \param i the index of a strongly connected component.
    //!
    //! \returns A vector of node_type.
    //!
    //! \throws LibsemigroupsException if \p i is greater than or equal
    //! to \ref number_of_components.
    //!
    //! \note This function triggers the computation of the strongly connected
    //! components (if they are not already known).
    //!
    //! \sa \ref component_of to obtain the component of a node.
    [[nodiscard]] std::vector<node_type> const& component(size_type i) const {
      run();
      validate_scc_index(i);
      return _comps[i];
    }

    //! \brief Returns a const reference to a vector containing the strongly
    //! connected component with given index.
    //!
    //! This function returns a const reference to a vector
    //! containing the strongly connected components with index \p i of the
    //! WordGraph (\ref word_graph) used to construct the Gabow instance.
    //!
    //! \param i the index of a strongly connected component.
    //!
    //! \returns A vector of node_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning This function does not check that its argument \p i.
    //!
    //! \note This function triggers the computation of the strongly connected
    //! components (if they are not already known).
    //!
    //! \sa \ref component_of_no_checks to obtain the strongly connected
    //! component of a node.
    [[nodiscard]] std::vector<node_type>
    component_no_checks(size_type i) const {
      run();
      return _comps[i];
    }

    //! \brief Returns the number of strongly connected components.
    //!
    //! This function returns the number of strongly connected components of the
    //! underlying WordGraph (returned by \ref word_graph).
    //!
    //! \returns
    //! A `size_t`.
    //!
    //! \note This function triggers the computation of the strongly connected
    //! components (if they are not already known).
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    [[nodiscard]] size_t number_of_components() const {
      run();
      return _comps.size();
    }

    //! \brief Returns a range object consisting of roots of the strongly
    //! connected components.
    //!
    //! This function returns a range object consisting of roots of the strongly
    //! connected components.
    //!
    //! \returns
    //! A range object by value.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note This function triggers the computation of the strongly connected
    //! components (if they are not already known).
    // TODO(0) add reference to range doc when available
    [[nodiscard]] auto roots() const {
      run();
      return (rx::iterator_range(_comps.cbegin(), _comps.cend())
              | rx::transform([](auto const& comp) { return comp[0]; }));
    }

    //! \brief Returns the root of the strongly connected component containing a
    //! given node.
    //!
    //! This function returns the root of the strongly connected component
    //! containing the node \p n of the underlying WordGraph. Two nodes \c a and
    //! \c b belong to the same strongly connected component if and only if
    //! `root_of(a) == root_of(b)`.
    //!
    //! \param n the node.
    //!
    //! \returns The root of the strongly connected component containing the
    //! node \p n, a value of \ref WordGraph::node_type.
    //!
    //! \throws LibsemigroupsException if \p n is greater than or equal to
    //! WordGraph::number_of_nodes of the underlying word graph.
    //!
    //! \note This function triggers the computation of the strongly connected
    //! components (if they are not already known).
    // Not noexcept because scc_id isn't
    [[nodiscard]] node_type root_of(node_type n) const {
      return component_of(n)[0];
    }

    //! \brief Returns the root of the strongly connected component containing a
    //! given node.
    //!
    //! This function returns the root of the strongly connected component
    //! containing the node \p n of the underlying WordGraph. Two nodes \c a and
    //! \c b belong to the same strongly connected component if and only if
    //! `root_of_no_checks(a) == root_of_no_checks(b)`.
    //!
    //! \param n the node.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \returns
    //! The root of the strongly connected component containing the node \p n,
    //! a value of \ref WordGraph::node_type.
    //!
    //! \warning This function does not check that its argument \p n.
    //!
    //! \note This function triggers the computation of the strongly connected
    //! components (if they are not already known).
    [[nodiscard]] node_type root_of_no_checks(node_type n) const {
      return component_of_no_checks(n)[0];
    }

    //! \brief Returns a const reference to a vector containing the strongly
    //! connected component of a given node.
    //!
    //! This function returns a const reference to a vector
    //! containing the strongly connected components of the node \p n of the
    //! WordGraph (returned by \ref word_graph) used to construct the Gabow
    //! instance.
    //!
    //! \param n the node.
    //!
    //! \returns A vector of node_type.
    //!
    //! \throws LibsemigroupsException if \p n is greater than or equal
    //! to `word_graph().number_of_nodes()`.
    //!
    //! \note This function triggers the computation of the strongly connected
    //! components (if they are not already known).
    [[nodiscard]] std::vector<node_type> const&
    component_of(node_type n) const {
      run();
      validate_node(n);
      return _comps[_id[n]];
    }

    //! \brief Returns a const reference to a vector containing the strongly
    //! connected component of a given node.
    //!
    //! This function returns a const reference to a vector
    //! containing the strongly connected components of the node \p n of the
    //! WordGraph (returned by \ref word_graph) used to construct the Gabow
    //! instance.
    //!
    //! \param n the node.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \returns A vector of node_type.
    //!
    //! \warning This function does not check that its argument \p n.
    //!
    //! \note This function triggers the computation of the strongly connected
    //! components (if they are not already known).
    [[nodiscard]] std::vector<node_type> const&
    component_of_no_checks(node_type n) const {
      run();
      return _comps[_id[n]];
    }

    //! \brief Returns a spanning forest of the strongly connected components.
    //!
    //! This function returns a Forest comprised of spanning trees for each
    //! strongly connected component of a Gabow object, rooted on the minimum
    //! node of that component, with edges oriented away from the root.
    //!
    //! \returns
    //! A const reference to a Forest.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note This function triggers the computation of the strongly connected
    //! components (if they are not already known).
    Forest const& spanning_forest() const;

    //! \brief Returns a reverse spanning forest of the strongly connected
    //! components (if they are not already known).
    //!
    //! This function returns a Forest comprised of spanning trees for each
    //! strongly connected component of a Gabow object, rooted on the minimum
    //! node of that component, with edges oriented towards the root.
    //!
    //! \returns
    //! A const reference to a Forest.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note This function triggers the computation of the strongly connected
    //! components (if they are not already known).
    Forest const& reverse_spanning_forest() const;

    //! \brief Returns a const reference to the underlying word graph.
    //!
    //! This function returns a const reference to the underlying word graph.
    //!
    //! \returns
    //! A const reference to a WordGraph<Node>.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \note This function does not trigger the computation of the strongly
    //! connected components.
    WordGraph<Node> const& word_graph() const noexcept {
      return *_graph;
    }

    //! \brief Check whether the strongly connected components have been found.
    //!
    //! This function returns \c true if the strongly connected components of a
    //! Gabow object have already been computed and \c false if not.
    //!
    //! \returns
    //! A \c bool.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] bool has_components() const noexcept {
      return _finished;
    }

   private:
    void reset() const noexcept;
    void run() const;
    void validate_node(node_type n) const;
    void validate_scc_index(size_t i) const;
  };

  //! \ingroup gabow_group
  //!
  //! \brief Deduction guide for Gabow objects.
  template <typename Node>
  Gabow(WordGraph<Node> const&) -> Gabow<Node>;

  //! \ingroup gabow_group
  //!
  //! \brief Return a human readable representation of a Gabow object.
  //!
  //! Return a human readable representation of a Gabow object.
  //!
  //! \tparam Node the type of the nodes in the underlying WordGraph
  //!
  //! \param g the Gabow object.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  template <typename Node>
  std::string to_human_readable_repr(Gabow<Node> const& g);

}  // namespace libsemigroups

#include "gabow.tpp"
#endif  // LIBSEMIGROUPS_GABOW_HPP_

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

#include <cstddef>  // for size_t
#include <queue>    // for queue
#include <stack>    // for stack
#include <vector>   // for vector

#include "forest.hpp"      // for Forest
#include "word-graph.hpp"  // for WordGraph

#include "ranges.hpp"  // for iterator_range, transform

namespace libsemigroups {

  template <typename Node>
  class Gabow {
   public:
    using node_type  = Node;
    using label_type = typename WordGraph<node_type>::label_type;
    using size_type  = size_t;

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
    Gabow()                        = default;
    Gabow(Gabow const&)            = default;
    Gabow(Gabow&&)                 = default;
    Gabow& operator=(Gabow const&) = default;
    Gabow& operator=(Gabow&&)      = default;
    ~Gabow();

    explicit Gabow(WordGraph<node_type> const& wg) : Gabow() {
      init(wg);
    }

    Gabow& init(WordGraph<node_type> const& wg);

    [[nodiscard]] size_type id_no_checks(node_type v) const {
      run();
      return _id[v];
    }

    //! Returns the id-number of the strongly connected component of a node.
    //!
    //! \param nd the node.
    //!
    //! \returns
    //! The index of the node \p nd, a value of type scc_index_type.
    //!
    //! \throws LibsemigroupsException if \p nd is not valid.
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    // Not noexcept because validate_node isn't
    [[nodiscard]] size_type id(node_type v) const {
      run();
      validate_node(v);
      return _id[v];
    }

    //! Returns an iterator pointing to the vector of nodes in the first scc.
    //!
    //! \returns
    //! A \ref const_iterator_sccs.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-targets. In other words, if
    //! target() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl.
    //! \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    //!
    //! \par Parameters
    //! (None)
    [[nodiscard]] auto components() const {
      run();
      return _comps;
    }

    [[nodiscard]] std::vector<node_type> component(size_type i) const {
      run();
      validate_scc_index(i);
      return _comps[i];
    }

    [[nodiscard]] std::vector<node_type>
    component_no_checks(size_type i) const {
      run();
      return _comps[i];
    }

    //! Returns the number of strongly connected components.
    //!
    //! \returns
    //! A `size_t`.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-targets. In other words, if
    //! target() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl.
    //! \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    //!
    //! \par Parameters
    //! (None)
    [[nodiscard]] size_t number_of_components() const {
      run();
      return _comps.size();
    }

    //! Returns an iterator pointing to the root of the first scc.
    //!
    //! \returns
    //! A \ref const_iterator_scc_roots.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-targets. In other words, if
    //! target() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl. \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    //!
    //! \par Parameters
    //! (None)
    [[nodiscard]] auto roots() const {
      run();
      return (rx::iterator_range(_comps.cbegin(), _comps.cend())
              | rx::transform([](auto const& comp) { return comp[0]; }));
    }

    //! Returns the root of a strongly connected components containing a given
    //! node.
    //!
    //! \param nd a node.
    //!
    //! \returns
    //! The root of the scc containing the node \p nd, a value of
    //! \ref node_type.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-targets. In other words, if
    //! target() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl.
    //! \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    // Not noexcept because scc_id isn't
    [[nodiscard]] node_type root_of(node_type n) const {
      return component_of(n)[0];
    }

    [[nodiscard]] node_type root_of_no_checks(node_type n) const {
      return component_of_no_checks(n)[0];
    }

    //! Returns an iterator pointing to the first node in the scc with
    //! the specified id-number.
    //!
    //! \param i the id-number of the scc.
    //!
    //! \returns
    //! A \ref const_iterator_scc.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-targets. In other words, if
    //! target() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl.
    //!
    //! \throws LibsemigroupsException if \p i is not in the range \c 0 to \c
    //! number_of_scc() - 1.
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    //!
    //! \note
    //! \basic_guarantee
    //!
    [[nodiscard]] std::vector<node_type> component_of(node_type n) const {
      run();
      validate_node(n);
      return _comps[_id[n]];
    }

    [[nodiscard]] std::vector<node_type>
    component_of_no_checks(node_type n) const {
      run();
      return _comps[_id[n]];
    }

    Gabow const& reset() const noexcept;

    //! Returns a spanning forest of the strongly connected components.
    //!
    //! Returns a Forest comprised of spanning trees for each
    //! scc of \c this, rooted on the minimum node of that component, with
    //! edges oriented away from the root.
    //!
    //! \returns
    //! A const reference to a Forest.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-targets. In other words, if
    //! target() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl. \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    //!
    //! \par Parameters
    //! (None)
    Forest const& spanning_forest() const;

    //! Returns a reverse spanning forest of the strongly connected components.
    //!
    //! Returns a Forest comprised of spanning trees for each
    //! scc of \c this, rooted on the minimum node of that component, with
    //! edges oriented towards the root.
    //!
    //! \returns
    //! A const reference to a Forest.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-targets. In other words, if
    //! target() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl. \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    //!
    //! \par Parameters
    //! (None)
    Forest const& reverse_spanning_forest() const;

   private:
    bool finished() const {
      return _finished;
    }
    void run() const;
    void validate_node(node_type n) const;
    void validate_scc_index(size_t i) const;
  };

  template <typename Node>
  Gabow(WordGraph<Node> const&) -> Gabow<Node>;

}  // namespace libsemigroups

#include "gabow.tpp"
#endif  // LIBSEMIGROUPS_GABOW_HPP_

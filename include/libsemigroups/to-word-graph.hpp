//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 James D. Mitchell
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
// This file contains declarations for overloads of the "to" function for
// outputting a WordGraph object.

#ifndef LIBSEMIGROUPS_TO_WORD_GRAPH_HPP_
#define LIBSEMIGROUPS_TO_WORD_GRAPH_HPP_

#include <type_traits>  // for is_same_v, enable_if_t

#include "forest.hpp"           // for Forest
#include "word-graph-view.hpp"  // for WordGraphView
#include "word-graph.hpp"       // for WordGraph

namespace libsemigroups {
  //! \defgroup to_word_graph_group to<WordGraph>
  //! \ingroup word_graph_group
  //!
  //! \brief Convert to WordGraph instance.
  //!
  //! This page contains documentation related to converting a `libsemigroups`
  //! object into WordGraph instances.
  //!
  //! \note The class template WordGraph is used extensively throughout
  //! `libsemigroups`, and many of the main algorithms involve constructing a
  //! WordGraph. These WordGraph objects are usually accessed via a member
  //! function `%word_graph` or `current_word_graph`. \ref to_group
  //! is only for conversion between different types that represent (in some
  //! sense at least) equivalent mathematical objects.
  //!
  //! \sa \ref to_group for an overview of possible conversions between
  //! `libsemigroups` types.

  //! \ingroup to_word_graph_group
  //!
  //! \brief Convert a Forest to a WordGraph.
  //!
  //! Defined in \c to-word-graph.hpp
  //!
  //! Despite the hideous signature, this function should be invoked as follows:
  //!
  //! \code
  //! to<WordGraph>(f);
  //! \endcode
  //!
  //! where \p f is a Forest object. The returned WordGraph object is
  //! isomorphic as a graph to \p f.
  //!
  //! \tparam Result used for SFINAE should be WordGraph.
  //! \param f the Forest instance to convert.
  //!
  //! \returns A WordGraph instance isomorphic to \p f.
  template <template <typename...> typename Result>
  auto to(Forest const& f)
      -> std::enable_if_t<std::is_same_v<Result<int>, WordGraph<int>>,
                          WordGraph<Forest::node_type>>;

  //! \ingroup to_word_graph_group
  //!
  //! \brief Convert a WordGraphView to a WordGraph.
  //!
  //! Defined in \c to-word-graph.hpp
  //!
  //! Despite the hideous signature, this function should be invoked as follows:
  //!
  //! \code
  //! to<WordGraph>(view);
  //! \endcode
  //!
  //! where \p view is a WordGraphView object. The returned WordGraph only
  //! contains those nodes and edges that are covered by \p view.
  //!
  //! \tparam Result used for SFINAE. Should be WordGraph.
  //!
  //! \param view the WordGraphView instance to convert.
  //!
  //! \returns A WordGraph instance isomorphic to the WordGraph covered by the
  //! view.
  //!
  //! \throws LibsemigroupsException if view is in an invalid state, or if the
  //! underlying graph has edges which crossed the boundaries of the view.
  //!
  //! \sa
  //! WordGraphView::throw_if_invalid_view and
  //! WordGraphView::throw_if_any_target_out_of_bounds.
  template <template <typename...> typename Result, typename Node>
  auto to(WordGraphView<Node> const& view)
      -> std::enable_if_t<std::is_same_v<Result<Node>, WordGraph<Node>>,
                          WordGraph<Node>>;
  ;
}  // namespace libsemigroups

#include "to-word-graph.tpp"

#endif  // LIBSEMIGROUPS_TO_WORD_GRAPH_HPP_

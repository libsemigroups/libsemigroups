#ifndef WORD_GRAPH_HELPERS_HPP_
#define WORD_GRAPH_HELPERS_HPP_

#include "libsemigroups/word-graph-view.hpp"
#include "libsemigroups/word-graph.hpp"

#include <stack>
#include <vector>

namespace libsemigroups {
  //! \ingroup word_graph_group
  //!
  //! \brief Namespace containing helper functions for the \ref WordGraph
  //! class.
  //!
  //! Defined in `word-graph-helpers.hpp`.
  //!
  //! \brief This namespace contains helper functions for the \ref WordGraph
  //! class.
  namespace word_graph {

    //////////////////////////////////////////////////////////////////////////
    // WordGraph - helper functions - in alphabetical order!!!
    //////////////////////////////////////////////////////////////////////////
    template <typename Node>
    [[nodiscard]] bool is_strictly_cyclic(WordGraphView<Node> const& wg);

    template <typename Node>
    [[nodiscard]] bool is_strictly_cyclic(WordGraph<Node> const& wg) {
      return is_strictly_cyclic(WordGraphView<Node>(wg));
    }
  }  // namespace word_graph
}  // namespace libsemigroups

#include "libsemigroups/word-graph-helpers.tpp"
#endif
#include "libsemigroups/word-graph-view.hpp"

namespace libsemigroups {
  template <typename Node>
  WordGraphView<Node>::WordGraphView(const WordGraph<Node>& graph,
                                     const size_type        _start,
                                     const size_type        _end)
      : graph(graph), _start(_start), _end(_end), _nr_nodes(_end - _start) {}

}  // namespace libsemigroups

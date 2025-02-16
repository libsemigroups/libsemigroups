#include "libsemigroups/word-graph-view.hpp"
#include "libsemigroups/detail"

namespace libsemigroups {
  template <typename Node>
  WordGraphView<Node>::WordGraphView(const WordGraph<Node>* graph,
                                     const size_type        start,
                                     const size_type        end)
      : graph(graph), _start(start), _end(end), _nr_nodes(end - start) {}

  template <typename Node>
  typename WordGraphView<Node>::const_iterator_targets
  WordGraphView<Node>::cbegin_targets(node_type source) const {
    word_graph_view::throw_if_node_out_of_bounds(*this, source);
    return cbegin_targets_no_checks(source);
  }

  template <typename Node>
  typename WordGraphView<Node>::const_iterator_targets
  WordGraphView<Node>::cend_targets(node_type source) const {
    word_graph_view::throw_if_node_out_of_bounds(*this, source);
    return cend_targets_no_checks(source);
  }

  template <typename Node>
  rx::iterator_range<typename WordGraphView<Node>::const_iterator_targets>
  WordGraphView<Node>::targets(node_type source) const {
    word_graph_view::throw_if_node_out_of_bounds(*this, source);
    return targets_no_checks(source);
  }

  template <typename Node>
  auto WordGraphView<Node>::labels_and_targets(node_type source) const {
    word_graph_view::throw_if_node_out_of_bounds(*this, source);
    return rx::enumerate(targets_no_checks(source));
  }

  template <typename Node>
  WordGraphView<Node>& WordGraphView<Node>::init(const size_type start,
                                                 const size_type end) {
    _start = start;
    _end   = end;
    return *this;
  }

  template <typename Node>
  typename WordGraphView<Node>::size_type
  WordGraphView<Node>::out_degree() const noexcept {
    return graph->out_degree();
  }

  namespace word_graph_view {
    template <typename Node1, typename Node2>
    void throw_if_node_out_of_bounds(WordGraphView<Node1> const& wgv, Node2 n) {
      static_assert(sizeof(Node2) <= sizeof(Node1));

      if (static_cast<Node1>(n) < wgv.start_node()
          || static_cast<Node1>(n) >= wgv.end_node()) {
        LIBSEMIGROUPS_EXCEPTION("node value out of bounds, expected value "
                                "in the range [{}, {}), got {}",
                                wgv.start_node(),
                                wgv.end_node(),
                                n);
      }
    }

    template <typename Node>
    auto adjacency_matrix(WordGraphView<Node> const& wgv) {
      using Mat = typename WordGraph<Node>::adjacency_matrix_type;
      Mat mat;
      
    }
  }  // namespace word_graph_view
}  // namespace libsemigroups

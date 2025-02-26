#include "libsemigroups/word-graph-view.hpp"

namespace libsemigroups {
  template <typename Node>
  WordGraphView<Node>::WordGraphView(const WordGraph<Node>* graph,
                                     const size_type        start,
                                     const size_type        end)
      : graph(graph), _start(start), _end(end), _nr_nodes(end - start) {}

  template <typename Node>
  WordGraphView<Node>::WordGraphView(const WordGraphView&) = default;

  template <typename Node>
  template <typename OtherNode>
  WordGraphView<Node>::WordGraphView(WordGraphView<OtherNode> const& wg)
      : WordGraphView(wg.number_of_nodes(), wg.out_degree()) {
    static_assert(sizeof(OtherNode) <= sizeof(Node));
    init(wg);
  }

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
  std::pair<typename WordGraphView<Node>::label_type,
            typename WordGraphView<Node>::node_type>
  WordGraphView<Node>::next_label_and_target_no_checks(node_type  s,
                                                       label_type a) const {
    return graph->next_label_and_target_no_checks(s, a);
  }

  template <typename Node>
  std::pair<typename WordGraphView<Node>::label_type,
            typename WordGraphView<Node>::node_type>
  WordGraphView<Node>::next_label_and_target(node_type s, label_type a) const {
    word_graph_view::throw_if_node_out_of_bounds(s);
    return graph->next_label_and_target_no_checks(s, a);
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
  bool WordGraphView<Node>::operator==(WordGraphView const& that) const {
    {
      if (number_of_nodes() != that.number_of_nodes()) {
        return false;
      }
      if (out_degree() != that.out_degree()) {
        return false;
      }
      auto this_node = this->cbegin_nodes();
      auto that_node = that.cbegin_nodes();
      while (this_node < this->cend_nodes()) {
        for (label_type a = 0; a < this->out_degree(); ++a) {
          if (graph->target_no_checks(*this_node, a)
              != that.graph->target_no_checks(*that_node, a)) {
            return false;
          }
        }
        ++this_node;
        ++that_node;
      }
      return true;
    }
  }

  template <typename Node>
  typename WordGraphView<Node>::node_type
  WordGraphView<Node>::target(node_type source, label_type a) const {
    word_graph_view::throw_if_node_out_of_bounds(*this, source);
    word_graph::throw_if_label_out_of_bounds(a);
    return target_no_checks(source, a);
  }
  template <typename Node>
  typename WordGraphView<Node>::node_type
  WordGraphView<Node>::target_no_checks(node_type source, label_type a) const {
    return graph->_dynamic_array_2.get(source, a);
  }
  template <typename Node>
  WordGraphView<Node>& WordGraphView<Node>::init(const size_type start,
                                                 const size_type end) {
    _start = start;
    _end   = end;
    return *this;
  }

  template <typename Node>
  template <typename OtherNode>
  WordGraphView<OtherNode>&
  WordGraphView<Node>::init(WordGraph<OtherNode> const& that) {
    static_assert(sizeof(OtherNode) <= sizeof(Node));
    this->graph = that;
    return this;
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
  }  // namespace word_graph_view
}  // namespace libsemigroups

namespace libsemigroups {
  namespace word_graph {
    template <typename Node>
    [[nodiscard]] bool is_strictly_cyclic(WordGraphView<Node> const& wg) {
      using node_type = typename WordGraph<Node>::node_type;
      auto const N    = wg.number_of_nodes();

      if (N == 0) {
        return true;
      }

      std::vector<bool> seen(N, false);
      std::stack<Node>  stack;

      for (node_type m = 0; m < N; ++m) {
        stack.push(m);
        size_t count = 0;
        while (!stack.empty()) {
          auto n = stack.top();
          stack.pop();
          if (!seen[n]) {
            seen[n] = true;
            if (++count == N) {
              return true;
            }
            for (auto t : wg.targets_no_checks(n)) {
              if (t < N) {
                stack.push(t);
              }
            }
          }
        }
        std::fill(seen.begin(), seen.end(), false);
      }
      return false;
    }
  }  // namespace word_graph

}  // namespace libsemigroups
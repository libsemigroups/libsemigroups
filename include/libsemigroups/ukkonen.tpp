//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021-23 James D. Mitchell + Maria Tsalakou
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

// This file contains an implementation of a generalised suffix tree, adapted
// from:
//
// https://cp-algorithms.com/string/suffix-tree-ukkonen.html

namespace libsemigroups {

  template <typename Iterator>
  size_t Ukkonen::index_no_checks(Iterator first, Iterator last) const {
    if (first < last) {
      auto st = traverse_no_checks(first, last);
      if (st.second == last) {
        // not "no_checks" because there aren't any anyway
        word_index_type i = is_suffix(st.first);
        if (i != UNDEFINED) {
          size_t const n = std::distance(first, last);
          if (_nodes[st.first.v].is_leaf()
              && word_length(word_index(_nodes[st.first.v])) == n) {
            return i;
          }

          for (auto const& child : _nodes[st.first.v].children) {
            if (_nodes[child.second].is_leaf()
                && word_length(word_index(_nodes[child.second])) == n) {
              return word_index(_nodes[child.second]);
            }
          }
        }
      }
    }
    return UNDEFINED;
  }

  template <typename Iterator>
  Iterator Ukkonen::traverse_no_checks(State&   st,
                                       Iterator first,
                                       Iterator last) const {
    auto it = first;
    while (it < last) {
      if (st.pos == _nodes[st.v].length()) {
        if (_nodes[st.v].child(*it) == UNDEFINED) {
          return it;
        } else {
          st.v   = _nodes[st.v].child(*it);
          st.pos = 0;
        }
      } else {
        auto prefix = detail::maximum_common_prefix(
            _word.cbegin() + _nodes[st.v].l + st.pos,
            _word.cbegin() + _nodes[st.v].r,
            it,
            last);
        it = prefix.second;
        st.pos += std::distance(_word.cbegin() + _nodes[st.v].l + st.pos,
                                prefix.first);
        if (prefix.first != _word.cbegin() + _nodes[st.v].r) {
          return it;
        }
      }
    }
    return it;
  }

  template <typename Iterator>
  void Ukkonen::throw_if_contains_unique_letter(Iterator first,
                                                Iterator last) const {
    auto it = std::find_if(
        first, last, [this](letter_type l) { return is_unique_letter(l); });

    if (it != last) {
      LIBSEMIGROUPS_EXCEPTION("illegal letter in word, expected value not "
                              "in the range ({}, {}) found {} in "
                              "position {}",
                              _next_unique_letter,
                              -1,
                              *it,
                              it - first);
    }
  }

  namespace ukkonen {
    template <typename Iterator>
    bool is_subword_no_checks(Ukkonen const& u, Iterator first, Iterator last) {
      if (last <= first) {
        return true;
      } else if (static_cast<size_t>(last - first) > u.max_word_length()) {
        return false;
      }
      return u.traverse_no_checks(first, last).second == last;
    }

    template <typename Iterator>
    bool is_suffix_no_checks(Ukkonen const& u, Iterator first, Iterator last) {
      if (last <= first) {
        return true;
      } else if (static_cast<size_t>(last - first) > u.max_word_length()) {
        return false;
      }

      auto pair = u.traverse_no_checks(first, last);

      return pair.second == last && u.is_suffix(pair.first) != UNDEFINED;
    }

    template <typename Iterator>
    Iterator maximal_piece_prefix_no_checks(Ukkonen const& u,
                                            Iterator       first,
                                            Iterator       last) {
      // Follow the path from the root labelled by [first, last), then go back
      // from the leaf to its parent (which is an internal node), corresponding
      // to the maximal_piece_prefix.
      auto        st    = u.traverse_no_checks(first, last);
      auto const& nodes = u.nodes();
      if (nodes[st.first.v].is_leaf()
          && nodes[nodes[st.first.v].parent].is_root()) {
        return first;
      }
      if (nodes[st.first.v].is_leaf()) {
        return first + std::distance(first, st.second) - st.first.pos;
      }
      return st.second;
    }

    template <typename Iterator>
    Iterator maximal_piece_suffix_no_checks(Ukkonen const& u,
                                            Iterator       first,
                                            Iterator       last) {
      // TODO(later) Currently we do this for each of the N relation words,
      // so the cost is 2 N ^ 2, when instead we could do it once for all
      // relation words in O(N) time.
      //
      // The analysis of the previous paragraph also assumes that
      // distance_from_root is constant time, which with the current
      // implementation it isn't. It's O(M) where M is the number of
      // (actual) nodes on the path from a given node to the root. This
      // could also be preprocessed in O(N) time, for O(1) retrieval later.
      size_t const m     = std::distance(first, last);
      auto const&  nodes = u.nodes();

      auto const ndx = u.index(first, last);

      if (ndx == UNDEFINED || m * m < nodes.size()) {
        for (auto it = first; it < last; ++it) {
          if (is_piece_no_checks(u, it, last)) {
            return it;
          }
        }
        return last;
      } else {
        size_t n = 0;
        for (size_t i = 0; i < nodes.size(); ++i) {
          if (nodes[i].child(u.unique_letter(ndx)) != UNDEFINED) {
            n = std::max(u.distance_from_root(nodes[i]), n);
          }
        }
        return last - n;
      }
    }

    template <typename Iterator>
    size_t number_of_pieces_no_checks(Ukkonen const& u,
                                      Iterator       first,
                                      Iterator       last) {
      size_t result = 0;
      size_t N      = 1;
      auto   it     = first;
      while (it < last && N != 0) {
        N = ukkonen::length_maximal_piece_prefix_no_checks(u, it, last);
        it += N;
        result += 1;
      }
      return (it == last ? result : POSITIVE_INFINITY);
    }

    template <typename Iterator>
    std::vector<Iterator> pieces_no_checks(Ukkonen const& u,
                                           Iterator       first,
                                           Iterator       last) {
      std::vector<Iterator> result = {first};
      size_t                N      = 1;
      auto                  it     = first;
      while (it < last && N != 0) {
        N = length_maximal_piece_prefix_no_checks(u, it, last);
        it += N;
        result.push_back(it);
      }
      if (it != last) {
        result.clear();
      }
      return result;
    }

    template <typename Word>
    std::vector<Word> pieces_no_checks(Ukkonen const& u, Word const& w) {
      auto              pcs = pieces_no_checks(u, w.cbegin(), w.cend());
      std::vector<Word> result;

      if (pcs.empty()) {
        return result;
      }

      for (size_t i = 0; i < pcs.size() - 1; ++i) {
        result.emplace_back(pcs[i], pcs[i + 1]);
      }
      return result;
    }

    template <typename T>
    auto dfs(Ukkonen const& u, T& helper) {
      std::stack<size_t> S;
      auto const&        nodes = u.nodes();
      size_t const       N     = nodes.size();
      S.push(0);
      while (!S.empty()) {
        size_t v = S.top();
        S.pop();
        if (v >= N) {
          // post-order, everything in subtree starting at v has been
          // processed.
          v -= N;
          helper.post_order(u, v);
        } else {
          // This is a tree so we've never seen v before!
          helper.pre_order(u, v);
          S.push(N + v);  // so that we can tell when we've finished
          // processing the subtree starting at v
          for (auto const& child : nodes[v].children) {
            S.push(child.second);
          }
        }
      }
      return helper.yield(u);
    }
  }  // namespace ukkonen
}  // namespace libsemigroups

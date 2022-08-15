//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021 James D. Mitchell + Maria Tsalakou
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

#ifndef LIBSEMIGROUPS_SUFFIX_TREE_HPP_
#define LIBSEMIGROUPS_SUFFIX_TREE_HPP_

#include <algorithm>      // for equal, find_if
#include <cstddef>        // for size_t
#include <cstdint>        // for uint64_t
#include <iosfwd>         // for string
#include <iterator>       // for operator+
#include <map>            // for map
#include <stack>          // for stack
#include <unordered_map>  // for unordered_map
#include <utility>        // for hash
#include <vector>         // for vector

#include "adapters.hpp"   // for Hash, EqualTo
#include "constants.hpp"  // for Undefined, UNDE...
#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "types.hpp"      // for word_type, lett..

namespace libsemigroups {
  namespace detail {
    class SuffixTree {
      ////////////////////////////////////////////////////////////////////////
      // SuffixTree - type aliases - private
      ////////////////////////////////////////////////////////////////////////

      // Alias for index in _nodes
      using node_index_type = size_t;

      // Alias for index in side an edge
      using edge_index_type = size_t;

      // Alias for any letter that is added by SuffixTree (so that unique
      // strings end in unique letters).
      using unique_letter_type = size_t;

      // Alias for order that words were added.
      using word_index_type = size_t;

      // Alias for index inside _word
      using index_type = size_t;

      ////////////////////////////////////////////////////////////////////////
      // SuffixTree - inner classes - private
      ////////////////////////////////////////////////////////////////////////

      struct Node {
        // Data
        index_type                                     l;
        index_type                                     r;
        node_index_type                                parent;
        node_index_type                                link;
        mutable std::map<letter_type, node_index_type> children;
        bool                                           is_real_suffix;

        // Constructor
        explicit Node(index_type      ll      = 0,
                      index_type      rr      = 0,
                      node_index_type pparent = UNDEFINED)
            : l(ll),
              r(rr),
              parent(pparent),
              link(UNDEFINED),
              children(),
              is_real_suffix(false) {}

        size_t length() const {
          return r - l;
        }

        node_index_type& child(letter_type c) {
          if (children.count(c) == 0) {
            children[c] = UNDEFINED;
          }
          return children[c];
        }

        node_index_type child(letter_type c) const {
          if (children.count(c) == 0) {
            return UNDEFINED;
          }
          return children[c];
        }

        bool is_leaf() const noexcept {
          return children.empty();
        }
      };

     public:
      ////////////////////////////////////////////////////////////////////////
      // SuffixTree - inner classes - public
      ////////////////////////////////////////////////////////////////////////

      struct State {
        node_index_type v;
        edge_index_type pos;
        State()             = default;
        State(State const&) = default;
        State(State&&)      = default;
        State& operator=(State const&) = default;
        State& operator=(State&&) = default;

        State(node_index_type vv, edge_index_type ppos) : v(vv), pos(ppos) {}

        bool valid() const noexcept {
          return v != UNDEFINED;
        }

        bool operator==(State const& that) const noexcept {
          return v == that.v && pos == that.pos;
        }
      };

      ////////////////////////////////////////////////////////////////////////
      // SuffixTree - constructors - public
      ////////////////////////////////////////////////////////////////////////

      SuffixTree();

      ////////////////////////////////////////////////////////////////////////
      // SuffixTree - initialisation - public
      ////////////////////////////////////////////////////////////////////////

      template <typename T>
      void add_word(T const& first, T const& last) {
        add_word(word_type(first, last));
      }

      void add_word(word_type const& w);

      template <typename T>
      void add_word(T const& w) {
        add_word(w.cbegin(), w.cend());
      }

      ////////////////////////////////////////////////////////////////////////
      // SuffixTree - attributes - public
      ////////////////////////////////////////////////////////////////////////

      size_t number_of_nodes() const {
        return _nodes.size();
      }

      size_t number_of_words() const noexcept {
        return -1 - _next_unique_letter;
      }

      size_t number_of_subwords() const;

      Node const& node(node_index_type v) const {
        return _nodes[v];
      }

      size_t multiplicity(node_index_type v) const {
        return _multiplicity[_word_index_lookup[v]];
      }

      bool is_leaf(node_index_type v) const {
        return _nodes[v].is_leaf();
      }

      bool is_root(node_index_type v) const {
        return _nodes[v].parent == UNDEFINED;
      }

      node_index_type parent(node_index_type v) const {
        return _nodes[v].parent;
      }

      index_type right(node_index_type v) const {
        return _nodes[v].r;
      }

      index_type left(node_index_type v) const {
        return _nodes[v].l;
      }

      size_t length(node_index_type v) const {
        return _nodes[v].length();
      }

      ////////////////////////////////////////////////////////////////////////
      // SuffixTree - member functions for queries - public
      ////////////////////////////////////////////////////////////////////////

      using const_iterator = typename word_type::const_iterator;

      std::pair<const_iterator, const_iterator> word(index_type l,
                                                     index_type r) const {
        return std::make_pair(_word.cbegin() + l, _word.cbegin() + r);
      }

      template <typename T>
      auto dfs(T& helper) const {
        std::stack<node_index_type> S;
        size_t const                N = _nodes.size();
        S.push(0);
        while (!S.empty()) {
          node_index_type v = S.top();
          S.pop();
          if (v >= N) {
            // post-order, everything in subtree starting at v has been
            // processed.
            v -= N;
            helper.post_order(*this, v);
          } else {
            // This is a tree so we've never seen v before!
            helper.pre_order(*this, v);
            S.push(N + v);  // so that we can tell when we've finished
            // processing the subtree starting at v
            for (auto const& child : _nodes[v].children) {
              S.push(child.second);
            }
          }
        }
        return helper.yield(*this);
      }

      template <typename T>
      bool is_subword(T const& w) const {
        return is_subword(w.cbegin(), w.cend());
      }

      bool is_subword(word_type const& w) const {
        return is_subword(w.cbegin(), w.cend());
      }

      template <typename T>
      bool is_subword(T const& first, T const& last) const {
        validate_word(first, last);
        if (last <= first) {
          return true;
        } else if (static_cast<size_t>(last - first) > _max_word_length) {
          return false;
        }
        return traverse(State(0, 0), first, last).valid();
      }

      bool is_suffix(word_type const& w) const {
        return is_suffix(w.cbegin(), w.cend());
      }

      template <typename T>
      bool is_suffix(T const& w) const {
        return is_suffix(w.cbegin(), w.cend());
      }

      template <typename T>
      bool is_suffix(T const& first, T const& last) const {
        validate_word(first, last);
        if (last <= first) {
          return true;
        } else if (static_cast<size_t>(last - first) > _max_word_length) {
          return false;
        }

        return is_real_suffix(traverse(State(0, 0), first, last)) != UNDEFINED;
      }

      template <typename T>
      size_t maximal_piece_prefix(T const& x) const {
        return maximal_piece_prefix(x.cbegin(), x.cend());
      }

      template <typename T>
      size_t maximal_piece_prefix(T const& first, T const& last) const {
        return maximal_piece_prefix(word_type(first, last));
      }

      size_t maximal_piece_prefix(word_type const& w) const {
        return maximal_piece_prefix(word_index(w));
      }

      template <typename T>
      size_t maximal_piece_suffix(T const& x) const {
        return maximal_piece_suffix(x.cbegin(), x.cend());
      }

      template <typename T>
      size_t maximal_piece_suffix(T const& first, T const& last) const {
        return maximal_piece_suffix(word_type(first, last));
      }

      size_t maximal_piece_suffix(word_type const& w) const {
        return maximal_piece_suffix(word_index(w));
      }

      template <typename T>
      size_t number_of_pieces(T const& x) const {
        return number_of_pieces(x.cbegin(), x.cend());
      }

      template <typename T>
      size_t number_of_pieces(T const& first, T const& last) const {
        return number_of_pieces(word_type(first, last));
      }

      size_t number_of_pieces(word_type const& w) const {
        return number_of_pieces(word_index(w));
      }

      State traverse(State st, word_type const& w) const {
        return traverse(st, w.cbegin(), w.cend());
      }

      template <typename T>
      State traverse(State st, T const& first, T const& last) const {
        if (last <= first || !st.valid()) {
          return st;
        }
        auto it = first;
        while (it < last) {
          if (st.pos == _nodes[st.v].length()) {
            st = State(_nodes[st.v].child(*it), 0);
            if (!st.valid()) {
              return st;
            }
          } else {
            if (_nodes[st.v].length() - st.pos
                <= static_cast<size_t>(last - it)) {
              if (!std::equal(_word.cbegin() + _nodes[st.v].l + st.pos,
                              _word.cbegin() + _nodes[st.v].r,
                              it)) {
                return State(UNDEFINED, UNDEFINED);
              } else {
                it += _nodes[st.v].length() - st.pos;
                st.pos = _nodes[st.v].length();
              }
            } else {
              if (!std::equal(
                      it, last, _word.cbegin() + _nodes[st.v].l + st.pos)) {
                return State(UNDEFINED, UNDEFINED);
              }
              return State(st.v, st.pos + last - it);
            }
          }
        }
        return st;
      }

      std::string tikz() const;
      std::string dot() const;

     private:
      ////////////////////////////////////////////////////////////////////////
      // SuffixTree - helpers - private
      ////////////////////////////////////////////////////////////////////////

      std::string tikz_traverse(size_t i, bool rotate = false) const;
      std::string tikz_word(size_t l, size_t r) const;
      std::string dot_word(size_t l, size_t r) const;

      bool is_real_letter(letter_type l) const noexcept {
        return l < _next_unique_letter;
      }

      word_index_type is_real_suffix(State const& st) const;

      unique_letter_type unique_letter(word_index_type i) const noexcept {
        LIBSEMIGROUPS_ASSERT(i < number_of_words());
        return -1 - i;
      }

      size_t distance_from_root(node_index_type i) const;

      // Returns one index i of the word (i.e. the i-th word added)
      // corresponding to the State st.
      word_index_type word_index(Node const& n) const {
        LIBSEMIGROUPS_ASSERT(n.parent != UNDEFINED);
        return _word_index_lookup[n.r - 1];
      }

      ////////////////////////////////////////////////////////////////////////
      // SuffixTree - member functions for queries - private
      ////////////////////////////////////////////////////////////////////////

      size_t maximal_piece_prefix(word_index_type j) const;
      size_t maximal_piece_prefix(index_type l, index_type r) const;
      size_t maximal_piece_suffix(word_index_type j) const;
      size_t number_of_pieces(word_index_type i) const;

      ////////////////////////////////////////////////////////////////////////
      // SuffixTree - validation - private
      ////////////////////////////////////////////////////////////////////////

      void validate_word(word_type const& w) const {
        validate_word(w.cbegin(), w.cend());
      }

      template <typename T>
      void validate_word(T const& first, T const& last) const {
        auto it = std::find_if(first, last, [this](letter_type l) {
          return !this->is_real_letter(l);
        });

        if (it != last) {
          LIBSEMIGROUPS_EXCEPTION("illegal letter in word, expected value not "
                                  "in the range (%llu, %llu) found %llu in "
                                  "position %llu",
                                  uint64_t(_next_unique_letter),
                                  uint64_t(-1),
                                  uint64_t(*it),
                                  uint64_t(it - first));
        }
      }

      word_index_type word_index(word_type const& w) const;

      ////////////////////////////////////////////////////////////////////////
      // The following functions go, split, get_link, and tree_extend are
      // minimally adapted from:
      //
      // https://cp-algorithms.com/string/suffix-tree-ukkonen.html
      ////////////////////////////////////////////////////////////////////////

      // Follow the path in the tree starting at the position described by
      // State st, and corresponding to the range [l, r) in _word.
      void  go(State& st, index_type l, index_type r) const;
      State go2(State& st, index_type l, index_type r) const {
        State result(st);
        go(result, l, r);
        return result;
      }

      // Split the node _nodes[st.v] into two nodes, the new node
      // with edge corresponding to
      //
      // [_nodes[st.v].l, _nodes[st.v].l + st.pos)
      //
      // and the old node with edge corresponding to
      //
      // [_nodes[st.v].l + st.pos, _nodes[st.v].r)
      node_index_type split(State const& st);

      // Get the suffix link of a node by index
      node_index_type get_link(node_index_type v);

      // Perform the phase starting with the pos letter of the word.
      void tree_extend(index_type pos);

      ////////////////////////////////////////////////////////////////////////
      // SuffixTree - private data
      ////////////////////////////////////////////////////////////////////////

      std::unordered_map<word_type,
                         word_index_type,
                         Hash<word_type>,
                         EqualTo<word_type>>
                              _map;
      size_t                  _max_word_length;
      std::vector<size_t>     _multiplicity;
      unique_letter_type      _next_unique_letter;
      std::vector<Node>       _nodes;
      State                   _ptr;
      std::vector<index_type> _word_begin;
      std::vector<index_type> _word_index_lookup;
      word_type               _word;
    };

    class DFSHelper {
     public:
      using const_iterator = typename detail::SuffixTree::const_iterator;
      explicit DFSHelper(detail::SuffixTree& st)
          : _best(),
            _best_goodness(),
            _distance_from_root(st.number_of_nodes(), 0),
            _num_leafs(st.number_of_nodes(), 0),
            _suffix_index() {}

      void pre_order(detail::SuffixTree const& st, size_t v) {
        // This is a tree so we've never seen v before!
        if (!st.is_root(v)) {
          _distance_from_root[v]
              = _distance_from_root[st.parent(v)] + st.length(v);
        }
        if (st.is_leaf(v)) {
          _num_leafs[v]++;
          // Starting index of the suffix that the leaf corresponds to
          _suffix_index.push_back(st.right(v) - _distance_from_root[v]);
        }
      }

      void post_order(detail::SuffixTree const& st, size_t v) {
        if (st.is_leaf(v)) {
          return;
        }

        for (auto const& child : st.node(v).children) {
          _num_leafs[v] += _num_leafs[child.second];
        }
        _scratch.assign(_suffix_index.cend() - _num_leafs[v],
                        _suffix_index.cend());
        std::sort(_scratch.begin(), _scratch.end());
        // number of non-overlapping subwords corresponding to the node v
        size_t num_non_overlap = st.multiplicity(_scratch[0]);
        for (size_t i = 0; i < _scratch.size() - 1; ++i) {
          if (_scratch[i] + _distance_from_root[v] <= _scratch[i + 1]) {
            num_non_overlap += st.multiplicity(_scratch[i]);
          }
        }
        int goodness = (_distance_from_root[v] * num_non_overlap)
                       - num_non_overlap - (_distance_from_root[v] + 1);
        if (goodness > _best_goodness) {
          _best          = v;
          _best_goodness = goodness;
        }
      }

      std::pair<const_iterator, const_iterator>
      yield(detail::SuffixTree const& st) {
        if (st.is_root(_best)) {
          return st.word(0, 0);
        }
        return st.word(st.left(_best) - _distance_from_root[st.parent(_best)],
                       st.right(_best));
      }

     private:
      size_t              _best;
      int                 _best_goodness;
      std::vector<size_t> _distance_from_root;
      std::vector<size_t> _num_leafs;
      std::vector<size_t> _scratch;
      std::vector<size_t> _suffix_index;
    };

    namespace suffix_tree_helper {

      void add_words(SuffixTree& st, std::vector<word_type> const& words);

      template <typename T>
      void add_words(SuffixTree& st, T first, T last) {
        for (auto it = first; it != last; ++it) {
          st.add_word(*it);
        }
      }
    };  // namespace suffix_tree_helper

  }  // namespace detail

  template <>
  struct Hash<detail::SuffixTree::State> {
    size_t operator()(detail::SuffixTree::State const& st) const {
      auto hasher = std::hash<size_t>();
      return hasher(st.v) + 17 * hasher(st.pos);
    }
  };

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_SUFFIX_TREE_HPP_

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

// This file contains the implementation of the SuffixTree class.

#include "libsemigroups/suffix-tree.hpp"

#include <array>    // for array
#include <cstddef>  // for size_t
#include <numeric>  // for accumulate
#include <string>   // for operator+, char_traits, to_st...
#include <tuple>    // for tie, tuple

#include "libsemigroups/adapters.hpp"   // for EqualTo, Hash
#include "libsemigroups/constants.hpp"  // for UNDEFINED
#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/string.hpp"     // for to_string

namespace libsemigroups {
  namespace detail {

    using State = SuffixTree::State;

    ////////////////////////////////////////////////////////////////////////
    // SuffixTree - constructors - public
    ////////////////////////////////////////////////////////////////////////

    SuffixTree::SuffixTree()
        : _map(),
          _max_word_length(0),
          _next_unique_letter(static_cast<unique_letter_type>(-1)),
          _nodes(1),
          _ptr(0, 0),
          _word_begin({0}),
          _word() {}

    ////////////////////////////////////////////////////////////////////////
    // SuffixTree - initialisation - public
    ////////////////////////////////////////////////////////////////////////

    void SuffixTree::add_word(word_type const& w) {
      if (w.empty()) {
        return;
      }
      bool                     inserted;
      decltype(_map)::iterator it;
      std::tie(it, inserted) = _map.emplace(w, number_of_words());
      if (!inserted) {
        _multiplicity[it->second]++;
        // Duplicate word, do nothing
        return;
      }
      _multiplicity.push_back(1);

      // Check that w doesn't contain illegal letters
      validate_word(w);

      _max_word_length    = std::max(w.size(), _max_word_length);
      size_t old_length   = _word.size();
      size_t old_nr_nodes = _nodes.size();

      _word.insert(_word.end(), w.cbegin(), w.cend());
      _word.push_back(_next_unique_letter--);
      _word_begin.push_back(_word.size());
      _word_index_lookup.resize(_word.size(), number_of_words() - 1);

      for (index_type i = old_length; i < _word.size(); ++i) {
        tree_extend(i);
      }

      for (node_index_type i = old_nr_nodes; i < _nodes.size(); ++i) {
        auto& n = _nodes[i];
        for (auto const& child : n.children) {
          if (!is_real_letter(child.first)) {
            n.is_real_suffix = true;
            break;
          }
        }
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // SuffixTree - attributes - public
    ////////////////////////////////////////////////////////////////////////

    size_t SuffixTree::number_of_subwords() const {
      return std::accumulate(_nodes.cbegin(),
                             _nodes.cend(),
                             size_t(0),
                             [](size_t& tot, Node const& n) {
                               tot += n.length();
                               return tot;
                             })
             - _word.size() + 1;  // empty word
    }

    ////////////////////////////////////////////////////////////////////////
    // SuffixTree - member functions for queries - public
    ////////////////////////////////////////////////////////////////////////

    std::string SuffixTree::tikz() const {
      if (number_of_words() == 0) {
        LIBSEMIGROUPS_EXCEPTION("expected at least 1 word, found 0");
      } else if (number_of_words() > 10) {
        LIBSEMIGROUPS_EXCEPTION("expected at most 10 words, found %llu",
                                uint64_t(number_of_words()));
      }

      std::string s = "\\documentclass{article}\n";
      s += "\\usepackage{tikz}\n";
      s += "\\usetikzlibrary{calc}\n";
      s += "\\usepackage[landscape]{geometry}\n";
      s += "\\begin{document}\n";
      s += "\\definecolor{color0}{RGB}{127, 0, 255}\n";    // purple
      s += "\\definecolor{color1}{RGB}{128, 128, 128}\n";  // gray
      s += "\\definecolor{color2}{RGB}{255, 0, 127}\n";    // fuschia
      s += "\\definecolor{color3}{RGB}{255, 0, 255}\n";    // pink
      s += "\\definecolor{color4}{RGB}{0, 0, 255}\n";      // blue
      s += "\\definecolor{color5}{RGB}{0, 255, 0}\n";      // green
      s += "\\definecolor{color6}{RGB}{255, 128, 0}\n";    // orange
      s += "\\definecolor{color7}{RGB}{255, 0, 0}\n";      // red
      s += "\\definecolor{color8}{RGB}{0, 128, 255}\n";    // light blue
      s += "\\definecolor{color9}{RGB}{128, 255, 0}\n";    // lime
      s += "\\begin{figure}\n";
      s += "\\resizebox{\\textwidth}{!}{%\n";
      s += "\\begin{tikzpicture}[level distance=3cm, sibling "
           "distance=3cm]\n";
      s += tikz_traverse(_word_begin[0], _word_begin[1]);
      s += ";";
      s += "\\end{tikzpicture}\n";
      s += "}\n";
      s += "\\caption{Generalised suffix tree for the word";
      s += number_of_words() > 1 ? "s " : " ";
      for (size_t i = 0; i < number_of_words(); ++i) {
        s += tikz_word(_word_begin[i], _word_begin[i + 1]);
        s += (i < number_of_words() - 1 ? ", " : "");
      }
      s += ".}\n";
      s += "\\end{figure}\n";
      s += "\\end{document}";
      return s;
    }

    std::string SuffixTree::dot() const {
      if (number_of_words() == 0) {
        LIBSEMIGROUPS_EXCEPTION("expected at least 1 word, found 0");
      } else if (number_of_words() > 10) {
        LIBSEMIGROUPS_EXCEPTION("expected at most 10 words, found %llu",
                                uint64_t(number_of_words()));
      }

      static constexpr std::array<char const*, 7> colors = {
          "red", "blue", "green", "orange", "cyan", "darkorchid1", "deeppink"};

      std::string result = "digraph {\nordering=\"out\"\n";
      for (size_t i = 0; i < _nodes.size(); ++i) {
        auto color_index = _word_index_lookup[_nodes[i].l];
        result += std::to_string(i) + "[shape=box, width=.5, color="
                  + colors[color_index] + "]\n";
        for (auto const& child : _nodes[i].children) {
          auto const& l = _nodes[child.second].l;
          auto const& r = _nodes[child.second].r;
          color_index   = _word_index_lookup[l];
          result += std::to_string(i) + "->" + std::to_string(child.second)
                    + "[color=\"" + colors[color_index] + "\" label=\"["
                    + std::to_string(l) + "," + std::to_string(r)
                    + ")=" + dot_word(l, r) + "\"]\n";
        }
      }
      result += "}\n";
      return result;
    }

    ////////////////////////////////////////////////////////////////////////
    // SuffixTree - helpers - private
    ////////////////////////////////////////////////////////////////////////

    std::string SuffixTree::dot_word(size_t l, size_t r) const {
      auto        first = _word.cbegin() + l;
      auto        last  = _word.cbegin() + r;
      std::string w;
      for (auto it = first; it != last; ++it) {
        if (*it < _next_unique_letter) {
          if (*it < 96) {
            w += to_string(*it);
          } else {
            w += std::string(1, *it);
          }
        } else {
          w += "\\$_" + to_string(_word_index_lookup[l]);
        }
      }
      return w;
    }

    std::string SuffixTree::tikz_word(size_t l, size_t r) const {
      auto        first = _word.cbegin() + l;
      auto        last  = _word.cbegin() + r;
      std::string w;
      for (auto it = first; it != last; ++it) {
        if (*it < _next_unique_letter) {
          if (*it < 96) {
            w += to_string(*it);
          } else {
            w += std::string(1, *it);
          }
        } else {
          w += "\\$_" + to_string(_word_index_lookup[l]);
        }
      }
      return string_format("${\\color{color%d} %s }$",
                           _word_index_lookup[l],  // color index
                           w.c_str());
    }

    std::string SuffixTree::tikz_traverse(size_t i, bool rotate) const {
      using detail::to_string;
      std::string s;
      if (i == 0) {
        s += "\\";
      }

      auto color_index = _word_index_lookup[_nodes[i].l];
      s += string_format("node(%d){\\color{color%d} %d}\n", i, color_index, i);
      size_t count = 0;
      for (auto const& pair : _nodes[i].children) {
        s += string_format(
            "child{%s}\n",
            tikz_traverse(pair.second, count < _nodes[i].children.size() / 2)
                .c_str());
        count++;
      }
      if (i != 0) {
        std::string r = (rotate ? ", rotate=180" : "");
        s += string_format(
            "edge from parent [color=color%d] node[above, sloped, "
            "color%d%s]{$[%d, %d)$} node[below, sloped%s]{%s}\n",
            color_index,
            color_index,
            r.c_str(),
            _nodes[i].l,
            _nodes[i].r,
            r.c_str(),
            tikz_word(_nodes[i].l, _nodes[i].r).c_str());
      }
      return s;
    }

    SuffixTree::word_index_type
    SuffixTree::is_real_suffix(State const& st) const {
      if (!st.valid() || number_of_words() == 0) {
        return UNDEFINED;
      }
      auto const& n = _nodes[st.v];
      LIBSEMIGROUPS_ASSERT(st.pos != 0);
      LIBSEMIGROUPS_ASSERT(st.v != 0);
      LIBSEMIGROUPS_ASSERT(n.parent != UNDEFINED);
      LIBSEMIGROUPS_ASSERT(n.parent < _nodes.size());
      if (st.pos == n.length()) {
        LIBSEMIGROUPS_ASSERT(st.v != 0);
        return (n.is_real_suffix ? word_index(n) : UNDEFINED);
      }

      return (n.is_leaf() && n.length() - 1 == st.pos ? word_index(n)
                                                      : UNDEFINED);
    }

    size_t SuffixTree::distance_from_root(node_index_type i) const {
      LIBSEMIGROUPS_ASSERT(i < _nodes.size());
      size_t result = 0;
      while (_nodes[i].parent != UNDEFINED) {
        result += _nodes[i].length();
        i = _nodes[i].parent;
      }
      return result;
    }

    ////////////////////////////////////////////////////////////////////////
    // SuffixTree - member functions for queries - private
    ////////////////////////////////////////////////////////////////////////

    size_t SuffixTree::maximal_piece_prefix(word_index_type j) const {
      LIBSEMIGROUPS_ASSERT(j + 1 < _word_begin.size());
      // Follow the path from the root labelled by _words[i], then go back
      // from the leaf to its parent (which is an internal node),
      // corresponding to the maximal_piece_prefix.
      index_type      l = _word_begin[j];
      index_type      r = _word_begin[j + 1];
      node_index_type m = 0;
      while (l < r) {
        m = _nodes[m].child(_word[l]);
        l += _nodes[m].length();
      }
      return distance_from_root(_nodes[m].parent);
    }

    size_t SuffixTree::maximal_piece_suffix(word_index_type j) const {
      // TODO(later) Currently we do this for each of the N relation words,
      // so the cost is 2 N ^ 2, when instead we could do it once for all
      // relation words in O(N) time.
      //
      // The analysis of the previous paragraph also assumes that
      // distance_from_root is constant time, which with the current
      // implementation it isn't. It's O(M) where M is the number of
      // (actual) nodes on the path from a given node to the root. This
      // could also be preprocessed in O(N) time, for O(1) retrieval later.
      LIBSEMIGROUPS_ASSERT(j + 1 < _word_begin.size());
      size_t result = 0;

      for (size_t n = 0; n < _nodes.size(); ++n) {
        if (_nodes[n].child(unique_letter(j)) != UNDEFINED) {
          result = std::max(distance_from_root(n), result);
        }
      }
      return result;
    }

    size_t SuffixTree::maximal_piece_prefix(index_type l, index_type r) const {
      LIBSEMIGROUPS_ASSERT(l <= r);
      LIBSEMIGROUPS_ASSERT(l < _word.size());
      LIBSEMIGROUPS_ASSERT(r <= _word.size());
      // Follow the path from the root labelled by _words[i], then go back
      // from the leaf to its parent (which is an internal node),
      // corresponding to the maximal_piece_prefix.
      node_index_type m = 0;
      while (l < r) {
        m = _nodes[m].child(_word[l]);
        LIBSEMIGROUPS_ASSERT(m != UNDEFINED);
        l += _nodes[m].length();
      }
      return distance_from_root(_nodes[m].parent);
    }

    size_t SuffixTree::number_of_pieces(word_index_type i) const {
      // TODO(later) should really return once at least a proper prefix of Y
      // is found to be a piece
      LIBSEMIGROUPS_ASSERT(i + 1 < _word_begin.size());
      size_t result = 0;
      auto   l = _word_begin[i], r = _word_begin[i + 1];
      size_t N = 1;
      while (l < r - 1 && N != 0) {
        N = maximal_piece_prefix(l, r);
        l += N;
        result += 1;
      }
      return (l == r - 1 ? result : POSITIVE_INFINITY);
    }

    ////////////////////////////////////////////////////////////////////////
    // SuffixTree - validation - private
    ////////////////////////////////////////////////////////////////////////

    SuffixTree::word_index_type
    SuffixTree::word_index(word_type const& w) const {
      auto it = _map.find(w);
      if (it == _map.cend()) {
        LIBSEMIGROUPS_EXCEPTION("unknown word, the word %s does not "
                                "belong to the suffix tree",
                                detail::to_string(w).c_str())
      }
      return it->second;
    }

    ////////////////////////////////////////////////////////////////////////
    // The following functions go, split, get_link, and tree_extend are
    // minimally adapted from:
    //
    // https://cp-algorithms.com/string/suffix-tree-ukkonen.html
    ////////////////////////////////////////////////////////////////////////

    // Follow the path in the tree starting at the position described by
    // State st, and corresponding to the range [l, r) in _word.
    void SuffixTree::go(State& st, index_type l, index_type r) const {
      LIBSEMIGROUPS_ASSERT(l < _word.size());
      LIBSEMIGROUPS_ASSERT(r <= _word.size());
      LIBSEMIGROUPS_ASSERT(l <= r);
      while (l < r) {
        if (st.pos == _nodes[st.v].length()) {
          st = State(_nodes[st.v].child(_word[l]), 0);
          if (st.v == UNDEFINED) {
            return;
          }
        } else {
          if (_word[_nodes[st.v].l + st.pos] != _word[l]) {
            st.v   = UNDEFINED;
            st.pos = UNDEFINED;
            return;
          }
          if (r - l < _nodes[st.v].length() - st.pos) {
            st.pos += r - l;
            return;
          }
          l += _nodes[st.v].length() - st.pos;
          st.pos = _nodes[st.v].length();
        }
      }
    }

    // Split the node _nodes[st.v] into two nodes, the new node
    // with edge corresponding to
    //
    // [_nodes[st.v].l, _nodes[st.v].l + st.pos)
    //
    // and the old node with edge corresponding to
    //
    // [_nodes[st.v].l + st.pos, _nodes[st.v].r)
    SuffixTree::node_index_type SuffixTree::split(State const& st) {
      LIBSEMIGROUPS_ASSERT(st.v != UNDEFINED);
      LIBSEMIGROUPS_ASSERT(st.v < _nodes.size());
      LIBSEMIGROUPS_ASSERT(st.pos <= _nodes[st.v].length());

      if (st.pos == _nodes[st.v].length()) {
        LIBSEMIGROUPS_ASSERT(st.v < _nodes.size());
        return st.v;
      } else if (st.pos == 0) {
        LIBSEMIGROUPS_ASSERT(st.v != 0);
        LIBSEMIGROUPS_ASSERT(_nodes[st.v].parent < _nodes.size());
        return _nodes[st.v].parent;
      }
      node_index_type id = _nodes.size();
      _nodes.emplace_back(
          _nodes[st.v].l, _nodes[st.v].l + st.pos, _nodes[st.v].parent);
      _nodes[_nodes[st.v].parent].child(_word[_nodes[st.v].l]) = id;
      _nodes[id].child(_word[_nodes[st.v].l + st.pos])         = st.v;
      _nodes[st.v].parent                                      = id;
      _nodes[st.v].l += st.pos;
      LIBSEMIGROUPS_ASSERT(id < _nodes.size());
      return id;
    }

    // Get the suffix link of a node by index
    SuffixTree::node_index_type SuffixTree::get_link(node_index_type v) {
      LIBSEMIGROUPS_ASSERT(v < _nodes.size());
      if (_nodes[v].link != UNDEFINED) {
        return _nodes[v].link;
      } else if (_nodes[v].parent == UNDEFINED) {
        return 0;
      }
      auto to = get_link(_nodes[v].parent);
      LIBSEMIGROUPS_ASSERT(to < _nodes.size());
      State st(to, _nodes[to].length());
      go(st, _nodes[v].l + (_nodes[v].parent == 0), _nodes[v].r);
      LIBSEMIGROUPS_ASSERT(st.v != UNDEFINED);
      LIBSEMIGROUPS_ASSERT(st.pos <= _nodes[st.v].length());
      // WARNING: the assignment of xxx to split(st) in the next line looks
      // redundant, but isn't! Without this the test fail when compiling
      // with gcc (9, 10, 11, at least)!
      auto xxx       = split(st);
      _nodes[v].link = xxx;
      LIBSEMIGROUPS_ASSERT(_nodes[v].link < _nodes.size());
      return _nodes[v].link;
    }

    // Perform the phase starting with the pos letter of the word.
    void SuffixTree::tree_extend(index_type pos) {
      LIBSEMIGROUPS_ASSERT(pos < _word.size());
      for (;;) {
        State nptr = go2(_ptr, pos, pos + 1);
        if (nptr.v != UNDEFINED) {
          _ptr = nptr;
          return;
        }

        auto mid  = split(_ptr);
        auto leaf = _nodes.size();
        _nodes.emplace_back(pos, _word.size(), mid);
        _nodes[mid].child(_word[pos]) = leaf;

        _ptr.v = get_link(mid);
        LIBSEMIGROUPS_ASSERT(_ptr.v < _nodes.size());
        _ptr.pos = _nodes[_ptr.v].length();
        if (mid == 0) {
          break;
        }
      }
    }
    namespace suffix_tree_helper {
      void add_words(SuffixTree& st, std::vector<word_type> const& words) {
        for (auto const& word : words) {
          st.add_word(word);
        }
      }
    }  // namespace suffix_tree_helper
  }    // namespace detail
}  // namespace libsemigroups

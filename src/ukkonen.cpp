//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021-2023 James D. Mitchell + Maria Tsalakou
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

// This file contains the implementation of the Ukkonen class.

#include "libsemigroups/ukkonen.hpp"

#include <algorithm>  // for lower_bound, sort, max
#include <array>      // for array
#include <cstddef>    // for size_t
#include <cstdint>    // for uint64_t
#include <numeric>    // for accumulate
#include <string>     // for operator+, char_traits, to_st...

#include "libsemigroups/constants.hpp"  // for UNDEFINED
#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/types.hpp"      // for word_type

#include "libsemigroups/detail/string.hpp"  // for to_string

namespace libsemigroups {

  using State = Ukkonen::State;
  using Node  = Ukkonen::Node;

  Node::Node(index_type ll,
             index_type rr,
             size_t     prnt)  // node_index_type
      : l(ll),
        r(rr),
        parent(prnt),
        link(UNDEFINED),
        is_real_suffix(false),
        children() {}

  // Return type is node_index_type
  size_t& Node::child(letter_type c) {
    if (children.count(c) == 0) {
      children[c] = UNDEFINED;
    }
    return children[c];
  }

  // Return type is node_index_type
  size_t Node::child(letter_type c) const {
    if (children.count(c) == 0) {
      return UNDEFINED;
    }
    return children[c];
  }

  ////////////////////////////////////////////////////////////////////////
  // Ukkonen - constructors - public
  ////////////////////////////////////////////////////////////////////////

  Ukkonen::Ukkonen() {
    init();
  }

  Ukkonen& Ukkonen::init() {
    _max_word_length = 0;
    _multiplicity.clear();
    _next_unique_letter = static_cast<unique_letter_type>(-1);
    _nodes              = {Node()};
    _ptr                = State(0, 0);
    _word_begin         = {0};
    _word_index_lookup.clear();
    _word.clear();
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // Ukkonen - initialisation - public
  ////////////////////////////////////////////////////////////////////////

  void Ukkonen::add_word_no_checks(const_iterator first, const_iterator last) {
    if (first >= last) {
      return;
    }
    size_t const ndx = index(first, last);
    if (ndx != UNDEFINED) {
      // Duplicate word, do nothing
      _multiplicity[ndx]++;
      return;
    }

    _multiplicity.push_back(1);
    size_t const n = std::distance(first, last);

    _max_word_length          = std::max(n, _max_word_length);
    size_t const old_length   = _word.size();
    size_t const old_nr_nodes = _nodes.size();

    _word.insert(_word.end(), first, last);
    _word.push_back(_next_unique_letter--);
    _word_begin.push_back(_word.size());
    _word_index_lookup.resize(_word.size(), number_of_distinct_words() - 1);

    for (index_type i = old_length; i < _word.size(); ++i) {
      tree_extend(i);
    }

    for (node_index_type i = old_nr_nodes; i < _nodes.size(); ++i) {
      auto& n = _nodes[i];
      for (auto const& child : n.children) {
        if (is_unique_letter(child.first)) {
          n.is_real_suffix = true;
          break;
        }
      }
    }
  }

  size_t Ukkonen::length_of_words() const noexcept {
    size_t const n      = number_of_distinct_words();
    size_t       result = 0;
    for (size_t i = 0; i < n; ++i) {
      result += word_length(i) * multiplicity(i);
    }
    return result;
  }

  size_t Ukkonen::multiplicity(word_index_type i) const {
    return _multiplicity[i];
  }

  // TODO(later) could do some more caching here, i.e. only check the children
  // if we have added more words since the value was last computed.
  bool Ukkonen::is_real_suffix(Node const& n) const {
    if (n.is_real_suffix) {
      return true;
    }
    for (auto const& child : n.children) {
      if (is_unique_letter(child.first)) {
        n.is_real_suffix = true;
        break;
      }
    }
    return n.is_real_suffix;
  }

  Ukkonen::word_index_type Ukkonen::is_suffix(State const& st) const {
    if (number_of_distinct_words() == 0) {
      return UNDEFINED;
    }
    auto const& n = _nodes[st.v];
    LIBSEMIGROUPS_ASSERT(st.pos != 0);
    LIBSEMIGROUPS_ASSERT(st.v != 0);
    LIBSEMIGROUPS_ASSERT(n.parent != UNDEFINED);
    LIBSEMIGROUPS_ASSERT(n.parent < _nodes.size());
    if (st.pos == n.length()) {
      LIBSEMIGROUPS_ASSERT(st.v != 0);
      return (is_real_suffix(n) ? word_index(n) : UNDEFINED);
    }

    return (n.is_leaf() && n.length() - 1 == st.pos ? word_index(n)
                                                    : UNDEFINED);
  }

  size_t Ukkonen::distance_from_root(Node const& n) const {
    size_t      result = 0;
    Node const* m      = &n;
    while (m->parent != UNDEFINED) {
      result += m->length();
      m = &_nodes[m->parent];
    }
    return result;
  }

  ////////////////////////////////////////////////////////////////////////
  // The following functions go, split, get_link, and tree_extend are
  // minimally adapted from:
  //
  // https://cp-algorithms.com/string/suffix-tree-ukkonen.html
  ////////////////////////////////////////////////////////////////////////

  // Follow the path in the tree starting at the position described by
  // State st, and corresponding to the range [l, r) in _word.
  void Ukkonen::go(State& st, index_type l, index_type r) const {
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
  Ukkonen::node_index_type Ukkonen::split(State const& st) {
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
  Ukkonen::node_index_type Ukkonen::get_link(node_index_type v) {
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
  void Ukkonen::tree_extend(index_type pos) {
    LIBSEMIGROUPS_ASSERT(pos < _word.size());
    for (;;) {
      State nptr(_ptr);
      go(nptr, pos, pos + 1);
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

  namespace ukkonen {
    void add_words_no_checks(Ukkonen& st, std::vector<word_type> const& words) {
      for (auto const& word : words) {
        st.add_word_no_checks(word);
      }
    }

    void add_words(Ukkonen& st, std::vector<word_type> const& words) {
      for (auto const& word : words) {
        st.add_word(word);
      }
    }

    size_t number_of_distinct_subwords(Ukkonen const& u) {
      auto nodes = u.nodes();
      return std::accumulate(nodes.cbegin(),
                             nodes.cend(),
                             size_t(0),
                             [](size_t& tot, auto const& n) {
                               tot += n.length();
                               return tot;
                             })
             - (u.length_of_distinct_words() + u.number_of_distinct_words())
             + 1;  // empty word
    }

    std::vector<word_type> pieces_no_checks(Ukkonen const&   u,
                                            word_type const& w) {
      return pieces_no_checks<word_type>(u, w);
    }

    std::vector<std::string> pieces_no_checks(Ukkonen const& u, char const* w) {
      return pieces_no_checks<std::string>(u, std::string(w));
    }

    namespace {
      std::string dot_word(Ukkonen const& u, Ukkonen::Node const& n) {
        auto        first = u.cbegin() + n.l;
        auto        last  = u.cbegin() + n.r;
        std::string w;
        for (auto it = first; it != last; ++it) {
          if (!u.is_unique_letter(*it)) {
            if (*it < 96) {
              w += ::libsemigroups::detail::to_string(*it);
            } else {
              w += std::string(1, *it);
            }
          } else {
            w += "\\$_" + ::libsemigroups::detail::to_string(u.word_index(n));
          }
        }
        return w;
      }
    }  // namespace

    std::string dot(Ukkonen const& u) {
      if (u.number_of_distinct_words() == 0) {
        LIBSEMIGROUPS_EXCEPTION("expected at least 1 word, found 0");
      }
      static const std::vector<std::array<char const*, 24>> colors = {
          {"#00ff00"},
          {"#00ff00", "#ff00ff"},
          {"#00ff00", "#ff00ff", "#007fff"},
          {"#00ff00", "#ff00ff", "#007fff", "#ff7f00"},
          {"#00ff00", "#ff00ff", "#007fff", "#ff7f00", "#7fbf7f"},
          {"#00ff00", "#ff00ff", "#007fff", "#ff7f00", "#7fbf7f", "#600aa7"},
          {"#08fd2d",
           "#ff00ff",
           "#ff7f00",
           "#007fff",
           "#7f3f7f",
           "#7cc8a4",
           "#b1fa0f"},
          {"#00ff00",
           "#ff00ff",
           "#007fff",
           "#ff7f00",
           "#7fbf7f",
           "#5104a5",
           "#b30a0e",
           "#00ffff"},
          {"#00ff00",
           "#ff00ff",
           "#007fff",
           "#ff7f00",
           "#7fbf7f",
           "#3503a6",
           "#cd0030",
           "#c278f8",
           "#028325"},
          {"#1cfe08",
           "#cd02fe",
           "#ff7f00",
           "#0688fd",
           "#63715c",
           "#c40240",
           "#0107a3",
           "#c5fe39",
           "#4cf6b1",
           "#ec88bd"},
          {"#00ff00",
           "#ff00ff",
           "#007fff",
           "#ff7f00",
           "#7fbf7f",
           "#640b98",
           "#f50244",
           "#e9fd0e",
           "#c779fd",
           "#027f18",
           "#00ffff"},
          {"#00ff00",
           "#ff00ff",
           "#007fff",
           "#ff7f00",
           "#7fbf7f",
           "#8e2e6b",
           "#0b02c9",
           "#fdfc2e",
           "#ff0000",
           "#00ffff",
           "#fb7ebf",
           "#1a810f"},
          {"#02fd27",
           "#ff00ff",
           "#007fff",
           "#ff7f00",
           "#7f3f7f",
           "#bafa35",
           "#5efcce",
           "#398506",
           "#cf0000",
           "#0000ff",
           "#d084f0",
           "#0aaa81",
           "#00007f"},
          {"#00ff00",
           "#ff00ff",
           "#007fff",
           "#ff7f00",
           "#7fbf7f",
           "#600aa0",
           "#b20505",
           "#b16efe",
           "#397807",
           "#dbfe01",
           "#00ffff",
           "#0b6782",
           "#00ff7f",
           "#fc4789"},
          {"#00ff00",
           "#e000f4",
           "#ff7f00",
           "#01a0f6",
           "#0102be",
           "#64695d",
           "#6cf893",
           "#c50404",
           "#b685f7",
           "#f8fe2a",
           "#88c005",
           "#007f00",
           "#fe518e",
           "#7f007f",
           "#ceba8a"},
          {"#00ff00",
           "#ff00ff",
           "#007fff",
           "#ff7f00",
           "#7fbf7f",
           "#4a01a2",
           "#c3002c",
           "#1efef5",
           "#ffff00",
           "#067048",
           "#d07bf8",
           "#925372",
           "#00ff7f",
           "#feb17f",
           "#6ba804",
           "#0000ff"},
          {"#00ff00",
           "#ff00ff",
           "#007fff",
           "#ff7f00",
           "#7fbf7f",
           "#4107a6",
           "#cb0331",
           "#5e6b00",
           "#cc81fb",
           "#d4fd0e",
           "#00ffff",
           "#09777b",
           "#00ff7f",
           "#fab181",
           "#a24d85",
           "#7fffff",
           "#8237f9"},
          {"#00ff00",
           "#fd01c8",
           "#007fff",
           "#f1a504",
           "#4b804c",
           "#4cf5b5",
           "#e40500",
           "#4603d1",
           "#c585f2",
           "#92f926",
           "#b13c78",
           "#f4fa71",
           "#212567",
           "#bba277",
           "#985906",
           "#08ae98",
           "#007f00",
           "#720616"},
          {"#00ff00",
           "#ff00ff",
           "#007fff",
           "#ff7f00",
           "#7fbf7f",
           "#611b95",
           "#fafb2c",
           "#f5023d",
           "#087e27",
           "#fc80df",
           "#00ffff",
           "#0000ff",
           "#894810",
           "#00ff7f",
           "#85e0fa",
           "#84af00",
           "#856dd6",
           "#1c6b92",
           "#7f00ff"},
          {"#e500ea", "#00ff00", "#007fff", "#ff7f00", "#7fbf7f",
           "#2d049f", "#e20131", "#2b7e04", "#d0fa01", "#00ffff",
           "#ff7fff", "#ad5490", "#00ff7f", "#007f7f", "#83b2fc",
           "#f5a680", "#761f38", "#7528f8", "#ffff7f", "#0000ff"},
          {"#fd02c8", "#00ff00", "#007fff", "#e2a209", "#4ffaaf", "#4a08aa",
           "#c81007", "#22825d", "#c781de", "#d8fd5e", "#76d11f", "#fa5b6b",
           "#634b07", "#805890", "#8bcbfd", "#0000ff", "#8a35fe", "#00ffff",
           "#06e560", "#a4b587", "#007f00"},
          {"#01fd34", "#ff00ff", "#0265fe", "#ff7f00", "#00ffff", "#8bad93",
           "#b80645", "#b2f203", "#3c7f00", "#ff7fff", "#2002aa", "#007f7f",
           "#9545bf", "#fec370", "#7fffff", "#45fe94", "#f65681", "#4e3541",
           "#a35504", "#7f00ff", "#5aa6f7", "#61ca33"},
          {"#00ff00", "#ff00ff", "#007fff", "#ff7f00", "#7fbf7f", "#6e0f8f",
           "#f97ec5", "#f50129", "#00ffff", "#ffff00", "#2a7d10", "#0000ff",
           "#00ff7f", "#944afe", "#16708c", "#831803", "#89fc18", "#be555b",
           "#84b1fe", "#f5c675", "#03156a", "#a1a616", "#80f9ce"},
          {"#00ff00", "#ff00ff", "#007fff", "#ff7f00", "#7fbf7f", "#4604ac",
           "#de0328", "#19801d", "#d881f5", "#00ffff", "#ffff00", "#00ff7f",
           "#ad5867", "#85f610", "#84e9f5", "#f5c778", "#207090", "#764ef3",
           "#7b4c00", "#0000ff", "#b80c9a", "#601045", "#29b7c0", "#839f12"}};
      if (u.number_of_distinct_words() > colors.size()) {
        LIBSEMIGROUPS_EXCEPTION("expected at most %llu words, found %llu",
                                uint64_t(colors.size()),
                                uint64_t(u.number_of_distinct_words()));
      }

      std::string  result = "digraph {\nordering=\"out\"\n";
      auto const&  nodes  = u.nodes();
      size_t const n      = u.number_of_distinct_words();
      for (size_t i = 0; i < nodes.size(); ++i) {
        auto color_index = (i == 0 ? 0 : u.word_index(nodes[i]));
        result += std::to_string(i) + "[shape=box, width=.5, color=\""
                  + colors[n][color_index] + "\"]\n";
        for (auto const& child : nodes[i].children) {
          auto const& l = nodes[child.second].l;
          auto const& r = nodes[child.second].r;
          color_index   = u.word_index(nodes[child.second]);
          result += std::to_string(i) + "->" + std::to_string(child.second)
                    + "[color=\"" + colors[n][color_index] + "\" label=\"["
                    + std::to_string(l) + "," + std::to_string(r)
                    + ")=" + dot_word(u, nodes[child.second]) + "\"]\n";
        }
      }
      result += "}\n";
      return result;
    }

    namespace detail {
      GreedyReduceHelper::GreedyReduceHelper(Ukkonen const& st)
          : _best(),
            _best_goodness(),
            _distance_from_root(st.nodes().size(), 0),
            _num_leafs(st.nodes().size(), 0),
            _scratch(),
            _suffix_index() {}

      void GreedyReduceHelper::pre_order(Ukkonen const& st, size_t v) {
        auto const& nodes = st.nodes();
        // This is a tree so we've never seen v before!
        if (!nodes[v].is_root()) {
          _distance_from_root[v]
              = _distance_from_root[nodes[v].parent] + nodes[v].length();
        }
        if (nodes[v].is_leaf()) {
          _num_leafs[v]++;
          // Starting index of the suffix that the leaf corresponds to
          _suffix_index.push_back(nodes[v].r - _distance_from_root[v]);
        }
      }

      void GreedyReduceHelper::post_order(Ukkonen const& st, size_t v) {
        auto const& nodes = st.nodes();
        if (nodes[v].is_leaf() || nodes[v].is_root()) {
          return;
        }

        for (auto const& child : nodes[v].children) {
          _num_leafs[v] += _num_leafs[child.second];
        }
        _scratch.assign(_suffix_index.cend() - _num_leafs[v],
                        _suffix_index.cend());
        std::sort(_scratch.begin(), _scratch.end());
        // number of non-overlapping subwords corresponding to the node v
        size_t num_non_overlap = st.multiplicity(st.word_index(_scratch[0]));

        // Only try greedily matching non-overlapping subwords from left to
        // right
        auto subword_begin = _scratch[0];
        auto it            = _scratch.cbegin();
        do {
          auto subword_end = subword_begin + _distance_from_root[v];
          it               = std::lower_bound(it, _scratch.cend(), subword_end);
          if (it != _scratch.cend()) {
            subword_begin = *it;
            num_non_overlap += st.multiplicity(st.word_index(subword_begin));
          }
        } while (it != _scratch.cend());
        int goodness = (_distance_from_root[v] * num_non_overlap)
                       - num_non_overlap - (_distance_from_root[v] + 1);
        if (goodness > _best_goodness) {
          _best          = v;
          _best_goodness = goodness;
        }
      }

      using const_iterator = typename Ukkonen::const_iterator;

      std::pair<const_iterator, const_iterator>
      GreedyReduceHelper::yield(Ukkonen const& st) {
        auto const& nodes = st.nodes();
        if (nodes[_best].is_root()) {
          return {st.cbegin(), st.cbegin()};
        }
        return {
            st.cbegin()
                + (nodes[_best].l - _distance_from_root[nodes[_best].parent]),
            st.cbegin() + nodes[_best].r};
      }
    }  // namespace detail
  }    // namespace ukkonen
}  // namespace libsemigroups

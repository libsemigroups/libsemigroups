//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023-2025 Murray T. Whyte
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

// This file contains implementations of the functions declared in
// presentation.hpp

#include <algorithm>      // for transform
#include <cctype>         // for isprint
#include <cstddef>        // for size_t
#include <string>         // for basic_string, operator+
#include <unordered_map>  // for operator==
#include <vector>         // for vector

#include "libsemigroups/exception.hpp"     // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/presentation.hpp"  // for Presentation, to_string, to_word
#include "libsemigroups/types.hpp"         // for word_type
#include "libsemigroups/word-range.hpp"    // for human_readable_letter

#include "libsemigroups/detail/fmt.hpp"  // for format

namespace libsemigroups {
  namespace detail {

    GreedyReduceHelper::GreedyReduceHelper(Ukkonen const& st)
        : _best(),
          _best_goodness(),
          _distance_from_root(st.nodes().size(), 0),
          _num_leafs(st.nodes().size(), 0),
          _scratch(),
          _suffix_index() {}

    GreedyReduceHelper::~GreedyReduceHelper() = default;

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
      return {st.cbegin()
                  + (nodes[_best].l - _distance_from_root[nodes[_best].parent]),
              st.cbegin() + nodes[_best].r};
    }
  }  // namespace detail

  namespace presentation {
    // TODO(1) can the following functions be simplified by using
    // std::string_view instead? I.e. reduce the number of functions

    void add_rule_no_checks(Presentation<std::string>& p,
                            char const*                lhop,
                            char const*                rhop) {
      // TODO(1) pass iterators to p.add_rule and don't construct std::string
      // here
      add_rule_no_checks(p, std::string(lhop), std::string(rhop));
    }

    void add_rule_no_checks(Presentation<std::string>& p,
                            std::string const&         lhop,
                            char const*                rhop) {
      // TODO(1) pass iterators to p.add_rule and don't construct std::string
      // here
      add_rule_no_checks(p, lhop, std::string(rhop));
    }

    void add_rule_no_checks(Presentation<std::string>& p,
                            char const*                lhop,
                            std::string const&         rhop) {
      add_rule_no_checks(p, std::string(lhop), rhop);
    }

    void add_rule(Presentation<std::string>& p,
                  char const*                lhop,
                  char const*                rhop) {
      // TODO(1) pass iterators to p.add_rule and don't construct std::string
      // here
      add_rule(p, std::string(lhop), std::string(rhop));
    }

    void add_rule(Presentation<std::string>& p,
                  std::string const&         lhop,
                  char const*                rhop) {
      // TODO(1) pass iterators to p.add_rule and don't construct std::string
      // here
      add_rule(p, lhop, std::string(rhop));
    }

    void add_rule(Presentation<std::string>& p,
                  char const*                lhop,
                  std::string const&         rhop) {
      // TODO(1) pass iterators to p.add_rule and don't construct std::string
      // here
      add_rule(p, std::string(lhop), rhop);
    }

    void add_inverse_rules(Presentation<std::string>& p,
                           char const*                vals,
                           char                       e) {
      add_inverse_rules(p, std::string(vals), e);
    }

    typename Presentation<std::string>::letter_type
    replace_word_with_new_generator(Presentation<std::string>& p,
                                    char const*                w) {
      return replace_word_with_new_generator(p, w, w + std::strlen(w));
    }

    std::string to_gap_string(Presentation<word_type> const& p,
                              std::string const&             var_name) {
      p.throw_if_bad_alphabet_or_rules();
      if (p.alphabet().size() > 49) {
        LIBSEMIGROUPS_EXCEPTION("expected at most 49 generators, found {}!",
                                p.alphabet().size());
      }

      auto to_gap_word = [](word_type const& w) -> std::string {
        if (w.empty()) {
          return "One(F)";
        }
        std::string out;
        std::string sep = "";
        for (auto it = w.cbegin(); it < w.cend(); ++it) {
          out += sep + words::human_readable_letter<>(*it);
          sep = " * ";
        }
        return out;
      };

      std::string out = "F := Free";
      if (p.contains_empty_word()) {
        out += "Monoid(";
      } else {
        out += "Semigroup(";
      }

      std::string sep = "";
      for (auto it = p.alphabet().cbegin(); it != p.alphabet().cend(); ++it) {
        out += fmt::format(
            "{}\"{}\"", sep, words::human_readable_letter<>(*it));
        sep = ", ";
      }
      out += ");\n";

      out += "AssignGeneratorVariables(F);;\n";

      out += "R := [";
      sep = "";
      for (auto it = p.rules.cbegin(); it < p.rules.cend(); it += 2) {
        out += fmt::format("{}\n          [{}, {}]",
                           sep,
                           to_gap_word(*it),
                           to_gap_word(*(it + 1)));
        sep = ", ";
      }
      out += "\n         ];\n";
      out += var_name + " := F / R;\n";
      return out;
    }

    std::string to_gap_string(Presentation<std::string> const& p,
                              std::string const&               var_name) {
      return to_gap_string(v4::to<Presentation<word_type>>(p), var_name);
    }

  }  // namespace presentation
}  // namespace libsemigroups

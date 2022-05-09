//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 James D. Mitchell
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

// This file contains the implementation of a class template for semigroup or
// monoid presentations.

namespace libsemigroups {

  template <typename W>
  Presentation<W>::Presentation()
      : _alphabet(), _alphabet_map(), _contains_empty_word(false), rules() {}

  template <typename W>
  Presentation<W>& Presentation<W>::alphabet(size_type n) {
    word_type lphbt(n, 0);
    std::iota(lphbt.begin(), lphbt.end(), 0);
    return alphabet(lphbt);
  }

  template <typename W>
  Presentation<W>& Presentation<W>::alphabet(word_type const& lphbt) {
    // We copy the _alphabet_map for exception safety
    decltype(_alphabet_map) alphabet_map;
    auto                    old_alphabet = std::move(_alphabet);
    _alphabet                            = lphbt;
    try_set_alphabet(alphabet_map, old_alphabet);
    return *this;
  }

  template <typename W>
  Presentation<W>& Presentation<W>::alphabet(word_type&& lphbt) {
    // We copy the _alphabet_map for exception safety
    decltype(_alphabet_map) alphabet_map;
    auto                    old_alphabet = std::move(_alphabet);
    _alphabet                            = std::move(lphbt);
    try_set_alphabet(alphabet_map, old_alphabet);
    return *this;
  }

  template <typename W>
  Presentation<W>& Presentation<W>::alphabet_from_rules() {
    _alphabet_map.clear();
    _alphabet.clear();
    size_type index = 0;
    for (auto const& rel : rules) {
      if (rel.empty()) {
        contains_empty_word(true);
      }
      for (auto const& letter : rel) {
        if (_alphabet_map.emplace(letter, index).second) {
          _alphabet.push_back(letter);
          ++index;
        }
      }
    }
    return *this;
  }

  template <typename W>
  void Presentation<W>::validate_letter(letter_type c) const {
    if (_alphabet.empty()) {
      LIBSEMIGROUPS_EXCEPTION("no alphabet has been defined");
    } else if (_alphabet_map.find(c) == _alphabet_map.cend()) {
      if (std::is_same<letter_type, char>::value) {
        LIBSEMIGROUPS_EXCEPTION("invalid letter %c, valid letter are %s",
                                c,
                                detail::to_string(_alphabet).c_str());
      } else {
        LIBSEMIGROUPS_EXCEPTION("invalid letter %llu, valid letter are %s",
                                uint64_t(c),
                                detail::to_string(_alphabet).c_str());
      }
    }
  }

  template <typename W>
  template <typename T>
  void Presentation<W>::validate_word(T first, T last) const {
    if (!_contains_empty_word && std::distance(first, last) == 0) {
      LIBSEMIGROUPS_EXCEPTION("words in rules cannot be empty");
    }
    for (auto it = first; it != last; ++it) {
      validate_letter(*it);
    }
  }

  template <typename W>
  void Presentation<W>::validate_rules() const {
    if (rules.size() % 2 == 1) {
      LIBSEMIGROUPS_EXCEPTION("expected even length, found %llu",
                              uint64_t(rules.size()));
    }
    for (auto const& rel : rules) {
      validate_word(rel.cbegin(), rel.cend());
    }
  }

  template <typename W>
  void Presentation<W>::validate_alphabet(
      decltype(_alphabet_map)& alphabet_map) const {
    size_type index = 0;
    for (auto const& letter : _alphabet) {
      auto it = alphabet_map.emplace(letter, index++);
      if (!it.second) {
        LIBSEMIGROUPS_EXCEPTION("invalid alphabet, duplicate letter %s!",
                                detail::to_string(letter).c_str());
      }
    }
  }

  template <typename W>
  void Presentation<W>::try_set_alphabet(decltype(_alphabet_map)& alphabet_map,
                                         word_type& old_alphabet) {
    try {
      validate_alphabet(alphabet_map);
    } catch (LibsemigroupsException& e) {
      _alphabet = std::move(old_alphabet);
      throw;
    }
    _alphabet_map = std::move(alphabet_map);
  }

  namespace presentation {
    template <typename W>
    void add_identity_rules(Presentation<W>&                      p,
                            typename Presentation<W>::letter_type id) {
      p.validate_letter(id);
      for (auto it = p.alphabet().cbegin(); it != p.alphabet().cend(); ++it) {
        W       lhs = {*it, id};
        W const rhs = {*it};
        add_rule(p, lhs, rhs);
        if (*it != id) {
          lhs = {id, *it};
          add_rule(p, lhs, rhs);
        }
      }
    }

    template <typename W>
    void add_inverse_rules(Presentation<W>&                      p,
                           W const&                              vals,
                           typename Presentation<W>::letter_type id) {
      p.validate_word(vals.begin(), vals.end());

      if (vals.size() != p.alphabet().size()) {
        LIBSEMIGROUPS_EXCEPTION("invalid inverses, expected %s but found %s",
                                detail::to_string(p.alphabet().size()).c_str(),
                                detail::to_string(vals.size()).c_str());
      }

      W cpy = vals;
      std::sort(cpy.begin(), cpy.end());
      for (auto it = cpy.cbegin(); it < cpy.cend() - 1; ++it) {
        if (*it == *(it + 1)) {
          LIBSEMIGROUPS_EXCEPTION(
              "invalid inverses, they contain the duplicate letter "
              + detail::to_string(*it));
        }
      }

      // Check that (x ^ - 1) ^ -1 = x
      for (size_t i = 0; i < p.alphabet().size(); ++i) {
        if (p.letter(i) == id && vals[i] != id) {
          LIBSEMIGROUPS_EXCEPTION(
              "invalid inverses, the identity is %c, but %c ^ -1 != %c",
              p.letter(i),
              p.letter(i),
              vals[i]);
        }
        for (size_t j = 0; j < p.alphabet().size(); ++j) {
          if (p.letter(j) == vals[i]) {
            if (vals[j] != p.letter(i)) {
              LIBSEMIGROUPS_EXCEPTION(
                  "invalid inverses, %c ^ -1 = %c but %c ^ -1 = %c",
                  p.letter(i),
                  vals[i],
                  vals[i],
                  vals[j]);
            }
            break;
          }
        }
      }

      W rhs;
      if (id == UNDEFINED) {
        rhs = {};
      } else {
        rhs = {id};
      }
      for (size_t i = 0; i < p.alphabet().size(); ++i) {
        W lhs = {p.letter(i), vals[i]};
        if (p.letter(i) != id) {
          add_rule(p, lhs, rhs);
        }
      }
    }

    template <typename W>
    void remove_duplicate_rules(Presentation<W>& p) {
      using libsemigroups::shortlex_compare;
      std::unordered_set<std::pair<W, W>, Hash<std::pair<W, W>>> relations_set;

      for (auto it = p.rules.begin(); it != p.rules.end(); it += 2) {
        if (shortlex_compare(*it, *(it + 1))) {
          relations_set.emplace(*it, *(it + 1));
        } else {
          relations_set.emplace(*(it + 1), *it);
        }
      }
      p.rules.clear();
      for (auto const& rel : relations_set) {
        add_rule(p, rel.first, rel.second);
      }
    }

    template <typename W>
    void reduce_complements(Presentation<W>& p) {
      libsemigroups::detail::Duf<> duf;
      duf.resize(p.rules.size());
      std::unordered_map<W, size_t, Hash<W>, EqualTo<W>> map;

      // Create equivalence relation of the equal relations
      for (size_t i = 0; i < p.rules.size(); ++i) {
        if (i % 2 == 0) {
          duf.unite(i, i + 1);
        }
        auto const&                      current_word = p.rules[i];
        typename decltype(map)::iterator it;
        bool                             inserted;
        std::tie(it, inserted) = map.emplace(current_word, i);
        if (!inserted) {
          duf.unite(it->second, i);
        }
      }

      using libsemigroups::shortlex_compare;

      // Class index -> index of min. length words in wrt + words
      std::unordered_map<size_t, W> mins;

      // Find index of minimum length word in every class
      for (auto const& word : p.rules) {
        auto                              i = map.find(word)->second;
        auto                              j = duf.find(i);
        typename decltype(mins)::iterator it;
        bool                              inserted;
        std::tie(it, inserted) = mins.emplace(j, word);
        auto const& min_word   = it->second;
        if (!inserted && shortlex_compare(word, min_word)) {
          it->second = word;
        }
      }

      p.rules.clear();
      for (auto it = map.cbegin(); it != map.cend(); ++it) {
        auto const& word     = it->first;
        auto const& index    = it->second;
        auto const& min_word = mins.find(duf.find(index))->second;
        if (!std::equal(word.cbegin(),
                        word.cend(),
                        min_word.cbegin(),
                        min_word.cend())) {
          add_rule(p, min_word, word);
        }
      }
    }

    template <typename W>
    void sort_each_rule(Presentation<W>& p) {
      // Sort each relation so that the lhs is greater than the rhs according
      // to func.
      for (auto it = p.rules.begin(); it < p.rules.end(); it += 2) {
        if (shortlex_compare(*it, *(it + 1))) {
          std::swap(*it, *(it + 1));
        }
      }
    }

    template <typename W>
    void sort_rules(Presentation<W>& p) {
      // Create a permutation of the even indexed entries in vec
      using letter_type = typename Presentation<W>::letter_type;

      std::vector<letter_type> perm;
      auto                     n = p.rules.size() / 2;
      perm.resize(n);
      std::iota(perm.begin(), perm.end(), 0);
      std::sort(perm.begin(), perm.end(), [&p](auto x, auto y) -> bool {
        return shortlex_compare(p.rules[2 * x], p.rules[2 * y]);
      });
      // Apply the permutation (adapted from stl.hpp:apply_permutation)
      for (letter_type i = 0; static_cast<decltype(n)>(i) < n; ++i) {
        letter_type current = i;
        while (i != perm[current]) {
          letter_type next = perm[current];
          std::swap(p.rules[2 * current], p.rules[2 * next]);
          std::swap(p.rules[2 * current + 1], p.rules[2 * next + 1]);
          perm[current] = current;
          current       = next;
        }
        perm[current] = current;
      }
    }

    template <typename W>
    auto longest_common_subword(Presentation<W>& p) {
      detail::SuffixTree st;
      detail::suffix_tree_helper::add_words(
          st, p.rules.cbegin(), p.rules.cend());
      detail::DFSHelper helper(st);
      // Get the best word [first, last) so that replacing every
      // non-overlapping occurrence of [first, last) in p.rules with a new
      // generator "x", and adding "x = [first, last)" as a relation reduces
      // the length of the presentation as much as possible.
      word_type::const_iterator first, last;
      std::tie(first, last) = st.dfs(helper);
      // It'd be more pleasing to return first and last here, but they point at
      // the word contained in the SuffixTree st, which is destroyed after we
      // exit this function.
      return W(first, last);
    }

    template <typename W, typename T>
    void replace_subword(Presentation<W>& p, T first, T last) {
      using letter_type   = typename Presentation<W>::letter_type;
      letter_type const x = static_cast<letter_type>(p.alphabet().size());
      p.alphabet(p.alphabet().size() + 1);
      auto replace_subword = [&first, &last, &x](W& word) {
        auto it = std::search(word.begin(), word.end(), first, last);
        while (it != word.end()) {
          // found [first, last)
          *it      = x;
          auto pos = it - word.begin();
          word.erase(it + 1, it + (last - first));  // it not valid
          it = std::search(word.begin() + pos + 1, word.end(), first, last);
        }
      };
      std::for_each(p.rules.begin(), p.rules.end(), replace_subword);
      p.rules.emplace_back(W({x}));
      p.rules.emplace_back(first, last);
    }

    template <typename W>
    void normalize_alphabet(Presentation<W>& p) {
      using size_type   = typename Presentation<W>::size_type;
      using letter_type = typename Presentation<W>::letter_type;

      p.validate();
      W    norm_alpha;
      bool requires_normalization = false;
      for (auto const& letter : p.alphabet()) {
        if (static_cast<size_type>(letter) != p.index(letter)) {
          requires_normalization = true;
          break;
        }
      }
      if (!requires_normalization) {
        return;
      }

      for (auto const& letter : p.alphabet()) {
        norm_alpha.push_back(p.index(letter));
      }

      std::sort(norm_alpha.begin(), norm_alpha.end());
      for (auto& rule : p.rules) {
        std::for_each(
            rule.begin(), rule.end(), [&p](letter_type& x) { x = p.index(x); });
      }
      p.alphabet(norm_alpha);
#ifdef LIBSEMIGROUPS_DEBUG
      p.validate();
#endif
    }
  }  // namespace presentation

}  // namespace libsemigroups

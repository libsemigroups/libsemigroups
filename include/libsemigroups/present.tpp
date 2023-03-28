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

  namespace detail {
    template <typename W>
    void validate_rules_length(Presentation<W> const& p) {
      if ((p.rules.size() % 2) == 1) {
        LIBSEMIGROUPS_EXCEPTION("expected even length, found %llu",
                                uint64_t(p.rules.size()));
      }
    }

    template <typename T>
    void validate_iterator_distance(T first, T last) {
      if ((std::distance(first, last) % 2) == 1) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected iterators at even distance, found %llu",
            uint64_t(std::distance(first, last)));
      }
    }

    // Lex compare of
    // [first11, last11) + [first12, last12)
    // and
    // [first21, last21) + [first22, last22)
    template <typename W>
    bool shortlex_compare_concat(W const& prefix1,
                                 W const& suffix1,
                                 W const& prefix2,
                                 W const& suffix2) {
      if (prefix1.size() + suffix1.size() < prefix2.size() + suffix2.size()) {
        return true;
      } else if (prefix1.size() + suffix1.size()
                 > prefix2.size() + suffix2.size()) {
        return false;
      }

      if (prefix1.size() < prefix2.size()) {
        size_t const k = prefix2.size() - prefix1.size();
        return std::lexicographical_compare(prefix1.cbegin(),
                                            prefix1.cend(),
                                            prefix2.cbegin(),
                                            prefix2.cbegin() + prefix1.size())
               || std::lexicographical_compare(suffix1.cbegin(),
                                               suffix1.cbegin() + k,
                                               prefix2.cbegin()
                                                   + prefix1.size(),
                                               prefix2.cend())
               || std::lexicographical_compare(suffix1.cbegin() + k,
                                               suffix1.cend(),
                                               suffix2.cbegin(),
                                               suffix2.cend());
      } else {
        size_t const k = prefix1.size() - prefix2.size();
        return std::lexicographical_compare(prefix1.cbegin(),
                                            prefix1.cbegin() + prefix2.size(),
                                            prefix2.cbegin(),
                                            prefix2.cend())
               || std::lexicographical_compare(prefix1.cbegin()
                                                   + prefix2.size(),
                                               prefix1.cend(),
                                               suffix2.cbegin(),
                                               suffix2.cbegin() + k)
               || std::lexicographical_compare(suffix1.cbegin(),
                                               suffix1.cend(),
                                               suffix2.cbegin() + k,
                                               suffix2.cend());
      }
    }
  }  // namespace detail

  template <typename W>
  Presentation<W>::Presentation()
      : _alphabet(), _alphabet_map(), _contains_empty_word(false), rules() {}

  template <typename W>
  void Presentation<W>::clear() {
    _alphabet.clear();
    _alphabet_map.clear();
    _contains_empty_word = false;
    rules.clear();
  }

  template <typename W>
  Presentation<W>& Presentation<W>::alphabet(size_type n) {
    if (n > std::numeric_limits<letter_type>::max()
                - std::numeric_limits<letter_type>::min()) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected a value in the range [0, %llu) found %llu",
          uint64_t(std::numeric_limits<letter_type>::max()
                   - std::numeric_limits<letter_type>::min()),
          uint64_t(n));
    }
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
        LIBSEMIGROUPS_EXCEPTION("invalid letter %c, valid letters are %s",
                                c,
                                detail::to_string(_alphabet).c_str());
      } else {
        LIBSEMIGROUPS_EXCEPTION("invalid letter %llu, valid letters are %s",
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
    detail::validate_rules_length(*this);
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
      _alphabet_map = std::move(alphabet_map);
    } catch (LibsemigroupsException& e) {
      _alphabet = std::move(old_alphabet);
      throw;
    }
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
    void add_zero_rules(Presentation<W>&                      p,
                        typename Presentation<W>::letter_type z) {
      p.validate_letter(z);
      for (auto it = p.alphabet().cbegin(); it != p.alphabet().cend(); ++it) {
        W       lhs = {*it, z};
        W const rhs = {z};
        add_rule(p, lhs, rhs);
        if (*it != z) {
          lhs = {z, *it};
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
      detail::validate_rules_length(p);

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
    void remove_trivial_rules(Presentation<W>& p) {
      detail::validate_rules_length(p);

      for (size_t i = 0; i < p.rules.size();) {
        if (p.rules[i] == p.rules[i + 1]) {
          p.rules.erase(p.rules.cbegin() + i, p.rules.cbegin() + i + 2);
        } else {
          i += 2;
        }
      }
    }

    template <typename W>
    void reduce_complements(Presentation<W>& p) {
      // the first loop below depends on p.rules being of even length
      detail::validate_rules_length(p);

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
      detail::validate_rules_length(p);
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
      detail::validate_rules_length(p);
      // Create a permutation of the even indexed entries in vec

      std::vector<size_t> perm;
      size_t const        n = p.rules.size() / 2;
      perm.resize(n);
      std::iota(perm.begin(), perm.end(), 0);
      std::sort(perm.begin(), perm.end(), [&p](auto x, auto y) -> bool {
        return detail::shortlex_compare_concat(p.rules[2 * x],
                                               p.rules[2 * x + 1],
                                               p.rules[2 * y],
                                               p.rules[2 * y + 1]);
      });
      // Apply the permutation (adapted from stl.hpp:apply_permutation)
      for (size_t i = 0; i < n; ++i) {
        size_t current = i;
        while (i != perm[current]) {
          size_t next = perm[current];
          std::swap(p.rules[2 * current], p.rules[2 * next]);
          std::swap(p.rules[2 * current + 1], p.rules[2 * next + 1]);
          perm[current] = current;
          current       = next;
        }
        perm[current] = current;
      }
    }

    template <typename W>
    bool are_rules_sorted(Presentation<W> const& p) {
      detail::validate_rules_length(p);
      IntegralRange<size_t> perm(0, p.rules.size() / 2);
      return std::is_sorted(
          perm.cbegin(), perm.cend(), [&p](auto x, auto y) -> bool {
            return detail::shortlex_compare_concat(p.rules[2 * x],
                                                   p.rules[2 * x + 1],
                                                   p.rules[2 * y],
                                                   p.rules[2 * y + 1]);
          });
    }

    template <typename W>
    W longest_common_subword(Presentation<W>& p) {
      Ukkonen u;
      ukkonen::add_words(u, p.rules.cbegin(), p.rules.cend());
      ukkonen::detail::GreedyReduceHelper helper(u);
      // Get the best word [first, last) so that replacing every
      // non-overlapping occurrence of [first, last) in p.rules with a new
      // generator "x", and adding "x = [first, last)" as a relation reduces
      // the length of the presentation as much as possible.
      word_type::const_iterator first, last;
      std::tie(first, last) = ukkonen::dfs(u, helper);
      // It'd be more pleasing to return first and last here, but they point at
      // the word contained in the Ukkonen u, which is destroyed after we
      // exit this function.
      return W(first, last);
    }

    // TODO(v3) this should replace a subword with another subword, and not do
    // what it currently does
    template <typename W, typename T, typename>
    void replace_subword(Presentation<W>& p, T first, T last) {
      auto x = first_unused_letter(p);
      replace_subword(p, first, last, &x, &x + 1);
      p.add_rule(&x, &x + 1, first, last);
      auto new_alphabet = p.alphabet();
      new_alphabet.push_back(x);
      p.alphabet(new_alphabet);
    }

    template <typename W>
    void replace_subword(Presentation<W>& p,
                         W const&         existing,
                         W const&         replacement) {
      replace_subword(p,
                      existing.cbegin(),
                      existing.cend(),
                      replacement.cbegin(),
                      replacement.cend());
    }

    template <typename W, typename S, typename T>
    void replace_subword(Presentation<W>& p,
                         S                first_existing,
                         S                last_existing,
                         T                first_replacement,
                         T                last_replacement) {
      if (first_existing == last_existing) {
        LIBSEMIGROUPS_EXCEPTION("the 2nd and 3rd argument must not be equal");
      }
      auto rplc_sbwrd = [&first_existing,
                         &last_existing,
                         &first_replacement,
                         &last_replacement](W& word) {
        size_t const M  = std::distance(first_existing, last_existing);
        size_t const N  = std::distance(first_replacement, last_replacement);
        auto         it = std::search(
            word.begin(), word.end(), first_existing, last_existing);
        while (it != word.end()) {
          // found existing
          auto replacement_first = it - word.begin();
          word.erase(it, it + M);
          word.insert(word.begin() + replacement_first,
                      first_replacement,
                      last_replacement);
          it = std::search(word.begin() + replacement_first + N,
                           word.end(),
                           first_existing,
                           last_existing);
        }
      };
      std::for_each(p.rules.begin(), p.rules.end(), rplc_sbwrd);
    }

    template <typename W>
    void replace_word(Presentation<W>& p,
                      W const&         existing,
                      W const&         replacement) {
      auto rplc_wrd = [&existing, &replacement](W& word) {
        if (word == existing) {
          word = replacement;
        }
      };
      std::for_each(p.rules.begin(), p.rules.end(), rplc_wrd);
    }

    template <typename W>
    void normalize_alphabet(Presentation<W>& p) {
      using letter_type = typename Presentation<W>::letter_type;

      p.validate();

      for (auto& rule : p.rules) {
        std::for_each(rule.begin(), rule.end(), [&p](letter_type& x) {
          x = letter(p, p.index(x));
        });
      }
      W A(p.alphabet().size(), 0);

      for (size_t i = 0; i < p.alphabet().size(); ++i) {
        A[i] = letter(p, i);
      }
      p.alphabet(std::move(A));
#ifdef LIBSEMIGROUPS_DEBUG
      p.validate();
#endif
    }

    template <typename W>
    void change_alphabet(Presentation<W>& p, W const& new_alphabet) {
      using letter_type = typename Presentation<W>::letter_type;

      p.validate();

      if (new_alphabet.size() != p.alphabet().size()) {
        LIBSEMIGROUPS_EXCEPTION("expected an alphabet of size %llu, found %llu",
                                uint64_t(p.alphabet().size()),
                                uint64_t(new_alphabet.size()));
      } else if (p.alphabet() == new_alphabet) {
        return;
      }

      std::map<letter_type, letter_type> old_to_new;
      for (size_t i = 0; i < p.alphabet().size(); ++i) {
        old_to_new.emplace(p.letter(i), new_alphabet[i]);
      }
      // Do this first so that it throws if new_alphabet contains repeats
      p.alphabet(new_alphabet);
      for (auto& rule : p.rules) {
        std::for_each(rule.begin(), rule.end(), [&old_to_new](letter_type& x) {
          x = old_to_new.find(x)->second;
        });
      }
#ifdef LIBSEMIGROUPS_DEBUG
      p.validate();
#endif
    }

    template <typename T>
    T longest_rule(T first, T last) {
      detail::validate_iterator_distance(first, last);

      auto   result = last;
      size_t max    = 0;
      for (auto it = first; it != last; it += 2) {
        size_t val = it->size() + (it + 1)->size();
        if (val > max) {
          max    = val;
          result = it;
        }
      }
      return result;
    }

    template <typename T>
    auto longest_rule_length(T first, T last) {
      auto it = longest_rule(first, last);
      return it->size() + (it + 1)->size();
    }

    template <typename T>
    T shortest_rule(T first, T last) {
      detail::validate_iterator_distance(first, last);

      auto   result = last;
      size_t min    = POSITIVE_INFINITY;
      for (auto it = first; it != last; it += 2) {
        size_t val = it->size() + (it + 1)->size();
        if (val < min) {
          min    = val;
          result = it;
        }
      }
      return result;
    }

    template <typename T>
    auto shortest_rule_length(T first, T last) {
      auto it = shortest_rule(first, last);
      return it->size() + (it + 1)->size();
    }

    template <typename W>
    void remove_redundant_generators(Presentation<W>& p) {
      using letter_type_ = typename Presentation<W>::letter_type;
      detail::validate_rules_length(p);

      remove_trivial_rules(p);
      for (size_t i = 0; i != p.rules.size(); i += 2) {
        auto lhs = p.rules[i];
        auto rhs = p.rules[i + 1];
        if (lhs.size() == 1
            && std::none_of(
                rhs.cbegin(), rhs.cend(), [&lhs](letter_type_ const& a) {
                  return a == lhs[0];
                })) {
          if (rhs.size() == 1 && lhs[0] < rhs[0]) {
            std::swap(lhs, rhs);
          }
          replace_subword(p, lhs, rhs);
        } else if (rhs.size() == 1
                   && std::none_of(
                       lhs.cbegin(), lhs.cend(), [&rhs](letter_type_ const& a) {
                         return a == rhs[0];
                       })) {
          replace_subword(p, rhs, lhs);
        }
      }
      remove_trivial_rules(p);
      p.alphabet_from_rules();
    }

    template <typename W>
    typename Presentation<W>::letter_type letter(Presentation<W> const&,
                                                 size_t i) {
      using letter_type = typename Presentation<W>::letter_type;
      if (i >= std::numeric_limits<letter_type>::max()) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected a value in the range [0, %llu) found %llu",
            uint64_t(std::numeric_limits<letter_type>::max()),
            uint64_t(i));
      }
      return static_cast<typename Presentation<W>::letter_type>(i);
    }

    template <>
    typename Presentation<std::string>::letter_type
    letter(Presentation<std::string> const&, size_t i) {
      return character(i);
    }

    typename Presentation<std::string>::letter_type character(size_t i) {
      using letter_type = typename Presentation<std::string>::letter_type;
      // Choose visible characters a-zA-Z0-9 first before anything else
      // The ascii ranges for these characters are: [97, 123), [65, 91),
      // [48, 58) so the remaining range of chars that are appended to the end
      // after these chars are [0,48), [58, 65), [91, 97), [123, 255)
      if (i >= std::numeric_limits<letter_type>::max()
                   - std::numeric_limits<letter_type>::min()) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected a value in the range [0, %llu) found %llu",
            uint64_t(std::numeric_limits<letter_type>::max()
                     - std::numeric_limits<letter_type>::min()),
            uint64_t(i));
      }
      static std::string letters
          = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
      static bool first_call = true;
      if (first_call) {
        letters.resize(255);
        std::iota(letters.begin() + 62,
                  letters.begin() + 110,
                  static_cast<letter_type>(0));
        std::iota(letters.begin() + 110,
                  letters.begin() + 117,
                  static_cast<letter_type>(58));
        std::iota(letters.begin() + 117,
                  letters.begin() + 123,
                  static_cast<letter_type>(91));
        std::iota(letters.begin() + 123,
                  letters.end(),
                  static_cast<letter_type>(123));
        first_call = false;
        LIBSEMIGROUPS_ASSERT(letters.size()
                             == std::numeric_limits<letter_type>::max()
                                    - std::numeric_limits<letter_type>::min());
        LIBSEMIGROUPS_ASSERT(letters.end() == letters.begin() + 255);
      }

      return letters[i];
    }

    template <typename W>
    typename Presentation<W>::letter_type
    first_unused_letter(Presentation<W> const& p) {
      using letter_type = typename Presentation<W>::letter_type;
      using size_type   = typename W::size_type;

      auto const max_letter
          = static_cast<size_type>(std::numeric_limits<letter_type>::max()
                                   - std::numeric_limits<letter_type>::min());

      if (p.alphabet().size() == max_letter) {
        LIBSEMIGROUPS_EXCEPTION(
            "the alphabet of the 1st argument already has the maximum size of "
            "%llu, there are no unused generators",
            uint64_t(std::numeric_limits<letter_type>::max()
                     - std::numeric_limits<letter_type>::min()));
      }

      letter_type x;
      for (size_type i = 0; i < max_letter; ++i) {
        x = letter(p, i);
        if (!p.in_alphabet(x)) {
          break;
        }
      }
      return x;
    }

    template <typename W>
    typename Presentation<W>::letter_type make_semigroup(Presentation<W>& p) {
      if (!p.contains_empty_word()) {
        return UNDEFINED;
      }
      auto e         = first_unused_letter(p);
      W    new_alpha = p.alphabet();
      new_alpha.insert(new_alpha.end(), e);
      p.alphabet(new_alpha);
      presentation::replace_word(p, {}, {e});
      presentation::add_identity_rules(p, e);
      p.contains_empty_word(false);
      return e;
    }

    template <typename W>
    void greedy_reduce_length(Presentation<W>& p) {
      auto w = longest_common_subword(p);
      while (!w.empty()) {
        replace_subword(p, w);
        w = longest_common_subword(p);
      }
    }

    template <typename W>
    bool is_strongly_compressible(Presentation<W> const& p) {
      if (p.rules.size() != 2) {
        return false;
      }
      auto const& u = p.rules[0];
      auto const& v = p.rules[1];
      return !(u.empty() || v.empty() || u.front() != v.front()
               || u.back() != v.back());
    }

    template <typename W>
    bool strongly_compress(Presentation<W>& p) {
      if (!is_strongly_compressible(p)) {
        return false;
      }

      auto const& u = p.rules[0];
      auto const& v = p.rules[1];

      size_t k = std::min(static_cast<size_t>(
                              detail::maximum_common_prefix(u, v).first
                              - u.cbegin()),
                          detail::maximum_common_suffix(u, v).size())
                 // TODO(later): ensure maximum_common_prefix/suffix have
                 // same return type
                 + 1;

      size_t const n = p.alphabet().size();

      auto word_to_num = [&k, &n](auto first, auto last) {
        LIBSEMIGROUPS_ASSERT(static_cast<size_t>(std::distance(first, last))
                             == k);
        (void) k;
        size_t result = 0;
        for (auto it = first; it != last; ++it) {
          result += std::pow(n, it - first) * (*it);
        }
        return result;
      };

      auto compress_word = [&k, &word_to_num](W const& word) {
        W result;
        for (auto it = word.cbegin(); it <= word.cend() - k; ++it) {
          result.push_back(word_to_num(it, it + k));
        }
        return result;
      };

      p.rules[0] = compress_word(p.rules[0]);
      p.rules[1] = compress_word(p.rules[1]);
      p.alphabet_from_rules();
      normalize_alphabet(p);
      return true;
    }

    template <typename W>
    bool reduce_to_2_generators(Presentation<W>& p, size_t index) {
      if (index > 1) {
        LIBSEMIGROUPS_EXCEPTION("the 2nd argument must be 0 or 1, found %llu",
                                uint64_t(index));
      } else if (p.rules.size() != 2) {
        return false;
      }
      auto const& u = p.rules[0];
      auto const& v = p.rules[1];
      if (u.empty() || v.empty() || u.front() == v.front()) {
        return false;
      }

      std::vector<typename Presentation<W>::letter_type> non_trivial_scc
          = {u.front(), v.front()};

      auto const other = non_trivial_scc[(index + 1) % 2];

      for (auto const& x : p.alphabet()) {
        if (x != other) {
          replace_subword(p, {x}, {letter(p, non_trivial_scc[index])});
        }
      }
      p.alphabet_from_rules();
      normalize_alphabet(p);
      return true;
    }

  }  // namespace presentation
}  // namespace libsemigroups

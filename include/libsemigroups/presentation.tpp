//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2023 James D. Mitchell
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

#include "libsemigroups/exception.hpp"
namespace libsemigroups {
  namespace detail {
    template <typename Word>
    void validate_rules_length(Presentation<Word> const& p) {
      if ((p.rules.size() % 2) == 1) {
        LIBSEMIGROUPS_EXCEPTION("expected even length, found {}",
                                p.rules.size());
      }
    }

    template <typename Iterator>
    void validate_iterator_distance(Iterator first, Iterator last) {
      if ((std::distance(first, last) % 2) == 1) {
        LIBSEMIGROUPS_EXCEPTION("expected iterators at even distance, found {}",
                                std::distance(first, last));
      }
    }

    std::string to_printable(char c);
    bool        isprint(std::string const& alphabet);
    std::string to_printable(std::string const& alphabet);

    template <typename Thing>
    std::string to_printable(Thing thing) {
      return fmt::format("{}", thing);
    }
  }  // namespace detail

  template <typename Word>
  Presentation<Word>::Presentation()
      : _alphabet(), _alphabet_map(), _contains_empty_word(false), rules() {}

  template <typename Word>
  Presentation<Word>& Presentation<Word>::init() {
    _alphabet.clear();
    _alphabet_map.clear();
    _contains_empty_word = false;
    rules.clear();
    return *this;
  }

  template <typename Word>
  Presentation<Word>::Presentation(Presentation const&) = default;

  template <typename Word>
  Presentation<Word>::Presentation(Presentation&&) = default;

  template <typename Word>
  Presentation<Word>& Presentation<Word>::operator=(Presentation<Word> const&)
      = default;

  template <typename Word>
  Presentation<Word>& Presentation<Word>::operator=(Presentation<Word>&&)
      = default;

  template <typename Word>
  Presentation<Word>::~Presentation() = default;

  template <typename Word>
  Presentation<Word>& Presentation<Word>::alphabet(size_type n) {
    if (n > std::numeric_limits<letter_type>::max()
                - std::numeric_limits<letter_type>::min()) {
      LIBSEMIGROUPS_EXCEPTION("expected a value in the range [0, {}) found {}",
                              std::numeric_limits<letter_type>::max()
                                  - std::numeric_limits<letter_type>::min() + 1,
                              n);
    }
    word_type lphbt(n, 0);
    std::iota(lphbt.begin(), lphbt.end(), 0);
    return alphabet(lphbt);
  }

  template <typename Word>
  Presentation<Word>& Presentation<Word>::alphabet(word_type const& lphbt) {
    // We copy the _alphabet_map for exception safety
    decltype(_alphabet_map) alphabet_map;
    auto                    old_alphabet = std::move(_alphabet);
    _alphabet                            = lphbt;
    try_set_alphabet(alphabet_map, old_alphabet);
    return *this;
  }

  template <typename Word>
  Presentation<Word>& Presentation<Word>::alphabet(word_type&& lphbt) {
    // We copy the _alphabet_map for exception safety
    decltype(_alphabet_map) alphabet_map;
    auto                    old_alphabet = std::move(_alphabet);
    _alphabet                            = std::move(lphbt);
    try_set_alphabet(alphabet_map, old_alphabet);
    return *this;
  }

  template <typename Word>
  Presentation<Word>& Presentation<Word>::alphabet_from_rules() {
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

  template <typename Word>
  typename Presentation<Word>::letter_type
  Presentation<Word>::letter(size_type i) const {
    if (i >= _alphabet.size()) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected a value in [0, {}), found {}", _alphabet.size(), i);
    }
    return letter_no_checks(i);
  }

  template <typename Word>
  typename Presentation<Word>::size_type
  Presentation<Word>::index(letter_type val) const {
    validate_letter(val);
    return _alphabet_map.find(val)->second;
  }

  // TODO use to_printable
  template <typename Word>
  void Presentation<Word>::validate_letter(
      typename Presentation<Word>::letter_type c) const {
    if (_alphabet.empty()) {
      LIBSEMIGROUPS_EXCEPTION("no alphabet has been defined");
    } else if (_alphabet_map.find(c) == _alphabet_map.cend()) {
      auto msg = fmt::format("invalid letter {}, valid letters are {}",
                             detail::to_printable(c),
                             detail::to_printable(_alphabet));
      if constexpr (std::is_same_v<typename Presentation<Word>::letter_type,
                                   char>) {
        if (!std::isprint(c) && detail::isprint(_alphabet)) {
          msg += fmt::format(
              " == {}", std::vector<int>(_alphabet.begin(), _alphabet.end()));
        }
      }
      LIBSEMIGROUPS_EXCEPTION(msg);
    }
  }

  template <typename Word>
  template <typename Iterator>
  void Presentation<Word>::validate_word(Iterator first, Iterator last) const {
    if (!_contains_empty_word && std::distance(first, last) == 0) {
      LIBSEMIGROUPS_EXCEPTION("words in rules cannot be empty, did you mean to "
                              "call contains_empty_word(true) first?");
    }
    for (auto it = first; it != last; ++it) {
      validate_letter(*it);
    }
  }

  template <typename Word>
  void Presentation<Word>::validate_rules() const {
    detail::validate_rules_length(*this);
    presentation::validate_rules(*this, rules.cbegin(), rules.cend());
  }

  template <typename Word>
  void Presentation<Word>::validate_alphabet(
      decltype(_alphabet_map)& alphabet_map) const {
    size_type index = 0;
    for (auto const& letter : _alphabet) {
      auto it = alphabet_map.emplace(letter, index++);
      // FIXME this doesn't compile when using a size_type to initialise the
      // alphabet (rather than a word), see 045
      // Is this fixed now? JE
      if (!it.second) {
        LIBSEMIGROUPS_EXCEPTION("invalid alphabet {}, duplicate letter {}!",
                                detail::to_printable(_alphabet),
                                detail::to_printable(letter));
      }
    }
  }

  template <typename Word>
  void
  Presentation<Word>::try_set_alphabet(decltype(_alphabet_map)& alphabet_map,
                                       word_type&               old_alphabet) {
    try {
      validate_alphabet(alphabet_map);
      _alphabet_map = std::move(alphabet_map);
    } catch (LibsemigroupsException& e) {
      _alphabet = std::move(old_alphabet);
      throw;
    }
  }

  namespace presentation {

    template <typename Word>
    void validate_semigroup_inverses(Presentation<Word> const& p,
                                     Word const&               vals) {
      if (vals.size() != p.alphabet().size()) {
        LIBSEMIGROUPS_EXCEPTION(
            "invalid number of inverses, expected {} but found {}",
            p.alphabet().size(),
            vals.size());
      }
      // Validate word after checking the size so that we get a more
      // meaningful exception message
      p.validate_word(vals.begin(), vals.end());

      // TODO does this actually check for duplicates?
      Word cpy = vals;
      std::sort(cpy.begin(), cpy.end());
      for (auto it = cpy.cbegin(); it < cpy.cend() - 1; ++it) {
        if (*it == *(it + 1)) {
          LIBSEMIGROUPS_EXCEPTION(
              "invalid inverses, the letter {} is duplicated!",
              detail::to_printable(*it));
        }
      }

      // Check that (x ^ - 1) ^ -1 = x
      for (size_t i = 0; i < p.alphabet().size(); ++i) {
        for (size_t j = 0; j < p.alphabet().size(); ++j) {
          if (p.letter_no_checks(j) == vals[i]) {
            if (vals[j] != p.letter_no_checks(i)) {
              LIBSEMIGROUPS_EXCEPTION(
                  "invalid inverses, {} ^ -1 = {} but {} ^ -1 = {}",
                  detail::to_printable(p.letter_no_checks(i)),
                  detail::to_printable(vals[i]),
                  detail::to_printable(vals[i]),
                  detail::to_printable(vals[j]));
            }
            break;
          }
        }
      }
    }

    template <typename Word>
    [[nodiscard]] bool contains_rule(Presentation<Word>& p,
                                     Word const&         lhs,
                                     Word const&         rhs) {
      for (auto it = p.rules.cbegin(); it != p.rules.cend(); it += 2) {
        if ((*it == lhs && *(it + 1) == rhs)
            || (*it == rhs && *(it + 1) == lhs)) {
          return true;
        }
      }
      return false;
    }

    template <typename Word>
    void add_identity_rules(Presentation<Word>&                      p,
                            typename Presentation<Word>::letter_type id) {
      p.validate_letter(id);
      for (auto it = p.alphabet().cbegin(); it != p.alphabet().cend(); ++it) {
        Word       lhs = {*it, id};
        Word const rhs = {*it};
        add_rule(p, lhs, rhs);
        if (*it != id) {
          lhs = {id, *it};
          add_rule(p, lhs, rhs);
        }
      }
    }

    template <typename Word>
    void add_zero_rules(Presentation<Word>&                      p,
                        typename Presentation<Word>::letter_type z) {
      p.validate_letter(z);
      for (auto it = p.alphabet().cbegin(); it != p.alphabet().cend(); ++it) {
        Word       lhs = {*it, z};
        Word const rhs = {z};
        add_rule(p, lhs, rhs);
        if (*it != z) {
          lhs = {z, *it};
          add_rule(p, lhs, rhs);
        }
      }
    }

    template <typename Word>
    void add_inverse_rules(Presentation<Word>&                      p,
                           Word const&                              vals,
                           typename Presentation<Word>::letter_type id) {
      validate_semigroup_inverses(p, vals);
      for (size_t i = 0; i < p.alphabet().size(); ++i) {
        if (p.letter_no_checks(i) == id && vals[i] != id) {
          LIBSEMIGROUPS_EXCEPTION(
              "invalid inverses, the identity is {}, but {} ^ -1 != {}",
              detail::to_printable(p.letter_no_checks(i)),
              detail::to_printable(p.letter_no_checks(i)),
              detail::to_printable(vals[i]));
        }
      }
      Word rhs = (id == UNDEFINED ? Word({}) : Word({id}));

      for (size_t i = 0; i < p.alphabet().size(); ++i) {
        Word lhs = {p.letter_no_checks(i), vals[i]};
        if (p.letter_no_checks(i) != id) {
          add_rule_no_checks(p, lhs, rhs);
        }
      }
    }

    template <typename Word>
    void remove_duplicate_rules(Presentation<Word>& p) {
      detail::validate_rules_length(p);

      std::unordered_set<std::pair<Word, Word>, Hash<std::pair<Word, Word>>>
          relations_set;

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

    template <typename Word>
    void remove_trivial_rules(Presentation<Word>& p) {
      detail::validate_rules_length(p);

      for (size_t i = 0; i < p.rules.size();) {
        if (p.rules[i] == p.rules[i + 1]) {
          p.rules.erase(p.rules.cbegin() + i, p.rules.cbegin() + i + 2);
        } else {
          i += 2;
        }
      }
    }

    // TODO this appears to be non-deterministic (different results with
    // g++-11 than with clang++). Not sure that this is a problem, but this
    // should be recorded in the doc.
    template <typename Word>
    void reduce_complements(Presentation<Word>& p) {
      // the first loop below depends on p.rules being of even length
      detail::validate_rules_length(p);

      libsemigroups::detail::Duf<> duf;
      duf.resize(p.rules.size());
      std::unordered_map<Word, size_t, Hash<Word>, EqualTo<Word>> map;

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
      std::unordered_map<size_t, Word> mins;

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

    template <typename Word, typename Compare>
    bool sort_each_rule(Presentation<Word>& p, Compare cmp) {
      bool result = false;
      detail::validate_rules_length(p);
      // Sort each relation so that the lhs is greater than the rhs
      // according to func.
      for (auto it = p.rules.begin(); it < p.rules.end(); it += 2) {
        if (cmp(*it, *(it + 1))) {
          std::swap(*it, *(it + 1));
          result = true;
        }
      }
      return result;
    }

    template <typename Word>
    bool sort_each_rule(Presentation<Word>& p) {
      return sort_each_rule(p, ShortLexCompare());
    }

    template <typename Word, typename Compare>
    void sort_rules(Presentation<Word>& p, Compare cmp) {
      using rx::chain;
      using rx::seq;
      using rx::sort;
      using rx::take;
      using rx::to_vector;
      detail::validate_rules_length(p);

      auto&        rules = p.rules;
      size_t const n     = rules.size() / 2;

      // Create a permutation of the even indexed entries in vec
      // TODO be nice to not have to create perm here but I couldn't quite
      // figure out how to remove it.
      auto perm
          = (seq<size_t>() | take(n) | sort([&rules, &cmp](auto i, auto j) {
               return cmp(chain(rules[2 * i], rules[2 * i + 1]),
                          chain(rules[2 * j], rules[2 * j + 1]));
             })
             | to_vector());
      // Apply the permutation (adapted from
      // detail/stl.hpp:apply_permutation)
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

    template <typename Word, typename Compare>
    bool are_rules_sorted(Presentation<Word> const& p, Compare cmp) {
      using namespace rx;  // NOLINT(build/namespaces)
      detail::validate_rules_length(p);
      auto const&  rules = p.rules;
      size_t const n     = rules.size() / 2;
      return is_sorted((seq<size_t>() | take(n)),
                       [&rules, &cmp](auto i, auto j) {
                         return cmp(chain(rules[2 * i], rules[2 * i + 1]),
                                    chain(rules[2 * j], rules[2 * j + 1]));
                       });
    }

    template <typename Word>
    Word longest_subword_reducing_length(Presentation<Word>& p) {
      Ukkonen u;
      ukkonen::add_words(u, p.rules.cbegin(), p.rules.cend());
      ukkonen::detail::GreedyReduceHelper helper(u);
      // Get the best word [first, last) so that replacing every
      // non-overlapping occurrence of [first, last) in p.rules with a new
      // generator "x", and adding "x = [first, last)" as a relation reduces
      // the length of the presentation as much as possible.
      word_type::const_iterator first, last;
      std::tie(first, last) = ukkonen::dfs(u, helper);
      // It'd be more pleasing to return first and last here, but they point
      // at the word contained in the Ukkonen u, which is destroyed after we
      // exit this function.
      return Word(first, last);
    }

    template <typename Word>
    typename Presentation<Word>::letter_type
    add_generator(Presentation<Word>& p) {
      auto result = first_unused_letter(p);
      add_generator_no_checks(p, result);
      return result;
    }

    template <typename Word>
    void add_generator_no_checks(Presentation<Word>&                      p,
                                 typename Presentation<Word>::letter_type x) {
      auto new_alphabet = p.alphabet();
      new_alphabet.push_back(x);
      p.alphabet(new_alphabet);
    }

    template <typename Word>
    void add_generator(Presentation<Word>&                      p,
                       typename Presentation<Word>::letter_type x) {
      try {
        p.validate_letter(x);  // throws if x does not belong to p.alphabet()
      } catch (LibsemigroupsException const& e) {
        add_generator_no_checks(p, x);
        return;
      }
      LIBSEMIGROUPS_EXCEPTION(
          "the 2nd argument {} already belongs to the alphabet {}, "
          "expected an unused letter",
          detail::to_printable(x),
          detail::to_printable(p.alphabet()));
    }

    template <typename Word, typename Iterator, typename>
    typename Presentation<Word>::letter_type
    replace_word_with_new_generator(Presentation<Word>& p,
                                    Iterator            first,
                                    Iterator            last) {
      auto x = add_generator(p);
      replace_subword(p, first, last, &x, &x + 1);
      p.add_rule_no_checks(&x, &x + 1, first, last);
      return x;
    }

    template <typename Word>
    void replace_subword(Presentation<Word>& p,
                         Word const&         existing,
                         Word const&         replacement) {
      replace_subword(p,
                      existing.cbegin(),
                      existing.cend(),
                      replacement.cbegin(),
                      replacement.cend());
    }

    template <typename Word, typename Iterator1, typename Iterator2>
    void replace_subword(Presentation<Word>& p,
                         Iterator1           first_existing,
                         Iterator1           last_existing,
                         Iterator2           first_replacement,
                         Iterator2           last_replacement) {
      if (first_existing == last_existing) {
        LIBSEMIGROUPS_EXCEPTION("the 2nd and 3rd argument must not be equal");
      }
      auto rplc_sbwrd = [&first_existing,
                         &last_existing,
                         &first_replacement,
                         &last_replacement](Word& word) {
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

    template <typename Word>
    void replace_word(Presentation<Word>& p,
                      Word const&         existing,
                      Word const&         replacement) {
      auto rplc_wrd = [&existing, &replacement](Word& word) {
        if (word == existing) {
          word = replacement;
        }
      };
      std::for_each(p.rules.begin(), p.rules.end(), rplc_wrd);
    }

    template <typename Word>
    void normalize_alphabet(Presentation<Word>& p) {
      using letter_type = typename Presentation<Word>::letter_type;

      p.validate();

      for (auto& rule : p.rules) {
        std::for_each(rule.begin(), rule.end(), [&p](letter_type& x) {
          x = human_readable_letter(p, p.index(x));
        });
      }
      Word A(p.alphabet().size(), 0);

      for (size_t i = 0; i < p.alphabet().size(); ++i) {
        A[i] = human_readable_letter(p, i);
      }
      p.alphabet(std::move(A));
#ifdef LIBSEMIGROUPS_DEBUG
      p.validate();
#endif
    }

    template <typename Word>
    void change_alphabet(Presentation<Word>& p, Word const& new_alphabet) {
      using letter_type = typename Presentation<Word>::letter_type;

      p.validate();

      if (new_alphabet.size() != p.alphabet().size()) {
        LIBSEMIGROUPS_EXCEPTION("expected an alphabet of size {}, found {}",
                                p.alphabet().size(),
                                new_alphabet.size());
      } else if (p.alphabet() == new_alphabet) {
        return;
      }

      std::map<letter_type, letter_type> old_to_new;
      for (size_t i = 0; i < p.alphabet().size(); ++i) {
        old_to_new.emplace(p.letter_no_checks(i), new_alphabet[i]);
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

    template <typename Iterator>
    Iterator longest_rule(Iterator first, Iterator last) {
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

    template <typename Iterator>
    auto longest_rule_length(Iterator first, Iterator last) {
      auto it = longest_rule(first, last);
      if (it != last) {
        return it->size() + (it + 1)->size();
      } else {
        return decltype(it->size())(0);  // TODO remove this!
      }
    }

    template <typename Iterator>
    Iterator shortest_rule(Iterator first, Iterator last) {
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

    template <typename Iterator>
    auto shortest_rule_length(Iterator first, Iterator last) {
      auto it = shortest_rule(first, last);
      if (it != last) {
        return it->size() + (it + 1)->size();
      } else {
        return decltype(it->size())(0);  // TODO remove this!
      }
    }

    template <typename Word>
    void remove_redundant_generators(Presentation<Word>& p) {
      using letter_type_ = typename Presentation<Word>::letter_type;
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

    template <typename Word>
    typename Presentation<Word>::letter_type
    human_readable_letter(Presentation<Word> const&, size_t i) {
      using letter_type = typename Presentation<Word>::letter_type;
      if (i >= std::numeric_limits<letter_type>::max()) {
        LIBSEMIGROUPS_EXCEPTION("expected the 2nd argument to be in "
                                "the range [0, {}), found {}",
                                std::numeric_limits<letter_type>::max(),
                                i);
      }
      return static_cast<typename Presentation<Word>::letter_type>(i);
    }

    template <typename Word>
    typename Presentation<Word>::letter_type
    first_unused_letter(Presentation<Word> const& p) {
      using letter_type = typename Presentation<Word>::letter_type;
      using size_type   = typename Word::size_type;

      auto const max_letter
          = static_cast<size_type>(std::numeric_limits<letter_type>::max()
                                   - std::numeric_limits<letter_type>::min());

      if (p.alphabet().size() == max_letter) {
        LIBSEMIGROUPS_EXCEPTION(
            "the alphabet of the 1st argument already has the maximum size "
            "of {}, there are no unused generators",
            std::numeric_limits<letter_type>::max()
                - std::numeric_limits<letter_type>::min());
      }

      letter_type x;
      for (size_type i = 0; i < max_letter; ++i) {
        x = human_readable_letter(p, i);
        if (!p.in_alphabet(x)) {
          break;
        }
      }
      return x;
    }

    template <typename Word>
    typename Presentation<Word>::letter_type
    make_semigroup(Presentation<Word>& p) {
      if (!p.contains_empty_word()) {
        return UNDEFINED;
      }
      auto e         = first_unused_letter(p);
      Word new_alpha = p.alphabet();
      new_alpha.insert(new_alpha.end(), e);
      p.alphabet(new_alpha);
      presentation::replace_word(p, {}, {e});
      presentation::add_identity_rules(p, e);
      p.contains_empty_word(false);
      return e;
    }

    template <typename Word>
    void greedy_reduce_length(Presentation<Word>& p) {
      auto w = longest_subword_reducing_length(p);
      while (!w.empty()) {
        replace_word_with_new_generator(p, w);
        w = longest_subword_reducing_length(p);
      }
    }

    // TODO declare and doc in hpp
    template <typename Word>
    void greedy_reduce_length_and_number_of_gens(Presentation<Word>& p) {
      auto w = longest_subword_reducing_length(p);
      while (!w.empty()) {
        auto copy = p;
        replace_word_with_new_generator(p, w);
        w = longest_subword_reducing_length(p);
        if (presentation::length(p) + p.alphabet().size()
            >= presentation::length(copy) + copy.alphabet().size()) {
          std::swap(copy, p);
          break;
        }
      }
    }

    template <typename Word>
    bool is_strongly_compressible(Presentation<Word> const& p) {
      if (p.rules.size() != 2) {
        return false;
      }
      auto const& u = p.rules[0];
      auto const& v = p.rules[1];
      return !(u.empty() || v.empty() || u.front() != v.front()
               || u.back() != v.back());
    }

    template <typename Word>
    bool strongly_compress(Presentation<Word>& p) {
      if (!is_strongly_compressible(p)) {
        return false;
      }

      auto const& u = p.rules[0];
      auto const& v = p.rules[1];

      size_t k = std::min(static_cast<size_t>(
                              detail::maximum_common_prefix(u, v).first
                              - u.cbegin()),
                          detail::maximum_common_suffix(u, v).size())
                 // TODO(v3): ensure maximum_common_prefix/suffix have
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

      auto compress_word = [&k, &word_to_num](Word const& word) {
        Word result;
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

    template <typename Word>
    bool reduce_to_2_generators(Presentation<Word>& p, size_t index) {
      if (index > 1) {
        LIBSEMIGROUPS_EXCEPTION("the 2nd argument must be 0 or 1, found {}",
                                index);
      } else if (p.rules.size() != 2) {
        return false;
      }
      auto const& u = p.rules[0];
      auto const& v = p.rules[1];
      if (u.empty() || v.empty() || u.front() == v.front()) {
        return false;
      }

      std::vector<typename Presentation<Word>::letter_type> non_trivial_scc
          = {u.front(), v.front()};

      auto const other = non_trivial_scc[(index + 1) % 2];

      for (auto const& x : p.alphabet()) {
        if (x != other) {
          replace_subword(
              p, {x}, {human_readable_letter(p, non_trivial_scc[index])});
        }
      }
      p.alphabet_from_rules();
      normalize_alphabet(p);
      return true;
    }

    template <typename Word>
    void add_commutes_rules_no_checks(Presentation<Word>& p,
                                      Word const&         letters1,
                                      Word const&         letters2) {
      using words::      operator+;
      size_t const       m = letters1.size();
      size_t const       n = letters2.size();
      Presentation<Word> q;
      for (size_t i = 0; i < m; ++i) {
        Word u = {letters1[i]};
        for (size_t j = 0; j < n; ++j) {
          Word v = {letters2[j]};
          if (u != v) {
            presentation::add_rule_no_checks(q, u + v, v + u);
          }
        }
      }
      q.alphabet_from_rules();
      presentation::remove_duplicate_rules(q);
      presentation::add_rules_no_checks(p, q);
    }

    template <typename Word>
    void add_commutes_rules_no_checks(Presentation<Word>&         p,
                                      Word const&                 letters,
                                      std::initializer_list<Word> words) {
      using words::operator+;

      size_t const       m = letters.size();
      size_t const       n = words.size();
      Presentation<Word> q;

      for (size_t i = 0; i < m; ++i) {
        Word u = {letters[i]};
        for (size_t j = 0; j < n; ++j) {
          Word const& v = *(words.begin() + j);
          if (u != v) {
            presentation::add_rule_no_checks(q, u + v, v + u);
          }
        }
      }
      q.alphabet_from_rules();
      presentation::remove_duplicate_rules(q);
      presentation::add_rules_no_checks(p, q);
    }

  }  // namespace presentation
}  // namespace libsemigroups

//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2025 James D. Mitchell
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
    // This checks that there are enough distinct Word::value_types to construct
    // an alphabet of size n. If the size of Word::value_type is the same as the
    // size of size_t, then one cannot specify the size of an alphabet large
    // enough for which there are not enough distinct letters to be contained
    // within it.
    if constexpr (sizeof(typename Word::value_type) < sizeof(size_t)) {
      if (n > 1 + std::numeric_limits<letter_type>::max()
                  - std::numeric_limits<letter_type>::min()) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected a value in the range [0, {}), found {}",
            std::numeric_limits<letter_type>::max()
                - std::numeric_limits<letter_type>::min() + 2,
            n);
      }
    }
    word_type lphbt(n, 0);

    // The below assertions exist to insure that we are not badly assigning
    // values. The subsequent pragmas exist to suppress the false-positive
    // warnings produced by g++ 13.2.0
    static_assert(
        std::is_same_v<std::decay_t<decltype(*lphbt.begin())>,
                       decltype(words::human_readable_letter<Word>(0))>);
#pragma GCC diagnostic push
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
    std::iota(
        lphbt.begin(), lphbt.end(), words::human_readable_letter<Word>(0));
#pragma GCC diagnostic pop
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
    throw_if_letter_not_in_alphabet(val);
    return _alphabet_map.find(val)->second;
  }

  template <typename Word>
  Presentation<Word>& Presentation<Word>::add_generator_no_checks(
      typename Presentation<Word>::letter_type x) {
    size_t index = _alphabet_map.size();
#ifdef LIBSEMIGROUPS_DEBUG
    auto inserted = _alphabet_map.emplace(x, index);
    LIBSEMIGROUPS_ASSERT(inserted.second);
#else
    _alphabet_map.emplace(x, index);
#endif
    _alphabet.push_back(x);
    return *this;
  }

  template <typename Word>
  Presentation<Word>& Presentation<Word>::add_generator(
      typename Presentation<Word>::letter_type x) {
    if (in_alphabet(x)) {
      LIBSEMIGROUPS_EXCEPTION("the argument {} already belongs to the alphabet "
                              "{}, expected an unused letter",
                              detail::to_printable(x),
                              detail::to_printable(alphabet()));
    }
    return add_generator_no_checks(x);
  }

  // TODO(now) should this be a helper function?
  template <typename Word>
  typename Presentation<Word>::letter_type Presentation<Word>::add_generator() {
    auto result = presentation::first_unused_letter(*this);
    add_generator_no_checks(result);
    return result;
  }

  template <typename Word>
  Presentation<Word>& Presentation<Word>::remove_generator_no_checks(
      typename Presentation<Word>::letter_type x) {
    size_t index = _alphabet_map[x];
    _alphabet_map.erase(x);
    typename Word::iterator start = _alphabet.begin() + index;
    for (auto it = start + 1; it != _alphabet.end(); ++it) {
      --_alphabet_map[*it];
    }
    _alphabet.erase(start, start + 1);
    return *this;
  }

  template <typename Word>
  Presentation<Word>& Presentation<Word>::remove_generator(
      typename Presentation<Word>::letter_type x) {
    throw_if_alphabet_has_duplicates();
    if (in_alphabet(x)) {
      remove_generator_no_checks(x);
    } else {
      LIBSEMIGROUPS_EXCEPTION("the argument {} does not belong to the alphabet "
                              "{}, expected an existing letter",
                              detail::to_printable(x),
                              detail::to_printable(alphabet()));
    }
    return *this;
  }

  template <typename Word>
  void Presentation<Word>::throw_if_letter_not_in_alphabet(
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
  template <typename Iterator1, typename Iterator2>
  void
  Presentation<Word>::throw_if_letter_not_in_alphabet(Iterator1 first,
                                                      Iterator2 last) const {
    if (!_contains_empty_word && first == last) {
      LIBSEMIGROUPS_EXCEPTION("words in rules cannot be empty, did you mean to "
                              "call contains_empty_word(true) first?");
    }
    for (auto it = first; it != last; ++it) {
      throw_if_letter_not_in_alphabet(*it);
    }
  }

  template <typename Word>
  void Presentation<Word>::throw_if_bad_rules() const {
    presentation::throw_if_odd_number_of_rules(*this);
    presentation::throw_if_bad_rules(*this, rules.cbegin(), rules.cend());
  }

  template <typename Word>
  void Presentation<Word>::throw_if_alphabet_has_duplicates(
      decltype(_alphabet_map)& alphabet_map) const {
    LIBSEMIGROUPS_ASSERT(alphabet_map.empty());
    size_type index = 0;
    for (auto const& letter : _alphabet) {
      auto it = alphabet_map.emplace(letter, index++);
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
      throw_if_alphabet_has_duplicates(alphabet_map);
      _alphabet_map = std::move(alphabet_map);
    } catch (LibsemigroupsException& e) {
      _alphabet = std::move(old_alphabet);
      throw;
    }
  }

  namespace presentation {

    template <typename Word>
    bool is_normalized(Presentation<Word> const& p) {
      using letter_type = typename Presentation<Word>::letter_type;
      auto first = std::begin(p.alphabet()), last = std::end(p.alphabet());
      if (!std::is_sorted(first, last)) {
        return false;
      }
      auto it = std::max_element(first, last);
      return it != last
             && *it == static_cast<letter_type>(p.alphabet().size() - 1);
    }

    template <typename Word>
    void throw_if_not_normalized(Presentation<Word> const& p,
                                 std::string_view          arg) {
      using letter_type = typename Presentation<Word>::letter_type;
      auto first = std::begin(p.alphabet()), last = std::end(p.alphabet());
      if (!std::is_sorted(first, last)) {
        LIBSEMIGROUPS_EXCEPTION("the {} argument (presentation) must have "
                                "sorted alphabet, found {}",
                                arg,
                                detail::to_printable(p.alphabet()));
      }

      auto it = std::max_element(first, last);

      if (it != last
          && *it != static_cast<letter_type>(p.alphabet().size() - 1)) {
        LIBSEMIGROUPS_EXCEPTION("the {} argument (presentation) has invalid "
                                "alphabet, expected [0, ..., {}] found {}",
                                arg,
                                p.alphabet().size() - 1,
                                detail::to_printable(p.alphabet()));
      }
    }

    template <typename Word1, typename Word2>
    void throw_if_bad_inverses(Presentation<Word1> const& p,
                               Word2 const&               vals) {
      if (vals.size() != p.alphabet().size()) {
        LIBSEMIGROUPS_EXCEPTION(
            "invalid number of inverses, expected {} but found {}",
            p.alphabet().size(),
            vals.size());
      }
      // throw_if_bad_alphabet_or_rules word after checking the size so that we
      // get a more meaningful exception message
      p.throw_if_letter_not_in_alphabet(vals.begin(), vals.end());

      Word1 cpy(vals);
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

    template <typename Word1, typename Word2>
    void throw_if_bad_inverses(Presentation<Word1> const& p,
                               Word2 const&               letters,
                               Word2 const&               inverses) {
      if (letters == p.alphabet()) {
        throw_if_bad_inverses(p, inverses);
      } else {
        // Must check that letters is valid because it obviously is when we
        // create q.
        p.throw_if_letter_not_in_alphabet(letters.begin(), letters.end());
        Presentation<Word1> q;
        q.alphabet(letters);
        throw_if_bad_inverses(q, inverses);
      }
    }

    template <typename Word>
    std::string to_report_string(Presentation<Word> const& p) {
      using detail::group_digits;
      return fmt::format("|A| = {}, |R| = {}, "
                         "|u| + |v| \u2208 [{}, {}], \u2211(|u| + |v|) = {}\n",
                         group_digits(p.alphabet().size()),
                         group_digits(p.rules.size() / 2),
                         group_digits(shortest_rule_length(p)),
                         group_digits(longest_rule_length(p)),
                         group_digits(length(p)));
    }

    template <typename Word>
    bool contains_rule(Presentation<Word>& p,
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
      p.throw_if_letter_not_in_alphabet(id);
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
      p.throw_if_letter_not_in_alphabet(z);
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
      throw_if_bad_inverses(p, vals);
      for (size_t i = 0; i < p.alphabet().size(); ++i) {
        if (p.letter_no_checks(i) == id && vals[i] != id) {
          LIBSEMIGROUPS_EXCEPTION(
              "invalid inverses, the identity is {}, but {} ^ -1 = {}",
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
      throw_if_odd_number_of_rules(p);

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
      throw_if_odd_number_of_rules(p);

      for (size_t i = 0; i < p.rules.size();) {
        if (p.rules[i] == p.rules[i + 1]) {
          p.rules.erase(p.rules.cbegin() + i, p.rules.cbegin() + i + 2);
        } else {
          i += 2;
        }
      }
    }

    // This appears to be non-deterministic (different results with
    // g++-11 than with clang++). This is reflected in the docs
    template <typename Word>
    void reduce_complements(Presentation<Word>& p) {
      // the first loop below depends on p.rules being of even length
      throw_if_odd_number_of_rules(p);

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
      throw_if_odd_number_of_rules(p);
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
      throw_if_odd_number_of_rules(p);

      auto&        rules = p.rules;
      size_t const n     = rules.size() / 2;

      // Create a permutation of the even indexed entries in vec
      // TODO(2) be nice to not have to create perm here but I couldn't quite
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
      throw_if_odd_number_of_rules(p);
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
      detail::GreedyReduceHelper helper(u);
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

    template <typename Word, typename Iterator>
    typename Presentation<Word>::letter_type
    replace_word_with_new_generator(Presentation<Word>& p,
                                    Iterator            first,
                                    Iterator            last) {
      auto x = p.add_generator();
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

    template <typename Iterator>
    size_t length(Iterator first, Iterator last) {
      auto op = [](size_t val, auto const& x) { return val + x.size(); };
      return std::accumulate(first, last, size_t(0), op);
    }

    template <typename Word>
    void normalize_alphabet(Presentation<Word>& p) {
      using letter_type = typename Presentation<Word>::letter_type;

      p.throw_if_bad_alphabet_or_rules();

      for (auto& rule : p.rules) {
        std::for_each(rule.begin(), rule.end(), [&p](letter_type& x) {
          x = words::human_readable_letter<Word>(p.index(x));
        });
      }
      Word A(p.alphabet().size(), 0);

      // The below assertion exists to insure that we are not badly assigning
      // values. The subsequent pragmas exist to suppress the false-positive
      // warnings produced by g++ 13.2.0
      static_assert(
          std::is_same_v<typename Word::value_type,
                         decltype(words::human_readable_letter<Word>(0))>);

#pragma GCC diagnostic push
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
      for (size_t i = 0; i < p.alphabet().size(); ++i) {
        A[i] = words::human_readable_letter<Word>(i);
      }
#pragma GCC diagnostic pop
      p.alphabet(std::move(A));
#ifdef LIBSEMIGROUPS_DEBUG
      p.throw_if_bad_alphabet_or_rules();
#endif
    }

    template <typename Word>
    void change_alphabet(Presentation<Word>& p, Word const& new_alphabet) {
      using letter_type = typename Presentation<Word>::letter_type;

      p.throw_if_bad_alphabet_or_rules();

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
      p.throw_if_bad_alphabet_or_rules();
#endif
    }

    template <typename Iterator>
    Iterator longest_rule(Iterator first, Iterator last) {
      throw_if_odd_number_of_rules(first, last);

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
    typename Iterator::value_type::size_type
    longest_rule_length(Iterator first, Iterator last) {
      auto it = longest_rule(first, last);
      if (it != last) {
        return it->size() + (it + 1)->size();
      } else {
        return 0;
      }
    }

    template <typename Iterator>
    Iterator shortest_rule(Iterator first, Iterator last) {
      throw_if_odd_number_of_rules(first, last);

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
    typename Iterator::value_type::size_type
    shortest_rule_length(Iterator first, Iterator last) {
      auto it = shortest_rule(first, last);
      if (it != last) {
        return it->size() + (it + 1)->size();
      } else {
        return 0;
      }
    }

    template <typename Word>
    void remove_redundant_generators(Presentation<Word>& p) {
      using letter_type_ = typename Presentation<Word>::letter_type;
      throw_if_odd_number_of_rules(p);

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
          p.remove_generator_no_checks(lhs[0]);
        } else if (rhs.size() == 1
                   && std::none_of(
                       lhs.cbegin(), lhs.cend(), [&rhs](letter_type_ const& a) {
                         return a == rhs[0];
                       })) {
          replace_subword(p, rhs, lhs);
          p.remove_generator_no_checks(rhs[0]);
        }
      }
      remove_trivial_rules(p);
    }

    template <typename Word>
    typename Presentation<Word>::letter_type
    first_unused_letter(Presentation<Word> const& p) {
      using letter_type = typename Presentation<Word>::letter_type;
      using size_type   = typename Word::size_type;

      auto const max_letter
          = static_cast<size_type>(std::numeric_limits<letter_type>::max()
                                   - std::numeric_limits<letter_type>::min());

      // If the size of letter_type is the same as the size of size_t, then the
      // largest possible alphabet [0, max] has size one larger than max. To
      // prevent alphabet.size() overflowing, we don't allow this.
      if constexpr (sizeof(letter_type) >= sizeof(size_type)) {
        if (p.alphabet().size() == max_letter) {
          LIBSEMIGROUPS_EXCEPTION(
              "the alphabet of the 1st argument already has the maximum size "
              "of {}, there are no unused generators",
              max_letter);
        }
      } else {
        if (p.alphabet().size() == max_letter + 1) {
          LIBSEMIGROUPS_EXCEPTION(
              "the alphabet of the 1st argument already has the maximum size "
              "of {}, there are no unused generators",
              std::numeric_limits<letter_type>::max()
                  - std::numeric_limits<letter_type>::min());
        }
      }

      letter_type x;
      for (size_type i = 0; i < max_letter; ++i) {
        x = words::human_readable_letter<Word>(i);
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
              p,
              {x},
              {words::human_readable_letter<Word>(non_trivial_scc[index])});
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
      using words::operator+;
      size_t       m = letters1.size(), n = letters2.size();
      Word const * shorter = &letters1, *longer = &letters2;
      if (m > n) {
        std::swap(shorter, longer);
        std::swap(m, n);
      }
      Presentation<Word> q;
      for (size_t i = 0; i < m; ++i) {
        Word u = {(*shorter)[i]};
        for (size_t j = i; j < n; ++j) {
          Word v = {(*longer)[j]};
          if (u != v) {
            presentation::add_rule_no_checks(q, u + v, v + u);
          }
        }
      }
      presentation::add_rules_no_checks(p, q);
    }

    template <typename Word>
    void add_commutes_rules_no_checks(Presentation<Word>&         p,
                                      Word const&                 letters,
                                      std::initializer_list<Word> words) {
      using words::operator+;

      Presentation<Word> q;
      for (auto a : letters) {
        Word u = {a};
        for (auto const& v : words) {
          if (u != v) {
            presentation::add_rule_no_checks(q, u + v, v + u);
          }
        }
      }
      presentation::add_rules_no_checks(p, q);
    }

    template <typename Word1, typename Word2>
    void balance_no_checks(Presentation<Word1>& p,
                           Word2 const&         letters,
                           Word2 const&         inverses) {
      // TODO(later) check args (including that p.contains_empty_word)
      // So that longer relations are on the lhs
      presentation::sort_each_rule(p);

      std::unordered_map<typename Word2::value_type, size_t> map;

      for (auto [i, x] : rx::enumerate(letters)) {
        map.emplace(x, i);
      }

      for (auto it = p.rules.begin(); it != p.rules.end(); it += 2) {
        auto& l = *it;
        auto& r = *(it + 1);
        // Check that we aren't actually about to remove one of the inverse
        // relations itself
        if (l.size() == 2 && r.empty()) {
          auto mit = map.find(l.front());
          if (mit != map.cend() && l.back() == inverses[mit->second]) {
            continue;
          }
        }

        size_t const min = (l.size() + r.size()) % 2;
        while (l.size() - r.size() > min) {
          auto mit = map.find(l.back());
          if (mit != map.cend()) {
            r.insert(r.end(), inverses[mit->second]);
            l.erase(l.end() - 1);
          } else {
            break;
          }
        }
        while (l.size() - r.size() > min) {
          auto mit = map.find(l.front());
          if (mit != map.cend()) {
            r.insert(r.begin(), inverses[mit->second]);
            l.erase(l.begin());
          } else {
            break;
          }
        }
      }
    }

    template <typename Word>
    void balance(Presentation<Word>& p) {
      p.throw_if_bad_alphabet_or_rules();
      if (!p.contains_empty_word()) {
        return;
      }
      auto [letters, inverses] = try_detect_inverses(p);
      balance_no_checks(p, letters, inverses);
    }

    template <typename Word>
    void try_detect_inverses(Presentation<Word>& p,
                             Word&               letters,
                             Word&               inverses) {
      p.throw_if_bad_alphabet_or_rules();
      using value_type = typename Word::value_type;
      // values in this map are pairs <p> such that <p.first> is a found
      // inverse for the key, and <p.second> is the index of the rule showing
      // this.
      std::unordered_map<value_type, std::pair<value_type, size_t>> map;

      for (size_t pos = 0; pos != p.rules.size(); pos += 2) {
        auto &lhs = p.rules[pos], &rhs = p.rules[pos + 1];
        if (lhs.empty() && rhs.size() == 2) {
          std::swap(lhs, rhs);
        }
        if (lhs.size() == 2 && rhs.empty()) {
          auto letter = lhs[0], inverse = lhs[1];
          auto [it, inserted] = map.emplace(letter, std::pair{inverse, pos});

          if (!inserted && it->second.first != inverse) {
            value_type alt_inverse = it->second.first;
            size_t     alt_pos     = it->second.second;
            LIBSEMIGROUPS_EXCEPTION(
                "the rules {} = {} (rule {}) and {} = {} (rule {}) yield "
                "the conflicting values {} != {} for the inverse of {}, "
                "please use the 2- or 3-argument version of this function "
                "to explicitly specify the inverses",
                detail::to_printable(lhs),
                detail::to_printable(rhs),
                pos / 2,
                detail::to_printable(p.rules[alt_pos]),
                detail::to_printable(p.rules[alt_pos + 1]),
                alt_pos / 2,
                detail::to_printable(inverse),
                detail::to_printable(alt_inverse),
                detail::to_printable(letter));
          }
        }
      }
      for (auto const& [key, val] : map) {
        if (map[val.first].first == key) {
          letters.push_back(key);
          inverses.push_back(val.first);
        }
      }
    }

    template <typename Word>
    std::pair<Word, Word> try_detect_inverses(Presentation<Word>& p) {
      Word letters;
      Word inverses;
      try_detect_inverses(p, letters, inverses);
      return {letters, inverses};
    }

    template <typename Word1, typename Word2>
    void add_cyclic_conjugates_no_checks(Presentation<Word1>& p,
                                         Word2 const&         relator) {
      for (size_t i = 0; i <= relator.size(); ++i) {
        Word1 copy(relator);
        std::rotate(copy.begin(), copy.begin() + i, copy.end());
        presentation::add_rule_no_checks(p, copy, Word1());
      }
    }

    template <typename Word1, typename Word2>
    void add_cyclic_conjugates(Presentation<Word1>& p, Word2 const& relator) {
      p.throw_if_letter_not_in_alphabet(relator.begin(), relator.end());
      if (!p.contains_empty_word()) {
        LIBSEMIGROUPS_EXCEPTION("this function requires the presentation to "
                                "contain the empty word, did you mean to "
                                "call contains_empty_word(true) first?");
      }
      add_cyclic_conjugates_no_checks(p, relator);
    }

    template <typename Word>
    [[nodiscard]] typename std::vector<Word>::iterator
    find_rule_no_checks(Presentation<Word>& p,
                        Word const&         lhs,
                        Word const&         rhs) {
      for (auto it = p.rules.begin(); it != p.rules.end(); it += 2) {
        if (*it == lhs && *(it + 1) == rhs) {
          return it;
        }
      }
      return p.rules.end();
    }

    template <typename Word>
    [[nodiscard]] size_t index_rule_no_checks(Presentation<Word> const& p,
                                              Word const&               lhs,
                                              Word const&               rhs) {
      auto it = find_rule_no_checks(p, lhs, rhs);
      if (it != p.rules.cend()) {
        return std::distance(p.rules.begin(), it);
      }
      return UNDEFINED;
    }
  }  // namespace presentation

  template <typename Word>
  std::string to_human_readable_repr(Presentation<Word> const& p) {
    using detail::group_digits;
    size_t alphabet_size = p.alphabet().size();
    size_t n_rules       = p.rules.size() / 2;
    return fmt::format(
        "<{} presentation with {} letter{}, {} rule{}, and length {}>",
        (p.contains_empty_word() ? "monoid" : "semigroup"),
        group_digits(alphabet_size),
        (alphabet_size == 1 ? "" : "s"),
        group_digits(n_rules),
        (n_rules == 1 ? "" : "s"),
        group_digits(presentation::length(p)));
  }

  template <typename Word>
  std::string to_human_readable_repr(InversePresentation<Word> const& p) {
    std::string out
        = to_human_readable_repr(static_cast<Presentation<Word> const&>(p));
    out.insert(1, "inverse ");
    return out;
  }

  template <typename Word>
  InversePresentation<Word>&
  InversePresentation<Word>::inverses_no_checks(word_type const& w) {
    // TODO(later) maybe don't throw_if_bad_alphabet_or_rules here but only in
    // the throw_if_bad_alphabet_or_rules function to be written. Set the
    // alphabet to include the inverses
    _inverses = w;
    return *this;
  }

  template <typename Word>
  typename InversePresentation<Word>::letter_type
  InversePresentation<Word>::inverse(letter_type x) const {
    if (_inverses.empty()) {
      LIBSEMIGROUPS_EXCEPTION("no inverses have been defined")
    }
    // TODO should throw if index is out of bounds
    return _inverses[Presentation<Word>::index(x)];
  }

  namespace v4 {
    ////////////////////////////////////////////////////////////////////////
    // Presentation + function -> Presentation
    ////////////////////////////////////////////////////////////////////////

    template <typename Result, typename Word, typename Func>
    auto to(Presentation<Word> const& p, Func&& f) -> std::enable_if_t<
        std::is_same_v<Presentation<typename Result::word_type>, Result>,
        Result> {
      using WordOutput = typename Result::word_type;

      static_assert(
          std::is_invocable_v<std::decay_t<Func>,
                              typename Presentation<Word>::letter_type>);

      // Must call p.throw_if_bad_alphabet_or_rules otherwise f(val) may
      // segfault if val is not in the alphabet
      p.throw_if_bad_alphabet_or_rules();

      Result result;
      result.contains_empty_word(p.contains_empty_word());
      WordOutput new_alphabet;
      new_alphabet.resize(p.alphabet().size());
      std::transform(
          p.alphabet().cbegin(), p.alphabet().cend(), new_alphabet.begin(), f);
      // TODO(1) use alphabet_no_checks when it is implemented
      result.alphabet(new_alphabet);
      WordOutput rel;
      for (auto it = p.rules.cbegin(); it != p.rules.cend(); ++it) {
        rel.resize(it->size());
        std::transform(it->cbegin(), it->cend(), rel.begin(), f);
        result.rules.push_back(std::move(rel));
        rel.clear();
      }
      return result;
    }

    ////////////////////////////////////////////////////////////////////////
    // InversePresentation + function -> InversePresentation
    ////////////////////////////////////////////////////////////////////////

    template <typename Result, typename Word, typename Func>
    auto to(InversePresentation<Word> const& ip, Func&& f) -> std::enable_if_t<
        std::is_same_v<InversePresentation<typename Result::word_type>, Result>,
        Result> {
      static_assert(
          std::is_invocable_v<std::decay_t<Func>,
                              typename Presentation<Word>::letter_type>);
      using WordOutput = typename Result::word_type;

      ip.throw_if_letter_not_in_alphabet(ip.inverses().begin(),
                                         ip.inverses().end());
      InversePresentation<WordOutput> result(
          std::move(v4::to<Presentation<WordOutput>>(ip, f)));

      WordOutput new_inverses;
      new_inverses.resize(ip.inverses().size());
      std::transform(ip.inverses().cbegin(),
                     ip.inverses().cend(),
                     new_inverses.begin(),
                     f);
      // TODO(1) should move new_inverses into result
      result.inverses_no_checks(new_inverses);
      return result;
    }

    ////////////////////////////////////////////////////////////////////////
    // Presentation -> Presentation
    ////////////////////////////////////////////////////////////////////////

    template <typename Result, typename Word>
    auto to(Presentation<Word> const& p) -> std::enable_if_t<
        std::is_same_v<Presentation<typename Result::word_type>, Result>
            && !std::is_same_v<typename Result::word_type, Word>,
        Result> {
      using WordOutput = typename Result::word_type;
      return v4::to<Result>(p, [&p](auto val) {
        return words::human_readable_letter<WordOutput>(p.index(val));
      });
    }

    ////////////////////////////////////////////////////////////////////////
    // InversePresentation -> InversePresentation
    ////////////////////////////////////////////////////////////////////////

    // Implemented in hpp file because very short

    ////////////////////////////////////////////////////////////////////////
    // Presentation -> InversePresentation
    ////////////////////////////////////////////////////////////////////////

    template <template <typename...> typename Thing, typename Word>
    auto to(Presentation<Word> const& p) -> std::enable_if_t<
        std::is_same_v<InversePresentation<Word>, Thing<Word>>,
        InversePresentation<Word>> {
      InversePresentation<Word> result(p);
      presentation::normalize_alphabet(
          result);  // calls p.throw_if_bad_alphabet_or_rules
      result.alphabet(2 * result.alphabet().size());
      auto invs = result.alphabet();

      // The below pragma exists to suppress the false-positive warnings
      // produced by g++ 13.2.0
#pragma GCC diagnostic push
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
      std::rotate(invs.begin(), invs.begin() + invs.size() / 2, invs.end());
#pragma GCC diagnostic pop
      result.inverses_no_checks(std::move(invs));
      return result;
    }

  }  // namespace v4

}  // namespace libsemigroups

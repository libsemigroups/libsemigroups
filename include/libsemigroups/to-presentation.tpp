//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024-2025 Joseph Edwards
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

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // FroidurePin -> Presentation
  ////////////////////////////////////////////////////////////////////////

  template <typename Result>
  auto to(FroidurePinBase& fp) -> std::enable_if_t<
      std::is_same_v<Presentation<typename Result::word_type>, Result>,
      Result> {
    using WordOutput = typename Result::word_type;

    Result p;
    p.alphabet(fp.number_of_generators());
    WordOutput lhs, rhs;
    auto       f = [](auto val) {
      return words::human_readable_letter<WordOutput>(val);
    };

    for (auto it = fp.cbegin_rules(); it != fp.cend_rules(); ++it) {
      lhs.resize(it->first.size());
      rhs.resize(it->second.size());
      std::transform(it->first.cbegin(), it->first.cend(), lhs.begin(), f);
      std::transform(it->second.cbegin(), it->second.cend(), rhs.begin(), f);
      p.rules.push_back(std::move(lhs));
      p.rules.push_back(std::move(rhs));
      lhs.clear();
      rhs.clear();
    }

    return p;
  }

  ////////////////////////////////////////////////////////////////////////
  // KnuthBendix -> Presentation
  ////////////////////////////////////////////////////////////////////////

  template <typename Result,
            typename WordIn,
            typename Rewriter,
            typename ReductionOrder>
  auto to(KnuthBendix<WordIn, Rewriter, ReductionOrder>& kb)
      -> std::enable_if_t<
          std::is_same_v<Presentation<typename Result::word_type>, Result>,
          Result> {
    using WordOut                      = typename Result::word_type;
    Presentation<WordIn> const& p_orig = kb.presentation();
    Presentation<WordIn>        p;
    p.alphabet(p_orig.alphabet())
        .contains_empty_word(p_orig.contains_empty_word());

    for (auto const& rule : kb.active_rules()) {
      presentation::add_rule(p, rule.first, rule.second);
    }

    if constexpr (std::is_same_v<WordIn, WordOut>) {
      return p;
    } else {
      // TODO(1) avoid double copy here
      return to<Presentation<WordOut>>(p);
    }
  }

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

    // Must call p.throw_if_bad_alphabet_or_rules otherwise f(val) may segfault
    // if val is not in the alphabet
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
        std::move(to<Presentation<WordOutput>>(ip, f)));

    WordOutput new_inverses;
    new_inverses.resize(ip.inverses().size());
    std::transform(
        ip.inverses().cbegin(), ip.inverses().cend(), new_inverses.begin(), f);
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
    return to<Result>(p, [&p](auto val) {
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

    // The below pragma exists to suppress the false-positive warnings produced
    // by g++ 13.2.0
#pragma GCC diagnostic push
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
    std::rotate(invs.begin(), invs.begin() + invs.size() / 2, invs.end());
#pragma GCC diagnostic pop
    result.inverses_no_checks(std::move(invs));
    return result;
  }
}  // namespace libsemigroups

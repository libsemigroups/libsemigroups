//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024 Joseph Edwards
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

  template <typename WordOutput>
  // TODO(later) why not just to_presentation with no template WordOutput,
  // always use word_type, and can be converted after if desirable
  Presentation<WordOutput> to_presentation(FroidurePinBase& fp) {
    Presentation<WordOutput> p;
    p.alphabet(fp.number_of_generators());
    WordOutput lhs, rhs;
    auto       f = [](auto val) {
      return presentation::human_readable_letter<WordOutput>(val);
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

  template <typename WordOutput, typename WordInput, typename Func>
  auto to_presentation(Presentation<WordInput> const& p, Func&& f)
      -> std::enable_if_t<
          std::is_invocable_v<std::decay_t<Func>,
                              typename Presentation<WordInput>::letter_type>
              && !std::is_same_v<WordOutput, WordInput>,
          Presentation<WordOutput>> {
    p.validate();
    // Must call p.validate otherwise f(val) may segfault if val is not in the
    // alphabet

    Presentation<WordOutput> result;
    result.contains_empty_word(p.contains_empty_word());
    WordOutput new_alphabet;
    new_alphabet.resize(p.alphabet().size());
    std::transform(
        p.alphabet().cbegin(), p.alphabet().cend(), new_alphabet.begin(), f);
    // TODO(later) use alphabet_no_checks when it is implemented
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

  template <typename WordOutput, typename WordInput>
  auto to_presentation(Presentation<WordInput> const& p)
      -> std::enable_if_t<!std::is_same_v<WordOutput, WordInput>,
                          Presentation<WordOutput>> {
    return to_presentation<WordOutput>(p, [&p](auto val) {
      return presentation::human_readable_letter<WordOutput>(p.index(val));
    });
  }

  template <typename WordOutput, typename WordInput, typename Func>
  auto to_inverse_presentation(InversePresentation<WordInput> const& ip,
                               Func&&                                f)
      -> std::enable_if_t<
          std::is_invocable_v<std::decay_t<Func>,
                              typename Presentation<WordInput>::letter_type>
              && !std::is_same_v<WordOutput, WordInput>,
          InversePresentation<WordOutput>> {
    ip.validate_word(ip.inverses().begin(), ip.inverses().end());
    InversePresentation<WordOutput> result(
        std::move(to_presentation<WordOutput>(ip, f)));

    WordOutput new_inverses;
    new_inverses.resize(ip.inverses().size());
    std::transform(
        ip.inverses().cbegin(), ip.inverses().cend(), new_inverses.begin(), f);
    result.inverses_no_checks(new_inverses);
    return result;
  }

  template <typename Word>
  InversePresentation<Word>
  to_inverse_presentation(Presentation<Word> const& p) {
    InversePresentation<Word> result(p);
    presentation::normalize_alphabet(result);
    result.alphabet(2 * result.alphabet().size());
    auto invs = result.alphabet();
    std::rotate(invs.begin(), invs.begin() + invs.size() / 2, invs.end());
    result.inverses_no_checks(std::move(invs));
    return result;
  }

  template <typename WordOutput, typename WordInput>
  auto to_inverse_presentation(InversePresentation<WordInput> const& ip)
      -> std::enable_if_t<!std::is_same_v<WordOutput, WordInput>,
                          InversePresentation<WordOutput>> {
    return to_inverse_presentation<WordOutput>(ip, [&ip](auto val) {
      return presentation::human_readable_letter<WordOutput>(ip.index(val));
    });
  }

}  // namespace libsemigroups

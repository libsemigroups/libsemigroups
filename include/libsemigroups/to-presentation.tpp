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
      return v4::to<Presentation<WordOut>>(p);
    }
  }
}  // namespace libsemigroups

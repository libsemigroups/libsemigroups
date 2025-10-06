//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024-2025 James D. Mitchell
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

  namespace congruence_common {

    // We have to pass the presentation here, because o/w we have no way of
    // knowing over what we should compute the non-trivial classes (i.e. we
    // cannot always recover p from cong).
    // TODO(1) reintegrate this function, but for every one of KnuthBendix,
    // Kambites, ToddCoxeter, and Congruence
    // template <typename Word>
    // std::vector<std::vector<Word>>
    // non_trivial_classes(Word& cong, Presentation<word_type> const& p) {
    //   using rx::operator|;
    //   cong.run();
    //   if (cong.has<ToddCoxeter<word_type>>()
    //       && cong.get<ToddCoxeter<word_type>>()->finished()) {
    //     ToddCoxeter<word_type> tc(cong.kind(), p);
    //     return non_trivial_classes(*cong.get<ToddCoxeter<word_type>>(), tc);
    //   } else if (cong.has<KnuthBendixImpl<>>()
    //              && cong.get<KnuthBendixImpl<>>()->finished()) {
    //     KnuthBendix kb(cong.kind(), to<Presentation<std::string>>(p));
    //     auto        strings
    //         = non_trivial_classes(kb, *cong.get<KnuthBendix<std::string>>());
    //     std::vector<std::vector<word_type>> result;
    //     for (auto const& klass : strings) {
    //       result.push_back(rx::iterator_range(klass.begin(), klass.end())
    //                        | ToWord(kb.presentation().alphabet())
    //                        | rx::to_vector());
    //     }
    //     return result;
    //   }
    //   // If a Kambites object wins the race in Congruence, then we cannot
    //   // really compute anything here unless the semigroup defined by p is
    //   // finite, but that'd be better handled explicitly in any code calling
    //   // this.
    //   LIBSEMIGROUPS_EXCEPTION("Cannot compute the non-trivial classes!");
    // }

    // TODO(1) reintegrate this function, but for every one of KnuthBendix,
    // Kambites, ToddCoxeter, and Congruence
    // std::vector<std::vector<std::string>>
    // non_trivial_classes(Congruence& cong, Presentation<std::string> const& p)
    // {
    //   using rx::operator|;
    //   cong.run();
    //   if (cong.has<KnuthBendixImpl<>>() &&
    //   cong.get<KnuthBendixImpl<>>()->finished())
    //   {
    //     KnuthBendixImpl<> kb(cong.kind(), p);
    //     return non_trivial_classes(kb, *cong.get<KnuthBendixImpl<>>());
    //   }
    //   if (cong.has<ToddCoxeter<word_type>>()
    //       && cong.get<ToddCoxeter<word_type>>()->finished()) {
    //     ToddCoxeter<word_type> tc(cong.kind(), p);
    //     auto                   words
    //         = non_trivial_classes(*cong.get<ToddCoxeter<word_type>>(), tc);
    //     std::vector<std::vector<std::string>> result;
    //     for (auto const& klass : words) {
    //       result.push_back(rx::iterator_range(klass.begin(), klass.end())
    //                        | ToString(p.alphabet()) | rx::to_vector());
    //     }
    //     return result;
    //   }

    // If a Kambites object wins the race in Congruence, then we cannot
    // really compute anything here unless the semigroup defined by p is
    // finite, but that'd be better handled explicitly in any code calling
    // this.
    // LIBSEMIGROUPS_EXCEPTION("Cannot compute the non-trivial classes!");
    // }
  }  // namespace congruence_common
}  // namespace libsemigroups

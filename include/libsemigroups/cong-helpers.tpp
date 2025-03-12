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

// This file contains the implementations of the helper function templates for
// the Congruence class, declared in cong-helpers.hpp. See also
// cong-helpers.cpp for the implementations of any helper functions
// (non-templates).

namespace libsemigroups {

  namespace congruence_common {

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - normal_forms
    ////////////////////////////////////////////////////////////////////////

    // The following doesn't work, because the types of the two returned
    // values aren't the same.
    // TODO(1) implement a class containing a variant for this.
    // template <typename Word>
    // auto normal_forms(Congruence& cong) {
    //   cong.run();
    //   if (cong.has<ToddCoxeterImpl>() &&
    //   cong.get<ToddCoxeterImpl>()->finished()) {
    //     return todd_coxeter::normal_forms(*cong.get<ToddCoxeterImpl>());
    //   } else if (cong.has<KnuthBendixImpl<>>()
    //              && cong.get<KnuthBendixImpl<>>()->finished()) {
    //     return knuth_bendix::normal_forms(*cong.get<KnuthBendixImpl<>>());
    //   }
    //   // There's currently no normal_forms function for Kambites, so can't
    //   // return anything in that case.
    //   LIBSEMIGROUPS_EXCEPTION("Cannot compute the non-trivial classes!");
    // }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - partition
    ////////////////////////////////////////////////////////////////////////

    template <typename Word, typename Range, typename>
    std::vector<std::vector<Word>> partition(Congruence<Word>& cong, Range r) {
      static_assert(
          std::is_same_v<Word, std::decay_t<typename Range::output_type>>);

      cong.run();
      if (cong.template has<ToddCoxeter<Word>>()
          && cong.template get<ToddCoxeter<Word>>()->finished()) {
        return partition(*cong.template get<ToddCoxeter<Word>>(), r);
      } else if (cong.template has<KnuthBendix<Word>>()
                 && cong.template get<KnuthBendix<Word>>()->finished()) {
        return partition(*cong.template get<KnuthBendix<Word>>(), r);
      } else if (cong.template has<Kambites<Word>>()
                 && cong.template get<Kambites<Word>>()->success()) {
        return partition(*cong.template get<Kambites<Word>>(), r);
      }
      LIBSEMIGROUPS_EXCEPTION("Cannot compute the partition");
    }

  }  // namespace congruence_common
}  // namespace libsemigroups

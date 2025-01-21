//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 James D. Mitchell
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

// This file contains out-of-line ToddCoxeter mem fn templates.

namespace libsemigroups {

  template <typename Word>
  template <typename Node>
  ToddCoxeter<Word>& ToddCoxeter<Word>::init(congruence_kind        knd,
                                             WordGraph<Node> const& wg) {
    ToddCoxeterBase::init(knd, wg);
    _presentation = to_presentation<Word>(current_word_graph().presentation());
    _generating_pairs.clear();
    return *this;
  }

}  // namespace libsemigroups

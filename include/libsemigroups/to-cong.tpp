//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025-2026 James D. Mitchell
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

  template <typename Result, typename Node>
  auto to(congruence_kind knd, FroidurePinBase& fpb, WordGraph<Node> const& wg)
      -> std::enable_if_t<
          std::is_same_v<Congruence<typename Result::native_word_type>, Result>,
          Result> {
    using Word = typename Result::native_word_type;
    if (&wg != &fpb.left_cayley_graph() && &wg != &fpb.right_cayley_graph()) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected the 3rd argument (WordGraph) to be the left_cayley_graph "
          "or right_cayley_graph of the 2nd argument (FroidurePin)!")
    }

    Congruence<Word> cong;

    // TODO(1) if necessary make a runner that tries to fpb.run(), then get
    // the Cayley graph and use that in the ToddCoxeter, at present
    // that'll happen here in the constructor, same for the creation of the
    // presentation this could take place in the Runner so that they are done
    // in parallel
    cong.add_runner(std::make_shared<ToddCoxeter<Word>>(
        to<ToddCoxeter<Word>>(knd, fpb, wg)));

    // FIXME(1) uncommenting the following lines causes multiple issues in the
    // extreme Congruence test
    // auto tc = to_todd_coxeter(knd, fpb, wg);
    // tc.strategy(ToddCoxeter<Word>::options::strategy::felsch);
    // add_runner(std::make_shared<ToddCoxeter<Word>>(std::move(tc)));

    // We can no longer add the following runner if wg is the left_cayley_graph
    // of fpb, then the below will compute the corresponding right congruence,
    // which is unavoidable. TODO(1) reconsider this?
    // auto tc = ToddCoxeter<Word>(knd, to<Presentation<Word>>(fpb));
    // add_runner(std::make_shared<ToddCoxeter<Word>>(std::move(tc)));

    // tc = ToddCoxeter<Word>(knd, to<Presentation<Word>>(fpb));
    // tc.strategy(ToddCoxeter<Word>::options::strategy::felsch);
    // add_runner(std::make_shared<ToddCoxeter<Word>>(std::move(tc)));

    return cong;
  }

  template <typename Result, typename Node>
  auto to(congruence_kind knd, WordGraph<Node> const& wg) -> std::enable_if_t<
      std::is_same_v<Congruence<typename Result::native_word_type>, Result>,
      Result> {
    using Word = typename Result::native_word_type;
    Congruence<Word> cong;

    cong.add_runner(std::make_shared<ToddCoxeter<Word>>(knd, wg));
    return cong;
  }
}  // namespace libsemigroups

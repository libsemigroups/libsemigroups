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

namespace libsemigroups {

  template <typename Result, typename Node>
  auto to(congruence_kind knd, FroidurePinBase& fpb, WordGraph<Node> const& wg)
      -> std::enable_if_t<
          std::is_same_v<Congruence<typename Result::native_word_type>, Result>,
          Result> {
    using Word = typename Result::native_word_type;
    if (fpb.is_finite() != tril::FALSE) {
      fpb.run();
    } else {
      LIBSEMIGROUPS_EXCEPTION(
          "the 2nd argument does not represent a finite semigroup!");
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
    // auto tc = ToddCoxeter<Word>(knd, to_presentation<Word>(fpb));
    // add_runner(std::make_shared<ToddCoxeter<Word>>(std::move(tc)));

    // tc = ToddCoxeter<Word>(knd, to_presentation<Word>(fpb));
    // tc.strategy(ToddCoxeter<Word>::options::strategy::felsch);
    // add_runner(std::make_shared<ToddCoxeter<Word>>(std::move(tc)));

    return cong;
  }
}  // namespace libsemigroups

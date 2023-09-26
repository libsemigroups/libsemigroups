//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 James D. Mitchell
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
// TODO:
// * nr_h_classes (easy)
// * h_class_reps (harder, as words, and/or as Stephen objects)
//   either all words labelling a path in both Stephen and Stephen^-1; or
//   use the automorphism group of Stephen::word_graph() (as per Theorem 3.5).
// * idempotents (easy, as words and/or Stephen objects)
// * natural partial order
// * is_natural_less(word, word) and is_natural_less(Stephen, Stephen)
// * Stephen.natural_order_filter() = S.words_accepted()
// * d_class reps (easy)
// * r_class reps (easy)
// * l_class reps (just inverses of r_class reps)
// * is_X_related(Stephen, Stephen) and/or is_X_related(word, word)
//   for X = L, R, H, D.
// * contains
// * is_group (Theorem 3.7(a))
// * is_bisimple (just nr_of_dclasses = 1)
// * is_e_unitary (Theorem 3.8)
// * elements(Stephen) and elements(Cutting)
// * to_froidure_pin impl
// * to_konieczny impl
// * to_schreier_sims_impl (when is_group)

#ifndef LIBSEMIGROUPS_CUTTING_HPP_
#define LIBSEMIGROUPS_CUTTING_HPP_

#include "cong-intf.hpp"  // for CongruenceInterface
#include "gabow.hpp"
#include "runner.hpp"  // for Runner
#include "stephen.hpp"
#include "types.hpp"

namespace libsemigroups {
  class Cutting : public Runner {
   public:
    using stephen_type
        = Stephen<std::shared_ptr<InversePresentation<word_type>>>;

   private:
    std::shared_ptr<InversePresentation<word_type>> _presentation;
    std::vector<stephen_type>                       _stephens;
    bool                                            _finished;
    // TODO use Stephen::node_type
    WordGraph<uint32_t> _graph;
    Gabow<uint32_t>     _gabow;

   public:
    // TODO to cpp
    explicit Cutting(InversePresentation<word_type> const& p)
        : Runner(),
          _presentation(std::make_shared<InversePresentation<word_type>>(p)),
          _stephens(),
          _finished(false),
          _graph(0, p.alphabet().size()),
          _gabow() {
      _presentation->validate();
      _presentation->contains_empty_word(true);  // TODO
      _stephens.emplace_back(_presentation);
      _stephens.back().set_word(word_type({}));
    }

    // TODO to cpp
    // TODO rename number_of_classes
    uint64_t size() {
      run();
      uint64_t result = 0;
      std::for_each(
          _stephens.cbegin(), _stephens.cend(), [&result](auto const& s) {
            result += s.word_graph().number_of_nodes();
          });
      return result;
    }

    uint64_t number_of_r_classes() {
      run();
      return _stephens.size();
    }

    uint64_t number_of_d_classes() {
      run();
      return _gabow.number_of_components();
    }

   private:
    bool finished_impl() const override {
      return _finished;
    }

    // TODO to cpp
    void run_impl() override {
      if (finished()) {
        return;
      }

      stephen_type tmp(_presentation);
      auto const&  p = *_presentation;

      for (size_t i = 0; i < _stephens.size(); ++i) {
        auto& s    = _stephens[i];
        auto  word = s.word();
        word.insert(word.begin(), 0);
        _graph.add_nodes(1);
        for (auto const& letter : p.alphabet()) {
          word[0] = letter;
          tmp.set_word(word).run();
          bool old = false;
          // TODO try multiplying the Stephen for letter by _stephen[j], then
          // since Stephen's are standardized, we can possibly do a binary
          // search in the sorted list of _stephens...
          // TODO or even better can't we just keep track of where we fall out
          // of one Stephen into the next, and just follow the edges
          // accordingly?
          for (size_t j = 0; j < _stephens.size(); ++j) {
            if (stephen::is_left_factor(_stephens[j], word)) {
              if (stephen::is_left_factor(tmp, _stephens[j].word())) {
                _graph.set_target_no_checks(i, letter, j);
                old = true;
                break;
              }
            }
          }
          if (!old) {
            _graph.set_target_no_checks(i, letter, _stephens.size());
            _stephens.push_back(tmp);
          }
        }
      }
      _gabow.init(_graph);
      _finished = true;
    }
  };

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_CUTTING_HPP_

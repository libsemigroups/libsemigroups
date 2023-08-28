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

#ifndef LIBSEMIGROUPS_CUTTING_HPP_
#define LIBSEMIGROUPS_CUTTING_HPP_

#include "cong-intf.hpp"  // for CongruenceInterface
#include "runner.hpp"     // for Runner
#include "stephen.hpp"
#include "types.hpp"

namespace libsemigroups {
  class Cutting : public Runner {
   public:
    using stephen_type
        = v3::Stephen<std::shared_ptr<InversePresentation<word_type>>>;

   private:
    std::shared_ptr<InversePresentation<word_type>> _presentation;
    std::vector<stephen_type>                       _stephens;
    bool                                            _finished;
    // TODO use StephenB::node_type
    ActionDigraph<size_t> _graph;

   public:
    // TODO to cpp
    explicit Cutting(InversePresentation<word_type> const& p)
        : Runner(),
          _presentation(std::make_shared<InversePresentation<word_type>>(p)),
          _stephens(),
          _finished(false),
          _graph(0, p.alphabet().size()) {
      _presentation->validate();
      _presentation->contains_empty_word(true);  // TODO
      _stephens.emplace_back(_presentation);
      _stephens.back().set_word(word_type({}));
    }

    // TODO to cpp
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
      return _graph.number_of_scc();
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
          for (size_t j = 0; j < _stephens.size(); ++j) {
            if (v3::stephen::is_left_factor(_stephens[j], word)) {
              if (v3::stephen::is_left_factor(tmp, _stephens[j].word())) {
                _graph.add_edge_nc(i, j, letter);
                old = true;
                break;
              }
            }
          }
          if (!old) {
            _graph.add_edge_nc(i, _stephens.size(), letter);
            _stephens.push_back(tmp);
          }
        }
      }
      _finished = true;
    }
  };

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_CUTTING_HPP_

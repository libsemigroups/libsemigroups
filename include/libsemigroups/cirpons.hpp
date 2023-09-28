//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 James D. Mitchell
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

// This file contains declaration of a class TODO

#ifndef LIBSEMIGROUPS_CIRPONS_HPP_
#define LIBSEMIGROUPS_CIRPONS_HPP_

#include <utility>  // for pair

#include <iostream>

#include "felsch-graph.hpp"  // for FelschGraph
#include "gabow.hpp"         // for Gabow
#include "runner.hpp"        // for Runner
#include "stephen.hpp"       // for WordGraph
#include "word-graph.hpp"    // for WordGraph

namespace libsemigroups {

  class Cirpons : public Runner {
   public:
    using node_type   = uint32_t;
    using label_type  = typename WordGraph<node_type>::label_type;
    using Definition  = std::pair<node_type, label_type>;
    using Definitions = std::vector<Definition>;

    class CirponsGraph : public FelschGraph<word_type, node_type, Definitions> {
      using FelschGraph_ = FelschGraph<word_type, node_type, Definitions>;
      Forest _forest;

     public:
      using FelschGraph_::FelschGraph;
      using FelschGraph_::labels_and_targets_no_checks;
      using FelschGraph_::nodes;

      void disjoint_union_inplace(WordGraph<node_type> const& that) {
        size_t N = number_of_nodes();
        FelschGraph_::add_nodes(that.number_of_nodes());
        _forest.add_nodes(that.number_of_nodes());
        for (auto s : that.nodes()) {
          for (auto [a, t] : that.labels_and_targets_no_checks(s)) {
            if (t != UNDEFINED) {
              FelschGraph_::set_target_no_checks(s + N, a, t + N);
              if (_forest.parent(t + N) == UNDEFINED && t + N != 0) {
                _forest.set(t + N, s, a);
              }
            }
          }
        }
      }

      Forest const& forest() const noexcept {
        return _forest;
      }
    };

   private:
    CirponsGraph                            _cirpons_graph;
    bool                                    _finished;
    Gabow<node_type>                        _gabow;
    Stephen<InversePresentation<word_type>> _stephen;

   public:
    explicit Cirpons(InversePresentation<word_type> const& p)
        : Runner(),
          _cirpons_graph(p),
          _finished(false),
          _gabow(),
          _stephen(p) {}

    uint64_t number_of_classes() {
      return _cirpons_graph.number_of_nodes();
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
      _stephen.set_word(word_type({})).run();
      _cirpons_graph.disjoint_union_inplace(_stephen.word_graph());
      word_type w;
      size_t    count = 0;
      for (node_type s = 0; s < _cirpons_graph.number_of_nodes(); ++s) {
        for (label_type a = 0; a < _cirpons_graph.out_degree(); ++a) {
          node_type t = _cirpons_graph.target_no_checks(s, a);
          if (t == UNDEFINED) {
            std::cout << "R-class " << ++count << std::endl;
            w.clear();
            _cirpons_graph.forest().path_to_root(w, s);
            w.push_back(a);
            _stephen.set_word(w).run();
            size_t n = _cirpons_graph.number_of_nodes();
            _cirpons_graph.disjoint_union_inplace(_stephen.word_graph());
            size_t e = _cirpons_graph.number_of_edges();
            _cirpons_graph.set_target_no_checks(
                s, a, _stephen.accept_state() + n);
            if (!_cirpons_graph.process_definitions(e)) {
              std::cout << "This shouldn't happen!";
            }
          }
        }
      }
    }
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_CIRPONS_HPP_

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

// This file contains a declaration of a class for performing the "low-index
// congruence" algorithm for semigroups and monoid.

#include "libsemigroups/sims2.hpp"

namespace libsemigroups {
  ///////////////////////////////////////////////////////////////////////////////
  // iterator_base nested class
  ///////////////////////////////////////////////////////////////////////////////

  Sims2::iterator_base::iterator_base(Sims2 const* s, size_type n)
      : Sims1::iterator_base<Sims2>(s, n),
        // protected
        _2_sided_include(),
        _2_sided_words() {
    // TODO could be slightly less space allocated here
    size_t const m = Sims1::iterator_base<Sims2>::maximum_number_of_classes();
    Presentation<word_type> const& p = this->_felsch_graph.presentation();
    _2_sided_include.resize(2 * m * p.alphabet().size());
    _2_sided_words.assign(n, word_type());
  }

  Sims2::iterator_base::iterator_base(Sims2::iterator_base const& that)
      : Sims1::iterator_base<Sims2>(that),
        _2_sided_include(that._2_sided_include),
        _2_sided_words(that._2_sided_words) {}

  Sims2::iterator_base::iterator_base(Sims2::iterator_base&& that)
      : Sims1::iterator_base<Sims2>(
          std::move(that)),  // TODO std::move correct?
        _2_sided_include(std::move(that._2_sided_include)),
        _2_sided_words(std::move(that._2_sided_words)) {}

  typename Sims2::iterator_base&
  Sims2::iterator_base::operator=(Sims2::iterator_base const& that) {
    Sims1::iterator_base<Sims2>::operator=(that);
    _2_sided_include = that._2_sided_include;
    _2_sided_words   = that._2_sided_words;

    return *this;
  }

  typename Sims2::iterator_base&
  Sims2::iterator_base::operator=(Sims2::iterator_base&& that) {
    Sims1::iterator_base<Sims2>::operator=(
        std::move(that));  // TODO std::move ok?
    _2_sided_include = std::move(that._2_sided_include);
    _2_sided_words   = std::move(that._2_sided_words);
    return *this;
  }

  void Sims2::iterator_base::swap(Sims2::iterator_base& that) noexcept {
    Sims1::iterator_base<Sims2>::swap(that);
    std::swap(_2_sided_include, that._2_sided_include);
    std::swap(_2_sided_words, that._2_sided_words);
  }

  Sims2::iterator_base::~iterator_base() = default;

  bool Sims2::iterator_base::try_define(PendingDef const& current) {
    LIBSEMIGROUPS_ASSERT(current.target < current.num_nodes);
    LIBSEMIGROUPS_ASSERT(current.num_nodes <= maximum_number_of_classes());

    if (!Sims1::iterator_base<Sims2>::try_define(current)) {
      return false;
    }

    _2_sided_include.backtrack(current.num_edges);

    if (current.target_is_new_node) {
      LIBSEMIGROUPS_ASSERT(current.target < _2_sided_words.size());
      LIBSEMIGROUPS_ASSERT(current.source < _2_sided_words.size());
      _2_sided_words[current.target] = _2_sided_words[current.source];
      _2_sided_words[current.target].push_back(current.generator);
    }

    size_type start = current.num_edges;
    // TODO avoid extra copies here
    // One relation in _2_sided_include for every non-tree edge
    while (start < _felsch_graph.definitions().size()) {
      for (size_t i = start; i < _felsch_graph.definitions().size(); ++i) {
        auto const& e = _felsch_graph.definitions()[i];  // TODO reference
        if (current.target_is_new_node && e.first == current.source
            && e.second == current.generator) {
          continue;
        }
        word_type u = _2_sided_words[e.first];
        u.push_back(e.second);
        _2_sided_include.add_rule(
            u,
            _2_sided_words[_felsch_graph.target_no_checks(e.first, e.second)],
            current.num_edges);
      }
      // TODO different things if current.target is a new node

      auto first = _2_sided_include.begin(current.num_edges);
      auto last  = _2_sided_include.end(current.num_edges);
      start      = _felsch_graph.definitions().size();
      if (!felsch_graph::make_compatible<RegisterDefs>(
              _felsch_graph,
              0,
              _felsch_graph.number_of_active_nodes(),
              first,
              last)
          || !_felsch_graph.process_definitions(start)) {
        return false;
      }
    }
    return true;
  }

}  // namespace libsemigroups

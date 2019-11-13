//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

#include "froidure-pin-base.hpp"

#include <vector>

#include "libsemigroups-exception.hpp"

namespace libsemigroups {
  ////////////////////////////////////////////////////////////////////////
  // FroidurePinBase - settings - public
  ////////////////////////////////////////////////////////////////////////

  FroidurePinBase& FroidurePinBase::batch_size(size_t batch_size) noexcept {
    _settings._batch_size = batch_size;
    return *this;
  }

  size_t FroidurePinBase::batch_size() const noexcept {
    return _settings._batch_size;
  }

  FroidurePinBase& FroidurePinBase::max_threads(size_t nr_threads) noexcept {
    unsigned int n
        = static_cast<unsigned int>(nr_threads == 0 ? 1 : nr_threads);
    _settings._max_threads = std::min(n, std::thread::hardware_concurrency());
    return *this;
  }

  size_t FroidurePinBase::max_threads() const noexcept {
    return _settings._max_threads;
  }

  FroidurePinBase&
  FroidurePinBase::concurrency_threshold(size_t thrshld) noexcept {
    _settings._concurrency_threshold = thrshld;
    return *this;
  }

  size_t FroidurePinBase::concurrency_threshold() const noexcept {
    return _settings._concurrency_threshold;
  }

  FroidurePinBase& FroidurePinBase::immutable(bool val) noexcept {
    _settings._immutable = val;
    return *this;
  }

  bool FroidurePinBase::immutable() const noexcept {
    return _settings._immutable;
  }

  ////////////////////////////////////////////////////////////////////////
  // FroidurePinBase - helper non-member functions
  ////////////////////////////////////////////////////////////////////////

  void relations(FroidurePinBase&                            S,
                 std::function<void(word_type, word_type)>&& hook) {
    S.run();

    std::vector<size_t> relation;  // a triple
    S.reset_next_relation();
    S.next_relation(relation);

    while (relation.size() == 2 && !relation.empty()) {
      hook(word_type({relation[0]}), word_type({relation[1]}));
      S.next_relation(relation);
    }
    word_type lhs, rhs;  // changed in-place by factorisation
    while (!relation.empty()) {
      S.factorisation(lhs, relation[0]);
      S.factorisation(rhs, relation[2]);
      lhs.push_back(relation[1]);
      hook(lhs, rhs);
      S.next_relation(relation);
    }
  }

  void relations(FroidurePinBase& S, std::function<void(word_type)>&& hook) {
    S.run();

    std::vector<size_t> relation;  // a triple
    S.reset_next_relation();
    S.next_relation(relation);

    while (relation.size() == 2 && !relation.empty()) {
      hook(word_type({relation[0]}));
      hook(word_type({relation[1]}));
      S.next_relation(relation);
    }
    word_type word;  // changed in-place by factorisation
    while (!relation.empty()) {
      S.factorisation(word, relation[0]);
      word.push_back(relation[1]);
      hook(word);
      S.factorisation(word, relation[2]);
      hook(word);
      S.next_relation(relation);
    }
  }
}  // namespace libsemigroups

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

#include "libsemigroups/froidure-pin-base.hpp"

#include <algorithm>  // for min, max
#include <cstdint>    // for uint64_t
#include <vector>     // for vector

#include "libsemigroups/constants.hpp"  // for Undefined, Max, UNDEF...
#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/runner.hpp"     // for Runner
#include "libsemigroups/types.hpp"      // for letter_type, word_type

#include "libsemigroups/detail/containers.hpp"  // for DynamicArray2
#include "libsemigroups/detail/report.hpp"      // for REPORT_DEFAULT, Reporter
#include "libsemigroups/detail/string.hpp"      // for group_digits
#include "libsemigroups/word-graph.hpp"

namespace libsemigroups {
  using element_index_type = FroidurePinBase::element_index_type;

  ////////////////////////////////////////////////////////////////////////
  // FroidurePinBase - constructors and destructor - public
  ////////////////////////////////////////////////////////////////////////

  FroidurePinBase::FroidurePinBase()
      : Runner(),
        _degree(),
        _duplicate_gens(),
        _enumerate_order(),
        _final(),
        _first(),
        _found_one(),
        _idempotents_found(),
        _is_idempotent(),
        _left(),
        _length(),
        _lenindex(),
        _letter_to_pos(),
        _nr(),
        _nr_products(),
        _nr_rules(),
        _pos(),
        _pos_one(),
        _prefix(),
        _reduced(),
        _right(),
        _suffix(),
        _wordlen() {
    init();
  }

  FroidurePinBase& FroidurePinBase::init() {
    Runner::init();
    _degree = UNDEFINED;
    _duplicate_gens.clear();
    _enumerate_order.clear();
    _final.clear();
    _first.clear();
    _found_one         = false;
    _idempotents_found = false;
    _is_idempotent.clear();
    _left.init();
    _length.clear();
    _lenindex = {0, 0};
    _letter_to_pos.clear();
    _nr          = 0;
    _nr_products = 0;
    _nr_rules    = 0;
    _pos         = 0;
    _pos_one     = 0;
    _prefix.clear();
    _reduced.clear();
    _right.init();
    _suffix.clear();
    // (length of the current word) - 1
    _wordlen = 0;
    return *this;
  }

  FroidurePinBase::FroidurePinBase(FroidurePinBase const& S)
      : Runner(S),
        _degree(UNDEFINED),  // _degree must be UNDEFINED until !_gens.empty()
        _duplicate_gens(S._duplicate_gens),
        _enumerate_order(S._enumerate_order),
        _final(S._final),
        _first(S._first),
        _found_one(S._found_one),
        _idempotents_found(S._idempotents_found),
        _is_idempotent(S._is_idempotent),
        _left(S._left),
        _length(S._length),
        _lenindex(S._lenindex),
        _letter_to_pos(S._letter_to_pos),
        _nr(S._nr),
        _nr_products(0),  // TODO what's the rationale for this being 0
        _nr_rules(S._nr_rules),
        _pos(S._pos),
        _pos_one(S._pos_one),
        _prefix(S._prefix),
        _reduced(S._reduced),
        _right(S._right),
        _suffix(S._suffix),
        _wordlen(S._wordlen) {}

  FroidurePinBase& FroidurePinBase::operator=(FroidurePinBase const& S) {
    Runner::operator=(S);
    _degree = UNDEFINED;  // _degree must be UNDEFINED until !_gens.empty=)
    _duplicate_gens    = S._duplicate_gens;
    _enumerate_order   = S._enumerate_order;
    _final             = S._final;
    _first             = S._first;
    _found_one         = S._found_one;
    _idempotents_found = S._idempotents_found;
    _is_idempotent     = S._is_idempotent;
    _left              = S._left;
    _length            = S._length;
    _lenindex          = S._lenindex;
    _letter_to_pos     = S._letter_to_pos;
    _nr                = S._nr;
    _nr_products       = 0;  // TODO what's the rationale for this being 0
    _nr_rules          = S._nr_rules;
    _pos               = S._pos;
    _pos_one           = S._pos_one;
    _prefix            = S._prefix;
    _reduced           = S._reduced;
    _right             = S._right;
    _suffix            = S._suffix;
    _wordlen           = S._wordlen;
    return *this;
  }

  FroidurePinBase::~FroidurePinBase() = default;

  ////////////////////////////////////////////////////////////////////////
  // FroidurePinBase - constructors - private
  ////////////////////////////////////////////////////////////////////////

  // Partial copy.
  // \p copy a semigroup
  // \p coll a collection of additional generators
  //
  // This is a constructor for a semigroup generated by the generators of the
  // FroidurePin copy and the (possibly) additional generators coll.
  //
  // The relevant parts of the data structure of copy are copied and
  // \c this will be corrupt unless add_generators or closure is called
  // subsequently. This is why this member function is private.
  //
  // The same effect can be obtained by copying copy using the copy
  // constructor and then calling add_generators or closure. However,
  // this constructor avoids copying those parts of the data structure of
  // copy that add_generators invalidates anyway. If copy has not been
  // enumerated at all, then these two routes for adding more generators are
  // equivalent.
  //
  // <add_generators> or <closure> should usually be called after this.
  void FroidurePinBase::partial_copy(FroidurePinBase const& S) {
    _degree         = S._degree;  // copy for comparison in add_generators
    _duplicate_gens = S._duplicate_gens;
    _found_one      = S._found_one;  // copy in case degree doesn't change in
    // add_generators
    _idempotents_found = S._idempotents_found;
    _is_idempotent     = S._is_idempotent;
    _left              = S._left;
    _lenindex          = {0, S._lenindex[1]};
    _letter_to_pos     = S._letter_to_pos;
    _nr                = S._nr;
    _nr_products       = 0;
    _nr_rules          = 0;
    _pos               = S._pos;
    _pos_one           = S._pos_one;  // copy in case degree doesn't change in
    // add_generators
    _reduced = S._reduced;
    _right   = S._right;
    _wordlen = 0;

    LIBSEMIGROUPS_ASSERT(S._lenindex.size() > 1);

    // the following are required for assignment to specific positions in
    // add_generators
    _final.resize(S._nr, 0);
    _first.resize(S._nr, 0);
    _length.resize(S._nr, 0);
    _prefix.resize(S._nr, 0);
    _suffix.resize(S._nr, 0);

    _enumerate_order.reserve(S._nr);

    // add the distinct old generators to new _enumerate_order
    LIBSEMIGROUPS_ASSERT(S._lenindex.size() > 1);
    for (enumerate_index_type i = 0; i < S._lenindex[1]; i++) {
      _enumerate_order.push_back(S._enumerate_order[i]);
      _final[_enumerate_order[i]]  = S._final[S._enumerate_order[i]];
      _first[_enumerate_order[i]]  = S._first[S._enumerate_order[i]];
      _prefix[_enumerate_order[i]] = UNDEFINED;
      _suffix[_enumerate_order[i]] = UNDEFINED;
      _length[_enumerate_order[i]] = 1;
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // FroidurePinBase - member functions - public
  ////////////////////////////////////////////////////////////////////////

  // TODO no_check version
  element_index_type
  FroidurePinBase::current_position(word_type const& w) const {
    // w is a word in the generators (i.e. a vector of letter_type's)
    if (w.size() == 0) {
      LIBSEMIGROUPS_EXCEPTION("expected a non-empty word, found empty word");
    }
    for (auto x : w) {
      throw_if_generator_index_out_of_range(x);
    }
    element_index_type s = current_position(w[0]);
    return word_graph::follow_path_no_checks(
        current_right_cayley_graph(), s, w.cbegin() + 1, w.cend());
  }

  element_index_type
  FroidurePinBase::product_by_reduction_no_checks(element_index_type i,
                                                  element_index_type j) const {
    if (current_length(i) <= current_length(j)) {
      while (i != UNDEFINED) {
        j = current_left_cayley_graph().target_no_checks(j, _final[i]);
        i = prefix_no_checks(i);
      }
      return j;
    } else {
      while (j != UNDEFINED) {
        i = current_right_cayley_graph().target_no_checks(i, _first[j]);
        j = suffix_no_checks(j);
      }
      return i;
    }
  }

  element_index_type
  FroidurePinBase::product_by_reduction(element_index_type i,
                                        element_index_type j) const {
    throw_if_element_index_out_of_range(i);
    throw_if_element_index_out_of_range(j);
    return product_by_reduction_no_checks(i, j);
  }

  void FroidurePinBase::enumerate(size_t limit) {
    if (finished() || limit <= current_size()) {
      return;
    } else if (LIMIT_MAX - batch_size() > current_size()) {
      limit = std::max(limit, current_size() + batch_size());
    } else {  // batch_size() is very big for some reason
      limit = batch_size();
    }
    report_default("FroidurePin: enumerating until ~{} elements are found\n",
                   detail::group_digits(limit));
    run_until([this, &limit]() -> bool { return current_size() >= limit; });
  }

  size_t FroidurePinBase::number_of_elements_of_length(size_t i) const {
    // _lenindex[i - 1] is the element_index_type where words of length i
    // begin so _lenindex[i] - _lenindex[i - 1]) is the number of words of
    // length i.
    if (i == 0 || i > _lenindex.size()) {
      return 0;
    } else if (i == _lenindex.size()) {
      return current_size() - _lenindex[i - 1];
    }
    return _lenindex[i] - _lenindex[i - 1];
  }

  size_t FroidurePinBase::number_of_elements_of_length(size_t min,
                                                       size_t max) const {
    size_t result = 0;
    for (size_t i = min; i < max; ++i) {
      size_t next = number_of_elements_of_length(i);
      result += next;
      if (i != 0 && next == 0) {
        break;
      }
    }
    return result;
  }

  ////////////////////////////////////////////////////////////////////////
  // FroidurePinBase - settings - public
  ////////////////////////////////////////////////////////////////////////

  // TODO(now) to hpp
  FroidurePinBase& FroidurePinBase::batch_size(size_t batch_size) noexcept {
    _settings._batch_size = batch_size;
    return *this;
  }

  // TODO to hpp
  size_t FroidurePinBase::batch_size() const noexcept {
    return _settings._batch_size;
  }

  // TODO to hpp
  FroidurePinBase&
  FroidurePinBase::max_threads(size_t number_of_threads) noexcept {
    unsigned int n = static_cast<unsigned int>(
        number_of_threads == 0 ? 1 : number_of_threads);
    _settings._max_threads = std::min(n, std::thread::hardware_concurrency());
    return *this;
  }

  // TODO to hpp
  size_t FroidurePinBase::max_threads() const noexcept {
    return _settings._max_threads;
  }

  // TODO to hpp
  FroidurePinBase&
  FroidurePinBase::concurrency_threshold(size_t thrshld) noexcept {
    _settings._concurrency_threshold = thrshld;
    return *this;
  }

  // TODO to hpp
  size_t FroidurePinBase::concurrency_threshold() const noexcept {
    return _settings._concurrency_threshold;
  }

  // TODO to hpp
  FroidurePinBase& FroidurePinBase::immutable(bool val) noexcept {
    _settings._immutable = val;
    return *this;
  }

  // TODO to hpp
  bool FroidurePinBase::immutable() const noexcept {
    return _settings._immutable;
  }

  void FroidurePinBase::throw_if_element_index_out_of_range(
      element_index_type i) const {
    if (i >= _nr) {
      LIBSEMIGROUPS_EXCEPTION(
          "element index out of bounds, expected value in [0, {}), got {}",
          _nr,
          i);
    }
  }

  void FroidurePinBase::throw_if_generator_index_out_of_range(
      generator_index_type i) const {
    if (i >= number_of_generators()) {
      LIBSEMIGROUPS_EXCEPTION(
          "generator index out of bounds, expected value in [0, {}), got {}",
          number_of_generators(),
          i);
    }
  }

  FroidurePinBase::const_rule_iterator::const_rule_iterator(
      FroidurePinBase const* ptr,
      enumerate_index_type   pos,
      generator_index_type   gen)
      : _current(),
        _froidure_pin(ptr),
        _gen(gen),
        _pos(pos),
        _relation({}, {}) {
    ++(*this);
  }

  FroidurePinBase::const_rule_iterator const&
  FroidurePinBase::const_rule_iterator::operator++() noexcept {
    auto const* ptr = _froidure_pin;

    if (_pos == ptr->current_size()) {  // no more relations
      return *this;
    }

    _relation.first.clear();
    _relation.second.clear();

    if (_pos != UNDEFINED) {
      while (_pos < ptr->current_size()) {
        while (_gen < ptr->number_of_generators()) {
          if (!ptr->_reduced.get(ptr->_enumerate_order[_pos], _gen)
              && (_pos < ptr->_lenindex[1]
                  || ptr->_reduced.get(
                      ptr->_suffix[ptr->_enumerate_order[_pos]], _gen))) {
            _current[0] = ptr->_enumerate_order[_pos];
            _current[1] = _gen;
            _current[2] = ptr->_right.target_no_checks(
                ptr->_enumerate_order[_pos], _gen);
            if (_current[2] != UNDEFINED) {
              _gen++;
              return *this;
            }
          }
          _gen++;
        }
        _gen = 0;
        _pos++;
      }
      return *this;
    } else {
      // duplicate generators
      if (_gen < ptr->_duplicate_gens.size()) {
        _current[0] = ptr->_duplicate_gens[_gen].first;
        _current[1] = ptr->_duplicate_gens[_gen].second;
        _current[2] = UNDEFINED;
        _gen++;
        return *this;
      }
      _gen = 0;
      _pos = 0;
      return operator++();
    }
  }

  void FroidurePinBase::const_rule_iterator::populate_relation() const {
    if (_relation.first.empty()) {
      if (_current[2] == UNDEFINED) {
        _relation.first  = word_type({_current[0]});
        _relation.second = word_type({_current[1]});
      } else {
        _froidure_pin->minimal_factorisation(_relation.first, _current[0]);
        _relation.first.push_back(_current[1]);
        _froidure_pin->minimal_factorisation(_relation.second, _current[2]);
      }
    }
  }
}  // namespace libsemigroups

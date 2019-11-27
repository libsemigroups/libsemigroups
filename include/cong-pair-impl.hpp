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

// This file contains implementations of the member functions for
// CongruenceByPairs.

// TODO(later):
//   1. the type of the congruence defined by class CongruenceByPairs could be
//      a template parameter

#ifndef LIBSEMIGROUPS_INCLUDE_CONG_PAIR_IMPL_HPP_
#define LIBSEMIGROUPS_INCLUDE_CONG_PAIR_IMPL_HPP_

#ifdef TEMPLATE
#undef TEMPLATE
#endif

#define TEMPLATE                      \
  template <typename TElementType,    \
            typename TElementHash,    \
            typename TElementEqual,   \
            typename TElementProduct, \
            typename TFroidurePinType>
#define P_CLASS                      \
  CongruenceByPairs<TElementType,    \
                    TElementHash,    \
                    TElementEqual,   \
                    TElementProduct, \
                    TFroidurePinType>

#define VOID TEMPLATE void
#define SIZE_T TEMPLATE size_t
#define CLASS_INDEX_TYPE TEMPLATE CongruenceInterface::class_index_type
#define SHARED_PTR_NON_TRIV_CLASSES \
  TEMPLATE                          \
  std::shared_ptr<CongruenceInterface::non_trivial_classes_type const>

namespace libsemigroups {
  ////////////////////////////////////////////////////////////////////////
  // CongruenceByPairs - constructor - protected
  ////////////////////////////////////////////////////////////////////////

  TEMPLATE
  P_CLASS::CongruenceByPairs(congruence_type type)
      : CongruenceInterface(type),
        _class_lookup(),
        _found_pairs(),
        _init_done(false),
        _lookup(0),
        _map(),
        _map_next(0),
        _next_class(0),
        _nr_non_trivial_classes(UNDEFINED),
        _nr_non_trivial_elemnts(UNDEFINED),
        _pairs_to_mult(),
        _reverse_map(),
        _tmp1(),
        _tmp2() {}

  ////////////////////////////////////////////////////////////////////////
  // CongruenceByPairs - constructor + destructor - public
  ////////////////////////////////////////////////////////////////////////

  TEMPLATE
  P_CLASS::CongruenceByPairs(congruence_type                  type,
                             std::shared_ptr<FroidurePinBase> S) noexcept
      : CongruenceByPairs(type) {
    set_nr_generators(S->nr_generators());
    set_parent_froidure_pin(S);
  }

  // TODO(later) use SFINAE to have another one of these where T is derived from
  // FpSemigroupInterface? If necessary?
  TEMPLATE
  template <typename T>
  P_CLASS::CongruenceByPairs(congruence_type type, T const& S)
      : CongruenceByPairs(type,
                          static_cast<std::shared_ptr<FroidurePinBase>>(
                              std::make_shared<T>(S))) {
    static_assert(std::is_base_of<FroidurePinBase, T>::value,
                  "the template parameter must be a derived class of "
                  "FroidurePinBase");
    static_assert(
        std::is_same<typename T::element_type, element_type>::value,
        "incompatible element_type's, the element_type of the FroidurePin "
        "instance must be the same as the CongruenceByPairs element_type");
    if (type != congruence_type::right
        && std::is_same<typename T::element_type, detail::TCE>::value) {
      LIBSEMIGROUPS_EXCEPTION("Cannot create a left or 2-sided congruence "
                              "over a semigroup of TCE's");
    }
  }

  TEMPLATE
  P_CLASS::~CongruenceByPairs() {
    delete_tmp_storage();
    this->internal_free(_tmp1);
    this->internal_free(_tmp2);
    for (auto& x : _map) {
      this->internal_free(x.first);
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // CongruenceInterface - non-pure virtual member functions - protected
  ////////////////////////////////////////////////////////////////////////

  CLASS_INDEX_TYPE
  P_CLASS::const_word_to_class_index(word_type const& w) const {
    if (!finished()) {
      return UNDEFINED;
    }
    auto   fp    = static_cast<froidure_pin_type*>(parent_froidure_pin().get());
    auto   x     = fp->word_to_element(w);
    size_t ind_x = get_index(this->to_internal(x));
    this->external_free(x);
    LIBSEMIGROUPS_ASSERT(ind_x < _class_lookup.size());
    LIBSEMIGROUPS_ASSERT(_class_lookup.size() == _map.size());
    return _class_lookup[ind_x];
  }

  /////////////////////////////////////////////////////////////////////////
  // CongruenceInterface - pure virtual member functions - private
  /////////////////////////////////////////////////////////////////////////

#pragma GCC diagnostic push
#if (defined(__GNUC__) && !(defined(__clang__) || defined(__INTEL_COMPILER)))
#pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#endif
  WORD_TYPE P_CLASS::class_index_to_word_impl(class_index_type) {
    // TODO(later) actually implement this
    LIBSEMIGROUPS_EXCEPTION("not yet implemented");
  }
#pragma GCC diagnostic pop

  SIZE_T P_CLASS::nr_classes_impl() {
    run();
    return parent_froidure_pin()->size() - _class_lookup.size() + _next_class;
  }

#pragma GCC diagnostic push
#if (defined(__GNUC__) && !(defined(__clang__) || defined(__INTEL_COMPILER)))
#pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#endif
  TEMPLATE
  std::shared_ptr<FroidurePinBase> P_CLASS::quotient_impl() {
    // TODO(later) actually implement this
    LIBSEMIGROUPS_EXCEPTION("not yet implemented");
  }
#pragma GCC diagnostic pop

  VOID P_CLASS::run_impl() {
    detail::Timer t;
    init();

    size_t tid = THREAD_ID_MANAGER.tid(std::this_thread::get_id());
    while (!_pairs_to_mult.empty() && !stopped()) {
      // Get the next pair
      auto& current_pair = _pairs_to_mult.front();

      auto prnt = static_cast<froidure_pin_type*>(parent_froidure_pin().get());
      // Add its left and/or right multiples
      for (size_t i = 0; i < prnt->nr_generators(); i++) {
        const_reference gen = prnt->generator(i);
        if (kind() == congruence_type::left
            || kind() == congruence_type::twosided) {
          Product()(this->to_external(_tmp1),
                    gen,
                    this->to_external_const(current_pair.first),
                    tid);
          Product()(this->to_external(_tmp2),
                    gen,
                    this->to_external_const(current_pair.second),
                    tid);
          internal_add_pair(_tmp1, _tmp2);
        }
        if (kind() == congruence_type::right
            || kind() == congruence_type::twosided) {
          Product()(this->to_external(_tmp1),
                    this->to_external_const(current_pair.first),
                    gen,
                    tid);
          Product()(this->to_external(_tmp2),
                    this->to_external_const(current_pair.second),
                    gen,
                    tid);
          internal_add_pair(_tmp1, _tmp2);
        }
      }
      _pairs_to_mult.pop();
      if (report()) {
        REPORT_DEFAULT(
            "found %d pairs: %d elements in %d classes, %d pairs on the "
            "stack\n",
            _found_pairs.size(),
            _map_next,
            _lookup.nr_blocks(),
            _pairs_to_mult.size())
      }
    }

    if (_pairs_to_mult.empty()) {
      // Make a normalised class lookup (class numbers {0, .., n-1}, in
      // order)
      if (_lookup.get_size() > 0) {
        _class_lookup.reserve(_lookup.get_size());
        _next_class = 1;
        size_t max  = 0;
        LIBSEMIGROUPS_ASSERT(_lookup.find(0) == 0);
        _class_lookup.push_back(0);
        for (size_t i = 1; i < _lookup.get_size(); i++) {
          size_t nr = _lookup.find(i);
          if (nr > max) {
            _class_lookup.push_back(_next_class++);
            max = nr;
          } else {
            _class_lookup.push_back(_class_lookup[nr]);
          }
        }
      }

      // Record information about non-trivial classes
      _nr_non_trivial_classes = _next_class;
      _nr_non_trivial_elemnts = _map_next;
      delete_tmp_storage();
    }

    REPORT_DEFAULT("stopping with %d pairs: %d elements in %d classes\n",
                   _found_pairs.size(),
                   _map_next,
                   _lookup.nr_blocks());
    REPORT_TIME(t);
    report_why_we_stopped();
  }

  BOOL P_CLASS::finished_impl() const {
    return _pairs_to_mult.empty();
  }

  CLASS_INDEX_TYPE P_CLASS::word_to_class_index_impl(word_type const& w) {
    run();
    LIBSEMIGROUPS_ASSERT(finished());
    return const_word_to_class_index(w);
  }

  /////////////////////////////////////////////////////////////////////////
  // CongruenceInterface - non-pure virtual member functions - private
  /////////////////////////////////////////////////////////////////////////

  VOID P_CLASS::add_pair_impl(word_type const& u, word_type const& v) {
    // Can't construct one of these without a parent.
    LIBSEMIGROUPS_ASSERT(has_parent_froidure_pin());

    auto prnt = static_cast<froidure_pin_type*>(parent_froidure_pin().get());
    auto x    = prnt->word_to_element(u);
    auto y    = prnt->word_to_element(v);
    internal_add_pair(this->to_internal(x), this->to_internal(y));
    this->external_free(x);
    this->external_free(y);
  }

  SHARED_PTR_NON_TRIV_CLASSES P_CLASS::non_trivial_classes_impl() {
    run();
    LIBSEMIGROUPS_ASSERT(_reverse_map.size() >= _nr_non_trivial_elemnts);
    LIBSEMIGROUPS_ASSERT(_class_lookup.size() >= _nr_non_trivial_elemnts);
    LIBSEMIGROUPS_ASSERT(has_parent_froidure_pin());

    auto ntc = non_trivial_classes_type(_nr_non_trivial_classes,
                                        std::vector<word_type>());
    auto fp  = static_cast<froidure_pin_type*>(parent_froidure_pin().get());
    for (size_t ind = 0; ind < _nr_non_trivial_elemnts; ++ind) {
      word_type word = fp->factorisation(this->to_external(_reverse_map[ind]));
      ntc[_class_lookup[ind]].push_back(word);
    }
    return std::make_shared<non_trivial_classes_type>(ntc);
  }

  ////////////////////////////////////////////////////////////////////////
  // CongruenceByPairs - member functions - protected
  ////////////////////////////////////////////////////////////////////////

  VOID P_CLASS::internal_add_pair(internal_element_type x,
                                  internal_element_type y) {
    if (!InternalEqualTo()(x, y)) {
      internal_element_type xx, yy;
      bool                  xx_new = false, yy_new = false;
      size_t                i, j;

      auto it_x = _map.find(x);
      if (it_x == _map.end()) {
        xx_new = true;
        xx     = this->internal_copy(x);
        i      = add_index(xx);
      } else {
        i = it_x->second;
      }

      auto it_y = _map.find(y);
      if (it_y == _map.end()) {
        yy_new = true;
        yy     = this->internal_copy(y);
        j      = add_index(yy);
      } else {
        j = it_y->second;
      }

      LIBSEMIGROUPS_ASSERT(i != j);
      std::pair<internal_element_type, internal_element_type> pair;
      if (xx_new || yy_new) {  // it's a new pair
        xx   = internal_element_type(xx_new ? xx : it_x->first);
        yy   = internal_element_type(yy_new ? yy : it_y->first);
        pair = (i < j ? std::make_pair(xx, yy) : std::make_pair(yy, xx));
      } else {
        pair = (i < j ? std::make_pair(internal_element_type(it_x->first),
                                       internal_element_type(it_y->first))
                      : std::make_pair(internal_element_type(it_y->first),
                                       internal_element_type(it_x->first)));
        if (_found_pairs.find(pair) != _found_pairs.end()) {
          return;
        }
      }
      _found_pairs.insert(pair);
      _pairs_to_mult.push(pair);
      _lookup.unite(i, j);
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // CongruenceByPairs - member functions - private
  ////////////////////////////////////////////////////////////////////////

  SIZE_T P_CLASS::add_index(internal_element_type x) const {
    LIBSEMIGROUPS_ASSERT(_reverse_map.size() == _map_next);
    LIBSEMIGROUPS_ASSERT(_map.size() == _map_next);
    _map.emplace(x, _map_next);
    _reverse_map.push_back(x);
    _lookup.add_entry();
    if (finished()) {
      _class_lookup.push_back(_next_class++);
    }
    return _map_next++;
  }

  VOID P_CLASS::delete_tmp_storage() {
    std::unordered_set<std::pair<internal_element_type, internal_element_type>,
                       PHash,
                       PEqual>()
        .swap(_found_pairs);
    std::queue<std::pair<internal_element_type, internal_element_type>>().swap(
        _pairs_to_mult);
  }

  SIZE_T P_CLASS::get_index(internal_element_type x) const {
    auto it = _map.find(x);
    if (it == _map.end()) {
      return add_index(this->internal_copy(x));
    }
    return it->second;
  }

  VOID P_CLASS::init() {
    if (!_init_done) {
      LIBSEMIGROUPS_ASSERT(has_parent_froidure_pin());
      LIBSEMIGROUPS_ASSERT(parent_froidure_pin()->nr_generators() > 0);
      auto fp = static_cast<froidure_pin_type*>(parent_froidure_pin().get());
      _tmp1   = this->internal_copy(this->to_internal_const(fp->generator(0)));
      _tmp2   = this->internal_copy(_tmp1);
      _init_done = true;
    }
  }
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_CONG_PAIR_IMPL_HPP_

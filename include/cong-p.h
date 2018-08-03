//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

// This file contains a method for enumerating a congruence by attempting to
// find all pairs of related elements using brute force. This does not work
// very well in most cases, due to the high complexity of the approach. Note
// that currently it is only used with Semigroup<RWSE*>, and so doesn't
// strictly have to be a class template.

#ifndef LIBSEMIGROUPS_INCLUDE_CONG_P_H_
#define LIBSEMIGROUPS_INCLUDE_CONG_P_H_

#include <queue>

#include "internal/uf.h"

#include "cong-intf.h"
#include "kbe.h"
#include "semigroup.h"

namespace libsemigroups {
  namespace congruence {
    // This version of P originates in the old cong.h file.
    template <
        typename TElementType  = Element const*,
        typename TElementHash  = libsemigroups::hash<TElementType>,
        typename TElementEqual = libsemigroups::equal_to<TElementType>,
        class TTraits
        = SemigroupTraitsHashEqual<TElementType, TElementHash, TElementEqual>>
    class P : public CongIntf, private TTraits {
      ////////////////////////////////////////////////////////////////////////
      // P - typedefs - private
      ////////////////////////////////////////////////////////////////////////
      // TODO check if the following are all actually used
      using element_type       = typename TTraits::element_type;
      using const_element_type = typename TTraits::const_element_type;
      using reference          = typename TTraits::reference;
      using const_reference    = typename TTraits::const_reference;

      using internal_element_type = typename TTraits::internal_element_type;
      using internal_const_element_type =
          typename TTraits::internal_const_element_type;
      using internal_reference = typename TTraits::internal_reference;
      using internal_const_reference =
          typename TTraits::internal_const_reference;

      using internal_equal_to = typename TTraits::internal_equal_to;
      using internal_hash     = typename TTraits::internal_hash;

      using product = ::libsemigroups::product<internal_element_type>;

      using semigroup_type
          = Semigroup<TElementType, TElementHash, TElementEqual, TTraits>;

     public:
      ////////////////////////////////////////////////////////////////////////
      // P - constructor + destructor - public
      ////////////////////////////////////////////////////////////////////////

      P(congruence_type type, SemigroupBase* S)
          : CongIntf(type),
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
            _tmp2() {
        LIBSEMIGROUPS_ASSERT(S != nullptr);
        auto parent = static_cast<semigroup_type*>(S);
        set_parent(parent);
      }

      P(congruence_type type, SemigroupBase& S) : P(type, &S) {}

      ~P() {
        delete_tmp_storage();
        this->internal_free(_tmp1);
        this->internal_free(_tmp2);
        for (auto& x : _map) {
          this->internal_free(const_cast<internal_element_type>(x.first));
        }
      }

      ////////////////////////////////////////////////////////////////////////
      // Runner - overridden pure virtual methods - public
      ////////////////////////////////////////////////////////////////////////

      void run() override {
        if (finished()) {
          return;
        }
        LIBSEMIGROUPS_ASSERT(has_parent());

        Timer t;
        init();

        size_t tid = REPORTER.thread_id(std::this_thread::get_id());
        while (!_pairs_to_mult.empty() && !dead() && !timed_out()) {
          // Get the next pair
          auto& current_pair = _pairs_to_mult.front();
          _pairs_to_mult.pop();
          // TODO can the previous be auto&, or does the pop make this UB?

          auto prnt = static_cast<semigroup_type*>(parent());
          // Add its left and/or right multiples
          for (size_t i = 0; i < prnt->nrgens(); i++) {
            const_reference gen = prnt->generator(i);
            if (type() == congruence_type::LEFT
                || type() == congruence_type::TWOSIDED) {
              product()(
                  _tmp1, this->to_internal_const(gen), current_pair.first, tid);
              product()(_tmp2,
                        this->to_internal_const(gen),
                        current_pair.second,
                        tid);
              internal_add_pair(_tmp1, _tmp2);
            }
            if (type() == congruence_type::RIGHT
                || type() == congruence_type::TWOSIDED) {
              product()(
                  _tmp1, current_pair.first, this->to_internal_const(gen), tid);
              product()(_tmp2,
                        current_pair.second,
                        this->to_internal_const(gen),
                        tid);
              internal_add_pair(_tmp1, _tmp2);
            }
          }
          if (report()) {
            REPORT("found " << _found_pairs.size() << " pairs: " << _map_next
                            << " elements in " << _lookup.nr_blocks()
                            << " classes, " << _pairs_to_mult.size()
                            << " pairs on the stack");
          }
        }

        if (!dead() && !timed_out()) {
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
        }

        REPORT("stopping with " << _found_pairs.size()
                                << " pairs: " << _map_next << " elements in "
                                << _lookup.nr_blocks() << " classes");
        REPORT("elapsed time = " << t);
        if (dead()) {
          REPORT("killed");
        } else if (timed_out()) {
          REPORT("timed out!");
        } else {
          set_finished();
          delete_tmp_storage();
          REPORT("finished!");
        }
      }

      ////////////////////////////////////////////////////////////////////////
      // CongIntf - overridden pure virtual methods - public
      ////////////////////////////////////////////////////////////////////////

      void add_pair(word_type l, word_type r) override {
        reference x
            = static_cast<semigroup_type*>(parent())->word_to_element(l);
        reference y
            = static_cast<semigroup_type*>(parent())->word_to_element(r);
        internal_add_pair(this->to_internal(x), this->to_internal(y));
        this->internal_free(x);
        this->internal_free(y);
        unset_finished();
      }

      word_type class_index_to_word(class_index_type) override {
        // TODO implement
        throw LibsemigroupsException("not yet implemented");
      }

      SemigroupBase* quotient_semigroup() override {
        // FIXME actually implement this
        throw LibsemigroupsException("not yet implemented");
      }

      size_t nr_classes() override {
        run();
        return parent()->size() - _class_lookup.size() + _next_class;
      }

      class_index_type word_to_class_index(word_type const& w) final {
        run();
        LIBSEMIGROUPS_ASSERT(finished());
        auto x = this->to_internal(
            static_cast<semigroup_type*>(parent())->word_to_element(w));
        size_t ind_x = get_index(x);
        this->internal_free(x);
        LIBSEMIGROUPS_ASSERT(ind_x < _class_lookup.size());
        LIBSEMIGROUPS_ASSERT(_class_lookup.size() == _map.size());
        return _class_lookup[ind_x];
      }

     private:
      ////////////////////////////////////////////////////////////////////////
      // CongIntf - overridden non-pure virtual methods - private
      ////////////////////////////////////////////////////////////////////////

      void init_non_trivial_classes() override {
        run();
        LIBSEMIGROUPS_ASSERT(_reverse_map.size() >= _nr_non_trivial_elemnts);
        LIBSEMIGROUPS_ASSERT(_class_lookup.size() >= _nr_non_trivial_elemnts);
        LIBSEMIGROUPS_ASSERT(has_parent());

        _non_trivial_classes.assign(_nr_non_trivial_classes,
                                    std::vector<word_type>());
        for (size_t ind = 0; ind < _nr_non_trivial_elemnts; ++ind) {
          word_type word
              = static_cast<semigroup_type*>(parent())->factorisation(
                  _reverse_map[ind]);
          _non_trivial_classes[_class_lookup[ind]].push_back(word);
        }
      }

      ////////////////////////////////////////////////////////////////////////
      // P - methods - private
      ////////////////////////////////////////////////////////////////////////

      size_t add_index(internal_element_type x) {
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

      void delete_tmp_storage() {
        std::unordered_set<
            std::pair<internal_element_type, internal_element_type>,
            PHash,
            PEqual>()
            .swap(_found_pairs);
        std::queue<std::pair<internal_element_type, internal_element_type>>()
            .swap(_pairs_to_mult);
      }

      size_t get_index(internal_const_element_type x) {
        auto it = _map.find(x);
        if (it == _map.end()) {
          return add_index(this->internal_copy(x));
        }
        return it->second;
      }

      void init() {
        if (!_init_done) {
          LIBSEMIGROUPS_ASSERT(has_parent());
          LIBSEMIGROUPS_ASSERT(parent()->nrgens() > 0);
          _tmp1      = this->internal_copy(this->to_internal_const(
              static_cast<semigroup_type*>(parent())->generator(0)));
          _tmp2      = this->internal_copy(_tmp1);
          _init_done = true;
        }
      }

      void internal_add_pair(internal_const_element_type x,
                             internal_const_element_type y) {
        if (!internal_equal_to()(x, y)) {
          internal_element_type xx, yy;
          bool                        xx_new = false, yy_new = false;
          size_t                      i, j;

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
          std::pair<internal_element_type, internal_element_type>
              pair;
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
      // TODO: revise from here
      ////////////////////////////////////////////////////////////////////////

      struct PHash {
       public:
        size_t
        operator()(std::pair<internal_const_element_type,
                             internal_const_element_type> const& pair) const {
          return internal_hash()(pair.first)
                 + 17 * internal_hash()(pair.second);
        }
      };

      struct PEqual {
        size_t operator()(std::pair<internal_const_element_type,
                                    internal_const_element_type> pair1,
                          std::pair<internal_const_element_type,
                                    internal_const_element_type> pair2) const {
          return internal_equal_to()(pair1.first, pair2.first)
                 && internal_equal_to()(pair1.second, pair2.second);
        }
      };

      ////////////////////////////////////////////////////////////////////////
      // P - data - private
      ////////////////////////////////////////////////////////////////////////

      std::vector<class_index_type> _class_lookup;
      std::unordered_set<
          std::pair<internal_element_type, internal_element_type>,
          PHash,
          PEqual>
           _found_pairs;
      bool _init_done;
      UF   _lookup;
      std::unordered_map<internal_const_element_type,
                         size_t,
                         internal_hash,
                         internal_equal_to>
                       _map;
      size_t           _map_next;
      class_index_type _next_class;
      size_t           _nr_non_trivial_classes;
      size_t           _nr_non_trivial_elemnts;
      std::queue<std::pair<internal_element_type, internal_element_type>>
                                         _pairs_to_mult;
      std::vector<internal_element_type> _reverse_map;
      KnuthBendix*                       _kb;
      internal_element_type              _tmp1;
      internal_element_type              _tmp2;
    };
  }  // namespace congruence
}  // namespace libsemigroups

// The following originates in the stable-1.0 branch
/*
namespace libsemigroups {
  namespace congruence {
    using congruence_t = Interface::congruence_t;
    using RWS          = fpsemigrouRWS;

    // Forward declaration
    template <typename TElementType,
              typename TElementHash,
              typename TElementEqual>
    class PE;

      // FIXME it'd be better to just not have the next constructor in the case
      // that TElementType != RWSE*.
      P(congruence_t type, RWS* rws, std::vector<relation_t> extra = {})
          : P(type, static_cast<Semigroup<TElementType>*>(nullptr), extra) {
        if (!std::is_same<TElementType, RWSE*>::value) {
          throw std::runtime_error(
              "Something has gone wrong, this only works with RWSE*");
        }
        _rws = rws;
      }

      // TODO contains method (so that we can rewrite words using the rws, and
      // if they are the same, then they represent the same class, i.e. we don't
      // have to worry about running this at all.


      void run() override {
        if (finished()) {
          return;
        }

        Timer t;
        init();

        size_t tid = glob_reporter.thread_id(std::this_thread::get_id());
        while (!_pairs_to_mult.empty() && !dead() && !timed_out()) {
          // Get the next pair
          std::pair<TElementType, TElementType> current_pair
              = _pairs_to_mult.front();

          _pairs_to_mult.pop();

          // Add its left and/or right multiples
          for (size_t i = 0; i < _semigroup->nrgens(); i++) {
            TElementType gen = _semigroup->gens(i);
            if (type() == LEFT || type() == TWOSIDED) {
              _tmp1 = this->multiply(_tmp1, gen, current_pair.first, tid);
              _tmp2 = this->multiply(_tmp2, gen, current_pair.second, tid);
              internal_add_pair(_tmp1, _tmp2);
            }
            if (type() == RIGHT || type() == TWOSIDED) {
              _tmp1 = this->multiply(_tmp1, current_pair.first, gen, tid);
              _tmp2 = this->multiply(_tmp2, current_pair.second, gen, tid);
              internal_add_pair(_tmp1, _tmp2);
            }
          }
          if (report()) {
            REPORT("found " << _found_pairs.size() << " pairs: " << _map_next
                            << " elements in " << _lookup.nr_blocks()
                            << " classes, " << _pairs_to_mult.size()
                            << " pairs on the stack");
          }
        }

        if (!dead() && !timed_out()) {
          // Make a normalised class lookup (class numbers {0, .., n-1}, in
          // order)
          if (_lookup.get_size() > 0) {
            _class_lookup.reserve(_lookup.get_size());
            _next_class = 1;
            size_t nr;
            size_t max = 0;
            LIBSEMIGROUPS_ASSERT(_lookup.find(0) == 0);
            _class_lookup.push_back(0);
            for (size_t i = 1; i < _lookup.get_size(); i++) {
              nr = _lookup.find(i);
              if (nr > max) {
                _class_lookup.push_back(_next_class++);
                max = nr;
              } else {
                _class_lookup.push_back(_class_lookup[nr]);
              }
            }
          }

          // Record information about non-trivial classes
          _nr_nontrivial_classes = _next_class;
          _nr_nontrivial_elms    = _map_next;
        }

        REPORT("stopping with " << _found_pairs.size()
                                << " pairs: " << _map_next << " elements in "
                                << _lookup.nr_blocks() << " classes");
        REPORT("elapsed time = " << t);
        if (dead()) {
          REPORT("killed");
        } else if (timed_out()) {
          REPORT("timed out!");
        } else {
          set_finished();
          delete_tmp_storage();
          REPORT("finished!");
        }
      }

      SemigroupBase* quotient_semigroup() override {
        // FIXME actually implement this
        throw std::runtime_error("not yet implemented");
      }

     private:
      struct PHash {
       public:
        size_t
        operator()(std::pair<TElementType, TElementType> const& pair) const {
          return TElementHash()(pair.first) + 17 * TElementHash()(pair.second);
        }
      };

      struct PEqual {
        size_t operator()(std::pair<TElementType, TElementType> pair1,
                          std::pair<TElementType, TElementType> pair2) const {
          return TElementEqual()(pair1.first, pair2.first)
                 && TElementEqual()(pair1.second, pair2.second);
        }
      };

      class_index_t const_word_to_class_index(word_t const& w) const override {
        if (!_init_done) {
          return UNDEFINED;
        }
        TElementType x  = _semigroup->word_to_element(w);
        auto         it = _map.find(x);
        this->free(x);
        if (it == _map.end()) {
          return UNDEFINED;
        }
        LIBSEMIGROUPS_ASSERT(it->second < _class_lookup.size());
        LIBSEMIGROUPS_ASSERT(_class_lookup.size() == _map.size());
        return _class_lookup[it->second];
      }


      size_t get_index(TElementType x) {
        auto it = _map.find(x);
        if (it == _map.end()) {
          return add_index(this->copy(x));
        }
        return it->second;
      }



      std::vector<class_index_t> _class_lookup;
      std::unordered_set<std::pair<TElementType, TElementType>, PHash, PEqual>
                                                        _found_pairs;
      std::vector<relation_t>                           _genpairs;
      bool                                              _init_done;
      UF                                                _lookup;
      std::unordered_map<TElementType, size_t>          _map;
      size_t                                            _map_next;
      class_index_t                                     _next_class;
      std::vector<std::vector<word_t>>                  _non_trivial_classes;
      size_t                                            _nr_nontrivial_classes;
      size_t                                            _nr_nontrivial_elms;
      std::queue<std::pair<TElementType, TElementType>> _pairs_to_mult;
      std::vector<TElementType>                         _reverse_map;
      RWS*                                              _rws;
      Semigroup<TElementType, TElementHash, TElementEqual>* _semigroup;
      TElementType                                          _tmp1;
      TElementType                                          _tmp2;
      congruence_t                                          _type;
    };

    ////////////////////////////////////////////////////////////////////////////
    // Class for elements of the quotient semigroup
    ////////////////////////////////////////////////////////////////////////////

    // FIXME the below is not finished!

    template <typename TElementType,
              typename TElementHash,
              typename TElementEqual>
    class PE {
     public:
      PE() = default;
      PE(P<TElementType, TElementHash, TElementEqual>* p, class_index_t i)
          : _p(p), _index(i) {}

      ~PE() = default;

      bool operator==(PE const& that) const {
        return _index == that._index;
      }

      bool operator<(PE const& that) const {
        return _index < that._index;
      }

      // Only works when that is a generator!!
      inline PE operator*(PE const& that) const {
        LIBSEMIGROUPS_ASSERT(that._index <= _p->nr_generators());
        return PE(_p, _p->right(_index, that._index - 1));
      }

      inline PE one() const {
        return PE(_p, 0);
      }

      class_index_t class_index() const {
        return _index;
      }

     private:
      P<TElementType, TElementHash, TElementEqual>* _p;
      class_index_t                                 _index;
    };
  }  // namespace congruence

  template <>
  size_t ElementContainer<PE<RWSE*, std::hash<RWSE*>, std::equal_to<RWSE*>>>::
      complexity(PE<RWSE*, std::hash<RWSE*>, std::equal_to<RWSE*>>) const {
    return LIMIT_MAX;
    }
}  // namespace libsemigroups

namespace std {
  using libsemigroups::RWSE;
  template <>
  struct hash<
      libsemigroups::PE<RWSE*, std::hash<RWSE*>, std::equal_to<RWSE*>>> {
    size_t operator()(
        libsemigroups::PE<RWSE*, std::hash<RWSE*>, std::equal_to<RWSE*>> const&
            x) const {
      return x.class_index();
    }
  };
}  // namespace std */
// From v0.6.3
//      result_t current_equals(word_type const& w1, word_type const& w2) final
//      {
//        if (is_done()) {
//          return word_to_class_index(w1) == word_to_class_index(w2) ? TRUE
//                                                                    : FALSE;
//        }
//        auto semigroup
//            = static_cast<Semigroup<TElementType>*>(_cong._semigroup);
//        element_type x     = semigroup->word_to_element(w1);
//        element_type y     = semigroup->word_to_element(w2);
//        size_t       ind_x = get_index(this->to_internal(x));
//        size_t       ind_y = get_index(this->to_internal(y));
//        this->external_free(x);
//        this->external_free(y);
//        return _lookup.find(ind_x) == _lookup.find(ind_y) ? TRUE : UNKNOWN;
//      }
#endif  // LIBSEMIGROUPS_INCLUDE_CONG_P_H_

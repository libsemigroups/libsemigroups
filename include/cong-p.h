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

#ifndef LIBSEMIGROUPS_SRC_CONG_P_H_
#define LIBSEMIGROUPS_SRC_CONG_P_H_

#include <exception>
#include <queue>

#include "cong-intf.h"
#include "eltcont.h"
#include "rwse.h"
#include "semigroups.h"
#include "uf.h"

// TODO - organise this file
//      - add max_pairs
/*
namespace libsemigroups {
  namespace congruence {
    using congruence_t = Interface::congruence_t;
    using RWS          = fpsemigroup::RWS;

    // Forward declaration
    template <typename TElementType,
              typename TElementHash,
              typename TElementEqual>
    class PE;

    template <typename TElementType  = Element*,
              typename TElementHash  = std::hash<TElementType>,
              typename TElementEqual = std::equal_to<TElementType>>
    class P : public Interface,
              public ElementContainer<TElementType> {
     public:
      P(congruence_t                                          type,
        Semigroup<TElementType, TElementHash, TElementEqual>* S,
        std::vector<relation_t>                               extra = {})
          : Interface(type),
            _class_lookup(),
            _found_pairs(),
            _genpairs(extra),
            _init_done(false),
            _lookup(0),
            _map(),
            _map_next(0),
            _next_class(0),
            _non_trivial_classes(),
            _pairs_to_mult(),
            _reverse_map(),
            _rws(nullptr),
            _semigroup(S),
            _tmp1(nullptr),
            _tmp2(nullptr),
            _type(type) {}

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

      congruence_t type() const override {
        return _type;
      }

      void add_pair(word_t l, word_t r) override {
        TElementType x = _semigroup->word_to_element(l);
        TElementType y = _semigroup->word_to_element(r);
        internal_add_pair(x, y);
        this->free(x);
        this->free(y);
        unset_finished();
      }

      void delete_tmp_storage() {
        std::unordered_set<std::pair<TElementType, TElementType>,
                           PHash,
                           PEqual>()
            .swap(_found_pairs);
        std::queue<std::pair<TElementType, TElementType>>().swap(
            _pairs_to_mult);
      }

      ~P() {
        delete_tmp_storage();
        if (_tmp1 != nullptr) {
          this->free(_tmp1);
        }
        if (_tmp2 != nullptr) {
          this->free(_tmp2);
        }
        for (auto& x : _map) {
          this->free(x.first);
        }
      }

      size_t nr_classes() final {
        if (!finished()) {
          run();
        }
        return _semigroup->size() - _class_lookup.size() + _next_class;
      }

      class_index_t word_to_class_index(word_t const& w) final {
        run();
        LIBSEMIGROUPS_ASSERT(finished());
        TElementType x     = _semigroup->word_to_element(w);
        size_t       ind_x = get_index(x);
        this->free(x);
        LIBSEMIGROUPS_ASSERT(ind_x < _class_lookup.size());
        LIBSEMIGROUPS_ASSERT(_class_lookup.size() == _map.size());
        return _class_lookup[ind_x];
      }

      // TODO class_index_t class_index_to_word(class_index_t i) {
      //}

      // TODO contains method (so that we can rewrite words using the rws, and
      // if they are the same, then they represent the same class, i.e. we don't
      // have to worry about running this at all.

      void init() {
        if (!_init_done) {
          if (_semigroup == nullptr) {
            // FIXME it'd be better to just not have the next constructor in the
            // case that TElementType != RWSE*.
            if (!std::is_same<TElementType, RWSE*>::value) {
              throw std::runtime_error(
                  "Something has gone wrong, this only works with RWSE*");
            }
            LIBSEMIGROUPS_ASSERT(_rws != nullptr);
            _semigroup = static_cast<Semigroup<TElementType>*>(
                _rws->isomorphic_non_fp_semigroup());
          }
          LIBSEMIGROUPS_ASSERT(_semigroup->nrgens() > 0);
          _tmp1 = this->copy(_semigroup->gens(0));
          _tmp2 = this->copy(_tmp1);
          for (relation_t const& rel : _genpairs) {
            add_pair(rel.first, rel.second);
          }
          _genpairs.clear();
          _init_done = true;
        }
      }

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

      std::vector<std::vector<word_t>>::const_iterator
      cbegin_non_trivial_classes() override {
        init_non_trivial_classes();
        return _non_trivial_classes.cbegin();
      }

      std::vector<std::vector<word_t>>::const_iterator
      cend_non_trivial_classes() override {
        init_non_trivial_classes();
        return _non_trivial_classes.cend();
      }

      size_t nr_non_trivial_classes() override {
        run();
        return _nr_nontrivial_classes;
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

      void internal_add_pair(TElementType x, TElementType y) {
        if (!TElementEqual()(x, y)) {
          TElementType xx, yy;
          bool         xx_new = false, yy_new = false;
          size_t       i, j;

          auto it_x = _map.find(x);
          if (it_x == _map.end()) {
            xx_new = true;
            xx     = this->copy(x);
            i      = add_index(xx);
          } else {
            i = it_x->second;
          }

          auto it_y = _map.find(y);
          if (it_y == _map.end()) {
            yy_new = true;
            yy     = this->copy(y);
            j      = add_index(yy);
          } else {
            j = it_y->second;
          }

          LIBSEMIGROUPS_ASSERT(i != j);
          std::pair<TElementType, TElementType> pair;
          if (xx_new || yy_new) {  // it's a new pair
            xx   = (xx_new ? xx : it_x->first);
            yy   = (yy_new ? yy : it_y->first);
            pair = (i < j ? std::pair<TElementType, TElementType>(xx, yy)
                          : std::pair<TElementType, TElementType>(yy, xx));
          } else {
            pair = (i < j ? std::pair<TElementType, TElementType>(it_x->first,
                                                                  it_y->first)
                          : std::pair<TElementType, TElementType>(it_y->first,
                                                                  it_x->first));
            if (_found_pairs.find(pair) != _found_pairs.end()) {
              return;
            }
          }
          _found_pairs.insert(pair);
          _pairs_to_mult.push(pair);
          _lookup.unite(i, j);
        }
      }

      size_t get_index(TElementType x) {
        auto it = _map.find(x);
        if (it == _map.end()) {
          return add_index(this->copy(x));
        }
        return it->second;
      }

      size_t add_index(TElementType x) {
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

      void init_non_trivial_classes() {
        run();
        LIBSEMIGROUPS_ASSERT(_reverse_map.size() >= _nr_nontrivial_elms);
        LIBSEMIGROUPS_ASSERT(_class_lookup.size() >= _nr_nontrivial_elms);

        _non_trivial_classes.assign(_nr_nontrivial_classes,
                                    std::vector<word_t>());
        word_t word;
        for (size_t ind = 0; ind < _nr_nontrivial_elms; ++ind) {
          _semigroup->factorisation(word, _reverse_map[ind]);
          _non_trivial_classes[_class_lookup[ind]].push_back(word);
        }
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
#endif  // LIBSEMIGROUPS_SRC_CONG_P_H_

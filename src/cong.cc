//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2016 James D. Mitchell
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

#include "cong.h"

#include <algorithm>
#include <thread>

#include "cong/kbfp.cc"
#include "cong/kbp.cc"
#include "cong/p.cc"
#include "cong/tc.cc"

namespace libsemigroups {

  // Define static data members
  size_t const Congruence::INFTY     = std::numeric_limits<size_t>::max();
  size_t const Congruence::UNDEFINED = std::numeric_limits<size_t>::max();

  // Get the type from a string
  Congruence::cong_t Congruence::type_from_string(std::string type) {
    if (type == "left") {
      return cong_t::LEFT;
    } else if (type == "right") {
      return cong_t::RIGHT;
    } else {
      LIBSEMIGROUPS_ASSERT(type == "twosided");
      return cong_t::TWOSIDED;
    }
  }

  Congruence::Congruence(cong_t                         type,
                         size_t                         nrgens,
                         std::vector<relation_t> const& relations,
                         std::vector<relation_t> const& extra)
      : _data(nullptr),
        _extra(extra),
        _max_threads(std::thread::hardware_concurrency()),
        _nrgens(nrgens),
        _prefill(),
        _relations(relations),
        _relations_done(false),
        _semigroup(nullptr),
        _type(type) {
    // TODO(JDM): check that the entries in extra/relations are properly defined
    // i.e. that every entry is at most nrgens - 1
  }

  Congruence::Congruence(std::string                    type,
                         size_t                         nrgens,
                         std::vector<relation_t> const& relations,
                         std::vector<relation_t> const& extra)
      : Congruence(type_from_string(type), nrgens, relations, extra) {}

  Congruence::Congruence(cong_t                         type,
                         Semigroup<>*                   semigroup,
                         std::vector<relation_t> const& genpairs)
      : Congruence(type,
                   semigroup->nrgens(),
                   std::vector<relation_t>(),
                   std::vector<relation_t>()) {
    _semigroup = semigroup;
    _extra     = genpairs;  // it is essential that this is set here!
  }

  Congruence::Congruence(std::string                    type,
                         Semigroup<>*                   semigroup,
                         std::vector<relation_t> const& extra)
      : Congruence(type_from_string(type), semigroup, extra) {}

  Congruence::DATA* Congruence::winning_data(
      std::vector<Congruence::DATA*>&                      data,
      std::vector<std::function<void(Congruence::DATA*)>>& funcs,
      bool                                                 ignore_max_threads,
      std::function<bool(Congruence::DATA*)>               goal_func) {
    std::vector<std::thread::id> tids(data.size(), std::this_thread::get_id());

    auto go = [this, &data, &funcs, &tids, &goal_func](size_t pos) {
      tids[pos] = std::this_thread::get_id();
      if (pos < funcs.size()) {
        funcs.at(pos)(data.at(pos));
      }
      try {
        data.at(pos)->run_until(goal_func);
      } catch (std::bad_alloc const& e) {
        REPORT("allocation failed: " << e.what())
        return;
      }
      _kill_mtx.lock();  // stop two DATA objects from killing each other
      if (!data.at(pos)->is_killed()) {
        for (auto it = data.begin(); it < data.begin() + pos; it++) {
          (*it)->kill();
        }
        for (auto it = data.begin() + pos + 1; it < data.end(); it++) {
          (*it)->kill();
        }
      }
      _kill_mtx.unlock();
    };

    size_t nr_threads;
    if (ignore_max_threads) {
      nr_threads = data.size();
    } else {
      nr_threads = std::min(data.size(), _max_threads);
    }

    REPORT("using " << nr_threads << " / "
                    << std::thread::hardware_concurrency()
                    << " threads");
    glob_reporter.reset_thread_ids();

    std::vector<std::thread> t;
    for (size_t i = 0; i < nr_threads; i++) {
      data.at(i)->unkill();
    }
    for (size_t i = 0; i < nr_threads; i++) {
      t.push_back(std::thread(go, i));
    }
    for (size_t i = 0; i < nr_threads; i++) {
      t.at(i).join();
    }
    for (auto winner = data.begin(); winner < data.end(); winner++) {
      if ((*winner)->is_done() || !(*winner)->is_killed()) {
        size_t tid = glob_reporter.thread_id(tids.at(winner - data.begin()));
        REPORT("Thread #" << tid << " is the winner!");
        if ((*winner)->is_done()) {
          // Delete the losers and clear _partial_data
          for (auto loser = data.begin(); loser < winner; loser++) {
            delete *loser;
          }
          for (auto loser = winner + 1; loser < data.end(); loser++) {
            delete *loser;
          }
          _partial_data.clear();
        } else {
          // Store all the data in _partial_data
          if (_partial_data.empty()) {
            _partial_data = data;
          }
          LIBSEMIGROUPS_ASSERT(_partial_data == data);
        }
        return *winner;
      }
    }
    REPORT("allocation failed in every thread, aborting!")
    std::abort();
  }

  // Non-const
  // @goal_func a function which returns true when it is time to stop running
  //            (or nullptr to run to completion)
  //
  // This function returns a pointer to a DATA object, which will be run either
  // to completion, or until **goal_func** is satisfied.  It will be created if
  // it does not already exist.
  Congruence::DATA* Congruence::get_data(std::function<bool(DATA*)> goal_func) {
    if (_data != nullptr) {
      _data->run_until(goal_func);
      return _data;
    }

    Timer timer;
    DATA* winner;
    if (!_partial_data.empty()) {
      // Continue the already-existing data objects
      std::vector<std::function<void(DATA*)>> funcs = {};
      winner = winning_data(_partial_data, funcs, true, goal_func);
    } else if (_semigroup != nullptr
               && (_max_threads == 1
                   || (_semigroup->is_done() && _semigroup->size() < 1024))) {
      REPORT("semigroup is small, not using multiple threads")
      winner = new TC(*this);
      static_cast<TC*>(winner)->prefill();
      winner->run();
      LIBSEMIGROUPS_ASSERT(winner->is_done());
    } else {
      if (_semigroup != nullptr) {
        auto prefillit = [this](Congruence::DATA* data) {
          static_cast<TC*>(data)->prefill();
        };

        std::vector<DATA*> data = {new TC(*this), new TC(*this)};
        std::vector<std::function<void(DATA*)>> funcs = {prefillit};
        // The next 4 lines are commented out because they do not seem to
        // improve the performance at present.
        /*if (_type == TWOSIDED) {
          data.push_back(new KBFP(*this));
        }
        data.push_back(new P(*this));*/
        winner = winning_data(data, funcs);
      } else if (!_prefill.empty()) {
        winner = new TC(*this);
        static_cast<TC*>(winner)->prefill(_prefill);
        winner->run();
        LIBSEMIGROUPS_ASSERT(winner->is_done());
      } else {  // Congruence is defined over an fp semigroup
        std::vector<DATA*>                      data  = {new KBP(*this)};
        std::vector<std::function<void(DATA*)>> funcs = {};
        if (_type == TWOSIDED) {
          data.push_back(new KBFP(*this));
        }
        // TC will be invalid/useless in certain cases; we check these here.
        if (!is_obviously_infinite()) {
          data.push_back(new TC(*this));
        }
        winner = winning_data(data, funcs, true, goal_func);
      }
    }
    REPORT("elapsed time = " << timer);
    if (winner->is_done()) {
      _data = winner;
    }
    return winner;
  }

  void Congruence::delete_data() {
    if (_data != nullptr) {
      delete _data;
    }
    if (!_partial_data.empty()) {
      for (size_t i = 0; i < _partial_data.size(); i++) {
        delete _partial_data.at(i);
      }
      _partial_data.clear();
    }
  }

  // We apply some simple, quick checks that may establish that the congruence
  // has an infinite number of classes; if so, Todd-Coxeter should not be run.
  // If this returns false, it is safe to run Todd-Coxeter, although of course
  // Todd-Coxeter may still run forever.
  bool Congruence::is_obviously_infinite() {
    // If we have a concrete semigroup, it is probably finite, or if it is not,
    // then we will never get any answers out of anything here.
    if (_semigroup != nullptr) {
      return false;
    }

    // If there are no relations, or more generators than relations, it must
    // be infinite
    if (_nrgens > _relations.size() + _extra.size()) {
      return true;
    }

    // Does there exist a generator which appears in no relation?
    for (size_t gen = 0; gen < _nrgens; gen++) {
      bool found = false;
      for (relation_t const& rel : _relations) {
        if (std::find(rel.first.cbegin(), rel.first.cend(), gen)
                != rel.first.cend()
            || std::find(rel.second.cbegin(), rel.second.cend(), gen)
                   != rel.second.cend()) {
          found = true;
          break;
        }
      }
      if (!found) {
        for (relation_t const& rel : _extra) {
          if (std::find(rel.first.cbegin(), rel.first.cend(), gen)
                  != rel.first.cend()
              || std::find(rel.second.cbegin(), rel.second.cend(), gen)
                     != rel.second.cend()) {
            found = true;
            break;
          }
        }
      }
      if (!found) {
        return true;  // we found a generator not in any relation.
      }
    }

    // Otherwise, it may be infinite or finite (undecidable in general)
    return false;
  }

  void Congruence::force_tc() {
    LIBSEMIGROUPS_ASSERT(!is_obviously_infinite());
    delete_data();
    _data = new TC(*this);
  }

  void Congruence::force_tc_prefill() {
    delete_data();
    _data = new TC(*this);
    static_cast<TC*>(_data)->prefill();
  }

  void Congruence::force_p() {
    LIBSEMIGROUPS_ASSERT(_semigroup != nullptr);
    delete_data();
    _data = new P(*this);
  }

  void Congruence::force_kbp() {
    LIBSEMIGROUPS_ASSERT(_semigroup == nullptr);
    delete_data();
    _data = new KBP(*this);
  }

  void Congruence::force_kbfp() {
    LIBSEMIGROUPS_ASSERT(_type == TWOSIDED);
    delete_data();
    _data = new KBFP(*this);
  }

  Partition<word_t>* Congruence::nontrivial_classes() {
    DATA* data;
    if (_semigroup == nullptr) {
      // If this is an fp semigroup congruence, then KBP is the only DATA
      // subtype which can return a sensible answer.  Forcing KBP is not an
      // ideal solution; perhaps fp semigroup congruences should be handled
      // differently in future.  In particular, we should use _data if it
      // happens to be a KBP object already.
      data = new KBP(*this);
      data->run();
      Partition<word_t>* out = data->nontrivial_classes();
      if (_data == nullptr) {
        delete_data();
        _data = data;
      } else {
        delete data;
      }
      return out;
    } else {
      data = get_data();
      LIBSEMIGROUPS_ASSERT(data->is_done());
      return data->nontrivial_classes();
    }
  }

  void Congruence::init_relations(Semigroup<>*       semigroup,
                                  std::atomic<bool>& killed) {
    _init_mtx.lock();
    if (_relations_done || semigroup == nullptr) {
      _init_mtx.unlock();
      _relations_done = true;
      return;
    }

    LIBSEMIGROUPS_ASSERT(semigroup != nullptr);
    semigroup->enumerate(killed);

    if (!killed) {
      std::vector<size_t> relation;  // a triple
      semigroup->reset_next_relation();
      semigroup->next_relation(relation);

      while (relation.size() == 2 && !relation.empty()) {
        // This is for the case when there are duplicate gens
        word_t lhs = {relation[0]};
        word_t rhs = {relation[1]};
        _relations.push_back(std::make_pair(lhs, rhs));
        semigroup->next_relation(relation);
        // We could remove the duplicate generators, and update any relation
        // that contains a removed generator but this would be more complicated
      }
      word_t lhs, rhs;  // changed in-place by factorisation
      while (!relation.empty()) {
        semigroup->factorisation(lhs, relation[0]);
        lhs.push_back(relation[1]);

        semigroup->factorisation(rhs, relation[2]);

        _relations.push_back(std::make_pair(lhs, rhs));
        semigroup->next_relation(relation);
      }
      _relations_done = true;
    }
    _init_mtx.unlock();
  }

  // This is the default method used by a DATA object, and is used only by TC
  // and KBFP; the P and KBP subclasses override with their own superior method.
  // This method requires a Semigroup pointer and therefore does not allow fp
  // semigroup congruences.
  Partition<word_t>* Congruence::DATA::nontrivial_classes() {
    LIBSEMIGROUPS_ASSERT(is_done());
    LIBSEMIGROUPS_ASSERT(_cong._semigroup != nullptr);

    partition_t* classes = new partition_t();

    if (_cong._extra.empty()) {
      return new Partition<word_t>(classes);  // no nontrivial classes
    }

    // Note: we assume classes are numbered contiguously {0 .. n-1}
    partition_t* all_classes = new partition_t();
    for (size_t i = 0; i < nr_classes(); i++) {
      all_classes->push_back(new class_t());
    }

    // Look up the class number of each element of the parent semigroup
    word_t* word;
    for (size_t pos = 0; pos < _cong._semigroup->size(); pos++) {
      word = _cong._semigroup->factorisation(pos);
      // FIXME use the two argument version of factorisation to avoid
      // unnecessary memory allocations in the previous line.
      LIBSEMIGROUPS_ASSERT(word_to_class_index(*word) < nr_classes());
      (*all_classes)[word_to_class_index(*word)]->push_back(word);
    }

    // Store the words
    LIBSEMIGROUPS_ASSERT(all_classes->size() == nr_classes());
    for (size_t class_nr = 0; class_nr < all_classes->size(); class_nr++) {
      // Use only the classes with at least 2 elements
      if ((*all_classes)[class_nr]->size() > 1) {
        classes->push_back((*all_classes)[class_nr]);
      } else {
        // Delete the unused class
        LIBSEMIGROUPS_ASSERT((*all_classes)[class_nr]->size() == 1);
        delete (*(*all_classes)[class_nr])[0];
        delete (*all_classes)[class_nr];
      }
    }
    delete all_classes;

    return new Partition<word_t>(classes);
  }

}  // namespace libsemigroups

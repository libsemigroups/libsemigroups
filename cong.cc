//
// Semigroups++ - C/C++ library for computing with semigroups and monoids
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

#include "cong/kbfp.h"
#include "cong/kbp.h"
#include "cong/p.h"
#include "cong/tc.h"

namespace libsemigroups {

  // Define static data members
  size_t const       Congruence::INFTY       = -1;
  size_t const       Congruence::UNDEFINED   = -1;
  unsigned int const Congruence::MAX_THREADS = 4;
  // This is the value of the maximum number of threads used by the Congruence
  // class

  unsigned int const MAX_THREADS =
      std::max(std::thread::hardware_concurrency(), Congruence::MAX_THREADS);

  // Get the type from a string
  Congruence::cong_t Congruence::type_from_string(std::string type) {
    if (type == "left") {
      return cong_t::LEFT;
    } else if (type == "right") {
      return cong_t::RIGHT;
    } else {
      assert(type == "twosided");
      return cong_t::TWOSIDED;
    }
  }

  Congruence::Congruence(cong_t                         type,
                         size_t                         nrgens,
                         std::vector<relation_t> const& relations,
                         std::vector<relation_t> const& extra)
      : _data(nullptr),
        _extra(extra),
        _nrgens(nrgens),
        _prefill(),
        _relations(relations),
        _relations_done(false),
        _semigroup(nullptr),
        _type(type) {
    // TODO(JDM): check that the entries in extra/relations are properly defined
    // i.e. that every entry is at most nrgens - 1

    if (!_relations.empty() && !_extra.empty()) {
      // This represents a non-trivial congruence on a non-free fp semigroup U,
      // and KBP is currently the only way of computing such a thing. In
      // particular, the nontrivial_classes method must use KBP if the fp
      // semigroup U is infinite. Since we currently have no way to tell if the
      // fp semigroup is finite or infinite (in the sense that the fp semigroup
      // is only specified by the parameters _nrgens, and _relations and is not
      // actually present as a Congruence object anywhere in memory), we just
      // use KBP for everything for the sake of simplicity. This could be
      // improved by adding a constructor for the Congruence class (for the
      // congruence of the fp semigroup) that takes a Congruence (representing
      // the fp semigroup itself). Then instead of forcing KBP here we could
      // try enumerating the Congruence representing the fp semigroup (using
      // get_data) and running KBP on the other Congruence in parallel. In the
      // case that the fp semigroup U is finite, this would probably be better
      // than forcing KBP. If the fp semigroup U is infinite, then  KBP will
      // win (for nontrivial_classes).
      //
      // It might be that if we do not force KBP here, that in the case that
      // the quotient of the fp semigroup U, by the congruence we are
      // constructing, is finite, we might get an answer to methods other than
      // nontrivial_classes, more quickly using a different method than KBP. We
      // decided not to care about this for the time being.
      force_kbp();  // TODO improve this in future
    }
  }

  Congruence::Congruence(std::string                    type,
                         size_t                         nrgens,
                         std::vector<relation_t> const& relations,
                         std::vector<relation_t> const& extra)
      : Congruence(type_from_string(type), nrgens, relations, extra) {}

  Congruence::Congruence(cong_t                         type,
                         Semigroup*                     semigroup,
                         std::vector<relation_t> const& genpairs)
      : Congruence(type,
                   semigroup->nrgens(),
                   std::vector<relation_t>(),
                   std::vector<relation_t>()) {
    _semigroup = semigroup;
    _extra = genpairs; // it is essential that this is set here!
  }

  Congruence::Congruence(std::string                    type,
                         Semigroup*                     semigroup,
                         std::vector<relation_t> const& extra)
      : Congruence(type_from_string(type), semigroup, extra) {}

  Congruence::DATA* Congruence::winning_data(
      std::vector<Congruence::DATA*>& data,
      std::vector<std::function<void(Congruence::DATA*)>>& funcs) {

    std::vector<std::thread::id> tids(data.size(), std::this_thread::get_id());

    auto go = [&data, &funcs, &tids](size_t pos) {
      tids[pos] = std::this_thread::get_id();
      if (pos < funcs.size()) {
        funcs.at(pos)(data.at(pos));
      }
      data.at(pos)->run();
      for (auto it = data.begin(); it < data.begin() + pos; it++) {
        (*it)->kill();
      }
      for (auto it = data.begin() + pos + 1; it < data.end(); it++) {
        (*it)->kill();
      }
    };

    REPORT("using " << data.size() << " / "
                    << std::thread::hardware_concurrency()
                    << " threads");
    glob_reporter.reset_thread_ids();

    std::vector<std::thread> t;
    for (size_t i = 0; i < data.size(); i++) {
      t.push_back(std::thread(go, i));
    }
    for (size_t i = 0; i < t.size(); i++) {
      t.at(i).join();
    }

    for (auto winner = data.begin(); winner < data.end(); winner++) {
      if ((*winner)->is_done()) {
        size_t tid = glob_reporter.thread_id(tids.at(winner - data.begin()));
        REPORT("Thread #" << tid << " is the winner!");
        for (auto loser = data.begin(); loser < winner; loser++) {
          delete *loser;
        }
        for (auto loser = winner + 1; loser < data.end(); loser++) {
          delete *loser;
        }
        return *winner;
      }
    }
    assert(false);
    return nullptr;
  }

  Congruence::DATA* Congruence::get_data() {
    Timer timer;
    timer.start();
    if (_data == nullptr) {
      if (_semigroup != nullptr && _semigroup->is_done()
          && _semigroup->size() < 1024) {
        REPORT("semigroup is small, not using multiple threads")
        _data = new TC(*this);
        static_cast<TC*>(_data)->prefill();
        _data->run();
      } else {
        if (_semigroup != nullptr) {
          // TODO don't use more threads than the hardware supports here.
          auto prefillit = [this](Congruence::DATA* data) {
            static_cast<TC*>(data)->prefill();
          };

          std::vector<DATA*> data = {new TC(*this), new TC(*this)};
          std::vector<std::function<void(DATA*)>> funcs = {prefillit};
          if (_type == TWOSIDED) {
            data.push_back(new KBFP(*this));
          }
          data.push_back(new P(*this));
          _data = winning_data(data, funcs);
        } else if (!_prefill.empty()) {
          // FIXME this should be combined with the previous case
          _data = new TC(*this);
          static_cast<TC*>(_data)->prefill(_prefill);
          _data->run();
        } else {  // Congruence is defined over an fp semigroup
          // FIXME don't use more than MAX_THREADS here
          if (_type == TWOSIDED) {
            std::vector<DATA*> data = {
                new TC(*this), new KBFP(*this), new KBP(*this)};
            std::vector<std::function<void(DATA*)>> funcs = {};
            _data = winning_data(data, funcs);
          } else {
            _data = new TC(*this);
            _data->run();
          }
        }
      }
      REPORT(timer.string("elapsed time = "));
    }
    return _data;
  }

  void Congruence::force_tc() {
    if (_data != nullptr) {
      delete _data;
    }
    _data = new TC(*this);
  }

  void Congruence::force_tc_prefill() {
    if (_data != nullptr) {
      delete _data;
    }
    _data = new TC(*this);
    static_cast<TC*>(_data)->prefill();
  }

  void Congruence::force_p() {
    if (_data != nullptr) {
      delete _data;
    }
    assert(_semigroup != nullptr);
    _data = new P(*this);
  }

  void Congruence::force_kbp() {
    if (_data != nullptr) {
      delete _data;
    }
    assert(_semigroup == nullptr);
    _data = new KBP(*this);
  }

  void Congruence::force_kbfp() {
    if (_data != nullptr) {
      delete _data;
    }
    assert(_type == TWOSIDED);
    _data = new KBFP(*this);
  }

  void Congruence::init_relations(Semigroup*         semigroup,
                                  std::atomic<bool>& killed) {
    _mtx.lock();
    if (_relations_done || semigroup == nullptr) {
      _mtx.unlock();
      _relations_done = true;
      return;
    }

    assert(semigroup != nullptr);
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
    _mtx.unlock();
  }
}  // namespace libsemigroups

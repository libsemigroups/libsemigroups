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

#ifndef LIBSEMIGROUPS_SRC_CONG_H_
#define LIBSEMIGROUPS_SRC_CONG_H_

#include <algorithm>
#include <atomic>
#include <mutex>
#include <queue>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "partition.h"
#include "report.h"
#include "rws.h"
#include "rwse.h"
#include "semigroups.h"
#include "uf.h"

#define RETURN_FALSE nullptr

namespace libsemigroups {

  // Possible result of questions that might not be answerable.
  enum result_t { TRUE = 0, FALSE = 1, UNKNOWN = 2 };

  // The different types of congruence.
  enum cong_t { LEFT = 0, RIGHT = 1, TWOSIDED = 2 };

  //! Class for congruence on a semigroup or fintely presented semigroup.
  //!
  //! This class represents a congruence on a semigroup defined either as an
  //! instance of a Semigroup object or as a finitely presented semigroup
  //! defined by generators and relations.
  //!
  //! The structure of a Congruence is determined by running several different
  //! methods concurrently until one method returns an answer. The number of
  //! different threads used can be controlled, in part, by the value passed to
  //! Congruence::set_max_threads. The use of multiple threads to find the
  //! structure of a Congruence can cause the return values of certain
  //! methods to differ for different instances of mathematically equal
  //! objects.
  //!
  //! This class and its implemented methods are somewhat rudimentary in
  //! the current version of libsemigroups.
  template <typename TElementType  = Element*,
            typename TElementHash  = std::hash<TElementType>,
            typename TElementEqual = std::equal_to<TElementType>>
  class Congruence : public ElementContainer<TElementType> {
    // Congruence objects are friends regardless of template parameters
    template <typename, typename, typename> friend class Congruence;

   private:
    // The maximum number of steps that can be done in one run of
    // Congruence::run.
    static const size_t LIMIT_MAX = std::numeric_limits<size_t>::max();

    // Forward declaration of DATA
    class DATA;

   public:
    //! Type for indices of congruence classes in a Congruence object.
    typedef size_t class_index_t;

    //! Constructor for congruences over a finitely presented semigroup.
    //!
    //! The parameters are as follows:
    //!
    //! * \p type: a std::string describing the type of congruence, must be one
    //!   of \c "left", \c "right", or \c "twosided".
    //!
    //! * \p nrgens: the number of generators.
    //!
    //! * \p relations: the defining relations of the semigroup over which the
    //! congruence being constructed is defined. Every relation_t in this
    //! parameter must consist of positive integers less than \p nrgens.
    //!
    //! * \p extra: additional relations corresponding to the generating pairs
    //! of the congruence being constructed.  Every relation_t in this
    //! parameter must consist of positive integers less than \p nrgens.
    //!
    //! This constructor returns an instance of a Congruence object whose type
    //! is described by the string \p type. The congruence is defined over the
    //! semigroup defined by \p nrgens generators and the relations \p relations
    //! and is the least congruence containing the generating pairs in \p extra.
    //!
    //! For example, to compute a congruence over the free semigroup the
    //! parameter \p relations should be empty, and the relations defining the
    //! congruence to be constructed should be passed as the parameter \p
    //! extra. To compute a congruence over a finitely presented semigroup the
    //! relations defining the fintely presented semigroup must be passed as
    //! the parameter \p relations and the relations defining the
    //! congruence to be constructed should be passed as the parameter \p
    //! extra.
    Congruence(std::string                    type,
               size_t                         nrgens,
               std::vector<relation_t> const& relations,
               std::vector<relation_t> const& extra)
        : Congruence(type_from_string(type), nrgens, relations, extra) {}

    //! Constructor for congruences over a Semigroup object.
    //!
    //! The parameters are as follows:
    //!
    //! * \p type: a std::string describing the type of congruence, must be one
    //!   of \c "left", \c "right", or \c "twosided".
    //!
    //! * \p semigroup: a pointer to an instance of Semigroup. It is the
    //! responsibility of the caller to delete \p semigroup.
    //!
    //! * \p genpairs: additional relations corresponding to the generating
    //! pairs
    //! of the congruence being constructed.  Every relation_t in this
    //! parameter must consist of positive integers less than the number of
    //! generators of \p semigroup (Semigroup::nrgens()).
    //!
    //! This constructor returns an instance of a Congruence object whose type
    //! is described by the string \p type. The congruence is defined over the
    //! Semigroup \p semigroup and is the least congruence containing the
    //! generating pairs in \p extra.
    Congruence(std::string type,
               Semigroup<TElementType, TElementHash, TElementEqual>* semigroup,
               std::vector<relation_t> const& extra)
        : Congruence(type_from_string(type), semigroup, extra) {}

    //! A default destructor.
    //!
    //! The caller is responsible for deleting the semigroup used to construct
    //! \c this, if any.
    ~Congruence() {
      delete_data();
    }

    size_t nrgens() const {
      return _nrgens;
    }

    //! Returns the index of the congruence class corresponding to \p word.
    //!
    //! The parameter \p word must be a libsemigroups::word_t consisting of
    //! indices of the generators of the semigroup over which \c this is
    //! defined.
    //!
    //! If \c this is defined over a semigroup with generators \f$A\f$, then
    //! Congruence::word_to_class_index defines a surjective function from the
    //! set of all words over \f$A\f$ to either \f$\{0, 1, \ldots, n - 1\}\f$,
    //! where \f$n\f$ is the number of classes, or to the non-negative integers
    //! \f$\{0, 1, \ldots\}\f$ if \c this has infinitely many classes.
    //!
    //! \warning The method for finding the structure of a congruence is
    //! non-deterministic, and the return value of this method may vary
    //! between different instances of the same congruence.
    class_index_t word_to_class_index(word_t const& word) {
      DATA* data = get_data();
      LIBSEMIGROUPS_ASSERT(data->is_done());
      return data->word_to_class_index(word);
    }

    //!  Returns \c true if the words \p w1 and \p w2 belong to the
    //! same congruence class.
    //!
    //! The parameters  \p w1  and \p w2 must be libsemigroups::word_t's
    //! consisting of indices of generators of the semigroup over which \c this
    //! is defined.
    //!
    //! \warning The problem of determining the return value of this method is
    //! undecidable in general, and this method may never terminate.
    bool test_equals(word_t const& w1, word_t const& w2) {
      if (w1 == w2) {
        return true;
      }
      DATA* data;
      if (is_done()) {
        data = _data;
      } else {
        std::function<bool(DATA*)> words_func = [&w1, &w2](DATA* d) {
          return d->current_equals(w1, w2) != UNKNOWN;
        };
        data = get_data(words_func);
      }
      result_t result = data->current_equals(w1, w2);
      LIBSEMIGROUPS_ASSERT(result != UNKNOWN);
      return result == TRUE;
    }

    //!  Returns \c true if the congruence class of \p w1 is less than
    //! that of \p w2.
    //!
    //! This method returns \c true if the congruence class of \p w1 is less
    //! than
    //! the class of \p w2 in a total ordering of congruence classes.
    //!
    //! The parameters \p w1 and \p w2 should be libsemigroups::word_t's
    //! consisting of indices of the generators of the semigroup over which \c
    //! this is defined.
    //!
    //! \warning The method for finding the structure of a congruence is
    //! non-deterministic, and the total order of congruences classes may vary
    //! between different instances of the same congruence.
    //!
    //! \warning The problem of determining the return value of this method is
    //! undecidable in general, and this method may never terminate.
    bool test_less_than(word_t const& w1, word_t const& w2) {
      DATA* data;
      if (is_done()) {
        data = _data;
      } else {
        std::function<bool(DATA*)> words_func = [&w1, &w2](DATA* d) {
          return d->current_less_than(w1, w2) != UNKNOWN;
        };
        data = get_data(words_func);
      }

      if (!_partial_data.empty()) {
        LIBSEMIGROUPS_ASSERT(_data == nullptr);
        // Delete the losers and clear _partial_data
        for (size_t i = 0; i < _partial_data.size(); i++) {
          if (_partial_data[i] != data) {
            delete _partial_data[i];
          }
        }
        _partial_data.clear();
      }
      _data = data;

      result_t result = data->current_less_than(w1, w2);
      LIBSEMIGROUPS_ASSERT(result != UNKNOWN);
      return result == TRUE;
    }

    //! Returns the number of congruences classes of \c this.
    //!
    //! This method is non-const because it may fully compute a data structure
    //! for the congruence.
    //!
    //! \warning The problem of determining the number of classes of a
    //! congruence over a finitely presented semigroup is undecidable in
    //! general, and this method may never terminate.
    size_t nr_classes() {
      DATA* data = get_data();
      LIBSEMIGROUPS_ASSERT(data->is_done());
      return data->nr_classes();
    }

    //! Returns the non-trivial classes of the congruence.
    //!
    //! The elements in these classes are represented as words in the
    //! generators of the semigroup over which the congruence is defined.
    //!
    //! \warning If \c this has infinitely many non-trivial congruence classes,
    //! then this method will only terminate when it can no longer allocate
    //! memory.

    // This method returns the non-trivial classes of the congruence.
    Partition<word_t>* nontrivial_classes() {
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

    //! Returns \c true if the structure of the congruence is known.
    bool is_done() const {
      if (_data == nullptr) {
        return false;
      }
      return _data->is_done();
    }

    //!  Returns the vector of relations used to define the semigroup
    //! over which the congruence is defined.
    //!
    //! This method is non-const since if the congruence is defined over a
    //! Semigroup object, then we may have to compute and store its relations.
    std::vector<relation_t> const& relations() {
      init_relations(_semigroup);
      return _relations;
    }

    //!  Returns the vector of extra relations (or equivalently,
    //! generating pairs) used to define the congruence.
    std::vector<relation_t> const& extra() const {
      return _extra;
    }

    //!  Define the relations defining the semigroup over which \c this
    //! is defined.
    //!
    //! This method allows the relations of the semigroup over which the
    //! congruence is defined to be specified. This method asserts that the
    //! relations have not previously been specified.

    // FIXME it would be better to provide a constructor from another
    // congruence that copied the relations.
    void set_relations(std::vector<relation_t> const& relations) {
      LIBSEMIGROUPS_ASSERT(_relations.empty());  // _extra can be non-empty!
      _relations = relations;
    }

    //! Turn reporting on or off.
    //
    //!  If \p val is true, then some methods for a Congruence object may
    //! report information about the progress of the computation.
    void set_report(bool val) const {
      glob_reporter.set_report(val);
      LIBSEMIGROUPS_ASSERT(glob_reporter.get_report() == val);
    }

    //! Specify a partial coset table for the Todd-Coxeter algorithm.
    //!
    //! The parameter \p table should be a partial coset table for use in the
    //! Todd-Coxeter algorithm:
    //!
    //! * \p table should have RecVec::nr_cols equal to the number of generators
    //! of the semigroup over which \c this is defined
    //!
    //! * every entry in \p table must be less than the number of rows in
    //! \p table.
    //!
    //! For example, \p table can represent the right Cayley graph of a
    //! finite semigroup.
    //!
    //! If this method is called after anything has been computed about the
    //! congruence, it has no effect.
    void set_prefill(RecVec<class_index_t> const& table) {
      if (_data == nullptr) {
        _prefill = table;
      }
    }

    //! Set the maximum number of threads for a Congruence over a Semigroup.
    //!
    //! This method sets the maximum number of threads to be used by any method
    //! of a Congruence object which is defined over a Semigroup. The
    //! number of threads is limited to the maximum of 1 and the minimum of
    //! \p nr_threads and the number of threads supported by the hardware.
    //!
    //! If the congruence is not defined over a Semigroup, then the number of
    //! threads is not limited by this method.
    void set_max_threads(size_t nr_threads) {
      unsigned int n
          = static_cast<unsigned int>(nr_threads == 0 ? 1 : nr_threads);
      _max_threads = std::min(n, std::thread::hardware_concurrency());
    }

    //! Set the maximum number of active cosets in Todd-Coxeter before entering
    //! packing phase.
    //!
    //! This method only has any effect if used after Congruence::force_tc.
    void set_pack(size_t val) {
      if (_data != nullptr) {
        _data->set_pack(val);
      }
    }

    //! Sets how often the core methods of Congruence report.
    //!
    //! The smaller this value, the more often information will be reported.
    void set_report_interval(size_t val) {
      if (_data != nullptr) {
        _data->set_report_interval(val);
      }
    }

    //! Use the Todd-Coxeter algorithm.
    //!
    //! This methods forces the use of the
    //! [Todd-Coxeter
    //! algorithm](https://en.wikipedia.org/wiki/Todd–Coxeter_algorithm) to
    //! compute the congruence.  The implementation is based on one by Goetz
    //! Pfeiffer in GAP.
    //!
    //! \warning Any existing data for the congruence is deleted by this
    //! method, and may have to be recomputed. The return values and runtimes
    //! of other methods applied to \c this may also be affected.
    //!
    //! \warning The Todd-Coxeter Algorithm may never terminate when applied to
    //! a finitely presented semigroup.
    void force_tc() {
      LIBSEMIGROUPS_ASSERT(!is_obviously_infinite());
      delete_data();
      _data = new TC(*this);
    }

    //! Use the Todd-Coxeter algorithm after prefilling the table.
    //!
    //! This methods forces the use of the
    //! [Todd-Coxeter
    //! algorithm](https://en.wikipedia.org/wiki/Todd–Coxeter_algorithm) to
    //! compute the congruence.
    //!
    //! When applied to a congruence defined over a Semigroup,
    //! this method differs from Congruence::force_tc in that the so-called
    //! coset table used in the Todd-Coxeter algorithm is initialised to
    //! contain the right (or left) Cayley graph of the semigroup over which
    //! \c this is defined.
    //!
    //! If \c this is not defined over a Semigroup (i.e. it is defined over a
    //! finitely presented semigroup), then this method does the same as
    //! force_tc.
    //!
    //! \warning Any existing data for the congruence is deleted by this
    //! method, and may have to be recomputed. The return values and runtimes
    //! of other methods applied to \c this may also be affected.
    //!
    //! \warning The Todd-Coxeter Algorithm may never terminate when applied to
    //! a finitely presented semigroup.
    void force_tc_prefill() {
      delete_data();
      _data = new TC(*this);
      static_cast<TC*>(_data)->prefill();
    }

    //! Use an elementary orbit algorithm which enumerates pairs of
    //! Element objects that are related in \c this.
    //!
    //! The method forced by this is unlikely
    //! to terminate, or to be faster than the other methods, unless there are
    //! a relatively few pairs of elements related by the congruence.
    //!
    //! This method only applies to a congruence created using a Semigroup
    //! object, and does not apply to finitely presented semigroups.
    //!
    //! \warning Any existing data for the congruence is deleted by this
    //! method, and may have to be recomputed. The return values and runtimes
    //! of other methods applied to \c this may also be affected.
    //!
    //! \warning The worst case space complexity of these algorithms
    //! is the square of the size of the semigroup over which \c this is
    //! defined.
    void force_p() {
      LIBSEMIGROUPS_ASSERT(_semigroup != nullptr);
      delete_data();
      _data = new P(*this);
    }

    //! Use the Knuth-Bendix algorithm on a rewriting system RWS with rules
    //! obtained from Congruence::relations followed by an elementary orbit on
    //! pairs method on the resulting semigroup.
    //!
    //! This method forces the use of the [Knuth-Bendix
    //! algorithm](https://en.wikipedia.org/wiki/Knuth–Bendix_completion_algorithm)
    //! to compute the congruence defined by the generators and relations of \c
    //! this followed by an elementary orbit algorithm which enumerates pairs
    //! of Element objects of that semigroup which are related in \c this.
    //! Knuth-Bendix is applied to a rewriting system obtained from
    //! Congruence::relations.
    //!
    //! This method only applies to a congruence over a finitely presented
    //! semigroups, and does not apply to a congruence defined using a concrete
    //! Semigroup object.
    //!
    //! Note that this algorithm can be applied to left, right, and two-sided
    //! congruences (unlike KBFP).
    //!
    //! \warning Any existing data for the congruence is deleted by this
    //! method, and may have to be recomputed. The return values and runtimes
    //! of other methods applied to \c this may also be affected.
    //!
    //! \warning The Knuth-Bendix Algorithm may never terminate when applied to
    //! a finitely presented semigroup. Even if Knuth-Bendix completes
    //! the other algorithm that this method forces a Congruence to use
    //! will only terminate if there are finitely many pairs of elements
    //! related by \c this. The worst case space complexity of these algorithms
    //! is the square of the size of the semigroup over which \c this is
    //! defined.
    // FIXME why don't we apply this to congruences of non-fp semigroups?
    void force_kbp() {
      LIBSEMIGROUPS_ASSERT(_semigroup == nullptr);
      delete_data();
      _data = new KBP(*this);
    }

    //! Use the Knuth-Bendix algorithm on a rewriting system RWS with rules
    //! obtained from Congruence::relations and Congruence::extra, followed by
    //! the Froidure-Pin algorithm on the resulting semigroup.
    //!
    //! This method forces the use of the [Knuth-Bendix
    //! algorithm](https://en.wikipedia.org/wiki/Knuth–Bendix_completion_algorithm)
    //! to compute the congruence defined by the generators,
    //! Congruence::relations, and Congruence::extra of \c this followed by the
    //! Froidure-Pin algorithm on the resulting semigroup. The resulting
    //! semigroup consists of RWSE's and is enumerated using
    //! Semigroup::enumerate.
    //!
    //! At present the Knuth-Bendix algorithm can only be applied to two-sided
    //! congruences, and this method asserts that \c this is a two-sided
    //! congruence.
    //!
    //! \warning Any existing data for the congruence is deleted by this
    //! method, and may have to be recomputed. The return values and runtimes
    //! of other methods applied to \c this may also be affected.
    //!
    //! \warning The Knuth-Bendix Algorithm may never terminate when applied to
    //! a finitely presented semigroup.
    void force_kbfp() {
      LIBSEMIGROUPS_ASSERT(_type == TWOSIDED);
      delete_data();
      _data = new KBFP(*this);
    }

    //! This method tries to quickly determine whether or not the Congruence
    //! has infinitely many classes.
    //!
    //! If \c true is returned, then there are infinitely many classes in the
    //! congruence, but if \c false is returned, then the method could not
    //! determine whether or not there are infinitely many classes.

    // We apply some simple, quick checks that may establish that the congruence
    // has an infinite number of classes; if so, Todd-Coxeter should not be run.
    // If this returns false, it is safe to run Todd-Coxeter, although of course
    // Todd-Coxeter may still run forever.
    bool is_obviously_infinite() {
      // If we have a concrete semigroup, it is probably finite, or if it is
      // not,
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

   private:
    // This deletes all DATA objects stored in this congruence, including
    // finished objects and partially-enumerated objects.
    void delete_data() {
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

    // Set the relations of a Congruence object to the relations of the
    // semigroup over which the Congruence is defined (if any).
    void init_relations(
        Semigroup<TElementType, TElementHash, TElementEqual>* semigroup,
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
          // that contains a removed generator but this would be more
          // complicated
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

    void init_relations(
        Semigroup<TElementType, TElementHash, TElementEqual>* semigroup) {
      std::atomic<bool> killed(false);
      init_relations(semigroup, killed);
    }

    DATA* cget_data() const {
      return _data;
    }

    // This function returns a pointer to a DATA object, which will be run
    // either
    // to completion, or until **goal_func** is satisfied.  It will be created
    // if
    // it does not already exist.
    DATA* get_data(std::function<bool(DATA*)> goal_func = RETURN_FALSE) {
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
          auto prefillit = [this](
              Congruence<TElementType, TElementHash, TElementEqual>::DATA*
                  data) { static_cast<TC*>(data)->prefill(); };

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

    DATA* winning_data(std::vector<DATA*>&                      data,
                       std::vector<std::function<void(DATA*)>>& funcs,
                       bool                       ignore_max_threads = false,
                       std::function<bool(DATA*)> goal_func = RETURN_FALSE) {
      std::vector<std::thread::id> tids(data.size(),
                                        std::this_thread::get_id());

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
          REPORT("#" << tid << " is the winner!");
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

    Congruence(cong_t                         type,
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
      // TODO(JDM): check that the entries in extra/relations are properly
      // defined i.e. that every entry is at most nrgens - 1
    }

    Congruence(cong_t type,
               Semigroup<TElementType, TElementHash, TElementEqual>* semigroup,
               std::vector<relation_t> const& extra)
        : Congruence<TElementType, TElementHash, TElementEqual>(
              type,
              semigroup->nrgens(),
              std::vector<relation_t>(),
              std::vector<relation_t>()) {
      _semigroup = semigroup;
      _extra     = extra;  // it is essential that this is set here!
    }

    // Get the type from a string
    cong_t type_from_string(std::string type) {
      if (type == "left") {
        return cong_t::LEFT;
      } else if (type == "right") {
        return cong_t::RIGHT;
      } else {
        LIBSEMIGROUPS_ASSERT(type == "twosided");
        return cong_t::TWOSIDED;
      }
    }

    DATA*                   _data;
    std::vector<relation_t> _extra;
    std::mutex              _init_mtx;
    std::mutex              _kill_mtx;
    size_t                  _max_threads;
    size_t                  _nrgens;
    std::vector<DATA*>      _partial_data;
    RecVec<class_index_t>   _prefill;
    std::vector<relation_t> _relations;
    std::atomic<bool>       _relations_done;

    Semigroup<TElementType, TElementHash, TElementEqual>* _semigroup;
    cong_t _type;

    static size_t const INFTY;
    static size_t const UNDEFINED;

    // Subclasses of DATA
    class KBFP;  // Knuth-Bendix followed by Froidure-Pin
    class KBP;   // Knuth-Bendix followed by P
    class P;     // Orbit of pairs
    class TC;    // Todd-Coxeter

    // Abstract base class for nested classes containing methods for actually
    // enumerating the classes etc of a congruence
    class DATA {
      friend KBFP;
      friend KBP;
      friend P;
      friend TC;

     public:
      DATA(Congruence& cong,
           size_t      default_nr_steps,
           size_t      report_interval = 1000)
          : _cong(cong),
            _default_nr_steps(default_nr_steps),
            _killed(false),
            _report_interval(report_interval),
            _report_next(0) {}

      // Default destructor, does nothing
      virtual ~DATA() {}

      // This method runs the algorithm used to determine the congruence, i.e.
      // Todd-Coxeter, Knuth-Bendix, etc., until completion.
      virtual void run() = 0;

      // This method runs the algorithm for a while, then stops after a
      // certain amount of work or when done \p steps the amount of work to do
      // before returning.
      virtual void run(size_t steps) = 0;

      // This method returns true if a DATA object's run method has been run to
      // conclusion, i.e. that it has not been killed by another instance.
      virtual bool is_done() const = 0;

      // This method returns the number of classes of the congruence.
      virtual size_t nr_classes() = 0;

      // This method returns the index of the congruence class containing the
      // element of the semigroup defined by word.
      virtual class_index_t word_to_class_index(word_t const& word) = 0;

      // This method returns \c true if the two words are known to describe
      // elements in the same congruence class, \c false if they are known to
      // lie in different classes, and **UNKNOWN** if the information has not
      // yet been discovered.
      virtual result_t current_equals(word_t const& w1, word_t const& w2) = 0;

      // This method returns \c true if the two words are known to be in
      // distinct classes, where w1's class is less than w2's class by some
      // total ordering; \c false if this is known to be untrue; and
      // **UNKNOWN** if the information has not yet been discovered.
      // \p w1 const reference to the first word
      // \p w2 const reference to the second word
      virtual result_t current_less_than(word_t const& w1, word_t const& w2) {
        if (is_done()) {
          return word_to_class_index(w1) < word_to_class_index(w2) ? TRUE
                                                                   : FALSE;
        } else if (current_equals(w1, w2) == TRUE) {
          return FALSE;  // elements are equal
        }
        return UNKNOWN;
      }

      // This is the default method used by a DATA object, and is used only by
      // TC and KBFP; the P and KBP subclasses override with their own superior
      // method.  This method requires a Semigroup pointer and therefore does
      // not allow fp semigroup congruences.
      virtual Partition<word_t>* nontrivial_classes() {
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
      // This method kills a given instance of a DATA object.
      void kill() {
        // TODO add killed-by-thread
        _killed = true;
      }

      // This method sets a given instance of a DATA object to "not killed"
      void unkill() {
        _killed = false;
      }

      // This method can be used to tell whether or not a given DATA object has
      // been killed by another instance.
      std::atomic<bool>& is_killed() {
        return _killed;
      }

      // \p goal_func a function to test whether we can stop running
      //
      // This function calls DATA::run in batches until goal_func returns
      // true.  If goal_func is RETURN_FALSE, then the object ! is instead run
      // to completion.
      void run_until(std::function<bool(DATA*)> goal_func) {
        if (is_done()) {
          return;
        }
        if (goal_func != RETURN_FALSE) {
          while (!_killed && !is_done() && !goal_func(this)) {
            run(_default_nr_steps);
          }
        } else {
          run();
        }
      }

      virtual void set_pack(size_t val) {
        (void) val;
      }

      void set_report_interval(size_t val) {
        _report_interval = val;
      }

     private:
      Congruence&                   _cong;
      size_t                        _default_nr_steps;
      std::atomic<bool>             _killed;
      size_t                        _report_interval;
      size_t                        _report_next;
      typedef std::vector<word_t*>  class_t;
      typedef std::vector<class_t*> partition_t;
    };  // class DATA

    // Knuth-Bendix followed by Froidure-Pin
    class KBFP : public DATA {
     public:
      explicit KBFP(Congruence& cong)
          : DATA(cong, 200), _rws(new RWS()), _semigroup(nullptr) {}

      ~KBFP() {
        delete _rws;
        delete _semigroup;
      }

      void run() final {
        while (!this->_killed && !is_done()) {
          run(Congruence::LIMIT_MAX);
        }
      }

      void run(size_t steps) final {
        LIBSEMIGROUPS_ASSERT(!is_done());

        init();

        if (!this->_killed) {
          REPORT("running Froidure-Pin . . .")
          // This if statement will never be entered - see top of file for
          // details
          if (steps != Congruence::LIMIT_MAX) {
            // The default batch_size is too large and can take a long time,
            // but if we are running Congruence::LIMIT_MAX steps, then the
            // usual batch size is ok.
            _semigroup->set_batch_size(steps);
          }
          _semigroup->enumerate(this->_killed, _semigroup->current_size() + 1);
        }
        if (this->_killed) {
          REPORT("killed")
        }
      }

      bool is_done() const final {
        return (_semigroup != nullptr && _semigroup->is_done());
      }

      size_t nr_classes() final {
        LIBSEMIGROUPS_ASSERT(is_done());
        return _semigroup->size();
      }

      class_index_t word_to_class_index(word_t const& word) final {
        LIBSEMIGROUPS_ASSERT(is_done());  // so that _semigroup != nullptr
        RWSE*  x   = new RWSE(*_rws, word);
        size_t pos = _semigroup->position(x);
        x->really_delete();
        delete x;
        LIBSEMIGROUPS_ASSERT(pos != Semigroup<>::UNDEFINED);
        return pos;
      }

      result_t current_equals(word_t const& w1, word_t const& w2) final {
        init();
        if (!is_done() && this->is_killed()) {
          // This cannot be reliably tested: see TC::current_equals for more
          // info
          return UNKNOWN;
        }
        return (_rws->test_equals(w1, w2) ? TRUE : FALSE);
      }

      result_t current_less_than(word_t const& w1, word_t const& w2) final {
        init();
        if (!is_done() && this->is_killed()) {
          // This cannot be reliably tested: see TC::current_equals for more
          // info
          return UNKNOWN;
        }
        return (_rws->test_less_than(w1, w2) ? TRUE : FALSE);
      }

     private:
      void init() {
        if (_semigroup != nullptr) {
          return;
        }
        this->_cong.init_relations(this->_cong._semigroup, this->_killed);
        _rws->add_rules(this->_cong.relations());
        _rws->add_rules(this->_cong.extra());

        LIBSEMIGROUPS_ASSERT(this->_cong._semigroup == nullptr
                             || !this->_cong.extra().empty());

        REPORT("running Knuth-Bendix . . .")
        _rws->knuth_bendix(this->_killed);
        if (this->_killed) {
          REPORT("killed");
          return;
        }

        LIBSEMIGROUPS_ASSERT(_rws->confluent());
        std::vector<RWSE*> gens;
        for (size_t i = 0; i < this->_cong._nrgens; i++) {
          gens.push_back(new RWSE(*_rws, i));
        }
        _semigroup = new Semigroup<RWSE*>(gens);
        really_delete_cont(gens);
      }

      RWS*              _rws;
      Semigroup<RWSE*>* _semigroup;
    };  // class KBFP

    // Knuth-Bendix followed by the orbit on pairs algorithm
    class KBP : public DATA {
     public:
      explicit KBP(Congruence& cong)
          : DATA(cong, 200),
            _rws(new RWS()),
            _semigroup(nullptr),
            _P_cong(nullptr) {}

      ~KBP() {
        delete _rws;
        delete _semigroup;
        delete _P_cong;
      }

      void run() final {
        while (!this->_killed && !is_done()) {
          run(Congruence<TElementType, TElementHash, TElementEqual>::LIMIT_MAX);
        }
      }

      void run(size_t steps) final {
        init();
        if (!this->_killed) {
          REPORT("running P . . .")
          Congruence<RWSE*>::P* p
              = static_cast<Congruence<RWSE*>::P*>(_P_cong->cget_data());
          LIBSEMIGROUPS_ASSERT(p != nullptr);
          p->run(steps, this->_killed);
        }
        if (this->_killed) {
          REPORT("killed")
        }
      }

      bool is_done() const final {
        return (_semigroup != nullptr) && (_P_cong != nullptr)
               && (_P_cong->is_done());
      }

      size_t nr_classes() final {
        LIBSEMIGROUPS_ASSERT(is_done());
        return _P_cong->nr_classes();
      }

      class_index_t word_to_class_index(word_t const& word) final {
        LIBSEMIGROUPS_ASSERT(is_done());
        return _P_cong->word_to_class_index(word);
      }

      result_t current_equals(word_t const& w1, word_t const& w2) final {
        init();
        if (!is_done() && this->is_killed()) {
          // This cannot be reliably tested: see TC::current_equals for more
          // info
          return UNKNOWN;
        }
        LIBSEMIGROUPS_ASSERT(_P_cong != nullptr);
        return _P_cong->cget_data()->current_equals(w1, w2);
      }

      Partition<word_t>* nontrivial_classes() final {
        LIBSEMIGROUPS_ASSERT(is_done());
        return _P_cong->nontrivial_classes();
      }

     private:
      void init() {
        if (_semigroup != nullptr) {
          return;
        }
        LIBSEMIGROUPS_ASSERT(_P_cong == nullptr);

        // Initialise the rewriting system
        _rws->add_rules(this->_cong.relations());
        REPORT("running Knuth-Bendix . . .");
        _rws->knuth_bendix(this->_killed);

        // Setup the P cong
        if (!this->_killed) {
          LIBSEMIGROUPS_ASSERT(_rws->confluent());
          std::vector<RWSE*> gens;
          for (size_t i = 0; i < this->_cong._nrgens; i++) {
            gens.push_back(new RWSE(*_rws, i));
          }
          _semigroup = new Semigroup<RWSE*>(gens);
          really_delete_cont(gens);

          _P_cong = new Congruence<RWSE*>(
              this->_cong._type, _semigroup, this->_cong._extra);
          _P_cong->set_relations(this->_cong.relations());
          _P_cong->force_p();
        }
      }
      RWS*               _rws;
      Semigroup<RWSE*>*  _semigroup;
      Congruence<RWSE*>* _P_cong;
    };  // class KBP

    class P : public DATA, ElementContainer<TElementType> {
     public:
      explicit P(Congruence& cong)
          : DATA(cong, 2000, 40000),
            _class_lookup(),
            _done(false),
            _found_pairs(),
            _lookup(0),
            _map(),
            _map_next(0),
            _next_class(0),
            _pairs_to_mult(),
            _reverse_map(),
            _tmp1(),
            _tmp2() {
        LIBSEMIGROUPS_ASSERT(cong._semigroup != nullptr);

        _tmp1 = this->copy(cong._semigroup->gens(0));
        _tmp2 = this->copy(_tmp1);

        // Set up _pairs_to_mult
        for (relation_t const& rel : cong._extra) {
          TElementType x = cong._semigroup->word_to_element(rel.first);
          TElementType y = cong._semigroup->word_to_element(rel.second);
          add_pair(x, y);
          this->free(x);
          this->free(y);
        }
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
        this->free(_tmp1);
        this->free(_tmp2);
        for (auto& x : _map) {
          this->free(x.first);
        }
      }

      bool is_done() const final {
        return _done;
      }

      size_t nr_classes() final {
        LIBSEMIGROUPS_ASSERT(is_done());
        return this->_cong._semigroup->size() - _class_lookup.size()
               + _next_class;
      }

      class_index_t word_to_class_index(word_t const& w) final {
        LIBSEMIGROUPS_ASSERT(is_done());

        TElementType x     = this->_cong._semigroup->word_to_element(w);
        size_t       ind_x = get_index(x);
        this->free(x);
        LIBSEMIGROUPS_ASSERT(ind_x < _class_lookup.size());
        LIBSEMIGROUPS_ASSERT(_class_lookup.size() == _map.size());
        return _class_lookup[ind_x];
      }

      result_t current_equals(word_t const& w1, word_t const& w2) final {
        if (is_done()) {
          return word_to_class_index(w1) == word_to_class_index(w2) ? TRUE
                                                                    : FALSE;
        }
        TElementType x     = this->_cong._semigroup->word_to_element(w1);
        TElementType y     = this->_cong._semigroup->word_to_element(w2);
        size_t       ind_x = get_index(x);
        size_t       ind_y = get_index(y);
        this->free(x);
        this->free(y);
        return _lookup.find(ind_x) == _lookup.find(ind_y) ? TRUE : UNKNOWN;
      }

      Partition<word_t>* nontrivial_classes() final {
        LIBSEMIGROUPS_ASSERT(is_done());
        LIBSEMIGROUPS_ASSERT(_reverse_map.size() >= _nr_nontrivial_elms);
        LIBSEMIGROUPS_ASSERT(_class_lookup.size() >= _nr_nontrivial_elms);

        Partition<word_t>* classes
            = new Partition<word_t>(_nr_nontrivial_classes);

        for (size_t ind = 0; ind < _nr_nontrivial_elms; ind++) {
          word_t* word
              = this->_cong._semigroup->factorisation(_reverse_map[ind]);
          (*classes)[_class_lookup[ind]]->push_back(word);
        }
        return classes;
      }

      void run() final {
        run(this->_killed);
      }

      void run(size_t steps) final {
        run(steps, this->_killed);
      }

      void run(std::atomic<bool>& killed) {
        while (!killed && !is_done()) {
          run(Congruence::LIMIT_MAX, killed);
        }
      }

      void run(size_t steps, std::atomic<bool>& killed) {
        REPORT("number of steps = " << steps);
        size_t tid = glob_reporter.thread_id(std::this_thread::get_id());
        while (!_pairs_to_mult.empty()) {
          // Get the next pair
          std::pair<TElementType, TElementType> current_pair
              = _pairs_to_mult.front();

          _pairs_to_mult.pop();

          // Add its left and/or right multiples
          for (size_t i = 0; i < this->_cong._nrgens; i++) {
            TElementType gen = this->_cong._semigroup->gens(i);
            if (this->_cong._type == LEFT || this->_cong._type == TWOSIDED) {
              _tmp1 = this->multiply(_tmp1, gen, current_pair.first, tid);
              _tmp2 = this->multiply(_tmp2, gen, current_pair.second, tid);
              add_pair(_tmp1, _tmp2);
            }
            if (this->_cong._type == RIGHT || this->_cong._type == TWOSIDED) {
              _tmp1 = this->multiply(_tmp1, current_pair.first, gen, tid);
              _tmp2 = this->multiply(_tmp2, current_pair.second, gen, tid);
              add_pair(_tmp1, _tmp2);
            }
          }
          if (this->_report_next++ > this->_report_interval) {
            REPORT("found " << _found_pairs.size() << " pairs: " << _map_next
                            << " elements in "
                            << _lookup.nr_blocks()
                            << " classes, "
                            << _pairs_to_mult.size()
                            << " pairs on the stack");
            this->_report_next = 0;
            if (tid != 0 && this->_cong._semigroup->is_done()
                && _found_pairs.size() > this->_cong._semigroup->size()) {
              // If the congruence is only using 1 thread, then this will never
              // happen, if the congruence uses > 1 threads, then it is ok for
              // P to kill itself, because another thread will complete and
              // return the required DATA*.
              REPORT("too many pairs found, stopping");
              killed = true;
              return;
            }
          }
          if (killed) {
            REPORT("killed");
            return;
          }
          if (--steps == 0) {
            return;
          }
        }
        // Make a normalised class lookup (class numbers {0, .., n-1}, in order)
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

        if (!killed) {
          REPORT("finished with " << _found_pairs.size() << " pairs: "
                                  << _map_next
                                  << " elements in "
                                  << _lookup.nr_blocks()
                                  << " classes");
          _done = true;
          delete_tmp_storage();
        } else {
          REPORT("killed");
        }
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

      void add_pair(TElementType x, TElementType y) {
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
        if (_done) {
          _class_lookup.push_back(_next_class++);
        }
        return _map_next++;
      }

      std::vector<class_index_t> _class_lookup;
      bool                       _done;
      std::unordered_set<std::pair<TElementType, TElementType>, PHash, PEqual>
          _found_pairs;
      UF  _lookup;
      std::unordered_map<TElementType, size_t> _map;
      size_t        _map_next;
      class_index_t _next_class;
      size_t        _nr_nontrivial_classes;
      size_t        _nr_nontrivial_elms;
      std::queue<std::pair<TElementType, TElementType>> _pairs_to_mult;
      std::vector<TElementType> _reverse_map;
      TElementType              _tmp1;
      TElementType              _tmp2;
    };  // class P

#define TC_KILLED                      \
  if (this->_killed) {                 \
    if (!_already_reported_killed) {   \
      _already_reported_killed = true; \
      REPORT("killed")                 \
    }                                  \
    _stop_packing = true;              \
    _steps        = 1;                 \
  }

    class TC : public DATA {
      typedef int64_t signed_class_index_t;

     public:
      explicit TC(Congruence& cong)
          : DATA(cong, 1000, 2000000),
            _active(1),
            _already_reported_killed(false),
            _bckwd(1, 0),
            _cosets_killed(0),
            _current(0),
            _current_no_add(UNDEFINED),
            _defined(1),
            _extra(),
            _forwd(1, UNDEFINED),
            _id_coset(0),
            _init_done(false),
            _last(0),
            _next(UNDEFINED),
            _pack(120000),
            _prefilled(false),
            _preim_init(cong._nrgens, 1, UNDEFINED),
            _preim_next(cong._nrgens, 1, UNDEFINED),
            _stop_packing(false),
            _table(cong._nrgens, 1, UNDEFINED),
            _tc_done(false) {}

      ~TC() {}

      void run() final {
        while (!is_done() && !this->is_killed()) {
          run(Congruence<TElementType, TElementHash, TElementEqual>::LIMIT_MAX);
          TC_KILLED
        }
      }

      void run(size_t steps) final {
        _steps = steps;

        init();

        if (_tc_done) {
          return;
        }

        // Run a batch
        REPORT("number of steps: " << _steps);
        do {
          // Apply each relation to the "_current" coset
          for (relation_t const& rel : _relations) {
            trace(_current, rel);  // Allow new cosets
          }

          // If the number of active cosets is too high, start a packing phase
          if (_active > _pack) {
            REPORT(_defined << " defined, " << _forwd.size() << " max, "
                            << _active
                            << " active, "
                            << (_defined - _active) - _cosets_killed
                            << " killed, "
                            << "current "
                            << _current);
            REPORT("Entering lookahead phase . . .");
            _cosets_killed = _defined - _active;

            size_t oldactive = _active;       // Keep this for stats
            _current_no_add  = _current + 1;  // Start packing from _current

            do {
              // Apply every relation to the "_current_no_add" coset
              for (relation_t const& rel : _relations) {
                trace(_current_no_add, rel, false);  // Don't allow new cosets
              }
              _current_no_add = _forwd[_current_no_add];

              // Quit loop if we reach an inactive coset OR we get a "stop"
              // signal
              TC_KILLED
            } while (_current_no_add != _next && !_stop_packing);

            REPORT("Entering lookahead complete " << oldactive - _active
                                                  << " killed");

            _pack += _pack / 10;  // Raise packing threshold 10%
            _stop_packing   = false;
            _current_no_add = UNDEFINED;
          }

          // Move onto the next coset
          _current = _forwd[_current];

          // Quit loop when we reach an inactive coset
          TC_KILLED
        } while (_current != _next && --_steps > 0);

        // Final report
        REPORT("stopping with " << _defined << " cosets defined,"
                                << " maximum "
                                << _forwd.size()
                                << ", "
                                << _active
                                << " survived");
        if (_current == _next) {
          _tc_done = true;
          compress();
          REPORT("finished!");
        }

        // No return value: all info is now stored in the class
      }

      bool is_done() const final {
        return _tc_done;
      }

      size_t nr_classes() final {
        LIBSEMIGROUPS_ASSERT(is_done());
        return _active - 1;
      }

      class_index_t word_to_class_index(word_t const& w) final {
        class_index_t c = _id_coset;
        if (this->_cong._type == LEFT) {
          // Iterate in reverse order
          for (auto rit = w.crbegin(); rit != w.crend() && c != UNDEFINED;
               ++rit) {
            c = _table.get(c, *rit);
          }
        } else {
          // Iterate in sequential order
          for (auto it = w.cbegin(); it != w.cend() && c != UNDEFINED; ++it) {
            c = _table.get(c, *it);
          }
        }
        // c in {1 .. n} (where 0 is the id coset)
        LIBSEMIGROUPS_ASSERT(c < _active || c == UNDEFINED);
        // Convert to {0 .. n-1}
        return (c == UNDEFINED ? c : c - 1);
      }

      result_t current_equals(word_t const& w1, word_t const& w2) final {
        if (!is_done() && this->is_killed()) {
          // This cannot be reliably tested since it relies on a race
          // condition: if this has been killed since the start of the
          // function, then we return immediately to run_until with an
          // inconclusive answer.  run_until will then quit, and allow the
          // winning DATA to answer the equality test.
          return UNKNOWN;
        }

        init();

        class_index_t c1 = word_to_class_index(w1);
        class_index_t c2 = word_to_class_index(w2);

        if (c1 == UNDEFINED || c2 == UNDEFINED) {
          return UNKNOWN;
        }

        // c in {1 .. n} (where 0 is the id coset)
        LIBSEMIGROUPS_ASSERT(c1 < _active);
        LIBSEMIGROUPS_ASSERT(c2 < _active);
        if (c1 == c2) {
          return TRUE;
        } else if (is_done()) {
          return FALSE;
        } else {
          return UNKNOWN;
        }
      }

      // This method compresses the coset table used by <todd_coxeter>.
      void compress() {
        LIBSEMIGROUPS_ASSERT(is_done());
        if (_active == _table.nr_rows()) {
          return;
        }

        RecVec<class_index_t> table(this->_cong._nrgens, _active);

        class_index_t pos = _id_coset;
        // old number to new numbers lookup
        std::unordered_map<class_index_t, class_index_t> lookup;
        size_t next_index = 0;

        while (pos != _next) {
          size_t curr_index;
          auto   it = lookup.find(pos);
          if (it == lookup.end()) {
            lookup.insert(std::make_pair(pos, next_index));
            curr_index = next_index;
            next_index++;
          } else {
            curr_index = it->second;
          }

          // copy row
          for (size_t i = 0; i < this->_cong._nrgens; i++) {
            class_index_t val = _table.get(pos, i);
            it                = lookup.find(val);
            if (it == lookup.end()) {
              lookup.insert(std::make_pair(val, next_index));
              val = next_index;
              next_index++;
            } else {
              val = it->second;
            }
            table.set(curr_index, i, val);
          }
          pos = _forwd[pos];
        }

        _table = table;
      }

      // no args means use the semigroup used to define this
      void prefill() {
        Semigroup<TElementType, TElementHash, TElementEqual>* semigroup
            = this->_cong._semigroup;
        if (semigroup == nullptr) {
          return;
        }
        _table.add_rows(semigroup->size());
        for (size_t i = 0; i < this->_cong._nrgens; i++) {
          _table.set(0, i, semigroup->letter_to_pos(i) + 1);
        }
        TC_KILLED
        if (this->_cong._type == LEFT) {
          for (size_t row = 0; row < semigroup->size(); ++row) {
            for (size_t col = 0; col < this->_cong._nrgens; ++col) {
              _table.set(row + 1, col, semigroup->left(row, col) + 1);
            }
          }
        } else {
          for (size_t row = 0; row < semigroup->size(); ++row) {
            for (size_t col = 0; col < this->_cong._nrgens; ++col) {
              _table.set(row + 1, col, semigroup->right(row, col) + 1);
            }
          }
        }
        TC_KILLED
        init_after_prefill();
      }

      void prefill(RecVec<class_index_t>& table) {
        // TODO(JDM) check table is valid
        LIBSEMIGROUPS_ASSERT(table.nr_cols() == this->_cong._nrgens);
        LIBSEMIGROUPS_ASSERT(table.nr_rows() > 0);

        _table = table;
        init_after_prefill();
      }

      void set_pack(size_t val) override {
        _pack = val;
      }

     private:
      void init() {
        if (!_init_done) {
          // This is the first run
          init_tc_relations();
          // Apply each "extra" relation to the first coset only
          for (relation_t const& rel : _extra) {
            trace(_id_coset, rel);  // Allow new cosets
          }
        }
        _init_done = true;
      }

      void init_after_prefill() {
        _prefilled = true;
        _active    = _table.nr_rows();
        _id_coset  = 0;

        _forwd.reserve(_active);
        _bckwd.reserve(_active);

        for (size_t i = 1; i < _active; i++) {
          _forwd.push_back(i + 1);
          _bckwd.push_back(i - 1);
        }

        TC_KILLED

        _forwd[0]           = 1;
        _forwd[_active - 1] = UNDEFINED;

        _last = _active - 1;

        _preim_init.add_rows(_table.nr_rows());
        _preim_next.add_rows(_table.nr_rows());

        for (class_index_t c = 0; c < _active; c++) {
          for (letter_t i = 0; i < this->_cong._nrgens; i++) {
            class_index_t b = _table.get(c, i);
            _preim_next.set(c, i, _preim_init.get(b, i));
            _preim_init.set(b, i, c);
          }
          // TC_KILLED?
        }
        _defined = _active;
      }

      void init_tc_relations() {
        // This should not have been run before
        LIBSEMIGROUPS_ASSERT(!_init_done);

        // Handle _extra first!
        switch (this->_cong._type) {
          case LEFT:
            _extra.insert(_extra.end(),
                          this->_cong._extra.begin(),
                          this->_cong._extra.end());
            for (relation_t& rel : _extra) {
              std::reverse(rel.first.begin(), rel.first.end());
              std::reverse(rel.second.begin(), rel.second.end());
            }
            break;
          case RIGHT:                     // do nothing
            _extra = this->_cong._extra;  // FIXME avoid copying here!
            break;
          case TWOSIDED:
            _relations.insert(_relations.end(),
                              this->_cong._extra.begin(),
                              this->_cong._extra.end());
            break;
          default:
            LIBSEMIGROUPS_ASSERT(false);
        }

        if (_prefilled) {
          // The information in the Congruence relations is already present
          // in the table since we prefilled it.
          return;
        }

        // Initialise the relations in the enclosing Congruence object. We do
        // not call relations() here so that we can pass _killed.

        this->_cong.init_relations(this->_cong._semigroup, this->_killed);

        // Must insert at _relations.end() since it might be non-empty
        _relations.insert(_relations.end(),
                          this->_cong._relations.begin(),
                          this->_cong._relations.end());
        // FIXME avoid copying in the RIGHT case
        switch (this->_cong._type) {
          case RIGHT:
          // intentional fall through
          case TWOSIDED:
            break;
          case LEFT:
            for (relation_t& rel : _relations) {
              std::reverse(rel.first.begin(), rel.first.end());
              std::reverse(rel.second.begin(), rel.second.end());
            }
            break;
          default:
            LIBSEMIGROUPS_ASSERT(false);
        }
      }

      void new_coset(class_index_t const& c, letter_t const& a) {
        TC_KILLED
        _active++;
        _defined++;
        this->_report_next++;

        if (_next == UNDEFINED) {
          // There are no free cosets to recycle: make a new one
          _next         = _active - 1;
          _forwd[_last] = _next;
          _forwd.push_back(UNDEFINED);
          _bckwd.push_back(_last);
          _table.add_rows(1);
          _preim_init.add_rows(1);
          _preim_next.add_rows(1);
        } else {
          _bckwd[_next] = _last;
        }

        // Mark one more coset as active
        _last = _next;
        _next = _forwd[_last];

        // Clear the new coset's row in each table
        for (letter_t i = 0; i < this->_cong._nrgens; i++) {
          _table.set(_last, i, UNDEFINED);
          _preim_init.set(_last, i, UNDEFINED);
        }

        // Set the new coset as the image of c under a
        _table.set(c, a, _last);

        // Set c as the one preimage of the new coset
        _preim_init.set(_last, a, c);
        _preim_next.set(c, a, UNDEFINED);
      }

      void identify_cosets(class_index_t lhs, class_index_t rhs) {
        TC_KILLED

        // Note that _lhs_stack and _rhs_stack may not be empty, if this was
        // killed before and has been restarted.

        // Make sure lhs < rhs
        if (lhs == rhs) {
          return;
        } else if (rhs < lhs) {
          class_index_t tmp = lhs;
          lhs               = rhs;
          rhs               = tmp;
        }

        while (!this->_killed) {
          // If <lhs> is not active, use the coset it was identified with
          while (_bckwd[lhs] < 0) {
            lhs = -_bckwd[lhs];
          }
          // Same with <rhs>
          while (_bckwd[rhs] < 0) {
            rhs = -_bckwd[rhs];
          }

          if (lhs != rhs) {
            _active--;
            // If any "controls" point to <rhs>, move them back one in the
            // list
            if (rhs == _current) {
              _current = _bckwd[_current];
            }
            if (rhs == _current_no_add) {
              _current_no_add = _bckwd[_current_no_add];
            }

            LIBSEMIGROUPS_ASSERT(rhs != _next);
            if (rhs == _last) {
              // Simply move the start of the free list back by 1
              _last = _bckwd[_last];
            } else {
              // Remove <rhs> from the active list
              _bckwd[_forwd[rhs]] = _bckwd[rhs];
              _forwd[_bckwd[rhs]] = _forwd[rhs];
              // Add <rhs> to the start of the free list
              _forwd[rhs]   = _next;
              _forwd[_last] = rhs;
            }
            _next = rhs;

            // Leave a "forwarding address" so we know what <rhs> was
            // identified
            // with
            _bckwd[rhs] = -static_cast<signed_class_index_t>(lhs);

            for (letter_t i = 0; i < this->_cong._nrgens; i++) {
              // Let <v> be the first PREIMAGE of <rhs>
              class_index_t v = _preim_init.get(rhs, i);
              while (v != UNDEFINED) {
                _table.set(v, i, lhs);  // Replace <rhs> by <lhs> in the table
                class_index_t u
                    = _preim_next.get(v, i);  // Get <rhs>'s next preimage
                _preim_next.set(v, i, _preim_init.get(lhs, i));
                _preim_init.set(lhs, i, v);
                // v is now a preimage of <lhs>, not <rhs>
                v = u;  // Let <v> be <rhs>'s next preimage, and repeat
              }

              // Now let <v> be the IMAGE of <rhs>
              v = _table.get(rhs, i);
              if (v != UNDEFINED) {
                class_index_t u = _preim_init.get(v, i);
                LIBSEMIGROUPS_ASSERT(u != UNDEFINED);
                if (u == rhs) {
                  // Remove <rhs> from the start of the list of <v>'s
                  // preimages
                  _preim_init.set(v, i, _preim_next.get(rhs, i));
                } else {
                  // Go through all <v>'s preimages until we find <rhs>
                  while (_preim_next.get(u, i) != rhs) {
                    u = _preim_next.get(u, i);
                  }
                  // Remove <rhs> from the list of <v>'s preimages
                  _preim_next.set(u, i, _preim_next.get(rhs, i));
                }

                // Let <u> be the image of <lhs>, and ensure <u> = <v>
                u = _table.get(lhs, i);
                if (u == UNDEFINED) {
                  _table.set(lhs, i, v);
                  _preim_next.set(lhs, i, _preim_init.get(v, i));
                  _preim_init.set(v, i, lhs);
                } else {
                  // Add (u,v) to the stack of pairs to be identified
                  _lhs_stack.push(std::min(u, v));
                  _rhs_stack.push(std::max(u, v));
                }
              }
            }
          }
          if (_lhs_stack.empty()) {
            LIBSEMIGROUPS_ASSERT(_rhs_stack.empty());
            break;
          }
          // Get the next pair to be identified
          lhs = _lhs_stack.top();
          _lhs_stack.pop();
          rhs = _rhs_stack.top();
          _rhs_stack.pop();
        }

        LIBSEMIGROUPS_ASSERT((_lhs_stack.empty() && _rhs_stack.empty())
                             || this->_killed);
      }

      inline void
      trace(class_index_t const& c, relation_t const& rel, bool add = true) {
        class_index_t lhs = c;
        for (auto it = rel.first.cbegin(); it < rel.first.cend() - 1; it++) {
          if (_table.get(lhs, *it) != UNDEFINED) {
            lhs = _table.get(lhs, *it);
          } else if (add) {
            new_coset(lhs, *it);
            lhs = _last;
          } else {
            return;
          }
        }
        // <lhs> is the image of <c> under <rel>[1] (minus the last letter)

        class_index_t rhs = c;
        for (auto it = rel.second.cbegin(); it < rel.second.cend() - 1; it++) {
          if (_table.get(rhs, *it) != UNDEFINED) {
            rhs = _table.get(rhs, *it);
          } else if (add) {
            new_coset(rhs, *it);
            rhs = _last;
          } else {
            return;
          }
        }
        // <rhs> is the image of <c> under <rel>[2] (minus the last letter)

        // Statistics and packing
        this->_report_next++;
        if (this->_report_next > this->_report_interval) {
          REPORT(_defined << " defined, " << _forwd.size() << " max, "
                          << _active
                          << " active, "
                          << (_defined - _active) - _cosets_killed
                          << " killed, "
                          << "current "
                          << (add ? _current : _current_no_add))
          // If we are killing cosets too slowly, then stop packing
          if ((_defined - _active) - _cosets_killed < 100) {
            _stop_packing = true;
          }
          this->_report_next = 0;
          _cosets_killed     = _defined - _active;
        }

        letter_t      a = rel.first.back();
        letter_t      b = rel.second.back();
        class_index_t u = _table.get(lhs, a);
        class_index_t v = _table.get(rhs, b);
        // u = lhs^a = c^rel[1]
        // v = rhs^b = c^rel[2]

        // We must now ensure lhs^a == rhs^b.
        if (u == UNDEFINED && v == UNDEFINED) {
          if (add) {
            // Create a new coset and set both lhs^a and rhs^b to it
            new_coset(lhs, a);
            _table.set(rhs, b, _last);
            if (a == b) {
              _preim_next.set(lhs, a, rhs);
              _preim_next.set(rhs, a, UNDEFINED);
            } else {
              _preim_init.set(_last, b, rhs);
              _preim_next.set(rhs, b, UNDEFINED);
            }
          } else {
            return;  // Packing phase: do nothing
          }
        } else if (u == UNDEFINED && v != UNDEFINED) {
          // Set lhs^a to v
          _table.set(lhs, a, v);
          _preim_next.set(lhs, a, _preim_init.get(v, a));
          _preim_init.set(v, a, lhs);
        } else if (u != UNDEFINED && v == UNDEFINED) {
          // Set rhs^b to u
          _table.set(rhs, b, u);
          _preim_next.set(rhs, b, _preim_init.get(u, b));
          _preim_init.set(u, b, rhs);
        } else {
          // lhs^a and rhs^b are both defined
          identify_cosets(u, v);
        }
      }

      size_t                            _active;  // Number of active cosets
      bool                              _already_reported_killed;
      std::vector<signed_class_index_t> _bckwd;
      size_t                            _cosets_killed;
      class_index_t                     _current;
      class_index_t                     _current_no_add;
      size_t                            _defined;
      std::vector<relation_t>           _extra;
      std::vector<class_index_t>        _forwd;
      class_index_t                     _id_coset;   // TODO(JDM) Remove?
      bool                              _init_done;  // Has init() been run yet?
      class_index_t                     _last;
      std::stack<class_index_t> _lhs_stack;  // Stack for identifying cosets
      class_index_t             _next;
      size_t                    _pack;  // Nr of active cosets allowed before a
                                        // packing phase starts
      bool                      _prefilled;
      RecVec<class_index_t>     _preim_init;
      RecVec<class_index_t>     _preim_next;
      std::vector<relation_t>   _relations;
      std::stack<class_index_t> _rhs_stack;  // Stack for identifying cosets
      size_t                    _steps;
      size_t                    _stop_packing;  // TODO(JDM): make this a bool?
      RecVec<class_index_t>     _table;
      bool                      _tc_done;  // Has Todd-Coxeter been completed?
    };                                     // class TC
  };                                       // class Congruence

  // Define static data members
  template <typename TElementType,
            typename TElementHash,
            typename TElementEqual>
  size_t const Congruence<TElementType, TElementHash, TElementEqual>::INFTY
      = std::numeric_limits<size_t>::max();

  template <typename TElementType,
            typename TElementHash,
            typename TElementEqual>
  size_t const Congruence<TElementType, TElementHash, TElementEqual>::UNDEFINED
      = std::numeric_limits<size_t>::max();
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_CONG_H_

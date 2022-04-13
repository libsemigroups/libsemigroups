//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021 Ewan Gilligan
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

// This file contains implementations of the member functions for the
// FroidurePinParallel class.

#include "algorithm"      // for std::lower_bound
#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "report.hpp"     // for REPORT
#include "thread"         // for std::thread

#ifndef LIBSEMIGROUPS_FROIDURE_PIN_PARALLEL_IMPL_HPP_
#define LIBSEMIGROUPS_FROIDURE_PIN_PARALLEL_IMPL_HPP_

#define TEMPLATE template <typename TElementType, typename TTraits>
#define FROIDURE_PIN_PARALLEL FroidurePinParallel<TElementType, TTraits>
#define BUCKET FROIDURE_PIN_PARALLEL::Bucket

#define VOID TEMPLATE void
#define SIZE_T TEMPLATE size_t
#define SIZE_TYPE TEMPLATE typename FROIDURE_PIN_PARALLEL::size_type
#define BOOL TEMPLATE bool
#define BUCKET_INDEX_TYPE \
  TEMPLATE typename FROIDURE_PIN_PARALLEL::bucket_index_type
#define BUCKET_POSITION_TYPE \
  TEMPLATE typename FROIDURE_PIN_PARALLEL::bucket_position_type
#define PAR_ELEMENT_INDEX_TYPE \
  TEMPLATE typename FROIDURE_PIN_PARALLEL::element_index_type
#define PAR_LETTER_TYPE TEMPLATE letter_type
#define PAR_CONST_REFERENCE \
  TEMPLATE typename typename FROIDURE_PIN_PARALLEL::const_reference
#define PAR_ELEMENT_TYPE TEMPLATE typename FROIDURE_PIN_PARALLEL::element_type
#define PAR_CAYLEY_GRAPH_TYPE \
  TEMPLATE typename FROIDURE_PIN_PARALLEL::cayley_graph_type
#define PAR_WORD_TYPE TEMPLATE word_type

// Potentially don't check for one
// Use struct instead of seperate arrays for final, first, etc

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // FroidurePinParallel - constructors + destructor - public
  ////////////////////////////////////////////////////////////////////////
  TEMPLATE
  FROIDURE_PIN_PARALLEL::FroidurePinParallel(size_t nr_threads)
      : detail::BruidhinnTraits<TElementType>(),
        Runner(),
        _buckets(),
        _gens(),
        _pos_one(),
        _found_one(),
        _id(),
        _degree(UNDEFINED),
        _gens_lookup(),
        _duplicate_gens(),
        _nr_threads(nr_threads),
        _sorted(),
        _started(false),
        _nr(0),
        _global_lenindex() {
    if (nr_threads == 0) {
      LIBSEMIGROUPS_EXCEPTION("number of threads must be non-zero");
    }

    for (size_type i = 0; i < _nr_threads; ++i) {
      _buckets.push_back(new Bucket(this, i));
    }
  }

  TEMPLATE
  template <typename T>
  FROIDURE_PIN_PARALLEL::FroidurePinParallel(T const &first,
                                             T const &last,
                                             size_t   nr_threads)
      : FroidurePinParallel(nr_threads) {
    add_generators(first, last);
  }

  TEMPLATE
  FROIDURE_PIN_PARALLEL::FroidurePinParallel(std::vector<element_type> gens,
                                             size_t nr_threads)
      : FroidurePinParallel(nr_threads) {
    add_generators(gens.begin(), gens.end());
  }

  TEMPLATE
  FROIDURE_PIN_PARALLEL::~FroidurePinParallel() {
    for (auto bucket : _buckets) {
      delete bucket;
    }
    if (number_of_generators() > 0) {
      this->internal_free(_tmp_product);
      this->internal_free(_id);
    }
    // Delete the generators not in buckets._elements, i.e. the duplicat ones
    for (auto &x : _duplicate_gens) {
      this->internal_free(_gens[x.first]);
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // FroidurePinParallel - member functions - public
  ////////////////////////////////////////////////////////////////////////

  VOID FROIDURE_PIN_PARALLEL::add_generator(const_reference x) {
    add_generators(&x, &x + 1);
  }

  TEMPLATE
  template <typename T>
  void FROIDURE_PIN_PARALLEL::add_generators(T const &first, T const &last) {
    // Can only add generators before running
    if (_started) {
      LIBSEMIGROUPS_EXCEPTION("cannot add generators, the FroidurePinParallel "
                              "instance has already began running")
    }
    validate_element_collection(first, last);
    size_t const m = std::distance(first, last);

    if (m != 0) {
      init_degree(*first);
    }

    // First add to generator set
    for (auto it_coll = first; it_coll < last; ++it_coll) {
      _gens.push_back(this->internal_copy(this->to_internal_const(*it_coll)));
      // Determine which bucket this generator should be in
      size_t bid = get_bucket_id(this->to_internal_const(*it_coll));
      auto   it  = _buckets[bid]->_map.find(this->to_internal_const(*it_coll));
      if (it != _buckets[bid]->_map.end()) {  // Duplicate generator
        // TODO add rule for duplciate generator
        bucket_position_type pair = std::make_pair(bid, it->second);
        _gens_lookup.push_back(pair);
        _duplicate_gens.push_back(std::make_pair(_gens.size() - 1, pair));
      } else {
        _buckets[bid]->add_generator(_gens.size() - 1);
        _nr++;
      }
    }

    // Add column for each generator in each bucket.
    for (auto bucket : _buckets) {
      bucket->_right.add_cols(m);
      bucket->_left.add_cols(m);
      bucket->_reduced.add_cols(m);
    }
  }

  SIZE_T FROIDURE_PIN_PARALLEL::size() {
    run();
    return _nr;
  }

  BOOL FROIDURE_PIN_PARALLEL::finished_impl() const {
    return !running() && is_done();
  }

  VOID FROIDURE_PIN_PARALLEL::run_impl() {
    // Can now assume all generators have been added
    if (!_started) {
      // Expand each bucket
      for (auto bucket : _buckets) {
        bucket->expand(false);
      }
      // Set initial global index bounds.
      _global_lenindex.push_back(0);
      for (size_type i = 1; i < _nr_threads; ++i) {
        // We store successive ranges for each bucket
        _global_lenindex.push_back(_global_lenindex[i - 1]
                                   + _buckets[i - 1]->_nr);
      }
      _started = true;
    }

    std::vector<std::thread> bucket_threads(_nr_threads);

    // Multiply words of length > 1 by every generator
    while (!is_done()) {
      // Apply generators to each bucket element
      for (size_type i = 0; i < _nr_threads; ++i) {
        bucket_threads[i]
            = std::thread(&FroidurePinParallel::apply_generators, this, i);
      }
      // Wait till all buckets have completed the words of a given length
      for (size_t i = 0; i < _nr_threads; ++i) {
        bucket_threads[i].join();
      }
      // Process the queue of each bucket
      for (size_t i = 0; i < _nr_threads; ++i) {
        bucket_threads[i]
            = std::thread(&FroidurePinParallel::process_queue, this, i);
      }
      // Again wait till buckets have processed all elements
      for (size_t i = 0; i < _nr_threads; ++i) {
        bucket_threads[i].join();
      }

      // Check if we have enumerated words up to the next length
      // If so then we can process the left cayley graph and expand data
      // structures.
      auto move_on = true;
      for (auto bucket : _buckets) {
        if (bucket->_pos != bucket->_lenindex[bucket->_wordlen + 1]) {
          move_on = false;
          break;
        }
      }
      if (move_on) {
        // Process the left cayley graph
        for (size_type i = 0; i < _nr_threads; ++i) {
          bucket_threads[i] = std::thread(
              &FroidurePinParallel::process_left_cayley_graph, this, i);
        }
        for (size_type i = 0; i < _nr_threads; ++i) {
          bucket_threads[i].join();
        }
        // Update global indexing
        // Cannot be done in parallel, as it relies on the ordering of the
        // buckets.
        for (auto bucket : _buckets) {
          // Determine how many new elements from each bucket we have
          _global_lenindex.push_back(_nr);
          size_type new_elements = bucket->_nr - bucket->_nr_shorter;
          _nr += new_elements;
        }
        // Expand the buckets for the newly found elements
        for (size_type i = 0; i < _nr_threads; ++i) {
          bucket_threads[i]
              = std::thread(&FroidurePinParallel::expand_bucket, this, i);
        }
        for (size_type i = 0; i < _nr_threads; ++i) {
          bucket_threads[i].join();
        }
        // Only stop runner at word length boundaries, as we need to update
        // global indexing.
        if (stopped()) {
          break;
        }
      }
    }
#ifdef LIBSEMIGROUPS_VERBOSE
    std::vector<size_t> bucket_distribution;
    // How the elements were distributed across buckets.
    REPORT_DEFAULT("distribution of elements into buckets\n");
    for (size_type i = 0; i < _nr_threads; ++i) {
      REPORT_DEFAULT("bucket %d - %d", i, _buckets[i]->_nr);
    }
#endif
  }

  ////////////////////////////////////////////////////////////////////////
  // FroidurePinParallel - member functions - private
  ////////////////////////////////////////////////////////////////////////
  BOOL FROIDURE_PIN_PARALLEL::is_done() const {
    for (auto bucket : _buckets) {
      if (!bucket->is_done()) {
        return false;
      }
    }
    return true;
  }

  VOID FROIDURE_PIN_PARALLEL::
      is_one(internal_const_element_type x, bucket_position_type pos) noexcept(
          std::is_nothrow_default_constructible<InternalEqualTo>::value
              &&noexcept(std::declval<InternalEqualTo>()(x, x))) {
    if (!_found_one && InternalEqualTo()(x, _id)) {
      _pos_one   = pos;
      _found_one = true;
    }
  }

  SIZE_T
  FROIDURE_PIN_PARALLEL::get_bucket_id(internal_const_element_type x) {
    size_t hash = InternalHash()(x);
    // Reverse the hash as some values will have many zeros at the low end.
    size_t rev_hash = 0;
    while (hash > 0) {
      rev_hash <<= 1;
      rev_hash |= hash & 1;
      hash >>= 1;
    }
    rev_hash = (rev_hash * 31) % ((1l << 31) - 1);
    return rev_hash % _nr_threads;
  }

  VOID FROIDURE_PIN_PARALLEL::init_degree(const_reference x) {
    if (_degree == UNDEFINED) {
      _degree      = Degree()(x);
      _id          = this->to_internal(One()(x));
      _tmp_product = this->to_internal(One()(x));
      // Each bucket has its own tmp_product
      for (auto bucket : _buckets) {
        bucket->_tmp_product = this->internal_copy(_tmp_product);
      }
    }
  }

  VOID FROIDURE_PIN_PARALLEL::validate_element(const_reference x) const {
    size_t const n = Degree()(x);
    if (degree() != UNDEFINED && n != degree()) {
      LIBSEMIGROUPS_EXCEPTION(
          "element has degree %d but should have degree %d", n, degree());
    }
  }

  TEMPLATE
  template <typename T>
  void FROIDURE_PIN_PARALLEL::validate_element_collection(T const &first,
                                                          T const &last) const {
    if (degree() == UNDEFINED && std::distance(first, last) != 0) {
      auto const n = Degree()(*first);
      for (auto it = first + 1; it < last; ++it) {
        auto const m = Degree()(*it);
        if (m != n) {
          LIBSEMIGROUPS_EXCEPTION(
              "element has degree %d but should have degree %d", n, m);
        }
      }
    } else {
      for (auto it = first; it < last; ++it) {
        validate_element(*it);
      }
    }
  }

  VOID FROIDURE_PIN_PARALLEL::validate_global_element_index(
      element_index_type i) const {
    if (i >= _nr) {
      LIBSEMIGROUPS_EXCEPTION(
          "element index out of bounds, expected value in [0, %d), got %d",
          _nr,
          i);
    }
  }

  VOID FROIDURE_PIN_PARALLEL::validate_bucket_element_index(
      bucket_position_type i) const {
    if (i.first >= _nr_threads) {
      LIBSEMIGROUPS_EXCEPTION(
          "bucket index out of bounds, expected value in [0, %d), got %d",
          _nr_threads,
          i.first);
    }
    if (i.second >= _buckets[i.first]._nr) {
      LIBSEMIGROUPS_EXCEPTION("element index out of bounds for bucket %d, "
                              "expected value in [0, %d), got %d",
                              i.first,
                              _buckets[i.first]._nr,
                              i.second);
    }
  }

  VOID FROIDURE_PIN_PARALLEL::init_sorted() {
    if (_sorted.size() == size()) {
      return;
    }
    size_t n = size();
    _sorted.reserve(n);
    // Fill with elements
    // Keep track of the current index for each bucket.
    std::vector<element_index_type> bucket_counter(_nr_threads, 0);
    bucket_index_type               bucket_idx      = 0;
    size_type                       partition_index = 0;
    for (element_index_type i = 0; i < n; i++) {
      // We advance till we are in the correct bucket for this element
      while (partition_index < _global_lenindex.size()
             && i == _global_lenindex[partition_index + 1]) {
        bucket_idx = (bucket_idx + 1) % _nr_threads;
        partition_index++;
      }
      _sorted.emplace_back(
          _buckets[bucket_idx]->_elements[bucket_counter[bucket_idx]], i);
      bucket_counter[bucket_idx]++;
    }
    std::sort(
        _sorted.begin(),
        _sorted.end(),
        [this](std::pair<internal_element_type, element_index_type> const &x,
               std::pair<internal_element_type, element_index_type> const &y)
            -> bool {
          return Less()(this->to_external_const(x.first),
                        this->to_external_const(y.first));
        });

    // Invert the permutation in _sorted[*].second
    std::vector<element_index_type> tmp_inverter;
    tmp_inverter.resize(n);
    for (element_index_type i = 0; i < n; i++) {
      tmp_inverter[_sorted[i].second] = i;
    }
    for (element_index_type i = 0; i < n; i++) {
      _sorted[i].second = tmp_inverter[i];
    }
  }

  VOID FROIDURE_PIN_PARALLEL::apply_generators(bucket_index_type i) {
    _buckets[i]->apply_generators();
  }

  VOID FROIDURE_PIN_PARALLEL::process_queue(bucket_index_type i) {
    _buckets[i]->process_queue();
  }

  VOID FROIDURE_PIN_PARALLEL::process_left_cayley_graph(bucket_index_type i) {
    _buckets[i]->process_left_cayley_graph();
  }

  VOID FROIDURE_PIN_PARALLEL::expand_bucket(bucket_index_type i) {
    _buckets[i]->expand();
  }

  PAR_ELEMENT_INDEX_TYPE
  FROIDURE_PIN_PARALLEL::to_global_index(bucket_position_type element) const {
    // First determine the word length of the given element
    bucket_index_type bucket_index = element.first;
    size_type         word_len     = std::distance(
        std::lower_bound(_buckets[bucket_index]._lenindex.begin(),
                         _buckets[bucket_index]._lenindex.end()),
        _buckets[bucket_index]._lenindex.begin());
    // Determine the offset in the bucket
    size_type bucket_offset
        = element.second - _buckets[bucket_index]._lenindex[word_len];
    return _global_lenindex[word_len * _nr_threads] + bucket_offset;
  }

  PAR_ELEMENT_TYPE
  FROIDURE_PIN_PARALLEL::from_global_index(element_index_type index) const {
    validate_global_element_index(index);
    // First determine the range this index is in.
    // Lower bound returns first element >= index, so this will either be the
    // end of the range or the start if this index is equal to the start.
    element_index_type idx = std::distance(
        std::lower_bound(
            _global_lenindex.begin(), _global_lenindex.end(), index),
        _global_lenindex.begin());
    if (_global_lenindex[idx] != index) {
      idx--;
    }
    // Index for each bucket stored in order, so modulo to work out which bucket
    bucket_index_type  bucket_index  = idx % _nr_threads;
    element_index_type bucket_offset = index - _global_lenindex[idx];
    // Also need the wordlength of the element
    size_type word_len = (idx - bucket_index) / _nr_threads;
    // TODO check off by one
    element_index_type bucket_pos
        = _buckets[bucket_index]._lenindex[word_len] + bucket_offset;
    return this->external_copy(
        this->to_external_const(_buckets[bucket_index]._elements[bucket_pos]));
  }

  ////////////////////////////////////////////////////////////////////////
  // FroidurePinParallel - iterators - public
  ////////////////////////////////////////////////////////////////////////
  TEMPLATE
  typename FROIDURE_PIN_PARALLEL::const_iterator_sorted
  FROIDURE_PIN_PARALLEL::cbegin_sorted() {
    init_sorted();
    return const_iterator_pair_first(_sorted.cbegin());
  }

  TEMPLATE
  typename FROIDURE_PIN_PARALLEL::const_iterator_sorted
  FROIDURE_PIN_PARALLEL::cend_sorted() {
    init_sorted();
    return const_iterator_pair_first(_sorted.cend());
  }

  ////////////////////////////////////////////////////////////////////////
  // FroidurePinParallel - iterators - private
  ////////////////////////////////////////////////////////////////////////

  TEMPLATE
  struct FROIDURE_PIN_PARALLEL::DerefPairFirst
      : detail::BruidhinnTraits<TElementType> {
    const_reference
    operator()(typename std::vector<
               std::pair<internal_element_type,
                         element_index_type>>::const_iterator const &it) const {
      return this->to_external_const((*it).first);
    }
  };

  TEMPLATE
  struct FROIDURE_PIN_PARALLEL::AddressOfPairFirst
      : detail::BruidhinnTraits<TElementType> {
    const_pointer
    operator()(typename std::vector<
               std::pair<internal_element_type,
                         element_index_type>>::const_iterator const &it) const {
      return &(this->to_external_const((*it).first));
    }
  };

  TEMPLATE
  struct FROIDURE_PIN_PARALLEL::IteratorPairFirstTraits
      : detail::ConstIteratorTraits<
            std::vector<std::pair<internal_element_type, element_index_type>>> {
    using value_type =
        typename FroidurePinParallel<TElementType, TTraits>::element_type;
    using const_reference =
        typename FroidurePinParallel<TElementType, TTraits>::const_reference;
    using const_pointer =
        typename FroidurePinParallel<TElementType, TTraits>::const_pointer;

    using Deref     = DerefPairFirst;
    using AddressOf = AddressOfPairFirst;
  };

  ////////////////////////////////////////////////////////////////////////
  // Bucket - constructors + destructor - public
  ////////////////////////////////////////////////////////////////////////

  TEMPLATE BUCKET::Bucket(FroidurePinParallel *fp_parallel, size_t bucket_id)
      : _fp_parallel(fp_parallel),
        _bucket_id(bucket_id),
        _elements(),
        _final(),
        _first(),
        _prefix(),
        _suffix(),
        _length(),
        _lenindex(),
        _map(),
        _left(0, 0, UNDEFINED_POS),
        _right(0, 0, UNDEFINED_POS),
        _tmp_product(),
        _wordlen(0),
        _reduced(),
        _nr(0),
        _nr_shorter(0),
        _pos(0),
        _queue_elements(_fp_parallel->_nr_threads,
                        std::vector<queue_element>()) {
    _lenindex.push_back(0);
    // TODO Reserve batch size in each queue
  }

  TEMPLATE
  BUCKET::~Bucket() {
    // Will only be initialised if we have at least one generator
    if (_fp_parallel->number_of_generators() > 0) {
      _fp_parallel->internal_free(_tmp_product);
    }
    for (auto &x : _elements) {
      _fp_parallel->internal_free(x);
    }
  }
  ////////////////////////////////////////////////////////////////////////
  // Bucket - member functions - public
  ////////////////////////////////////////////////////////////////////////
  BOOL BUCKET::is_done() const {
    return _pos >= _nr;
  }

  VOID BUCKET::apply_generators() {
    size_t tid = THREAD_ID_MANAGER.tid(std::this_thread::get_id());
    // TODO stop condition
    // TODO reconsider batch size
    while (_pos != _lenindex[_wordlen + 1]
           && _queue_elements[0].size() < _fp_parallel->batch_size()) {
      element_index_type   i = _index[_pos];
      letter_type          b = _first[i];
      bucket_position_type s = _suffix[i];
      // Apply generators to element at _pos
      for (letter_type j = 0; j != _fp_parallel->number_of_generators(); ++j) {
        if (_wordlen > 0 && !reduced_get(s, j)) {
          bucket_position_type r = right_get(s, j);
          // r is the identity
          if (_fp_parallel->_found_one
              && r == _fp_parallel->_pos_one) {  // r is the identity
            _right.set(i, j, _fp_parallel->_gens_lookup[b]);
            continue;
          } else if (length_get(r) > 1) {  // r is not a generator
            bucket_position_type w = left_get(prefix_get(r), b);
            if (w.first == _bucket_id
                && _right.get(w.second, final_get(r)) != UNDEFINED_POS) {
              _right.set(i, j, _right.get(w.second, final_get(r)));
              continue;
            } else if (length_get(w) < _wordlen + 1) {
              _right.set(i, j, right_get(w, final_get(r)));
              continue;
            }
            // Otherwise we need to compute the product.
          } else {
            _right.set(
                i, j, right_get(_fp_parallel->_gens_lookup[b], final_get(r)));
            continue;
          }
        }
        InternalProduct()(
            _fp_parallel->to_external(_tmp_product),
            _fp_parallel->to_external_const(_elements[i]),
            _fp_parallel->to_external_const(_fp_parallel->_gens[j]),
            tid);
        // Determine which bucket this element should be in
        size_t bid = _fp_parallel->get_bucket_id(_tmp_product);
        auto   it  = _fp_parallel->_buckets[bid]->_map.find(_tmp_product);

        if (it != _map.end()) {  // Already seen this element
          _right.set(i, j, std::make_pair(bid, it->second));
          // TODO add rule
        } else {  // New element
          // Add the new element to the queue for the relevent bucket
          bucket_position_type pair = std::make_pair(_bucket_id, i);
          // Add to the queue for the bucket this element should be in
          struct queue_element new_element;
          new_element._element = _fp_parallel->internal_copy(_tmp_product);
          new_element._prefix  = pair;
          new_element._final   = j;
          new_element._first   = _first[i];

          if (_wordlen > 0) {
            new_element._suffix = right_get(s, j);
          } else {
            new_element._suffix = _fp_parallel->_gens_lookup[j];
          }
          _queue_elements[bid].push_back(std::move(new_element));
        }
      }  // Finished applying generators to element _pos
      _pos++;
    }  // Finished words of length <wordlen> + 1
  }

  VOID BUCKET::process_queue() {
    for (auto bucket : _fp_parallel->_buckets) {
      for (size_t k = 0; k < bucket->_queue_elements[_bucket_id].size(); k++) {
        // Take element from the queue
        queue_element new_element = bucket->_queue_elements[_bucket_id][k];

        internal_element_type x = new_element._element;
        bucket_position_type  i = new_element._prefix;
        letter_type           j = new_element._final;

        auto it = _map.find(x);
        if (it != _map.end()) {
          right_set(i, j, std::make_pair(_bucket_id, it->second));
          // Free element as it is not needed.
          _fp_parallel->internal_free(x);
        } else {
          bucket_position_type pair = std::make_pair(_bucket_id, _nr);
          _fp_parallel->is_one(x, pair);

          _first.push_back(new_element._first);
          _suffix.push_back(new_element._suffix);

          reduced_set(i, j);
          right_set(i, j, pair);

          // Set the bucket internal data
          _elements.push_back(x);
          _final.push_back(j);
          _index.push_back(_nr);
          _length.push_back(_wordlen + 2);
          _map.insert(std::make_pair(_elements.back(), _nr));
          _prefix.push_back(i);
          _nr++;
        }
      }
      // Clear the queues
      bucket->_queue_elements[_bucket_id].clear();
    }
  }

  VOID BUCKET::process_left_cayley_graph() {
    // If current words are not generators
    if (_wordlen > 0) {
      for (element_index_type i = _lenindex[_wordlen]; i != _pos; ++i) {
        bucket_position_type p = _prefix[i];
        letter_type          b = _final[i];
        for (letter_type j = 0; j < _fp_parallel->number_of_generators(); ++j) {
          _left.set(i, j, right_get(left_get(p, j), b));
        }
      }
    } else {  // Otherwise we're dealing with a generator
      for (element_index_type i = 0; i != _pos; ++i) {
        letter_type b = _final[i];
        for (letter_type j = 0; j < _fp_parallel->number_of_generators(); ++j) {
          _left.set(i, j, right_get(_fp_parallel->_gens_lookup[j], b));
        }
      }
    }
  }

  VOID BUCKET::expand(bool increase_wordlen) {
    if (increase_wordlen) {
      _wordlen++;
    }
    auto expand_amount = _nr - _nr_shorter;
    _left.add_rows(expand_amount);
    _right.add_rows(expand_amount);
    _reduced.add_rows(expand_amount);
    // TODO check if this is just _nr + 1
    _lenindex.push_back(_index.size());
    _nr_shorter = _nr;
  }

  VOID BUCKET::add_generator(element_index_type i) {
    bucket_position_type pos = std::make_pair(_bucket_id, _nr);
    _fp_parallel->is_one(_fp_parallel->_gens[i], pos);
    _fp_parallel->_gens_lookup.push_back(pos);

    _elements.push_back(_fp_parallel->_gens[i]);
    _first.push_back(i);
    _final.push_back(i);
    _length.push_back(1);
    _map.insert(std::make_pair(_elements.back(), _nr));
    _prefix.push_back(UNDEFINED_POS);
    _suffix.push_back(UNDEFINED_POS);
    _index.push_back(_nr);
    _nr++;
  }

  ////////////////////////////////////////////////////////////////////////
  // Bucket - member functions - private
  ////////////////////////////////////////////////////////////////////////
  BOOL BUCKET::reduced_get(bucket_position_type position, letter_type gen) {
    return _fp_parallel->_buckets[position.first]->_reduced.get(position.second,
                                                                gen);
  }

  VOID BUCKET::reduced_set(bucket_position_type position, letter_type gen) {
    _fp_parallel->_buckets[position.first]->_reduced.set(
        position.second, gen, true);
  }

  PAR_LETTER_TYPE BUCKET::final_get(bucket_position_type position) {
    return _fp_parallel->_buckets[position.first]->_final[position.second];
  }

  BUCKET_POSITION_TYPE BUCKET::prefix_get(bucket_position_type position) {
    return _fp_parallel->_buckets[position.first]->_prefix[position.second];
  }

  BUCKET_POSITION_TYPE BUCKET::suffix_get(bucket_position_type position) {
    return _fp_parallel->_buckets[position.first]->_suffix[position.second];
  }

  SIZE_TYPE BUCKET::length_get(bucket_position_type position) {
    return _fp_parallel->_buckets[position.first]->_length[position.second];
  }

  BUCKET_POSITION_TYPE BUCKET::left_get(bucket_position_type position,
                                        letter_type          gen) {
    return _fp_parallel->_buckets[position.first]->_left.get(position.second,
                                                             gen);
  }

  BUCKET_POSITION_TYPE BUCKET::right_get(bucket_position_type position,
                                         letter_type          gen) {
    return _fp_parallel->_buckets[position.first]->_right.get(position.second,
                                                              gen);
  }
  VOID BUCKET::right_set(bucket_position_type position,
                         letter_type          gen,
                         bucket_position_type elem_position) {
    _fp_parallel->_buckets[position.first]->_right.set(
        position.second, gen, elem_position);
  }

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_FROIDURE_PIN_PARALLEL_IMPL_HPP_
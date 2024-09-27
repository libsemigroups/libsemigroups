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

#include <cstddef>        // for size_t
#include <iterator>       // for reverse_iterator
#include <memory>         // for shared_ptr, make_shared
#include <mutex>          // for mutex
#include <thread>         // for thread
#include <type_traits>    // for is_const, remove_pointer
#include <unordered_map>  // for unordered_map
#include <utility>        // for pair
#include <vector>         // for vector

#include "adapters.hpp"          // for Complexity, Degree, IncreaseDegree
#include "bruidhinn-traits.hpp"  // for detail::BruidhinnTraits
#include "constants.hpp"         // For UNDEFINED
#include "containers.hpp"        // for DynamicArray2
#include "debug.hpp"             // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"         // for LIBSEMIGROUPS_EXCEPTION
#include "froidure-pin.hpp"      // for FroidurePinTraits
#include "iterator.hpp"          // for ConstIteratorStateless
#include "runner.hpp"            // for Runner
#include "stl.hpp"               // for EqualTo, Hash
#include "types.hpp"             // for letter_type, word type

#ifndef LIBSEMIGROUPS_FROIDURE_PIN_PARALLEL_HPP_
#define LIBSEMIGROUPS_FROIDURE_PIN_PARALLEL_HPP_

// TODO
// rules

namespace libsemigroups {

  // TODO link to parallel paper
  // TODO code example

  //! Defined in ``froidure-pin-parallel.hpp``.
  //!
  //! The class template FroidurePinParallel implements a concurrent version of
  //! the Froidure-Pin algorithm as described in the article "Algorithms for
  //! computing finite semigroups" by Veronique Froidure and Jean-Eric Pin; see
  //! [this] for more details.
  //!
  //! A FroidurePinParallel instance is defined by a generating set and a number
  //! of threads, and the main function is \ref run, which implements the
  //! [Froidure-Pin Algorithm].  If \ref run is invoked and
  //! \ref finished returns \c true, then the size, the left and right
  //! Cayley graphs are determined, and a confluent terminating presentation
  //! for the semigroup is known.
  //!
  //! \tparam TElementType the type of the elements in the represented
  //! semigroup
  //!
  //! \tparam TTraits a traits class holding various adapters used by the
  //! implementation (defaults to FroidurePinTraits).
  //!
  //! \sa FroidurePinTraits.
  //!
  //! [this]: https://www.irif.fr/~jep/PDF/Rio.pdf
  //! [Froidure-Pin Algorithm]: https://www.irif.fr/~jep/PDF/Rio.pdf
  //!
  template <typename TElementType,
            typename TTraits = FroidurePinTraits<TElementType>>
  class FroidurePinParallel final
      : private detail::BruidhinnTraits<TElementType>,
        public Runner {
   private:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePinParallel - typedefs - private
    ////////////////////////////////////////////////////////////////////////

    using internal_element_type =
        typename detail::BruidhinnTraits<TElementType>::internal_value_type;
    using internal_const_element_type = typename detail::BruidhinnTraits<
        TElementType>::internal_const_value_type;
    using internal_const_reference = typename detail::BruidhinnTraits<
        TElementType>::internal_const_reference;

    static_assert(
        std::is_const<internal_const_element_type>::value
            || std::is_const<typename std::remove_pointer<
                internal_const_element_type>::type>::value,
        "internal_const_element_type must be const or pointer to const");

    // This alias is used to distinguish variables that refer to positions in
    // _elements (element_index_type) from those that refer to positions in
    // _enumerate_order (enumerate_index_type).

    using size_type            = size_t;
    using enumerate_index_type = size_type;
    using element_index_type   = size_type;

   public:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePinParallel - typedefs - public
    ////////////////////////////////////////////////////////////////////////

    //! Type of the elements.
    using element_type =
        typename detail::BruidhinnTraits<TElementType>::value_type;

    //! Type of const elements.
    using const_element_type =
        typename detail::BruidhinnTraits<TElementType>::const_value_type;

    //! Type of element const references.
    using const_reference =
        typename detail::BruidhinnTraits<TElementType>::const_reference;

    //! Type of element references.
    using reference = typename detail::BruidhinnTraits<TElementType>::reference;

    //! Type of element const pointers.
    using const_pointer =
        typename detail::BruidhinnTraits<TElementType>::const_pointer;

    //! Type of the state used for multiplication (if any).
    using state_type = typename TTraits::state_type;

    //! \copydoc libsemigroups::Complexity
    using Complexity = typename TTraits::Complexity;

    //! \copydoc libsemigroups::Degree
    using Degree = typename TTraits::Degree;

    //! \copydoc libsemigroups::EqualTo
    using EqualTo = typename TTraits::EqualTo;

    //! \copydoc libsemigroups::Hash
    using Hash = typename TTraits::Hash;

    //! \copydoc libsemigroups::IncreaseDegree
    using IncreaseDegree = typename TTraits::IncreaseDegree;

    //! \copydoc libsemigroups::Less
    using Less = typename TTraits::Less;

    //! \copydoc libsemigroups::One
    using One = typename TTraits::One;

    //! \copydoc libsemigroups::Product
    using Product = typename TTraits::Product;

    //! \copydoc libsemigroups::Swap
    using Swap = typename TTraits::Swap;

    struct InternalEqualTo : private detail::BruidhinnTraits<TElementType> {
      bool operator()(internal_const_reference x,
                      internal_const_reference y) const {
        return EqualTo()(this->to_external_const(x),
                         this->to_external_const(y));
      }
    };

    struct InternalHash : private detail::BruidhinnTraits<TElementType> {
      size_t operator()(internal_const_reference x) const {
        return Hash()(this->to_external_const(x));
      }
    };

    using map_type = std::unordered_map<internal_const_element_type,
                                        element_index_type,
                                        InternalHash,
                                        InternalEqualTo>;

    struct InternalProduct {
      template <typename SFINAE = void>
      auto operator()(reference       xy,
                      const_reference x,
                      const_reference y,
                      size_t          tid = 0) -> void {
        Product()(xy, x, y, tid);
      }
    };

    using bucket_index_type = size_t;
    // Element positions are stored as a pair, where the first element is the
    // bucket index and the second is the position in the bucket elements.
    using bucket_position_type
        = std::pair<bucket_index_type, element_index_type>;
    using cayley_graph_type = detail::DynamicArray2<bucket_position_type>;

    ////////////////////////////////////////////////////////////////////////
    // FroidurePinParallel - constructors + destructor - public
    ////////////////////////////////////////////////////////////////////////
    FroidurePinParallel() = delete;
    //! Construct by specifying the number of threads.
    //!
    //! This function constructs a FroidurePinParallel instance that will use \p
    //! nr_threads when ran.
    //!
    //! \param nr_threads the number of threads this instance will use.
    //!
    //! \throws LibsemigroupsException if \p nr_threads is zero
    //!
    FroidurePinParallel(size_t nr_threads);
    template <typename T>

    //! Construct from generator iterator.
    //!
    //! This function constructs a FroidurePinParallel instance generated by
    //! the specified iterator of generators. There can be duplicate generators,
    //! and although they do not count as distinct elements, they do count as
    //! distinct generators. In other words, the generators are precisely (a
    //! copy of) the generators between \p first and \p last in the same order
    //! they occur. This instance will use \p nr_threads when ran.
    //!
    //! \param nr_threads the number of threads this instance will use.
    //!
    //! \throws LibsemigroupsException if any of the following hold:
    //! * \p nr_threads is zero;
    //! * Degree`{}(x) != `Degree`{}(y)` for some \c x and \c y between \p
    //! first and \p last .
    //!
    explicit FroidurePinParallel(T const &first,
                                 T const &last,
                                 size_t   nr_threads);
    //! Construct from generator iterator.
    //!
    //! This function constructs a FroidurePinParallel instance generated by
    //! the specified iterator of generators. There can be duplicate generators,
    //! and although they do not count as distinct elements, they do count as
    //! distinct generators. In other words, the generators are precisely (a
    //! copy of) the generators between \p first and \p last in the same order
    //! they occur. This instance will use \p nr_threads when ran.
    //!
    //! \param nr_threads the number of threads this instance will use.
    //!
    //! \throws LibsemigroupsException if any of the following hold:
    //! * \p nr_threads is zero;
    //! * Degree`{}(x) != `Degree`{}(y)` for some \c x and \c y in \p gens.
    //!
    explicit FroidurePinParallel(std::vector<element_type> gens,
                                 size_t                    nr_threads);
    ~FroidurePinParallel();

   public:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePinParallel - member functions - public
    ////////////////////////////////////////////////////////////////////////

    //! Add a copy of an element to the generators.
    //!
    //! This  function can be used to add new generators to an existing
    //! FroidurePinParallel instance that has not been ran yet. Once the
    //! FroidurePinParallel instance has been ran, it is no longer possible to
    //! add new generators.
    //!
    //! \param x the generator to add.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if any of the following hold:
    //! * started() returs \c true
    //! * the degree of \p x is incompatible with the existing degree.
    void add_generator(const_reference x);

    //! Add collection of generators via iterators.
    //!
    //! See \ref add_generator for a detailed description.
    //!
    //! \tparam the type of an iterator pointing to an \ref element_type.
    //!
    //! \param first iterator pointing to the first generator to add.
    //! \param last iterator pointing one past the last generator to add.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if any of the following hold:
    //! * started() returns \c true
    //! * the degree of \p x is incompatible with the existing degree.
    template <typename T>
    void   add_generators(T const &first, T const &last);
    size_t size();
    // Indexing functions

   private:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePinParallel - member functions - private
    ////////////////////////////////////////////////////////////////////////

    size_t number_of_generators() const {
      return _gens.size();
    }

    size_type degree() const {
      return _degree;
    }

    size_t batch_size() const noexcept {
      return _settings._batch_size;
    }

    void init_sorted();

    // Expand all the buckets
    void expand_buckets(bool increase_wordlen = true);
    // Run bucket function for a specific bucket
    void apply_generators(bucket_index_type id);
    void process_left_cayley_graph(bucket_index_type id);
    void process_queue(bucket_index_type id);
    void expand_bucket(bucket_index_type id);
    // We're done when all buckets are done
    bool is_done() const;
    void
    is_one(internal_const_element_type x, bucket_position_type pos) noexcept(
        std::is_nothrow_default_constructible<InternalEqualTo>::value
            &&noexcept(std::declval<InternalEqualTo>()(x, x)));

    size_t get_bucket_id(internal_const_element_type x);

    // Internal global indexing functions
    element_index_type to_global_index(bucket_position_type element) const;
    element_type       from_global_index(element_index_type index) const;

    ////////////////////////////////////////////////////////////////////////
    // FroidurePinParallel - validation member functions - private
    ////////////////////////////////////////////////////////////////////////
    void init_degree(const_reference);

    void validate_element(const_reference) const;

    void validate_global_element_index(element_index_type) const;

    void validate_bucket_element_index(bucket_position_type) const;

    template <typename T>
    void validate_element_collection(T const &, T const &) const;

   public:
    // Forward declarations - implemented in froidure-pin-parallel-impl.hpp
    //! No doc
    struct DerefPairFirst;

    //! No doc
    struct AddressOfPairFirst;

    //! No doc
    struct IteratorPairFirstTraits;

   private:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePinParallel - iterators - private
    ////////////////////////////////////////////////////////////////////////

    // A type for const iterators through (element, index) pairs of \c this.
    using const_iterator_pair_first
        = detail::ConstIteratorStateless<IteratorPairFirstTraits>;

    // A type for const reverse iterators through (element_type,
    // element_index_type) pairs of this.
    using const_reverse_iterator_pair_first
        = std::reverse_iterator<const_iterator_pair_first>;

   public:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePinParallel - iterators - public
    ////////////////////////////////////////////////////////////////////////

    //! Return type of \ref cbegin_sorted and \ref cend_sorted.
    //!
    //! A type for const random access iterators through the elements, sorted
    //! according to Less.
    //!
    //! \sa const_reverse_iterator_sorted.
    using const_iterator_sorted = const_iterator_pair_first;

    //! Returns a const iterator pointing one past the last element (sorted by
    //! Less).
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value of type \ref const_iterator_sorted.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note This function triggers a full enumeration.
    const_iterator_sorted cbegin_sorted();

    //! Returns a const iterator pointing one past the last element (sorted by
    //! Less).
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value of type \ref const_iterator_sorted.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note This function triggers a full enumeration.
    const_iterator_sorted cend_sorted();

   private:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePinParallel - Bucket - private
    ////////////////////////////////////////////////////////////////////////
    class Bucket {
      friend FroidurePinParallel;

      // Struct to store element information before it can be added to a bucket.
      struct queue_element {
        internal_element_type _element;
        bucket_position_type  _prefix;
        letter_type           _final;
        letter_type           _first;
        bucket_position_type  _suffix;
      };

      static constexpr std::pair<size_t, size_t> UNDEFINED_POS
          = std::make_pair(-1, -1);

     private:
      Bucket()                   = delete;
      Bucket(Bucket const &copy) = delete;
      Bucket(FroidurePinParallel *fp_parallel, size_t bucket_id);
      ~Bucket();

      void apply_generators();

      void process_queue();

      void process_left_cayley_graph();

      void expand(bool increase_wordlen = true);

      bool is_done() const;

      // Should only be used at the start
      // Cannot be run in parallel
      void add_generator(element_index_type gen);

      bool reduced_get(bucket_position_type position, letter_type gen);
      void reduced_set(bucket_position_type position, letter_type gen);

      letter_type          final_get(bucket_position_type position);
      bucket_position_type prefix_get(bucket_position_type position);
      bucket_position_type suffix_get(bucket_position_type position);
      size_type            length_get(bucket_position_type position);

      bucket_position_type left_get(bucket_position_type position,
                                    letter_type          j);

      bucket_position_type right_get(bucket_position_type position,
                                     letter_type          j);
      void                 right_set(bucket_position_type position,
                                     letter_type          j,
                                     bucket_position_type elem_position);

      FroidurePinParallel               *_fp_parallel;
      bucket_index_type                  _bucket_id;
      std::vector<internal_element_type> _elements;
      std::vector<letter_type>           _final;
      std::vector<letter_type>           _first;
      std::vector<bucket_position_type>  _prefix;
      std::vector<bucket_position_type>  _suffix;
      std::vector<size_type>             _length;
      std::vector<size_type>             _index;
      std::vector<element_index_type>    _lenindex;
      map_type                           _map;
      cayley_graph_type                  _left;
      cayley_graph_type                  _right;
      mutable internal_element_type      _tmp_product;
      size_type                          _wordlen;
      detail::DynamicArray2<bool>        _reduced;
      size_type                          _nr;
      size_type                          _nr_shorter;
      element_index_type                 _pos;

      std::vector<std::vector<queue_element>> _queue_elements;
    };

   private:
    void run_impl() override;
    bool finished_impl() const override;

    ////////////////////////////////////////////////////////////////////////
    // FroidurePinParallel - settings data - private
    ////////////////////////////////////////////////////////////////////////
    struct Settings {
      Settings() noexcept
          : _batch_size(8192),
            _concurrency_threshold(823543),
            _max_threads(std::thread::hardware_concurrency()),
            _immutable(false) {}
      Settings(Settings const &) noexcept = default;
      Settings(Settings &&) noexcept      = default;
      ~Settings()                         = default;
      size_t _batch_size;
      size_t _concurrency_threshold;
      size_t _max_threads;
      bool   _immutable;
    } _settings;

    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - data - private
    ////////////////////////////////////////////////////////////////////////
    std::vector<Bucket *>                                   _buckets;
    std::vector<internal_element_type>                      _gens;
    bucket_position_type                                    _pos_one;
    bool                                                    _found_one;
    internal_element_type                                   _id;
    size_type                                               _degree;
    std::vector<bucket_position_type>                       _gens_lookup;
    std::vector<std::pair<size_type, bucket_position_type>> _duplicate_gens;
    size_type                                               _nr_threads;
    std::vector<std::pair<internal_element_type, element_index_type>> _sorted;
    mutable internal_element_type _tmp_product;
    bool                          _started;
    // Store the global indices of elements so that we can determine which
    // bucket they belong to.
    size_type                       _nr;
    std::vector<element_index_type> _global_lenindex;
  };

  // Needed to allow UNDEFINED_POS to be declared in template class
  template <typename TElementType, typename TTraits>
  constexpr
      typename FroidurePinParallel<TElementType, TTraits>::bucket_position_type
          FroidurePinParallel<TElementType, TTraits>::Bucket::UNDEFINED_POS;

}  // namespace libsemigroups

// Include the implementation of the member functions for FroidurePinParallel
#include "froidure-pin-parallel-impl.hpp"
#endif  // LIBSEMIGROUPS_FROIDURE_PIN_PARALLEL_HPP_
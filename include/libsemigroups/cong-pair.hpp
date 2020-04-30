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

// This file contains the class template CongruenceByPairs for enumerating a
// congruence by attempting to find all pairs of related elements using brute
// force. This does not work very well in most cases, due to the high complexity
// of the approach. However, there are examples where this approach is the only
// one that works, such as, for example, the semigroup in the test
// KnuthBendixCongruenceByPairs 001.

// There is a fair amount of overlap between CongruenceByPairsHelper and
// FroidurePin. In particular, the need to wrangle all of the types, and the
// InternalProduct, InternalEqualTo, and InternalHash. The main aim of this
// class is to provide the derived class KnuthBendixByPairs, which is designed
// to run without fully enumerating the underlying FroidurePin instance. In
// particular, because this might be infinite. We therefore require all of the
// machinery to implemented in CongruenceByPairsHelper for KnuthBendixByPairs
// (specifically, we won't know element_index_type's for elements where the
// underlying FroidurePin<KBE> is infinite) and so we must be able to work
// directly with the KBE's themselves, rather than with their indices. It would
// be possible to improve CongruenceByPairsHelper, using:
//
// * use pairs of element_index_type and FroidurePin::fast_product
//   instead of the current setup, i.e. in this case, this would be a
//   CongruenceByPairs where the state was the given FroidurePin, the element
//   type would just be the index type of the FroidurePin, and product would
//   be passed the FroidurePin as the "state" parameter, and use fast_product
//   to compute the product.
//
// but given that CongruenceByPairsHelper is likely to be a terrible algorithm
// even with this optimisation, it does not seem worthwhile implementing the
// above change.
//
// TODO(later):
//
// 1. Check the performance of CongruenceByPairs against FroidurePin,
//    in the case of 2-sided congruences, since they compute the same things.
//    The principal difference will be that CongruenceByPairs possibly uses
//    less space by using union-find, but I'm not 100% sure about this, and it
//    would be a good idea to double check. Although it isn't important really
//    because the principal use case for this is for
//    KnuthBendixCongruenceByPairs, as mentioned above.

#ifndef LIBSEMIGROUPS_CONG_PAIR_HPP_
#define LIBSEMIGROUPS_CONG_PAIR_HPP_

#include <cstddef>        // for size_t
#include <memory>         // for shared_ptr
#include <queue>          // for queue
#include <unordered_map>  // for unordered_map
#include <unordered_set>  // for unordered_set
#include <utility>        // for pair
#include <vector>         // for vector

#include "bruidhinn-traits.hpp"  // for detail::BruidhinnTraits
#include "cong-intf.hpp"         // for CongruenceInterface::class_index_type
#include "cong-wrap.hpp"         // for CongruenceWrapper
#include "froidure-pin.hpp"      // for FroidurePin
#include "kbe.hpp"               // for detail::KBE
#include "knuth-bendix.hpp"      // for fpsemigroup::KnuthBendix
#include "libsemigroups-exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "types.hpp"                    // for word_type
#include "uf.hpp"                       // for UF

namespace libsemigroups {
  class FroidurePinBase;  // Forward declaration, for constructor parameter
  namespace detail {
    class TCE;
  }

  //! Defined in ``cong-pair.hpp``.
  //!
  //! This class contains an implementation of a brute force breadth first
  //! search algorithm for computing left, right, and 2-sided congruences on
  //! semigroups and monoids.
  //!
  //! This page contains a summary of the member functions of the class
  //! CongruenceByPairs, and related things in libsemigroups.
  //! CongruenceByPairsHelper is a helper class, CongruenceByPairs has
  //! identical functionality, please use CongruenceByPairs, but not
  //! CongruenceByPairsHelper since this will be deprecated in a later version
  //! of ``libsemigroups``.
  //!
  //! \sa congruence_type and tril.
  //!
  //! \par Example
  //! \code
  //! using namespace libsemigroups;
  //! auto      rg = ReportGuard();
  //! using Transf = typename TransfHelper<8>::type;
  //! FroidurePin<Transf> S({Transf({7, 3, 5, 3, 4, 2, 7, 7}),
  //!                        Transf({1, 2, 4, 4, 7, 3, 0, 7}),
  //!                        Transf({0, 6, 4, 2, 2, 6, 6, 4}),
  //!                        Transf({3, 6, 3, 4, 0, 6, 0, 7})});
  //!
  //! using P = CongruenceByPairs<decltype(S)>;
  //!
  //! P cong1(right, S);
  //! cong1.nr_classes();   // 11804
  //! P cong2(left, S);
  //! cong2.nr_classes();   // 11804
  //! P cong3(twosided, S);
  //! cong3.nr_classes();   // 11804
  //! \endcode

  // Implemented in cong-pair-impl.hpp
  template <typename TFroidurePinType>
  class CongruenceByPairsHelper : public CongruenceInterface,
                                  protected detail::BruidhinnTraits<
                                      typename TFroidurePinType::element_type> {
    static_assert(
        std::is_base_of<FroidurePinBase, TFroidurePinType>::value,
        "the template parameter must be derived from FroidurePinBase");

   public:
    ////////////////////////////////////////////////////////////////////////
    // CongruenceByPairsHelper - typedefs - public
    ////////////////////////////////////////////////////////////////////////

    //! No doc
    using froidure_pin_type = TFroidurePinType;

    //! The type of elements over which an instance of CongruenceByPairsHelper
    //! is defined.
    using element_type = typename froidure_pin_type::element_type;

    //! No doc
    using state_type = typename froidure_pin_type::state_type;

    //! The type of a const reference to an element_type.
    using const_reference = typename froidure_pin_type::const_reference;

    //! The type of a reference to an element_type.
    using reference = typename froidure_pin_type::reference;

    //! No doc
    using Hash = typename froidure_pin_type::Hash;

    //! No doc
    using EqualTo = typename froidure_pin_type::EqualTo;

    //! No doc
    using Product = typename froidure_pin_type::Product;

   protected:
    ////////////////////////////////////////////////////////////////////////
    // CongruenceByPairsHelper - typedefs - protected
    ////////////////////////////////////////////////////////////////////////
    //! No doc
    using internal_element_type =
        typename detail::BruidhinnTraits<element_type>::internal_value_type;

    ////////////////////////////////////////////////////////////////////////
    // CongruenceByPairsHelper - constructor - protected
    ////////////////////////////////////////////////////////////////////////
    // This is protected because it is not possible to create a
    // CongruenceByPairsHelper object without it being defined over a parent
    // semigroup.

    //! No doc
    explicit CongruenceByPairsHelper(congruence_type);

   private:
    ////////////////////////////////////////////////////////////////////////
    // CongruenceByPairsHelper - typedefs - private
    ////////////////////////////////////////////////////////////////////////
    using internal_const_element_type = typename detail::BruidhinnTraits<
        element_type>::internal_const_value_type;

    using internal_const_reference = typename detail::BruidhinnTraits<
        element_type>::internal_const_reference;

    template <typename T>
    struct IsState final
        : std::integral_constant<bool,
                                 !std::is_void<T>::value
                                     && std::is_same<state_type, T>::value> {};

   public:
    ////////////////////////////////////////////////////////////////////////
    // CongruenceByPairsHelper - constructor + destructor - public
    ////////////////////////////////////////////////////////////////////////

    //! No doc
    template <typename T,
              typename SFINAE
              = typename std::enable_if<IsState<T>::value, T>::type>
    CongruenceByPairsHelper(congruence_type type, std::shared_ptr<T> stt, bool)
        : CongruenceByPairsHelper(type) {
      _state = stt;
    }

    //! Construct a CongruenceByPairsHelper over the FroidurePin instance \p S
    //! representing a left/right/2-sided congruence according to \p type.
    //! \param type whether the congruence is left, right, or 2-sided
    //! \param S  a shared_ptr to the semigroup over which the congruence is
    //! defined.
    //!
    //! \throws LibsemigroupsException if \p type and the template parameter
    //! \p T are incompatible. Currently, this is when \p type is not
    //! `congruence_type::right` and `T::element_type` is TCE.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \note
    //! The FroidurePinBase pointed to by \p S is not copied
    CongruenceByPairsHelper(congruence_type                  type,
                            std::shared_ptr<FroidurePinBase> S) noexcept;

    //! Construct a CongruenceByPairsHelper over the FroidurePin instance \p fp
    //! representing a left/right/2-sided congruence according to \p type.
    //!
    //! \tparam T a class derived from FroidurePinBase. It is required that
    //! `T::element_type` is the same as CongruenceByPairsHelper::element_type
    //! (which is the same as the template parameter \p element_type).
    //!
    //! \param type whether the congruence is left, right, or 2-sided
    //! \param S  a reference to the semigroup over which the congruence is
    //! defined.
    //!
    //! \throws LibsemigroupsException if \p type and the template parameter
    //! \p T are incompatible. Currently, this is when \p type is not
    //! `congruence_type::right` and `T::element_type` is TCE.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \warning The parameter `T& S` is copied, this might be expensive, use
    //! a std::shared_ptr to avoid the copy!
    template <typename T>
    CongruenceByPairsHelper(congruence_type type, T const& S)
        : CongruenceByPairsHelper(type,
                                  static_cast<std::shared_ptr<FroidurePinBase>>(
                                      std::make_shared<T>(S))) {
      static_assert(std::is_base_of<FroidurePinBase, T>::value,
                    "the template parameter must be a derived class of "
                    "FroidurePinBase");
      static_assert(
          std::is_same<typename T::element_type, element_type>::value,
          "incompatible element_type's, the element_type of the FroidurePin "
          "instance must be the same as the CongruenceByPairsHelper "
          "element_type");
      if (type != congruence_type::right
          && std::is_same<typename T::element_type, detail::TCE>::value) {
        LIBSEMIGROUPS_EXCEPTION("Cannot create a left or 2-sided congruence "
                                "over a semigroup of TCE's");
      }
    }

    ~CongruenceByPairsHelper();

    //! A CongruenceByPairsHelper instance is not default-constructible.
    //! This constructor is deleted.
    CongruenceByPairsHelper() = delete;

    //! A default copy constructor.
    CongruenceByPairsHelper(CongruenceByPairsHelper const&) = default;

    //! A CongruenceByPairsHelper instance is not copy assignable.
    //! This constructor is deleted.
    CongruenceByPairsHelper& operator=(CongruenceByPairsHelper const&) = delete;

    //! A default move constructor.
    CongruenceByPairsHelper(CongruenceByPairsHelper&&) = default;

    //! A CongruenceByPairsHelper instance is not move assignable.
    //! This constructor is deleted.
    CongruenceByPairsHelper& operator=(CongruenceByPairsHelper&&) = delete;

   protected:
    ////////////////////////////////////////////////////////////////////////
    // CongruenceByPairsHelper - member functions - protected
    ////////////////////////////////////////////////////////////////////////

    //! No doc
    template <typename SFINAE = state_type>
    auto state() ->
        typename std::enable_if<IsState<SFINAE>::value, SFINAE&>::type {
      return *_state;
    }

    //! No doc
    void internal_add_pair(internal_element_type, internal_element_type);

    ////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - non-pure virtual member functions - protected
    ////////////////////////////////////////////////////////////////////////

    // non_trivial_classes_impl is private in CongruenceInterface but
    // protected here since KnuthBendixCongruenceByPairs uses it and
    // derives from class CongruenceByPairsHelper.
    //! No doc
    std::shared_ptr<non_trivial_classes_type const>
    non_trivial_classes_impl() override;

    ////////////////////////////////////////////////////////////////////////
    // Runner - pure virtual member functions - protected
    ////////////////////////////////////////////////////////////////////////

    //! No doc
    void run_impl() override;

    //! No doc
    bool finished_impl() const override;

   private:
    ////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - pure virtual member functions - private
    ////////////////////////////////////////////////////////////////////////

    word_type class_index_to_word_impl(class_index_type) override;
    size_t    nr_classes_impl() override;
    std::shared_ptr<FroidurePinBase> quotient_impl() override;
    class_index_type word_to_class_index_impl(word_type const&) override;

    ////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - non-pure virtual member functions - private
    ////////////////////////////////////////////////////////////////////////

    class_index_type const_word_to_class_index(word_type const&) const override;
    void             add_pair_impl(word_type const&, word_type const&) override;
    bool             is_quotient_obviously_finite_impl() override {
      return finished();
    }

    ////////////////////////////////////////////////////////////////////////
    // CongruenceByPairsHelper - member functions - private
    ////////////////////////////////////////////////////////////////////////

    size_t add_index(internal_element_type) const;
    void   delete_tmp_storage();
    size_t get_index(internal_element_type) const;
    void   init();

    ////////////////////////////////////////////////////////////////////////
    // CongruenceByPairsHelper - inner structs - private
    ////////////////////////////////////////////////////////////////////////

    struct InternalEqualTo : private detail::BruidhinnTraits<element_type> {
      bool operator()(internal_const_reference x,
                      internal_const_reference y) const {
        return EqualTo()(this->to_external_const(x),
                         this->to_external_const(y));
      }
    };

    struct InternalHash : private detail::BruidhinnTraits<element_type> {
      size_t operator()(internal_const_reference x) const {
        return Hash()(this->to_external_const(x));
      }
    };

    struct InternalProduct {
      template <typename SFINAE = void>
      auto operator()(reference       xy,
                      const_reference x,
                      const_reference y,
                      void*,
                      size_t tid = 0) ->
          typename std::enable_if<std::is_void<state_type>::value,
                                  SFINAE>::type {
        Product()(xy, x, y, tid);
      }

      template <typename SFINAE = void>
      auto operator()(reference       xy,
                      const_reference x,
                      const_reference y,
                      state_type*     stt,
                      size_t          tid = 0) ->
          typename std::enable_if<!std::is_void<state_type>::value,
                                  SFINAE>::type {
        Product()(xy, x, y, stt, tid);
      }
    };

    struct PHash {
     public:
      size_t
      operator()(std::pair<internal_const_element_type,
                           internal_const_element_type> const& pair) const {
        return InternalHash()(pair.first) + 17 * InternalHash()(pair.second);
      }
    };

    struct PEqual {
      size_t operator()(std::pair<internal_const_element_type,
                                  internal_const_element_type> pair1,
                        std::pair<internal_const_element_type,
                                  internal_const_element_type> pair2) const {
        return InternalEqualTo()(pair1.first, pair2.first)
               && InternalEqualTo()(pair1.second, pair2.second);
      }
    };

    ////////////////////////////////////////////////////////////////////////
    // CongruenceByPairsHelper - data - private
    ////////////////////////////////////////////////////////////////////////

    mutable std::vector<class_index_type> _class_lookup;
    std::unordered_set<std::pair<internal_element_type, internal_element_type>,
                       PHash,
                       PEqual>
                       _found_pairs;
    bool               _init_done;
    mutable detail::UF _lookup;
    mutable std::unordered_map<internal_element_type,
                               size_t,
                               InternalHash,
                               InternalEqualTo>
                             _map;
    mutable size_t           _map_next;
    mutable class_index_type _next_class;
    size_t                   _nr_non_trivial_classes;
    size_t                   _nr_non_trivial_elemnts;
    std::queue<std::pair<internal_element_type, internal_element_type>>
                                               _pairs_to_mult;
    mutable std::vector<internal_element_type> _reverse_map;
    std::shared_ptr<state_type>                _state;
    internal_element_type                      _tmp1;
    internal_element_type                      _tmp2;
  };

  // The following only exist for backwards compatibility, so that it is
  // possible to create a CongruenceByPairs where the template parameter is the
  // element type. TODO(2.0): remove these and rename CongruenceByPairsHelper
  // -> CongruenceByPairs.
  template <typename T, typename = void>
  class CongruenceByPairs;

  template <typename T>
  class CongruenceByPairs<
      T,
      typename std::enable_if<std::is_base_of<FroidurePinBase, T>::value>::type>
      : public CongruenceByPairsHelper<T> {
    using CongruenceByPairsHelper<T>::CongruenceByPairsHelper;
  };

  template <typename T>
  class CongruenceByPairs<
      T,
      typename std::enable_if<
          !std::is_base_of<FroidurePinBase, T>::value>::type>
      : public CongruenceByPairsHelper<FroidurePin<T>> {
    using CongruenceByPairsHelper<FroidurePin<T>>::CongruenceByPairsHelper;
  };

  //! Defined in ``cong-pair.hpp``.
  //!
  //! On this page, we describe the functionality relating to the brute force
  //! enumeration of pairs of elements belonging to a congruence (of any type)
  //! that is implemented in the class KnuthBendixCongruenceByPairs. This class
  //! is derived from CongruenceByPairs and implements the same algorithm. The
  //! only difference is that KnuthBendixCongruenceByPairs runs the Knuth-Bendix
  //! algorithm (see fpsemigroup::KnuthBendix) to completion, and then runs the
  //! brute force enumeration of pairs on the semigroup represented by the
  //! output of fpsemigroup::KnuthBendix. If the semigroup represented by the
  //! output of fpsemigroup::KnuthBendix is infinite, but the number of pairs
  //! belonging to the congruence represented by a KnuthBendixCongruenceByPairs
  //! instance is finite, then KnuthBendixCongruenceByPairs will terminate
  //! eventually, where as CongruenceByPairs would not (since it would attempt
  //! to enumerate the infinite semigroup represented by the output of
  //! fpsemigroup::KnuthBendix first).
  //!
  //! \sa congruence_type, tril, and CongruenceByPairs.
  //!
  //! \par Example
  //! \code
  //! fpsemigroup::KnuthBendix kb;
  //! kb.set_alphabet(3);
  //! kb.add_rule({0, 1}, {1, 0});
  //! kb.add_rule({0, 2}, {2, 0});
  //! kb.add_rule({0, 0}, {0});
  //! kb.add_rule({0, 2}, {0});
  //! kb.add_rule({2, 0}, {0});
  //! kb.add_rule({1, 2}, {2, 1});
  //! kb.add_rule({1, 1, 1}, {1});
  //! kb.add_rule({1, 2}, {1});
  //! kb.add_rule({2, 1}, {1});
  //!
  //! KnuthBendixCongruenceByPairs kbp(twosided, kb);
  //! kbp.add_pair({0}, {1});
  //!
  //! kbp.nr_non_trivial_classes();  // == 1
  //! kbp.cbegin_ntc()->size();      // == 5
  //! \endcode
  // Implemented in cong-pair.cpp.
  class KnuthBendixCongruenceByPairs
      : public CongruenceByPairsHelper<FroidurePin<
            detail::KBE,
            FroidurePinTraits<detail::KBE, fpsemigroup::KnuthBendix>>> {
    ////////////////////////////////////////////////////////////////////////
    // KnuthBendixCongruenceByPairs - typedefs - private
    ////////////////////////////////////////////////////////////////////////

    using KnuthBendix              = fpsemigroup::KnuthBendix;
    using CongruenceByPairsHelper_ = CongruenceByPairsHelper<
        FroidurePin<detail::KBE,
                    FroidurePinTraits<detail::KBE, fpsemigroup::KnuthBendix>>>;

   public:
    ////////////////////////////////////////////////////////////////////////
    // KnuthBendixCongruenceByPairs - constructors - public
    ////////////////////////////////////////////////////////////////////////
    //! Construct a KnuthBendixCongruenceByPairs over the
    //! fpsemigroup::KnuthBendix instance \p kb representing a
    //! left/right/2-sided congruence according to \p type.
    //!
    //! \param type whether the congruence is left, right, or 2-sided
    //! \param kb  a reference to the semigroup over which the congruence is
    //! defined.
    //!
    //! \par Exceptions
    //! This function is `noexcept` and is guaranteed never to throw.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \note
    //! The fpsemigroup::KnuthBendix referred to by \p kb is not copied.
    KnuthBendixCongruenceByPairs(congruence_type              type,
                                 std::shared_ptr<KnuthBendix> kb) noexcept;

    //! Construct a KnuthBendixCongruenceByPairs over the
    //! fpsemigroup::KnuthBendix instance \p kb representing a
    //! left/right/2-sided congruence according to \p type.  \param type
    //! whether the congruence is left, right, or 2-sided \param kb  a
    //! reference to the semigroup over which the congruence is defined.
    //!
    //! \par Exceptions
    //! This function is `noexcept` and is guaranteed never to throw.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \warning The parameter \p kb is copied, this might be expensive, use a
    //! std::shared_ptr to avoid the copy!
    KnuthBendixCongruenceByPairs(congruence_type    type,
                                 KnuthBendix const& kb) noexcept;

    //! Deleted.
    KnuthBendixCongruenceByPairs() = delete;

    //! Deleted.
    KnuthBendixCongruenceByPairs(KnuthBendixCongruenceByPairs const&) = delete;

    //! Deleted.
    KnuthBendixCongruenceByPairs(KnuthBendixCongruenceByPairs&&) = delete;

    //! Deleted.
    KnuthBendixCongruenceByPairs& operator=(KnuthBendixCongruenceByPairs const&)
        = delete;

    //! Deleted.
    KnuthBendixCongruenceByPairs& operator=(KnuthBendixCongruenceByPairs&&)
        = delete;

   private:
    ////////////////////////////////////////////////////////////////////////
    // Runner - pure virtual member functions - public
    ////////////////////////////////////////////////////////////////////////
    void run_impl() override;
    bool finished_impl() const override;

    ////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - non-pure virtual member functions - private
    ////////////////////////////////////////////////////////////////////////

    using CongruenceByPairsHelper_::non_trivial_classes_impl;
    // Override the method for the class CongruenceByPairs to avoid having to
    // know the parent semigroup (found as part of
    // KnuthBendixCongruenceByPairs::run) to add a pair.
    void add_pair_impl(word_type const&, word_type const&) override;
  };

  template <typename TFroidurePinType>
  //! This class is a wrapper for CongruenceByPairs that provides
  //! the FpSemigroupInterface.
  //!
  //! See CongruenceWrapper for further details.
  using FpSemigroupByPairs
      = CongruenceWrapper<CongruenceByPairs<TFroidurePinType>>;
}  // namespace libsemigroups

#include "cong-pair-impl.hpp"
#endif  // LIBSEMIGROUPS_CONG_PAIR_HPP_

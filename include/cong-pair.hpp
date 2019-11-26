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

// TODO(later):
//
// 1. A CongruenceByPairs should have a template parameter representing a
//    "state", for example:
//
//    * for TCE, we'd want to pass the table used to create the TCE, and use
//      this in the multiplication/product method (which would probably have to
//      be variadic, to allow for multiplying elements with and without state
//      passed as an argument). In this case, TCE's could cease to exist
//      altogether, and the element type could just be the smallest integer type
//      that could contain the number of rows in the "state" table. This would
//      have the added benefit that TCE's store a shared_ptr and the index, and
//      this would be reduced to just the index. Noting that the shared_ptr is
//      only ever used for the "generators", and no further elements, this is an
//      obvious inefficiency that would be resolved by this.
//
//    * for detail::KBE, we'd want to pass the KnuthBendix instance, and we'd
//      pass that when multiplying detail::KBE's. As in the previous case, we
//      wouldn't need detail::KBE's any more and could simply use std::string's
//      instead, again reducing memory usage. This would also mean that
//      KnuthBendixCongruenceByPairs could be reduced to just overriding the
//      add_pair_impl method.
//
//    * use pairs of element_index_type and FroidurePin::fast_product
//      instead of the current setup, i.e. in this case, this would be a
//      CongruenceByPairs where the state was the given FroidurePin, the element
//      type would just be the index type of the FroidurePin, and product would
//      be passed the FroidurePin as the "state" parameter, and use fast_product
//      to compute the product.
//
//      (Note that all of these comments apply to FroidurePinBase also).
//
// 2. Check the performance of CongruenceByPairs against FroidurePin,
//    in the case of 2-sided congruences, since they compute the same things.
//    The principal difference will be that CongruenceByPairs possibly uses
//    less space by using union-find, but I'm not 100% sure about this, and it
//    would be a good idea to double check.

#ifndef LIBSEMIGROUPS_INCLUDE_CONG_PAIR_HPP_
#define LIBSEMIGROUPS_INCLUDE_CONG_PAIR_HPP_

#include <cstddef>        // for size_t
#include <memory>         // for shared_ptr
#include <queue>          // for queue
#include <unordered_map>  // for unordered_map
#include <unordered_set>  // for unordered_set
#include <utility>        // for pair
#include <vector>         // for vector

#include "adapters.hpp"          // for Product
#include "bruidhinn-traits.hpp"  // for detail::BruidhinnTraits
#include "cong-intf.hpp"         // for CongruenceInterface::class_index_type
#include "cong-wrap.hpp"         // for CongruenceWrapper
#include "froidure-pin.hpp"      // for FroidurePin
#include "kbe.hpp"               // for detail::KBE
#include "knuth-bendix.hpp"      // for fpsemigroup::KnuthBendix
#include "tce.hpp"               // for TCE
#include "types.hpp"             // for word_type
#include "uf.hpp"                // for UF

namespace libsemigroups {
  class Element;          // Forward declaration, for default template parameter
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
  //! using P = CongruenceByPairs<decltype(S)::element_type>;
  //!
  //! P cong1(right, S);
  //! cong1.nr_classes();   // 11804
  //! P cong2(left, S);
  //! cong2.nr_classes();   // 11804
  //! P cong3(twosided, S);
  //! cong3.nr_classes();   // 11804
  //! \endcode
  // Implemented in cong-pair-impl.hpp
  // TODO(now) remove the first 4 template parameters
  template <typename TElementType,
            typename TElementHash     = Hash<TElementType>,
            typename TElementEqual    = EqualTo<TElementType>,
            typename TElementProduct  = Product<TElementType>,
            typename TFroidurePinType = FroidurePin<TElementType>>
  class CongruenceByPairs : public CongruenceInterface,
                            protected detail::BruidhinnTraits<TElementType> {
    static_assert(
        std::is_same<typename detail::BruidhinnTraits<TElementType>::value_type,
                     TElementType>::value,
        "detail::BruidhinnTraits and FroidurePinTraits should be the same");

   protected:
    ////////////////////////////////////////////////////////////////////////
    // CongruenceByPairs - typedefs - protected
    ////////////////////////////////////////////////////////////////////////
    //! No doc
    using Hash = TElementHash;

    //! No doc
    using EqualTo = TElementEqual;

    //! No doc
    using Product = TElementProduct;

    //! No doc
    using froidure_pin_type = TFroidurePinType;

    //! No doc
    using froidure_pin_parent_type = std::shared_ptr<TFroidurePinType>;

    //! No doc
    using internal_element_type =
        typename detail::BruidhinnTraits<TElementType>::internal_value_type;

    //! No doc
    using internal_const_element_type = typename detail::BruidhinnTraits<
        TElementType>::internal_const_value_type;

    //! No doc
    using internal_const_reference = typename detail::BruidhinnTraits<
        TElementType>::internal_const_reference;

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

   public:
    //! The type of elements over which an instance of CongruenceByPairs is
    //! defined.
    using element_type = TElementType;

    //! The type of a const reference to an element_type.
    using const_reference =
        typename detail::BruidhinnTraits<TElementType>::const_reference;

   protected:
    ////////////////////////////////////////////////////////////////////////
    // CongruenceByPairs - constructor - protected
    ////////////////////////////////////////////////////////////////////////
    // This is protected because it is not possible to create a
    // CongruenceByPairs object without it being defined over a parent
    // semigroup.

    //! No doc
    explicit CongruenceByPairs(congruence_type);

   public:
    ////////////////////////////////////////////////////////////////////////
    // CongruenceByPairs - constructor + destructor - public
    ////////////////////////////////////////////////////////////////////////

    //! Construct a CongruenceByPairs over the FroidurePin instance \p S
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
    CongruenceByPairs(congruence_type                  type,
                      std::shared_ptr<FroidurePinBase> S) noexcept;

    //! Construct a CongruenceByPairs over the FroidurePin instance \p fp
    //! representing a left/right/2-sided congruence according to \p type.
    //!
    //! \tparam T a class derived from FroidurePinBase. It is required that
    //! `T::element_type` be the same as CongruenceByPairs::element_type
    //! (which is the same as the template parameter \p TElementType).
    //!
    //! \param type whether the congruence is left, right, or 2-sided
    //! \param fp  a reference to the semigroup over which the congruence is
    //! defined.
    //!
    //! \throws LibsemigroupsException if \p type and the template parameter
    //! \p T are incompatible. Currently, this is when \p type is not
    //! `congruence_type::right` and `T::element_type` is TCE.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \warning the parameter `T& S` is copied, this might be expensive, use
    //! a std::shared_ptr to avoid the copy!
    template <typename T>
    CongruenceByPairs(congruence_type type, T const& fp);

    ~CongruenceByPairs();

    //! A CongruenceByPairs instance is not default-constructible.
    //! This constructor is deleted.
    CongruenceByPairs() = delete;

    //! A CongruenceByPairs instance is not copyable.
    //! This constructor is deleted.
    CongruenceByPairs(CongruenceByPairs const&) = delete;

    //! A CongruenceByPairs instance is not copy assignable.
    //! This constructor is deleted.
    CongruenceByPairs& operator=(CongruenceByPairs const&) = delete;

    //! A CongruenceByPairs instance is not move copyable.
    //! This constructor is deleted.
    CongruenceByPairs(CongruenceByPairs&&) = delete;

    //! A CongruenceByPairs instance is not move assignable.
    //! This constructor is deleted.
    CongruenceByPairs& operator=(CongruenceByPairs&&) = delete;

   protected:
    ////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - non-pure virtual member functions - protected
    ////////////////////////////////////////////////////////////////////////
    //! No doc
    class_index_type const_word_to_class_index(word_type const&) const override;

    // non_trivial_classes_impl is private in CongruenceInterface but
    // protected here since KnuthBendixCongruenceByPairs uses it and derives
    // from class CongruenceByPairs.
    //! No doc
    std::shared_ptr<non_trivial_classes_type const>
    non_trivial_classes_impl() override;

    ////////////////////////////////////////////////////////////////////////
    // CongruenceByPairs - member functions - protected
    ////////////////////////////////////////////////////////////////////////

    //! No doc
    void internal_add_pair(internal_element_type, internal_element_type);

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

    void add_pair_impl(word_type const&, word_type const&) override;
    bool is_quotient_obviously_finite_impl() override {
      return finished();
    }

    ////////////////////////////////////////////////////////////////////////
    // CongruenceByPairs - member functions - private
    ////////////////////////////////////////////////////////////////////////

    size_t add_index(internal_element_type) const;
    void   delete_tmp_storage();
    size_t get_index(internal_element_type) const;
    void   init();

    ////////////////////////////////////////////////////////////////////////
    // CongruenceByPairs - inner structs - private
    ////////////////////////////////////////////////////////////////////////

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
    // CongruenceByPairs - data - private
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
    internal_element_type                      _tmp1;
    internal_element_type                      _tmp2;
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
      : public CongruenceByPairs<detail::KBE,
                                 Hash<detail::KBE>,
                                 EqualTo<detail::KBE>> {
    ////////////////////////////////////////////////////////////////////////
    // KnuthBendixCongruenceByPairs - typedefs - private
    ////////////////////////////////////////////////////////////////////////

    using KnuthBendix = fpsemigroup::KnuthBendix;
    using p_type      = CongruenceByPairs<detail::KBE,
                                     libsemigroups::Hash<detail::KBE>,
                                     libsemigroups::EqualTo<detail::KBE>>;

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

    using p_type::non_trivial_classes_impl;
    // Override the method for the class CongruenceByPairs to avoid having to
    // know the parent semigroup (found as part of
    // KnuthBendixCongruenceByPairs::run) to add a pair.
    void add_pair_impl(word_type const&, word_type const&) override;

    ////////////////////////////////////////////////////////////////////////
    // KnuthBendixCongruenceByPairs - data - private
    ////////////////////////////////////////////////////////////////////////

    std::shared_ptr<KnuthBendix> _kb;
  };

  template <typename TElementType  = Element const*,
            typename TElementHash  = Hash<TElementType>,
            typename TElementEqual = EqualTo<TElementType>>
  //! This class is a wrapper for CongruenceByPairs that provides
  //! the FpSemigroupInterface.
  //!
  //! See CongruenceWrapper for further details.
  using FpSemigroupByPairs = CongruenceWrapper<
      CongruenceByPairs<TElementType, TElementHash, TElementEqual>>;
}  // namespace libsemigroups

#include "cong-pair-impl.hpp"
#endif  // LIBSEMIGROUPS_INCLUDE_CONG_PAIR_HPP_

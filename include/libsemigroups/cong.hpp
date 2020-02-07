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

// This file contains stuff for creating congruence over FroidurePin objects or
// over FpSemigroup objects.

#ifndef LIBSEMIGROUPS_CONG_HPP_
#define LIBSEMIGROUPS_CONG_HPP_

#include <cstddef>  // for size_t
#include <memory>   // for shared_ptr

#include "cong-intf.hpp"     // for congruence::type
#include "knuth-bendix.hpp"  // for KnuthBendix
#include "race.hpp"          // for Race
#include "runner.hpp"        // for Runner
#include "todd-coxeter.hpp"  // for ToddCoxeter
#include "types.hpp"         // for word_type

namespace libsemigroups {
  class FpSemigroup;      // Forward declaration
  class FroidurePinBase;  // Forward declaration, constructor parameters

  //! Defined in ``cong.hpp``.
  //!
  //! On this page we describe the functionality relating to the
  //! Congruence class. This class can be used for computing a congruence
  //! over a semigroup by running every applicable algorithm from
  //! ``libsemigroups`` (and some variants of the same algorithm) in parallel.
  //! This class is provided for convenience, at present it is not very
  //! customisable, and lacks some of the fine grained control offered by the
  //! classes implementing individual algorithms, such as
  //! congruence::ToddCoxeter and congruence::KnuthBendix.
  //!
  //! \sa congruence_type and tril.
  //! \par Example
  //! \code
  //! FpSemigroup S;
  //! S.set_alphabet(3);
  //! S.set_identity(0);
  //! S.add_rule({1, 2}, {0});
  //! S.is_obviously_infinite();  // false
  //!
  //! Congruence cong(twosided, S);
  //! cong.add_pair({1, 1, 1}, {0});
  //! cong.nr_classes(); // 3
  //! \endcode
  class Congruence final : public CongruenceInterface {
   public:
    //! This struct holds various enums which effect the coset enumeration
    //! process used by ToddCoxeter::run.
    //!
    //! \sa policy::runners.
    struct policy {
      //! This enum allows setting the policy used when adding runners to an
      //! instance of this type during construction.
      enum class runners {
        //! Adds a predetermined selection of runners.
        standard,
        //! Adds no runners.
        none
      };
    };

    //////////////////////////////////////////////////////////////////////////
    // Congruence - constructors - public
    //////////////////////////////////////////////////////////////////////////

    //! Constructs an empty instance of an interface to a congruence of type
    //! specified by the argument.
    //!
    //! \param type the type of the congruence.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \sa set_nr_generators and add_pair.
    explicit Congruence(congruence_type type,
                        policy::runners = policy::runners::standard);

    //! Constructs a Congruence over the FroidurePin instance \p S
    //! representing a left/right/2-sided congruence according to \p type.
    //!
    //! \tparam T a class derived from FroidurePinBase.
    //!
    //! \param type whether the congruence is left, right, or 2-sided
    //! \param S  a const reference to the semigroup over which the congruence
    //! is defined.
    //!
    //! \par Exceptions
    //! Does not throw itself but functions called by this function may
    //! throw.
    //!
    //! \par Complexity
    //! Linear in `S.size()`.
    //!
    //! \warning the parameter `T const& S` is copied, this might be expensive,
    //! use a std::shared_ptr to avoid the copy!
    template <typename T>
    Congruence(congruence_type type, T const& S)
        : Congruence(type,
                     static_cast<std::shared_ptr<FroidurePinBase>>(
                         std::make_shared<T>(S))) {
      static_assert(
          std::is_base_of<FroidurePinBase, T>::value,
          "the template parameter must be a derived class of FroidurePinBase");
    }

    //! Constructs a Congruence over the FroidurePin instance \p S
    //! representing a left/right/2-sided congruence according to \p type.
    //! \param type whether the congruence is left, right, or 2-sided
    //! \param S  a shared_ptr to the semigroup over which the congruence is
    //! defined.
    //!
    //! \par Exceptions
    //! Does not throw itself but functions called by this function may
    //! throw.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \note
    //! The FroidurePinBase pointed to by \p S is not copied.
    Congruence(congruence_type type, std::shared_ptr<FroidurePinBase> S);

    //! Constructs a Congruence over the FpSemigroup instance \p S
    //! representing a left/right/2-sided congruence according to \p type.
    //! \param type whether the congruence is left, right, or 2-sided
    //! \param S  a const reference to the finitely presented semigroup over
    //! which the congruence is defined.
    //!
    //! \par Exceptions
    //! Does not throw itself but functions called by this function may
    //! throw.
    //!
    //! \par Complexity
    //! Constant.
    Congruence(congruence_type type, FpSemigroup& S);

    ~Congruence() = default;

    //! A Congruence instance is not default-constructible.
    //! This constructor is deleted.
    Congruence() = delete;

    //! A Congruence instance is not copyable.
    //! This constructor is deleted.
    Congruence(Congruence const&) = delete;

    //! A Congruence instance is not copy assignable.
    //! This constructor is deleted.
    Congruence& operator=(Congruence const&) = delete;

    //! A Congruence instance is not move copyable.
    //! This constructor is deleted.
    Congruence(Congruence&&) = delete;

    //! A Congruence instance is not move assignable.
    //! This constructor is deleted.
    Congruence& operator=(Congruence&&) = delete;

    //////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - non-pure virtual member functions - public
    //////////////////////////////////////////////////////////////////////////

    bool contains(word_type const&, word_type const&) override;
    tril const_contains(word_type const&, word_type const&) const override;

    //////////////////////////////////////////////////////////////////////////
    // Congruence - member functions - public
    //////////////////////////////////////////////////////////////////////////

    //! Returns the congruence::KnuthBendix instance used to compute the
    //! congruence (if any).
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \returns A shared_ptr to a congruence::KnuthBendix or nullptr.
    //!
    //! \par Exceptions
    //! Does not throw itself but functions called by this function may
    //! throw.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \sa has_knuth_bendix().
    std::shared_ptr<congruence::KnuthBendix> knuth_bendix() const {
      using KnuthBendix = congruence::KnuthBendix;
      return _race.find_runner<KnuthBendix>();
    }

    //! Checks if a congruence::KnuthBendix instance is being used to compute
    //! the congruence.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \returns A `bool`.
    //!
    //! \par Exceptions
    //! Does not throw itself but functions called by this function may
    //! throw.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \sa knuth_bendix().
    bool has_knuth_bendix() const {
      return knuth_bendix() != nullptr;
    }

    //! Returns the congruence::ToddCoxeter instance used to compute the
    //! congruence (if any).
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \returns A shared_ptr to a congruence::ToddCoxeter or nullptr.
    //!
    //! \par Exceptions
    //! Does not throw itself but functions called by this function may
    //! throw.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \sa has_todd_coxeter.
    std::shared_ptr<congruence::ToddCoxeter> todd_coxeter() const {
      using congruence::ToddCoxeter;
      return _race.find_runner<ToddCoxeter>();
    }

    //! Checks if a congruence::ToddCoxeter instance is being used to compute
    //! the congruence.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \returns A `bool`.
    //!
    //! \par Exceptions
    //! Does not throw itself but functions called by this function may
    //! throw.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \sa todd_coxeter.
    bool has_todd_coxeter() const {
      return todd_coxeter() != nullptr;
    }

    // The next function is required by the GAP package Semigroups.
    //! No doc
    template <typename T>
    void add_runner(T const& r) {
      static_assert(std::is_base_of<CongruenceInterface, T>::value,
                    "the template parameter T must be a derived class of "
                    "CongruenceInterface");
      _race.add_runner(std::make_shared<T>(r));
    }

   private:
    //////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - pure virtual member functions - private
    //////////////////////////////////////////////////////////////////////////

    word_type class_index_to_word_impl(class_index_type) override;
    size_t    nr_classes_impl() override;
    std::shared_ptr<FroidurePinBase> quotient_impl() override;
    class_index_type word_to_class_index_impl(word_type const&) override;

    void run_impl() override {
      _race.run();
    }

    bool finished_impl() const override {
      return _race.finished();
    }

    //////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - non-pure virtual member functions - private
    //////////////////////////////////////////////////////////////////////////

    void add_pair_impl(word_type const&, word_type const&) override;
    std::shared_ptr<CongruenceInterface::non_trivial_classes_type const>
         non_trivial_classes_impl() override;
    bool is_quotient_obviously_finite_impl() override;
    bool is_quotient_obviously_infinite_impl() override;
    void set_nr_generators_impl(size_t) override;

    /////////////////////////////////////////////////////////////////////////
    // Congruence - data - private
    /////////////////////////////////////////////////////////////////////////
    detail::Race _race;
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_CONG_HPP_

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
#include "kambites.hpp"      // for Kambites
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
  //! \sa congruence_kind and tril.
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
  //! cong.number_of_classes(); // 3
  //! \endcode
  class Congruence final : public CongruenceInterface {
   public:
    //! Holds values of various options.
    //!
    //! This struct holds various enums which can be used to
    //! set various options.
    //!
    //! \sa options::runners.
    struct options {
      //! Holds options for specifying the algorithms to be used.
      //!
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

    //! Construct from kind (left/right/2-sided) and options.
    //!
    //! Constructs an empty instance of an interface to a congruence of type
    //! specified by the argument.
    //!
    //! \param type the type of the congruence.
    //! \param opt  optionally specify algorithms to be used (defaults to
    //! options::runners::standard).
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa set_number_of_generators and add_pair.
    explicit Congruence(congruence_kind  type,
                        options::runners opt = options::runners::standard);

    //! Construct from kind (left/right/2-sided) and FroidurePin.
    //!
    //! Constructs a Congruence over the FroidurePin instance \p S
    //! representing a left/right/2-sided congruence according to \p type.
    //!
    //! \tparam T a class derived from FroidurePinBase.
    //!
    //! \param type whether the congruence is left, right, or 2-sided
    //! \param S  a const reference to the semigroup over which the congruence
    //! is defined.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in  the size of \p S.
    //!
    //! \warning the parameter `T const& S` is copied, this might be expensive,
    //! use a std::shared_ptr to avoid the copy!
    template <typename T>
    Congruence(congruence_kind type, T const& S)
        : Congruence(type,
                     static_cast<std::shared_ptr<FroidurePinBase>>(
                         std::make_shared<T>(S))) {
      static_assert(
          std::is_base_of<FroidurePinBase, T>::value,
          "the template parameter must be a derived class of FroidurePinBase");
    }

    //! Construct from kind (left/right/2-sided) and shared pointer to
    //! FroidurePin.
    //!
    //! Constructs a Congruence over the FroidurePin instance \p S
    //! representing a left/right/2-sided congruence according to \p type.
    //! \param type whether the congruence is left, right, or 2-sided
    //! \param S  a shared_ptr to the semigroup over which the congruence is
    //! defined.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \note
    //! The FroidurePinBase pointed to by \p S is not copied.
    Congruence(congruence_kind type, std::shared_ptr<FroidurePinBase> S);

    //! Construct from kind (left/right/2-sided) and FpSemigroup.
    //!
    //! Constructs a Congruence over the FpSemigroup instance \p S
    //! representing a left/right/2-sided congruence according to \p type.
    //! \param type whether the congruence is left, right, or 2-sided
    //! \param S  a const reference to the finitely presented semigroup over
    //! which the congruence is defined.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    Congruence(congruence_kind type, FpSemigroup& S);

    ~Congruence() = default;

    //! Deleted.
    Congruence() = delete;

    //! Deleted.
    Congruence(Congruence const&) = delete;

    //! Deleted.
    Congruence& operator=(Congruence const&) = delete;

    //! Deleted.
    Congruence(Congruence&&) = delete;

    //! Deleted.
    Congruence& operator=(Congruence&&) = delete;

    //////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - non-pure virtual member functions - public
    //////////////////////////////////////////////////////////////////////////

    //! \copydoc CongruenceInterface::contains
    bool contains(word_type const& u, word_type const& v) override;

    //! \copydoc CongruenceInterface::const_contains
    tril const_contains(word_type const& u, word_type const& v) const override;

    //////////////////////////////////////////////////////////////////////////
    // Congruence - member functions - public
    //////////////////////////////////////////////////////////////////////////

    //! Returns the KnuthBendix instance used to compute the congruence (if
    //! any).
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A std::shared_ptr to a congruence::KnuthBendix or \c nullptr.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa has_knuth_bendix().
    std::shared_ptr<congruence::KnuthBendix> knuth_bendix() const {
      using KnuthBendix = congruence::KnuthBendix;
      return _race.find_runner<KnuthBendix>();
    }

    //! Checks if a KnuthBendix instance is being used to compute
    //! the congruence.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value of type `bool`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa knuth_bendix().
    bool has_knuth_bendix() const {
      return knuth_bendix() != nullptr;
    }

    //! Returns the ToddCoxeter instance used to compute the
    //! congruence (if any).
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A shared_ptr to a congruence::ToddCoxeter or \c nullptr.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa has_todd_coxeter().
    std::shared_ptr<congruence::ToddCoxeter> todd_coxeter() const {
      using congruence::ToddCoxeter;
      return _race.find_runner<ToddCoxeter>();
    }

    //! Returns the Kambites instance used to compute the congruence (if any).
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A shared_ptr to a congruence::Kambites or \c nullptr.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa has_kambites().
    std::shared_ptr<congruence::Kambites> kambites() const {
      using congruence::Kambites;
      return _race.find_runner<Kambites>();
    }

    //! Checks if a ToddCoxeter instance is being used to compute
    //! the congruence.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value to type `bool`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa todd_coxeter.
    bool has_todd_coxeter() const {
      return todd_coxeter() != nullptr;
    }

    //! Checks if a Kambites instance is being used to compute
    //! the congruence.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value to type `bool`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa \ref kambites.
    bool has_kambites() const {
      return kambites() != nullptr;
    }

    // The next function is required by the GAP package Semigroups.
    //! Adds a class derived from CongruenceInterface to the algorithms used to
    //! compute the congruence.
    //!
    //! This function adds the algorithm represented by the parameter \p r to
    //! the list of runners that are invoked when \ref run is called.
    //!
    //! \tparam T a type derived from CongruenceInterface.
    //!
    //! \param r a const reference to an instance of a type derived from
    //! CongruenceInterface.
    //!
    //! \returns
    //! (None)
    //!
    //! \throws
    //! LibsemigroupsException if started() returns \c true.
    //!
    //! \warning
    //! It's the responsibility of the caller to ensure that \p r is compatible
    //! with any existing CongruenceInterface instances in \c this.
    template <typename T>
    void add_runner(T const& r) {
      static_assert(std::is_base_of<CongruenceInterface, T>::value,
                    "the template parameter T must be a derived class of "
                    "CongruenceInterface");
      if (started()) {
        LIBSEMIGROUPS_EXCEPTION("cannot add a runner at this stage");
      }
      _race.add_runner(std::make_shared<T>(r));
    }

    //! Get the current maximum number of threads.
    //!
    //! \returns
    //! A value of type \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \parameters
    //! (None)
    size_t max_threads() const noexcept {
      return _race.max_threads();
    }

    //! Set the maximum number of threads.
    //!
    //! \param val the number of threads.
    //!
    //! \returns A reference to \c this.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    Congruence& max_threads(size_t val) noexcept {
      _race.max_threads(val);
      return *this;
    }

   private:
    //////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - pure virtual member functions - private
    //////////////////////////////////////////////////////////////////////////

    word_type class_index_to_word_impl(class_index_type) override;
    size_t    number_of_classes_impl() override;
    std::shared_ptr<FroidurePinBase> quotient_impl() override;
    class_index_type word_to_class_index_impl(word_type const&) override;

    void run_impl() override {
      if (kambites() != nullptr) {
        if (kambites()->kambites().small_overlap_class() >= 4) {
          // Race always checks for finished in the other runners, and the
          // kambites is finished and will be declared the winner.
        } else {
          _race.erase_runners(_race.cbegin());
        }
      }
      _race.run_until([this]() { return this->stopped(); });
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
    void set_number_of_generators_impl(size_t) override;

    /////////////////////////////////////////////////////////////////////////
    // Congruence - data - private
    /////////////////////////////////////////////////////////////////////////
    detail::Race _race;
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_CONG_HPP_

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

#include "cong-intf-new.hpp"
#include "kambites.hpp"          // for Kambites
#include "knuth-bendix-new.hpp"  // for KnuthBendix
#include "race.hpp"              // for Race
#include "runner.hpp"            // for Runner
#include "todd-coxeter-new.hpp"  // for ToddCoxeter
#include "types.hpp"             // for word_type

namespace libsemigroups {
  class FroidurePinBase;  // Forward declaration, constructor parameters

  // The purpose of this class in v3 is just to deliver the winner, then that
  // object is used to answer whatever questions.

  //! Defined in ``cong.hpp``.
  //!
  //! On this page we describe the functionality relating to the Congruence
  //! class. This class can be used for computing a congruence over a semigroup
  //! by running every applicable algorithm from ``libsemigroups`` (and some
  //! variants of the same algorithm) in parallel. This class is provided for
  //! convenience, at present it is not very customisable, and lacks some of
  //! the fine grained control offered by the classes implementing individual
  //! algorithms, such as ToddCoxeter and KnuthBendix.
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
  class Congruence : public v3::CongruenceInterface {
   public:
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
    explicit Congruence(congruence_kind type);

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
    // TODO if keeping this for v3 then change S -> Presentation
    Congruence(congruence_kind type, Presentation<word_type> const& S);
    // TODO constructor for rval ref, and init versions

    ~Congruence() = default;

    //! Deleted.
    // TODO undelete
    Congruence() = delete;

    //! Deleted.
    // TODO undelete
    Congruence(Congruence const&) = delete;

    //! Deleted.
    // TODO undelete
    Congruence& operator=(Congruence const&) = delete;

    //! Deleted.
    // TODO undelete
    Congruence(Congruence&&) = delete;

    //! Deleted.
    // TODO undelete
    Congruence& operator=(Congruence&&) = delete;

    //////////////////////////////////////////////////////////////////////////
    // Congruence - member functions - public
    //////////////////////////////////////////////////////////////////////////

    //! Returns the KnuthBendix instance used to compute the congruence (if
    //! any).
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A std::shared_ptr to a KnuthBendix or \c nullptr.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa has_knuth_bendix().
    std::shared_ptr<KnuthBendix> knuth_bendix() const {
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
    //! \returns A shared_ptr to a ToddCoxeter or \c nullptr.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa has_todd_coxeter().
    std::shared_ptr<ToddCoxeter> todd_coxeter() const {
      return _race.find_runner<ToddCoxeter>();
    }

    //! Returns the Kambites instance used to compute the congruence (if any).
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A shared_ptr to a Kambites or \c nullptr.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa has_kambites().
    std::shared_ptr<Kambites<detail::MultiStringView>> kambites() const {
      return _race.find_runner<Kambites<detail::MultiStringView>>();
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
      static_assert(std::is_base_of<Runner, T>::value,
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

    void validate_word(word_type const&) const override {
      // TODO impl, or remove from CongruenceInterface
    }

    size_t number_of_runners() const noexcept {
      return _race.number_of_runners();
    }

   private:
    //////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - pure virtual member functions - private
    //////////////////////////////////////////////////////////////////////////

    // TODO to cpp file
    void run_impl() override {
      if (kambites() != nullptr) {
        init_kambites();
        if (kambites()->small_overlap_class() >= 4) {
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

    // TODO require init_todd_coxeter + init_knuth_bendix also
    void init_kambites() {
      auto k = kambites();
      LIBSEMIGROUPS_ASSERT(k != nullptr);
      // Add pairs into kambites object
      auto p     = k->presentation();
      auto first = generating_pairs().cbegin();
      auto last  = generating_pairs().cend();
      for (auto it = first; it != last; ++it) {
        p.rules.push_back(to_string(p, *it));
      }
      k->init(p);
    }

    /////////////////////////////////////////////////////////////////////////
    // Congruence - data - private
    /////////////////////////////////////////////////////////////////////////
    detail::Race _race;
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_CONG_HPP_

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

// This file contains the declaration of a class for finitely presented
// semigroups.

#ifndef LIBSEMIGROUPS_FPSEMI_HPP_
#define LIBSEMIGROUPS_FPSEMI_HPP_

#include <cstddef>  // for size_t
#include <memory>   // for shared_ptr
#include <string>   // for string

#include "fpsemi-intf.hpp"   // for FpSemigroupInterface
#include "kambites.hpp"      // for Kambites
#include "knuth-bendix.hpp"  // for KnuthBendix
#include "race.hpp"          // for Race
#include "todd-coxeter.hpp"  // for ToddCoxeter

namespace libsemigroups {
  class FroidurePinBase;  // Forward declaration

  //! Defined in ``fpsemi.hpp``.
  //!
  //! This is a class for representing finitely presented semigroups and
  //! monoids.
  //!
  //! On this page we describe the functionality relating to the FpSemigroup
  //! class. This class can be used for computing a finitely presented semigroup
  //! or monoid by running every applicable algorithm from libsemigroups (and
  //! possibly some variants of the same algorithm) in parallel. This class is
  //! provided for convenience, at present it is not very customisable, and
  //! lacks some of the fine grained control offered by the classes
  //! implementing individual algorithms, such as fpsemigroup::ToddCoxeter and
  //! fpsemigroup::KnuthBendix.
  //!
  //! \par Example
  //! \code
  //! FpSemigroup S;
  //! S.set_alphabet(3);
  //! S.set_identity(0);
  //! S.add_rule({1, 2}, {0});
  //! S.is_obviously_infinite();  // false
  //! \endcode
  class FpSemigroup final : public FpSemigroupInterface {
    using KnuthBendix = fpsemigroup::KnuthBendix;
    using ToddCoxeter = fpsemigroup::ToddCoxeter;
    using Kambites    = fpsemigroup::Kambites<std::string>;

    enum class use_kambites : bool { yes = true, no = false };
    FpSemigroup(use_kambites);

   public:
    //////////////////////////////////////////////////////////////////////////
    // FpSemigroup - constructors - public
    //////////////////////////////////////////////////////////////////////////

    //! Default construct an FpSemigroup.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Parameters
    //! (None)
    FpSemigroup();

    //! Construct an FpSemigroup isomorphic to the FroidurePin instance \p S.
    //!
    //! \tparam T a class derived from FroidurePinBase.
    //!
    //! \param S  a const reference to the semigroup isomorphic to the one
    //! being constructed.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! Linear in `S.size()`.
    //!
    //! \warning the parameter `T const& S` is copied, this might be expensive,
    //! use a std::shared_ptr to avoid the copy!
    template <typename T>
    explicit FpSemigroup(T const& S)
        : FpSemigroup(static_cast<std::shared_ptr<FroidurePinBase>>(
            std::make_shared<T>(S))) {
      static_assert(std::is_base_of<FroidurePinBase, T>::value,
                    "the template parameter must be a derived class of "
                    "FroidurePinBase");
    }

    //! Construct an FpSemigroup isomorphic to the FroidurePin instance \p S.
    //!
    //! \param S  a shared_ptr to the semigroup isomorphic to the finitely
    //! presented semigroup being defined.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \note
    //! The FroidurePinBase pointed to by \p S is not copied.
    explicit FpSemigroup(std::shared_ptr<FroidurePinBase> S);

    //! Default copy constructor.
    FpSemigroup(FpSemigroup const&) = default;

    //! A FpSemigroup instance is not copy assignable.
    //! This constructor is deleted.
    FpSemigroup& operator=(FpSemigroup const&) = delete;

    //! A FpSemigroup instance is not move copyable.
    //! This constructor is deleted.
    FpSemigroup(FpSemigroup&&) = delete;

    //! A FpSemigroup instance is not move assignable.
    //! This constructor is deleted.
    FpSemigroup& operator=(FpSemigroup&&) = delete;

    ~FpSemigroup() = default;

    //////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - pure virtual member functions - public
    //////////////////////////////////////////////////////////////////////////

    // Documented in FpSemigroupInterface
    uint64_t size() override;

    // Documented in FpSemigroupInterface
    bool equal_to(std::string const& u, std::string const& v) override {
      run();  // to ensure the state is correct
      return static_cast<FpSemigroupInterface*>(_race.winner().get())
          ->equal_to(u, v);
    }

    // Documented in FpSemigroupInterface
    std::string normal_form(std::string const& w) override {
      run();  // to ensure the state is correct
      return static_cast<FpSemigroupInterface*>(_race.winner().get())
          ->normal_form(w);
    }

    //////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - non-pure virtual member functions - public
    //////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // The following are required for overload resolution.
    // Documented in FpSemigroupInterface.
    // Sphinx/doxygen get confused by this, so we don't allow Doxygen to parse
    // these two declarations.
    using FpSemigroupInterface::equal_to;
    using FpSemigroupInterface::normal_form;
#endif

    //////////////////////////////////////////////////////////////////////////
    // FpSemigroup - non-virtual member functions - public
    //////////////////////////////////////////////////////////////////////////

    //! Checks if a fpsemigroup::KnuthBendix instance is being used to compute
    //! the finitely presented semigroup represented by \c this.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \returns A `bool`.
    //!
    //! \par Exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \sa knuth_bendix().
    bool has_knuth_bendix() const {
      return knuth_bendix() != nullptr;
    }

    //! Checks if a fpsemigroup::ToddCoxeter instance is being used to compute
    //! the finitely presented semigroup represented by \c this.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \returns A `bool`.
    //!
    //! \par Exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \sa todd_coxeter().
    bool has_todd_coxeter() const {
      return todd_coxeter() != nullptr;
    }

    //! Returns the fpsemigroup::Kambites instance used to compute the
    //! finitely presented semigroup (if any).
    //!
    //! \returns A shared_ptr to a congruence::Kambites or nullptr.
    //!
    //! \exceptions
    //! \no_libsemigroups_except.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \sa has_kambites().
    //!
    //! \par Parameters
    //! (None)
    std::shared_ptr<Kambites> kambites() const {
      return _race.find_runner<Kambites>();
    }

    //! Checks if a fpsemigroup::Kambites instance is being used to compute
    //! the finitely presented semigroup represented by \c this.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A `bool`.
    //!
    //! \par Exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa kambites().
    bool has_kambites() const noexcept {
      return kambites() != nullptr && kambites()->small_overlap_class() >= 4;
    }

    //! Returns the fpsemigroup::KnuthBendix instance used to compute the
    //! finitely presented semigroup (if any).
    //!
    //! \returns A shared_ptr to a congruence::KnuthBendix or nullptr.
    //!
    //! \exceptions
    //! \no_libsemigroups_except.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \sa has_knuth_bendix().
    //!
    //! \par Parameters
    //! (None)
    std::shared_ptr<KnuthBendix> knuth_bendix() const {
      return _race.find_runner<KnuthBendix>();
    }

    //! Returns the libsemigroups::fpsemigroup::ToddCoxeter instance used to
    //! compute the finitely presented semigroup (if any).
    //!
    //! \returns A shared_ptr to a congruence::KnuthBendix or nullptr.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \sa has_todd_coxeter().
    //!
    //! \par Parameters
    //! (None)
    std::shared_ptr<ToddCoxeter> todd_coxeter() const {
      return _race.find_runner<ToddCoxeter>();
    }

    //! Get the current maximum number of threads.
    //!
    //! \returns
    //! A value of type \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    size_t max_threads() const noexcept {
      return _race.max_threads();
    }

    //! Set the maximum number of threads.
    //!
    //! \param val the number of threads.
    //!
    //! \returns a reference to \c this.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \par Complexity
    //! Constant.
    FpSemigroup& max_threads(size_t val) noexcept {
      _race.max_threads(val);
      return *this;
    }

   private:
    //////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - pure virtual member functions - private
    //////////////////////////////////////////////////////////////////////////

    void add_rule_impl(std::string const&, std::string const&) override;
    std::shared_ptr<FroidurePinBase> froidure_pin_impl() override;
    bool                             is_obviously_infinite_impl() override;

    void run_impl() override {
      if (kambites() != nullptr) {
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

    //////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - non-pure virtual member functions - private
    //////////////////////////////////////////////////////////////////////////

    void set_alphabet_impl(std::string const&) override;
    void set_alphabet_impl(size_t) override;
    bool is_obviously_finite_impl() override;

    //////////////////////////////////////////////////////////////////////////
    // FpSemigroup - data - private
    //////////////////////////////////////////////////////////////////////////

    detail::Race _race;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_FPSEMI_HPP_

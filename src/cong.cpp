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

// This file contains stuff for creating congruence over FroidurePinBase
// objects or over Presentation objects.

#include "libsemigroups/cong.hpp"

#include <memory>  // for shared_ptr

#include "libsemigroups/debug.hpp"              // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"          // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/froidure-pin-base.hpp"  // for FroidurePinBase
#include "libsemigroups/kambites.hpp"           // for Kambites
#include "libsemigroups/knuth-bendix.hpp"       // for KnuthBendix_
#include "libsemigroups/to-presentation.hpp"    // for to_presentation
#include "libsemigroups/to-todd-coxeter.hpp"    // for to_todd_coxeter
#include "libsemigroups/todd-coxeter.hpp"       // for ToddCoxeter
#include "libsemigroups/types.hpp"              // for word_type

namespace libsemigroups {

  //////////////////////////////////////////////////////////////////////////
  // Congruence - constructors - public
  //////////////////////////////////////////////////////////////////////////

  Congruence& Congruence::init() {
    CongruenceInterface::init();
    _race.init();
    _runners_initted = false;
    return *this;
  }

  Congruence& Congruence::init(congruence_kind                type,
                               Presentation<word_type> const& p) {
    init(type);
    _race.max_threads(POSITIVE_INFINITY);
    if (type == congruence_kind::twosided) {
      _race.add_runner(std::make_shared<Kambites<word_type>>(p));
    }
    _race.add_runner(std::make_shared<KnuthBendix<>>(type, p));
    _race.add_runner(std::make_shared<ToddCoxeter>(type, p));
    auto tc = std::make_shared<ToddCoxeter>(type, p);
    tc->strategy(ToddCoxeter::options::strategy::felsch);
    _race.add_runner(tc);
    // TODO(later) add a Runner that tries to create a ToddCoxeter using the
    // Cayley graph of some FroidurePin
    return *this;
  }

  Congruence& Congruence::init(congruence_kind type, FroidurePinBase& S) {
    if (S.is_finite() != tril::FALSE) {
      S.run();
    } else {
      LIBSEMIGROUPS_EXCEPTION(
          "the 2nd argument does not represent a finite semigroup!");
    }
    init(type);
    _race.max_threads(POSITIVE_INFINITY);

    auto p  = to_presentation<word_type>(S);
    auto tc = std::make_shared<ToddCoxeter>(type, p);
    _race.add_runner(tc);
    tc = std::make_shared<ToddCoxeter>(type, p);
    tc->strategy(ToddCoxeter::options::strategy::felsch);
    _race.add_runner(tc);

    // TODO(later) if necessary make a runner that tries to S.run(), then get
    // the Cayley graph and use that in the ToddCoxeter, at present that'll
    // happen here in the constructor
    tc = std::make_shared<ToddCoxeter>(to_todd_coxeter(type, S));
    _race.add_runner(tc);
    tc = std::make_shared<ToddCoxeter>(type, p);
    tc->strategy(ToddCoxeter::options::strategy::felsch);
    _race.add_runner(tc);
    if (p.rules.size() < 256) {
      // TODO(later) at present if there are lots of rules it takes a long
      // time to construct a KnuthBendix<> instance since it reduces the rules
      // as they are added maybe better to defer this until running
      _race.add_runner(std::make_shared<KnuthBendix<>>(type, p));
    }
    return *this;
  }

  void Congruence::init_runners() {
    if (!_runners_initted) {
      _runners_initted = true;
      for (auto& runner : _race) {
        auto first = generating_pairs().cbegin();
        auto last  = generating_pairs().cend();
        for (auto it = first; it != last; it += 2) {
          std::static_pointer_cast<CongruenceInterface>(runner)
              ->add_pair_no_checks_no_reverse(*it, *(it + 1));
        }
      }
    }
  }

  void Congruence::run_impl() {
    init_runners();
    if (has<Kambites<word_type>>()) {
      if (get<Kambites<word_type>>()->small_overlap_class() >= 4) {
        // Race always checks for finished in the other runners, and the
        // kambites is finished and will be declared the winner.
        return;
      }
    }
    _race.run_until([this]() { return this->stopped(); });
  }

  namespace congruence {
    // We have to pass the presentation here, because o/w we have no way of
    // knowing over what we should compute the non-trivial classes (i.e. we
    // cannot always recover p from cong).
    std::vector<std::vector<word_type>>
    non_trivial_classes(Congruence& cong, Presentation<word_type> const& p) {
      using rx::operator|;
      cong.run();
      if (cong.has<ToddCoxeter>() && cong.get<ToddCoxeter>()->finished()) {
        ToddCoxeter tc(cong.kind(), p);
        return ::libsemigroups::todd_coxeter::non_trivial_classes(
            *cong.get<ToddCoxeter>(), tc);
      } else if (cong.has<KnuthBendix<>>()
                 && cong.get<KnuthBendix<>>()->finished()) {
        KnuthBendix<> kb(cong.kind(), p);
        auto strings = ::libsemigroups::knuth_bendix::non_trivial_classes(
            *cong.get<KnuthBendix<>>(), kb);
        std::vector<std::vector<word_type>> result;
        for (auto const& klass : strings) {
          result.push_back(rx::iterator_range(klass.begin(), klass.end())
                           | ToWords(kb.presentation().alphabet())
                           | rx::to_vector());
        }
        return result;
      }
      // If a Kambites object wins the race in Congruence, then we cannot
      // really compute anything here unless the semigroup defined by p is
      // finite, but that'd be better handled explicitly in any code calling
      // this.
      LIBSEMIGROUPS_EXCEPTION("Cannot compute the non-trivial classes!");
    }

    std::vector<std::vector<std::string>>
    non_trivial_classes(Congruence& cong, Presentation<std::string> const& p) {
      using rx::operator|;
      cong.run();
      if (cong.has<KnuthBendix<>>() && cong.get<KnuthBendix<>>()->finished()) {
        KnuthBendix<> kb(cong.kind(), p);
        return ::libsemigroups::knuth_bendix::non_trivial_classes(
            *cong.get<KnuthBendix<>>(), kb);
      }
      if (cong.has<ToddCoxeter>() && cong.get<ToddCoxeter>()->finished()) {
        ToddCoxeter tc(cong.kind(), p);
        auto        words = ::libsemigroups::todd_coxeter::non_trivial_classes(
            *cong.get<ToddCoxeter>(), tc);
        std::vector<std::vector<std::string>> result;
        for (auto const& klass : words) {
          result.push_back(rx::iterator_range(klass.begin(), klass.end())
                           | ToStrings(p.alphabet()) | rx::to_vector());
        }
        return result;
      }

      // If a Kambites object wins the race in Congruence, then we cannot
      // really compute anything here unless the semigroup defined by p is
      // finite, but that'd be better handled explicitly in any code calling
      // this.
      LIBSEMIGROUPS_EXCEPTION("Cannot compute the non-trivial classes!");
    }
  }  // namespace congruence
}  // namespace libsemigroups

//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_TIETZE_HPP_
#define LIBSEMIGROUPS_TIETZE_HPP_

#include <algorithm>  // for sort
#include <chrono>
#include <cstddef>      // for size_t
#include <numeric>      // for accumulate
#include <queue>        // for queue
#include <string_view>  // for basic_string_view, string_view
#include <tuple>
#include <type_traits>    // for remove_cvref_t
#include <unordered_set>  // for unordered_set
#include <utility>        // for move
#include <vector>         // for vector

#include "adapters.hpp"      // for Hash
#include "constants.hpp"     // for POSITIVE_INFINITY
#include "debug.hpp"         // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"     // for LIBSEMIGROUPS_EXCEPTION
#include "presentation.hpp"  // for Presentation, operator!=, rep...
#include "runner.hpp"        // for Runner

#include "detail/fmt.hpp"   // for print
#include "detail/race.hpp"  // for Race

// TODO:
// * all calls to next, get must first check !at_end()
// * add static_assertions

namespace libsemigroups {

  class Subwords;  // forward decl

  template <typename InputRange>
  class SubwordsRange {
   private:
    using Word =
        typename std::decay_t<typename InputRange::output_type>::word_type;

    std::pair<Presentation<Word>, Word>  _current;
    size_t                               _current_rule;
    InputRange                           _input;
    InputRange                           _input_orig;  // TODO comment
    size_t                               _max_length;
    size_t                               _min_length;
    size_t                               _prefix_end;
    bool                                 _proper;
    std::unordered_set<Word, Hash<Word>> _seen;
    size_t                               _suffix_begin;

   public:
    ////////////////////////////////////////////////////////////////////////
    // Aliases + static data
    ////////////////////////////////////////////////////////////////////////

    using output_type = std::pair<Presentation<Word>, Word> const&;

    static constexpr bool is_finite     = rx::is_finite_v<InputRange>;
    static constexpr bool is_idempotent = rx::is_idempotent_v<InputRange>;

    ////////////////////////////////////////////////////////////////////////
    // Constructors + initializers
    ////////////////////////////////////////////////////////////////////////
    template <typename Settings>  // TODO rm template param when out of lining
    explicit SubwordsRange(InputRange&& input, Settings const& subwords)
        : _current(),
          _current_rule(),
          _input(std::move(input)),
          _input_orig(_input),
          _max_length(subwords.max_length()),
          _min_length(subwords.min_length()),
          _prefix_end(),
          _proper(subwords.proper()),
          _seen(),
          _suffix_begin() {
      reset();
    }

    template <typename Settings>  // TODO rm template param when out of lining
    explicit SubwordsRange(InputRange const& input, Settings const& subwords)
        : _current(),
          _current_rule(),
          _input(input),
          _input_orig(_input),
          _max_length(subwords.max_length()),
          _min_length(subwords.min_length()),
          _prefix_end(),
          _proper(subwords.proper()),
          _seen(),
          _suffix_begin() {
      reset();
    }

    // TODO use or rm
    // SubwordsRange& init() {
    //   _current.first.init();
    //   _current.second.clear();
    //   _max_length = POSITIVE_INFINITY;
    //   _min_length = 0;
    //   _proper     = false;
    //   _seen.clear();
    //   return *this;
    // }

    SubwordsRange(SubwordsRange const&)            = default;
    SubwordsRange(SubwordsRange&&)                 = default;
    SubwordsRange& operator=(SubwordsRange const&) = default;
    SubwordsRange& operator=(SubwordsRange&&)      = default;

    ~SubwordsRange() = default;

    // TODO private
    SubwordsRange& reset() {
      if (!_input.at_end()) {
        _current.first = _input.get();
        _current_rule  = 0;
        _seen.clear();
        init_prefix_suffix();
        next();
      }
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // Settings
    ////////////////////////////////////////////////////////////////////////

    [[nodiscard]] size_t max_length() const noexcept {
      return _max_length;
    }

    SubwordsRange& max_length(size_t val) {
      _max_length = val;
      _input      = _input_orig;
      reset();
      return *this;
    }

    [[nodiscard]] size_t min_length() const noexcept {
      return _min_length;
    }

    SubwordsRange& min_length(size_t val) {
      _min_length = val;
      _input      = _input_orig;
      reset();
      return *this;
    }

    [[nodiscard]] size_t proper() const noexcept {
      return _proper;
    }

    SubwordsRange& proper(bool val) {
      _proper = val;
      // No reset, we just continue from the current value but only output
      // proper subwords from here on out.
      // WARN: this is not the same as min/max_length
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // rx::ranges stuff
    ////////////////////////////////////////////////////////////////////////

    // Important note, you can modify _current.second (Word), because its
    // contents will be discarded at next call of next. You can also modify
    // _current.first (Presentation<Word>) if you put it back into its original
    // state before the next call to next().
    [[nodiscard]] output_type get() const {
      return _current;
    }

    void next() {
      while (_current_rule != _current.first.rules.size()) {
        auto const& rule = _current.first.rules[_current_rule];
        while (_suffix_begin < rule.size()) {
          size_t prefix_last = rule.size();
          if (_suffix_begin == 0) {
            prefix_last -= _proper;
          }
          while (_prefix_end - _suffix_begin <= _max_length
                 && _prefix_end <= prefix_last) {
            auto first = rule.begin() + _suffix_begin;
            auto last  = rule.begin() + _prefix_end;
            if (_seen.emplace(first, last).second) {
              _current.second.assign(first, last);
              if (_prefix_end != rule.size()) {
                ++_prefix_end;
              } else {
                advance_prefix_suffix();
              }
              return;
            }
            ++_prefix_end;
          }
          advance_prefix_suffix();
        }

        ++_current_rule;
        init_prefix_suffix();
      }
      if (!_input.at_end()) {
        _input.next();
        reset();
      }
    }

    [[nodiscard]] bool at_end() const noexcept {
      return _input.at_end();
    }

    [[nodiscard]] size_t size_hint() const {
      return std::numeric_limits<size_t>::max();
    }

   private:
    ////////////////////////////////////////////////////////////////////////
    // Private
    ////////////////////////////////////////////////////////////////////////
    void advance_prefix() {
      LIBSEMIGROUPS_ASSERT(_current_rule < _current.first.rules.size());
      size_t const n = _current.first.rules[_current_rule].size();
      if (_prefix_end + _min_length <= n) {
        _prefix_end += _min_length;
      } else {
        _prefix_end = n + 1;
      }
    }

    void init_prefix_suffix() {
      _suffix_begin = 0;
      _prefix_end   = 0;
      if (_current_rule < _current.first.rules.size()) {
        advance_prefix();
      }
    }

    void advance_prefix_suffix() {
      LIBSEMIGROUPS_ASSERT(_current_rule < _current.first.rules.size());
      ++_suffix_begin;
      _prefix_end = _suffix_begin;
      advance_prefix();
    }
  };  // class SubwordsRange

  class Subwords {
   private:
    size_t _min_length;
    size_t _max_length;
    bool   _proper;

   public:
    Subwords()
        : _min_length(0), _max_length(POSITIVE_INFINITY), _proper(false) {}

    Subwords(Subwords const&)            = default;
    Subwords(Subwords&&)                 = default;
    Subwords& operator=(Subwords const&) = default;
    Subwords& operator=(Subwords&&)      = default;

    ~Subwords() = default;

    template <typename InputRange,
              typename = std::enable_if_t<rx::is_input_or_sink_v<InputRange>>>
    [[nodiscard]] auto operator()(InputRange&& input) const {
      // TODO static_assert that InputRange::output_type is a specialization
      // of Presentation We pass *this thru so that the settings are copied
      // too
      return SubwordsRange(std::forward<InputRange>(input), *this);
    }

    template <typename Word>
    [[nodiscard]] auto operator()(Presentation<Word> const& input) const {
      return operator()(Singleton(input));
    }

    ////////////////////////////////////////////////////////////////////////
    // Settings
    ////////////////////////////////////////////////////////////////////////

    [[nodiscard]] size_t min_length() const noexcept {
      return _min_length;
    }

    Subwords& min_length(size_t val) {
      _min_length = val;
      return *this;
    }

    [[nodiscard]] size_t max_length() const noexcept {
      return _max_length;
    }

    Subwords& max_length(size_t val) {
      _max_length = val;
      return *this;
    }

    [[nodiscard]] size_t proper() const noexcept {
      return _proper;
    }

    Subwords& proper(bool val) {
      _proper = val;
      return *this;
    }
  };

  template <typename InputRange>
  class TietzeAddGeneratorsRange {
   public:
    using native_word_type
        = std::tuple_element_t<1,
                               std::decay_t<typename InputRange::output_type>>;

   private:
    InputRange _input;
    // TODO remove, just modify the incoming presentation, then unmodify it
    Presentation<native_word_type> _get_presentation;

   public:
    ////////////////////////////////////////////////////////////////////////
    // Aliases
    ////////////////////////////////////////////////////////////////////////
    static constexpr bool is_finite     = rx::is_finite_v<InputRange>;
    static constexpr bool is_idempotent = rx::is_idempotent_v<InputRange>;

    using output_type = Presentation<native_word_type> const&;

    ////////////////////////////////////////////////////////////////////////
    // Constructors + initializers
    ////////////////////////////////////////////////////////////////////////
    // TODO? init functions?
    TietzeAddGeneratorsRange(InputRange const& input)
        : _input(input), _get_presentation() {
      if (!_input.at_end()) {
        auto const& value = _input.get();
        _get_presentation = value.first;
        presentation::replace_word_with_new_generator(_get_presentation,
                                                      value.second);
      }
    }
    // TODO rval ref constructor

    ////////////////////////////////////////////////////////////////////////
    // rx::ranges stuff
    ////////////////////////////////////////////////////////////////////////

    [[nodiscard]] output_type get() const {
      return _get_presentation;
    }

    // TODO try_get_and_advance

    void next() {
      _input.next();
      if (!_input.at_end()) {
        _get_presentation = _input.get().first;
        presentation::replace_word_with_new_generator(_get_presentation,
                                                      _input.get().second);
      }
    }

    [[nodiscard]] bool at_end() const {
      return _input.at_end();
    }

    [[nodiscard]] size_t size_hint() const {
      return _input.size_hint();
    }
  };  // class TietzeAddGeneratorsRange

  struct TietzeAddGenerators {
    template <typename InputRange>
    [[nodiscard]] auto operator()(InputRange&& input) const {
      using Inner = rx::get_range_type_t<InputRange>;

      return TietzeAddGeneratorsRange<Inner>(std::forward<InputRange>(input));
    }
  };

  template <typename InputRange>
  class TietzeAddRelationRange {
   public:
    using native_word_type
        = std::tuple_element_t<1,
                               std::decay_t<typename InputRange::output_type>>;

   private:
    InputRange _input;
    // TODO?? remove, just modify the incoming presentation, then unmodify it
    Presentation<native_word_type> _get_presentation;

   public:
    ////////////////////////////////////////////////////////////////////////
    // Aliases
    ////////////////////////////////////////////////////////////////////////
    static constexpr bool is_finite     = rx::is_finite_v<InputRange>;
    static constexpr bool is_idempotent = rx::is_idempotent_v<InputRange>;

    using output_type = Presentation<native_word_type> const&;

    ////////////////////////////////////////////////////////////////////////
    // Constructors + initializers
    ////////////////////////////////////////////////////////////////////////
    // TODO? init functions?
    TietzeAddRelationRange(InputRange const& input)
        : _input(input), _get_presentation() {
      if (!_input.at_end()) {
        auto const& value = _input.get();
        _get_presentation = std::get<0>(value);
        presentation::add_rule(
            _get_presentation, std::get<1>(value), std::get<2>(value));
      }
    }
    // TODO rval ref constructor

    ////////////////////////////////////////////////////////////////////////
    // rx::ranges stuff
    ////////////////////////////////////////////////////////////////////////

    [[nodiscard]] output_type get() const {
      return _get_presentation;
    }

    // TODO try_get_and_advance

    void next() {
      _input.next();
      if (!_input.at_end()) {
        auto const& value = _input.get();
        _get_presentation = std::get<0>(value);
        presentation::add_rule(
            _get_presentation, std::get<1>(value), std::get<2>(value));
      }
    }

    [[nodiscard]] bool at_end() const {
      return _input.at_end();
    }

    [[nodiscard]] size_t size_hint() const {
      return _input.size_hint();
    }
  };  // class TietzeAddRelationRange

  struct TietzeAddRelation {
    template <typename InputRange>
    [[nodiscard]] auto operator()(InputRange&& input) const {
      return TietzeAddRelationRange(std::forward<InputRange>(input));
    }
  };

  template <typename Func>
  struct FindIf;

  template <typename InputRange, typename Func>
  class FindIfRange : public Runner {
    using input_type
        = std::decay_t<typename std::decay_t<InputRange>::output_type>;
    using input_const_reference = input_type const&;
    using input_reference       = input_type&;

    using invocable_type = Func;
    using invoke_result_type
        = std::invoke_result_t<Func, input_const_reference>;

    static_assert(std::is_same_v<invoke_result_type, bool>);

   private:
    class FindIfRunner : public Runner {
      FindIfRange*              _enclosing;
      bool                      _finished;
      invocable_type            _func;
      std::optional<input_type> _result;

     public:
      explicit FindIfRunner(FindIfRange* enclosing, Func const& func)
          : Runner(),
            _enclosing(enclosing),
            _finished(false),
            _func(func),
            _result(std::nullopt) {
        Runner::report_prefix("FindIf");
      }

      std::optional<input_type> const& result() const {
        return _result;
      }

     private:
      void run_impl() override {
        ReportGuard rg(false);
        input_type  input;
        while (!stopped() && _enclosing->try_get_and_advance(input)) {
          ++_enclosing->_counter;
          if (_func(input)) {
            _result   = input;
            _finished = true;
            return;
          }
        }
      }

      [[nodiscard]] bool finished_impl() const override {
        return _finished;
      }

    };  // class FindIfRunner

    ////////////////////////////////////////////////////////////////////////
    // Private data
    ////////////////////////////////////////////////////////////////////////
    std::atomic_size_t _counter;
    bool               _finished;
    Func               _func;
    InputRange         _input_range;
    size_t             _input_range_count;
    std::mutex         _mtx;
    size_t             _number_of_threads;
    detail::Race       _race;

   public:
    ////////////////////////////////////////////////////////////////////////
    // Constructors + initializers
    ////////////////////////////////////////////////////////////////////////

    FindIfRange(InputRange const&   input_range,
                Func&&              func,
                FindIf<Func> const& other)
        : _counter(0),
          _finished(false),
          _func(std::move(func)),
          _input_range(input_range),
          _input_range_count(other.total()),
          _mtx(),
          _number_of_threads(other.number_of_threads()),
          _race() {
      Runner::report_prefix("FindIf");
      _race.report_prefix("FindIf");
    }

    FindIfRange(InputRange const&   input_range,
                Func const&         func,
                FindIf<Func> const& other)
        : _counter(0),
          _finished(false),
          _func(std::move(func)),
          _input_range(input_range),
          _input_range_count(other.total()),
          _mtx(),
          _number_of_threads(other.number_of_threads()),
          _race() {
      Runner::report_prefix("FindIf");
      _race.report_prefix("FindIf");
    }

    FindIfRange(InputRange&&        input_range,
                Func&&              func,
                FindIf<Func> const& other)
        : _counter(0),
          _finished(false),
          _func(std::move(func)),
          _input_range(std::move(input_range)),
          _input_range_count(other.total()),
          _mtx(),
          _number_of_threads(other.number_of_threads()),
          _race() {
      Runner::report_prefix("FindIf");
      _race.report_prefix("FindIf");
    }

    FindIfRange(InputRange&&        input_range,
                Func const&         func,
                FindIf<Func> const& other)
        : _counter(0),
          _finished(false),
          _func(func),
          _input_range(std::move(input_range)),
          _input_range_count(other.total()),
          _mtx(),
          _number_of_threads(other.number_of_threads()),
          _race() {
      Runner::report_prefix("FindIf");
      _race.report_prefix("FindIf");
    }

    // TODO private
    [[nodiscard]] bool try_get_and_advance(input_reference result) {
      std::lock_guard lg(_mtx);
      if (!_input_range.at_end()) {
        result = _input_range.get();
        _input_range.next();
        return true;
      }
      return false;
    }

    // TODO rename get, implement next, at_end etc
    [[nodiscard]] std::optional<input_type> result() {
      Runner::run();
      if (_race.winner() == nullptr) {
        return std::nullopt;
      }
      return std::static_pointer_cast<FindIfRunner>(_race.winner())->result();
    }

    [[nodiscard]] size_t number_of_threads() const noexcept {
      return _number_of_threads;
    }

    FindIfRange& number_of_threads(size_t val) {
      if (val == 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "the argument (number of threads) must be at least 1, found {}",
            val);
      }
      _number_of_threads = val;
      return *this;
    }

    [[nodiscard]] size_t total() const noexcept {
      return _input_range_count;
    }

    FindIfRange& total(size_t val) {
      _input_range_count = val;
      return *this;
    }

   private:
    void report_progress_from_thread() const {
      using ::libsemigroups::detail::group_digits;
      using ::libsemigroups::detail::string_time;
      if (delta(start_time()) >= std::chrono::milliseconds(500)) {
        if (_input_range_count != std::numeric_limits<size_t>::max()) {
          size_t count         = _counter.load();
          auto   num_runs      = group_digits(_input_range_count);
          auto   elapsed       = delta(start_time());
          auto   mean_run_time = elapsed / count;
          auto   estimate      = _input_range_count * mean_run_time;
          fmt::print("#0: FindIf: {:>{}} / {} ({:>4.1f}%) @ ~{} "
                     "per run | {:>7} / {:<}\n",
                     group_digits(count),
                     num_runs.size(),
                     num_runs,
                     static_cast<float>(100 * count) / _input_range_count,
                     string_time(mean_run_time),
                     string_time(elapsed),
                     fmt::format("~{}", string_time(estimate)));
        } else {
          size_t count         = _counter.load();
          auto   elapsed       = delta(start_time());
          auto   mean_run_time = elapsed / count;
          fmt::print("#0: FindIf: {} @ ~{} per run | {}\n",
                     group_digits(count),
                     string_time(mean_run_time),
                     string_time(elapsed));
        }
      }
    }

    void run_impl() override {
      using std::chrono::duration_cast;
      using std::chrono::seconds;

      // TODO this is bad if there are already existing runners
      while (_race.number_of_runners() < number_of_threads()) {
        _race.add_runner(std::make_shared<FindIfRunner>(this, _func));
      }

      ::libsemigroups::detail::Ticker ticker;
      if ((!running_for()
           || duration_cast<seconds>(running_for_how_long()) >= seconds(1))) {
        ticker([this]() { report_progress_from_thread(); });
      }
      _race.run_until([this]() { return this->stopped(); });

      // TODO report_after_run();
      if (_race.finished() || !stopped()) {
        _finished = true;
      }
    }

    [[nodiscard]] bool finished_impl() const override {
      return _finished;
    }
  };  // class FindIfRange

  // TODO struct -> class
  template <typename Func>
  struct FindIf {
    Func   _func;
    size_t _number_of_threads;
    size_t _input_range_count;

    FindIf(Func&& func)
        : _func(std::forward<Func>(func)),
          _number_of_threads(1),
          _input_range_count(std::numeric_limits<size_t>::max()) {}

    template <typename InputRange>
    [[nodiscard]] auto operator()(InputRange&& input) {
      return FindIfRange(std::forward<InputRange>(input), _func, *this);
    }

    [[nodiscard]] size_t number_of_threads() const noexcept {
      return _number_of_threads;
    }

    FindIf& number_of_threads(size_t val) {
      if (val == 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "the argument (number of threads) must be at least 1, found {}",
            val);
      }
      _number_of_threads = val;
      return *this;
    }

    [[nodiscard]] size_t total() const noexcept {
      return _input_range_count;
    }

    FindIf& total(size_t val) {
      _input_range_count = val;
      return *this;
    }
  };

  template <typename InputRange>
  class AllAlphabetOrdersRange {
    using Word =
        typename std::decay_t<typename InputRange::output_type>::word_type;

    Word                _alphabet_orig;
    InputRange          _input;
    std::vector<size_t> _perm;
    Presentation<Word>  _presentation;

   public:
    ////////////////////////////////////////////////////////////////////////
    // Aliases
    ////////////////////////////////////////////////////////////////////////
    static constexpr bool is_finite     = true;
    static constexpr bool is_idempotent = true;
    using output_type                   = Presentation<Word> const&;

    AllAlphabetOrdersRange(InputRange&& input)
        : _alphabet_orig(), _input(std::move(input)), _perm(), _presentation() {
      if (!_input.at_end()) {
        _presentation  = _input.get();
        _alphabet_orig = _presentation.alphabet();
        _perm.resize(_alphabet_orig.size());
        std::iota(_perm.begin(), _perm.end(), 0);
      }
    }

    AllAlphabetOrdersRange(InputRange const& input)
        : AllAlphabetOrdersRange(InputRange(input)) {}

    [[nodiscard]] output_type get() const {
      return _presentation;
    }

    void next() {
      if (std::next_permutation(_perm.begin(), _perm.end())) {
        std::string alphabet_next;
        alphabet_next = _alphabet_orig;
        detail::apply_permutation(alphabet_next, _perm);
        _presentation.alphabet(alphabet_next);
        return;
      }
      _input.next();
      if (!_input.at_end()) {
        _presentation  = _input.get();
        _alphabet_orig = _presentation.alphabet();
        _perm.resize(_alphabet_orig.size());
        std::iota(_perm.begin(), _perm.end(), 0);
      }
    }

    [[nodiscard]] bool at_end() const {
      return _input.at_end();
    }

    [[nodiscard]] size_t size_hint() const {
      return std::numeric_limits<size_t>::max();
    }
  };

  struct AllAlphabetOrders {
    template <typename InputRange,
              typename = std::enable_if_t<rx::is_input_or_sink_v<InputRange>>>
    [[nodiscard]] auto operator()(InputRange&& input) const {
      return AllAlphabetOrdersRange(std::forward<InputRange>(input));
    }

    template <typename Word>
    [[nodiscard]] auto operator()(Presentation<Word> const& input) const {
      return operator()(Singleton(input));
    }
  };

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_TIETZE_HPP_

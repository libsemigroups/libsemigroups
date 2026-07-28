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
#include <memory>       // for make_shared, shared_ptr
#include <numeric>      // for accumulate
#include <optional>     // for optional
#include <queue>        // for queue
#include <string_view>  // for basic_string_view, string_view
#include <tuple>
#include <type_traits>    // for remove_cvref_t
#include <unordered_map>  // for unordered_map
#include <utility>        // for move
#include <vector>         // for vector

#include "adapters.hpp"      // for Hash
#include "constants.hpp"     // for POSITIVE_INFINITY
#include "debug.hpp"         // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"     // for LIBSEMIGROUPS_EXCEPTION
#include "presentation.hpp"  // for Presentation, operator!=, rep...
#include "runner.hpp"        // for Runner

#include "knuth-bendix-class.hpp"  // for KnuthBendix

#include "detail/fmt.hpp"   // for print
#include "detail/race.hpp"  // for Race

// TODO:
// * all calls to next + get must first check !at_end()
// * add static_assertions
// * iwyu
// * out of line

namespace libsemigroups {
  // forward decls
  class Subwords;

  template <typename Score>
  class SubwordsFreq;

  template <typename Func>
  struct FindIf;

  namespace detail {

    ////////////////////////////////////////////////////////////////////////
    // SubwordsSettings
    ////////////////////////////////////////////////////////////////////////

    // Class containing the common settings for SubwordsRange, Subwords,
    // SubwordsFreq, and SubwordsFreqRange
    class SubwordsSettings {
      size_t _max_length;
      size_t _min_length;
      bool   _proper;

     public:
      SubwordsSettings()
          : _max_length(POSITIVE_INFINITY), _min_length(0), _proper(false) {}

      SubwordsSettings(SubwordsSettings const&)            = default;
      SubwordsSettings(SubwordsSettings&&)                 = default;
      SubwordsSettings& operator=(SubwordsSettings const&) = default;
      SubwordsSettings& operator=(SubwordsSettings&&)      = default;

      ~SubwordsSettings() = default;

      [[nodiscard]] size_t max_length() const noexcept {
        return _max_length;
      }

      void max_length(size_t val) {
        _max_length = val;
      }

      [[nodiscard]] size_t min_length() const noexcept {
        return _min_length;
      }

      void min_length(size_t val) {
        _min_length = val;
      }

      [[nodiscard]] size_t proper() const noexcept {
        return _proper;
      }

      void proper(bool val) {
        _proper = val;
      }
    };

    ////////////////////////////////////////////////////////////////////////
    // SubwordsRange
    ////////////////////////////////////////////////////////////////////////

    template <typename InputRange>
    class SubwordsRange : public detail::SubwordsSettings {
     private:
      static_assert(
          is_specialization_of_v<std::decay_t<typename InputRange::output_type>,
                                 Presentation>);

      using Word =
          typename std::decay_t<typename InputRange::output_type>::word_type;

      using Settings = detail::SubwordsSettings;

      std::pair<Presentation<Word>, Word> _current;
      size_t                              _current_rule;
      InputRange                          _input;
      // We retain a copy of the input range so that we can re-initialise the
      // object if/when the settings are updated.
      InputRange                       _input_orig;
      size_t                           _prefix_end;
      std::unordered_map<Word, size_t> _seen;
      size_t                           _suffix_begin;

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

      SubwordsRange()                                = default;
      SubwordsRange(SubwordsRange const&)            = default;
      SubwordsRange(SubwordsRange&&)                 = default;
      SubwordsRange& operator=(SubwordsRange const&) = default;
      SubwordsRange& operator=(SubwordsRange&&)      = default;

      ~SubwordsRange() = default;

      SubwordsRange(InputRange&& input, Subwords const& subwords);
      SubwordsRange(InputRange const& input, Subwords const& subwords);

      ////////////////////////////////////////////////////////////////////////
      // Settings
      ////////////////////////////////////////////////////////////////////////

      using Settings::max_length;
      using Settings::min_length;
      using Settings::proper;

      SubwordsRange& max_length(size_t val);
      SubwordsRange& min_length(size_t val);
      SubwordsRange& proper(bool val);

      ////////////////////////////////////////////////////////////////////////
      // rx::ranges stuff
      ////////////////////////////////////////////////////////////////////////

      // Important note, you can modify _current.second (Word), because its
      // contents will be discarded at next call of next. You can also modify
      // _current.first (Presentation<Word>) if you put it back into its
      // original state before the next call to next().
      [[nodiscard]] output_type get() const {
        return _current;
      }

      void next();

      [[nodiscard]] bool at_end() const noexcept {
        return _input.at_end();
      }

      [[nodiscard]] size_t size_hint() const {
        return 0;
      }

      ////////////////////////////////////////////////////////////////////////
      // Other
      ////////////////////////////////////////////////////////////////////////

      [[nodiscard]] size_t frequency(Word const& w) const;

     private:
      ////////////////////////////////////////////////////////////////////////
      // Private
      ////////////////////////////////////////////////////////////////////////

      void init_from_input();
      void advance_prefix();
      void init_prefix_suffix();
      void advance_prefix_suffix();
    };  // class SubwordsRange
  }  // namespace detail

  // TODO doc
  class Subwords : public detail::SubwordsSettings {
    using Settings = detail::SubwordsSettings;

   public:
    ////////////////////////////////////////////////////////////////////////
    // Constructors + initializers
    ////////////////////////////////////////////////////////////////////////

    // TODO doc
    Subwords() = default;
    // TODO doc
    Subwords(Subwords const&) = default;
    // TODO doc
    Subwords(Subwords&&) = default;
    // TODO doc
    Subwords& operator=(Subwords const&) = default;
    // TODO doc
    Subwords& operator=(Subwords&&) = default;

    ~Subwords() = default;

    // TODO doc
    explicit Subwords(Settings const& settings) : Settings(settings) {}

    ////////////////////////////////////////////////////////////////////////
    // Call operator
    ////////////////////////////////////////////////////////////////////////

    // TODO doc
    template <typename InputRange,
              typename = std::enable_if_t<rx::is_input_or_sink_v<InputRange>>>
    [[nodiscard]] auto operator()(InputRange&& input) const {
      static_assert(
          is_specialization_of_v<std::decay_t<typename InputRange::output_type>,
                                 Presentation>);
      // We pass *this thru so that the settings are copied too
      return detail::SubwordsRange(std::forward<InputRange>(input), *this);
    }

    // TODO doc
    template <typename Word>
    [[nodiscard]] auto operator()(Presentation<Word> const& input) const {
      return operator()(Singleton(input));
    }

    // TODO operator()(Presentation<Word>&&)

    ////////////////////////////////////////////////////////////////////////
    // Settings
    ////////////////////////////////////////////////////////////////////////

    // TODO doc
    Subwords& min_length(size_t val) {
      Settings::min_length(val);
      return *this;
    }

    // TODO doc
    Subwords& max_length(size_t val) {
      Settings::max_length(val);
      return *this;
    }

    // TODO doc
    Subwords& proper(bool val) {
      Settings::proper(val);
      return *this;
    }
    // TODO doc from Settings
  };

  namespace detail {
    ////////////////////////////////////////////////////////////////////////
    // SubwordsFreqRange
    ////////////////////////////////////////////////////////////////////////

    template <typename InputRange, typename Score>
    class SubwordsFreqRange : public detail::SubwordsSettings {
      static_assert(
          is_specialization_of_v<std::decay_t<typename InputRange::output_type>,
                                 Presentation>);
      using Word =
          typename std::decay_t<typename InputRange::output_type>::word_type;

      using Settings = detail::SubwordsSettings;

      using value_type = std::tuple<Presentation<Word>, Word, size_t>;

      size_t     _index;
      InputRange _input;
      // We retain a copy of the input range so that we can re-initialise the
      // object if/when the settings are updated.
      InputRange              _input_orig;
      std::vector<value_type> _output_for_current_input;
      Score                   _score;

     public:
      ////////////////////////////////////////////////////////////////////////
      // Aliases + static data
      ////////////////////////////////////////////////////////////////////////

      using output_type = value_type const&;

      static constexpr bool is_finite     = rx::is_finite_v<InputRange>;
      static constexpr bool is_idempotent = rx::is_idempotent_v<InputRange>;

      ////////////////////////////////////////////////////////////////////////
      // Constructors + initializers
      ////////////////////////////////////////////////////////////////////////

      SubwordsFreqRange(InputRange const&          input,
                        SubwordsFreq<Score> const& settings,
                        Score const&               score);

      // TODO rvalue reference from InputRange and SubwordsFreq

      SubwordsFreqRange(SubwordsFreqRange const&)            = default;
      SubwordsFreqRange(SubwordsFreqRange&&)                 = default;
      SubwordsFreqRange& operator=(SubwordsFreqRange const&) = default;
      SubwordsFreqRange& operator=(SubwordsFreqRange&&)      = default;

      ~SubwordsFreqRange() = default;

      ////////////////////////////////////////////////////////////////////////
      // Settings
      ////////////////////////////////////////////////////////////////////////

      using Settings::max_length;
      using Settings::min_length;
      using Settings::proper;

      SubwordsFreqRange& min_length(size_t val);
      SubwordsFreqRange& max_length(size_t val);
      SubwordsFreqRange& proper(bool val);

      ////////////////////////////////////////////////////////////////////////
      // rx::ranges stuff
      ////////////////////////////////////////////////////////////////////////

      [[nodiscard]] output_type get() const {
        LIBSEMIGROUPS_ASSERT(!at_end());
        return _output_for_current_input[_index];
      }

      void next();

      [[nodiscard]] bool at_end() const noexcept {
        // The second part of the expression below is in case !_input.at_end()
        // but _input.get() is empty.
        return _input.at_end() || _index >= _output_for_current_input.size();
      }

      [[nodiscard]] size_t size_hint() const {
        return 0;
      }

      // TODO begin/end?

     private:
      void init_from_input();
    };  // class SubwordsFreqRange
  }  // namespace detail

  // TODO doc
  template <typename Score>
  class SubwordsFreq : public detail::SubwordsSettings {
   private:
    using Settings = detail::SubwordsSettings;

    Score _score;

   public:
    ////////////////////////////////////////////////////////////////////////
    // Constructors + initializers
    ////////////////////////////////////////////////////////////////////////

    // TODO doc
    SubwordsFreq(SubwordsFreq const&) = default;
    // TODO doc
    SubwordsFreq(SubwordsFreq&&) = default;
    // TODO doc
    SubwordsFreq& operator=(SubwordsFreq const&) = default;
    // TODO doc
    SubwordsFreq& operator=(SubwordsFreq&&) = default;

    ~SubwordsFreq() = default;

    // TODO doc
    explicit SubwordsFreq(Score&& score)
        : Settings(), _score(std::move(score)) {}
    // TODO doc
    explicit SubwordsFreq(Score const& score) : Settings(), _score(score) {}

    ////////////////////////////////////////////////////////////////////////
    // Call operator
    ////////////////////////////////////////////////////////////////////////

    // TODO doc
    template <typename InputRange,
              typename = std::enable_if_t<rx::is_input_or_sink_v<InputRange>>>
    [[nodiscard]] auto operator()(InputRange&& input) const {
      static_assert(
          is_specialization_of_v<std::decay_t<typename InputRange::output_type>,
                                 Presentation>);
      // Pass *this to pass thru the settings
      return detail::SubwordsFreqRange(
          std::forward<InputRange>(input), *this, _score);
    }

    // TODO doc
    template <typename Word>
    [[nodiscard]] auto operator()(Presentation<Word> const& input) const {
      return operator()(Singleton(input));
    }

    ////////////////////////////////////////////////////////////////////////
    // Settings
    ////////////////////////////////////////////////////////////////////////

    // TODO doc
    SubwordsFreq& min_length(size_t val) {
      Settings::min_length(val);
      return *this;
    }

    // TODO doc
    SubwordsFreq& max_length(size_t val) {
      Settings::max_length(val);
      return *this;
    }

    // TODO doc
    SubwordsFreq& proper(bool val) {
      Settings::proper(val);
      return *this;
    }
    // TODO doc other things from Settings
  };  // class SubwordsFreq

  ////////////////////////////////////////////////////////////////////////
  // TietzeAddGeneratorRange
  ////////////////////////////////////////////////////////////////////////

  namespace detail {
    template <typename InputRange>
    class TietzeAddGeneratorRange {
      // TODO add static assertion that the InputRange has the correct type of
      // output, whatever that is
     public:
      using native_word_type = std::
          tuple_element_t<1, std::decay_t<typename InputRange::output_type>>;

     private:
      InputRange                     _input;
      Presentation<native_word_type> _get_presentation;

     public:
      ////////////////////////////////////////////////////////////////////////
      // Aliases + static data
      ////////////////////////////////////////////////////////////////////////

      using output_type = Presentation<native_word_type> const&;

      static constexpr bool is_finite     = rx::is_finite_v<InputRange>;
      static constexpr bool is_idempotent = rx::is_idempotent_v<InputRange>;

      ////////////////////////////////////////////////////////////////////////
      // Constructors + initializers
      ////////////////////////////////////////////////////////////////////////

      TietzeAddGeneratorRange()                               = default;
      TietzeAddGeneratorRange(TietzeAddGeneratorRange const&) = default;
      TietzeAddGeneratorRange(TietzeAddGeneratorRange&&)      = default;
      TietzeAddGeneratorRange& operator=(TietzeAddGeneratorRange const&)
          = default;
      TietzeAddGeneratorRange& operator=(TietzeAddGeneratorRange&&) = default;

      ~TietzeAddGeneratorRange() = default;

      // TODO init functions?

      explicit TietzeAddGeneratorRange(InputRange const& input)
          : _input(input), _get_presentation() {
        init_from_input();
      }

      explicit TietzeAddGeneratorRange(InputRange&& input)
          : _input(std::move(input)), _get_presentation() {
        init_from_input();
      }

      ////////////////////////////////////////////////////////////////////////
      // rx::ranges stuff
      ////////////////////////////////////////////////////////////////////////

      [[nodiscard]] output_type get() const {
        return _get_presentation;
      }

      void next();

      [[nodiscard]] bool at_end() const {
        return _input.at_end();
      }

      [[nodiscard]] size_t size_hint() const {
        return _input.size_hint();
      }

     private:
      void init_from_input();
    };  // class TietzeAddGeneratorRange
  }  // namespace detail

  // TODO doc
  struct TietzeAddGenerator {
    // TODO doc
    TietzeAddGenerator() = default;
    // TODO doc
    TietzeAddGenerator(TietzeAddGenerator const&) = default;
    // TODO doc
    TietzeAddGenerator(TietzeAddGenerator&&) = default;
    // TODO doc
    TietzeAddGenerator& operator=(TietzeAddGenerator const&) = default;
    // TODO doc
    TietzeAddGenerator& operator=(TietzeAddGenerator&&) = default;

    ~TietzeAddGenerator() = default;

    // TODO doc
    template <typename InputRange>
    [[nodiscard]] auto operator()(InputRange&& input) const {
      return detail::TietzeAddGeneratorRange(std::forward<InputRange>(input));
    }

    template <typename Word>
    [[nodiscard]] auto operator()(Presentation<Word> const& input) const {
      return operator()(Singleton(input));
    }

    // TODO operator()(Presentation<Word>&&)
  };  // struct TietzeAddGenerator

  ////////////////////////////////////////////////////////////////////////
  // TietzeAddRelationRange
  ////////////////////////////////////////////////////////////////////////

  namespace detail {
    template <typename InputRange>
    class TietzeAddRelationRange {
      // TODO add static assertion that the InputRange has the correct type of
      // output, whatever that is
     public:
      using native_word_type = std::
          tuple_element_t<1, std::decay_t<typename InputRange::output_type>>;

     private:
      InputRange                     _input;
      Presentation<native_word_type> _get_presentation;

     public:
      ////////////////////////////////////////////////////////////////////////
      // Aliases
      ////////////////////////////////////////////////////////////////////////

      using output_type = Presentation<native_word_type> const&;

      static constexpr bool is_finite     = rx::is_finite_v<InputRange>;
      static constexpr bool is_idempotent = rx::is_idempotent_v<InputRange>;

      ////////////////////////////////////////////////////////////////////////
      // Constructors + initializers
      ////////////////////////////////////////////////////////////////////////

      TietzeAddRelationRange()                              = default;
      TietzeAddRelationRange(TietzeAddRelationRange const&) = default;
      TietzeAddRelationRange(TietzeAddRelationRange&&)      = default;
      TietzeAddRelationRange& operator=(TietzeAddRelationRange const&)
          = default;
      TietzeAddRelationRange& operator=(TietzeAddRelationRange&&) = default;

      ~TietzeAddRelationRange() = default;

      // TODO init functions?

      explicit TietzeAddRelationRange(InputRange const& input)
          : _input(input), _get_presentation() {
        init_from_input();
      }

      explicit TietzeAddRelationRange(InputRange&& input)
          : _input(std::move(input)), _get_presentation() {
        init_from_input();
      }

      ////////////////////////////////////////////////////////////////////////
      // rx::ranges stuff
      ////////////////////////////////////////////////////////////////////////

      [[nodiscard]] output_type get() const {
        return _get_presentation;
      }

      void next();

      [[nodiscard]] bool at_end() const {
        return _input.at_end();
      }

      [[nodiscard]] size_t size_hint() const {
        return _input.size_hint();
      }

     private:
      void init_from_input();
    };  // class TietzeAddRelationRange
  }  // namespace detail

  // TODO doc
  struct TietzeAddRelation {
    // TODO doc
    TietzeAddRelation() = default;
    // TODO doc
    TietzeAddRelation(TietzeAddRelation const&) = default;
    // TODO doc
    TietzeAddRelation(TietzeAddRelation&&) = default;
    // TODO doc
    TietzeAddRelation& operator=(TietzeAddRelation const&) = default;
    // TODO doc
    TietzeAddRelation& operator=(TietzeAddRelation&&) = default;

    ~TietzeAddRelation() = default;

    template <typename InputRange>
    [[nodiscard]] auto operator()(InputRange&& input) const {
      return detail::TietzeAddRelationRange(std::forward<InputRange>(input));
    }

    template <typename Word>
    [[nodiscard]] auto operator()(Presentation<Word> const& input) const {
      return operator()(Singleton(input));
    }

    // TODO operator()(Presentation<Word>&&)
  };  // struct TietzeAddRelation

  // HERE
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
    [[nodiscard]] std::optional<input_type> get() {
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

    explicit FindIf(Func&& func)
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
    static constexpr bool is_finite     = true;  // TODO depends on InputRange
    static constexpr bool is_idempotent = true;
    using output_type                   = Presentation<Word> const&;

    explicit AllAlphabetOrdersRange(InputRange&& input)
        : _alphabet_orig(), _input(std::move(input)), _perm(), _presentation() {
      if (!_input.at_end()) {
        _presentation  = _input.get();
        _alphabet_orig = _presentation.alphabet();
        _perm.resize(_alphabet_orig.size());
        std::iota(_perm.begin(), _perm.end(), 0);
      }
    }

    explicit AllAlphabetOrdersRange(InputRange const& input)
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
      // We opt for 0 instead of std::numeric_limits<size_t>::max() because it
      // seems that rx::ranges uses this to std::vector::reserve in some places,
      // and using the max possible value leads to exceptions being thrown.
      return 0;
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

  template <typename InputRange>
  class AllAlphabetOrderExtsRange {
    // TODO static assert
    using Word =
        typename std::decay_t<typename InputRange::output_type>::word_type;

    Presentation<Word> _get_presentation;
    size_t             _index;
    InputRange         _input;

   public:
    ////////////////////////////////////////////////////////////////////////
    // Aliases
    ////////////////////////////////////////////////////////////////////////
    static constexpr bool is_finite     = rx::is_finite_v<InputRange>;
    static constexpr bool is_idempotent = rx::is_idempotent_v<InputRange>;
    using output_type                   = Presentation<Word> const&;

    explicit AllAlphabetOrderExtsRange(InputRange&& input)
        : _get_presentation(), _index(), _input(std::move(input)) {
      if (!_input.at_end()) {
        _get_presentation = _input.get();
        if (!_get_presentation.alphabet().empty()) {
          _index = _get_presentation.alphabet().size() - 1;
        } else {
          _index = 0;
        }
      }
    }

    explicit AllAlphabetOrderExtsRange(InputRange const& input)
        : AllAlphabetOrderExtsRange(InputRange(input)) {}

    [[nodiscard]] output_type get() const {
      return _get_presentation;
    }

    void next() {
      if (at_end()) {
        return;
      }
      if (_index > 0) {
        _index--;
        auto new_alphabet = _get_presentation.alphabet();
        std::swap(new_alphabet[_index], new_alphabet[_index + 1]);
        _get_presentation.alphabet(new_alphabet);
        return;
      }
      LIBSEMIGROUPS_ASSERT(!_input.at_end());
      _input.next();
      if (!_input.at_end()) {
        _get_presentation = _input.get();
        if (!_get_presentation.alphabet().empty()) {
          _index = _get_presentation.alphabet().size() - 1;
        } else {
          _index = 0;
        }
      }
    }

    [[nodiscard]] bool at_end() const {
      return _input.at_end();
    }

    [[nodiscard]] size_t size_hint() const {
      // Can't guess this because we don't know what _input might contain.
      // We opt for 0 instead of std::numeric_limits<size_t>::max() because it
      // seems that rx::ranges uses this to std::vector::reserve in some places,
      // and using the max possible value leads to exceptions being thrown.
      return 0;
    }
  };

  struct AllAlphabetOrderExts {
    template <typename InputRange,
              typename = std::enable_if_t<rx::is_input_or_sink_v<InputRange>>>
    [[nodiscard]] auto operator()(InputRange&& input) const {
      return AllAlphabetOrderExtsRange(std::forward<InputRange>(input));
    }

    template <typename Word>
    [[nodiscard]] auto operator()(Presentation<Word> const& input) const {
      return operator()(Singleton(input));
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // PedersenPestov
  ////////////////////////////////////////////////////////////////////////

  template <size_t Depth,
            typename InputRange,
            typename Word,
            typename RewritingSystem>
  class PedersenPestovRange {
   private:
    struct State {
      KnuthBendix<Word, RewritingSystem> kb;
      std::vector<Presentation<Word>>    presentations;

      State(KnuthBendix<Word, RewritingSystem> const& kb, size_t depth)
          : kb(kb), presentations(depth) {}
    };

    using StatePtr = std::shared_ptr<State>;

    class Builder : public detail::SubwordsSettings {
      using Settings = detail::SubwordsSettings;

     public:
      explicit Builder(Settings const& settings) : Settings(settings) {}

      template <size_t N, typename Range>
      auto add_steps(Range&& input, StatePtr state) const {
        static_assert(N < Depth);

        LIBSEMIGROUPS_ASSERT(N < state->presentations.size());

        using rx::operator|;

        auto run_knuth_bendix = [state](Presentation<Word> const& p) {
          state->kb.init(congruence_kind::twosided, p);
          state->kb.rewriting_system().sort_pending_rules_by(nullptr);
          state->kb.rewriting_system().settings().reduction_threshold
              = POSITIVE_INFINITY;
          state->kb.max_rounds(2).run();
          return to<Presentation>(state->kb);
        };

        auto score_by_freq
            = [](std::tuple<Presentation<Word>, Word, size_t> const& tup1,
                 std::tuple<Presentation<Word>, Word, size_t> const& tup2) {
                return std::get<2>(tup1) > std::get<2>(tup2);
              };

        if constexpr (N == 0) {
          auto step = (std::forward<Range>(input) | AllAlphabetOrders()
                       | Ref(state->presentations[N])
                       | rx::transform(run_knuth_bendix)
                       | SubwordsFreq(score_by_freq)
                             .max_length(max_length())
                             .min_length(min_length())
                             .proper(proper())
                       | rx::transform([state](auto const& tup) {
                           auto copy(state->presentations[N]);
                           presentation::replace_word_with_new_generator(
                               copy, std::get<1>(tup));
                           return copy;
                         }));

          if constexpr (N + 1 == Depth) {
            return (step | AllAlphabetOrderExts());
          } else {
            return add_steps<N + 1>(std::move(step), state);
          }
        } else {
          auto step = (std::forward<Range>(input) | AllAlphabetOrderExts()
                       | Ref(state->presentations[N])
                       | rx::transform(run_knuth_bendix)
                       | SubwordsFreq(score_by_freq)
                             .max_length(max_length())
                             .min_length(min_length())
                             .proper(proper())
                       | rx::transform([state](auto const& tup) {
                           auto copy(state->presentations[N]);
                           presentation::replace_word_with_new_generator(
                               copy, std::get<1>(tup));
                           return copy;
                         }));

          if constexpr (N + 1 == Depth) {
            return (step | AllAlphabetOrderExts());
          } else {
            return add_steps<N + 1>(std::move(step), state);
          }
        }
      }
    };

    using InnerRange
        = decltype(std::declval<Builder const&>().template add_steps<0>(
            std::declval<InputRange>(),
            std::declval<StatePtr>()));

    InputRange                         _input_orig;
    Builder                            _builder;
    KnuthBendix<Word, RewritingSystem> _kb_orig;
    StatePtr                           _state;
    std::optional<InnerRange>          _inner;

   public:
    ////////////////////////////////////////////////////////////////////////
    // Aliases
    ////////////////////////////////////////////////////////////////////////

    using output_type = typename InnerRange::output_type;

    static constexpr bool is_finite     = rx::is_finite_v<InnerRange>;
    static constexpr bool is_idempotent = rx::is_idempotent_v<InnerRange>;

    ////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////

    PedersenPestovRange(InputRange&&                              input,
                        KnuthBendix<Word, RewritingSystem> const& kb,
                        detail::SubwordsSettings const&           settings)
        : _input_orig(std::move(input)),
          _builder(settings),
          _kb_orig(kb),
          _state(),
          _inner() {
      init_inner();
    }

    PedersenPestovRange(InputRange const&                         input,
                        KnuthBendix<Word, RewritingSystem> const& kb,
                        detail::SubwordsSettings const&           settings)
        : _input_orig(input),
          _builder(settings),
          _kb_orig(kb),
          _state(),
          _inner() {
      init_inner();
    }

    PedersenPestovRange(PedersenPestovRange const& that)
        : _input_orig(that._input_orig),
          _builder(that._builder),
          _kb_orig(that._kb_orig),
          _state(),
          _inner() {
      init_inner();
    }

    PedersenPestovRange(PedersenPestovRange&&) = default;

    PedersenPestovRange& operator=(PedersenPestovRange const& that) {
      _input_orig = that._input_orig;
      _builder    = that._builder;
      _kb_orig    = that._kb_orig;
      init_inner();
      return *this;
    }

    PedersenPestovRange& operator=(PedersenPestovRange&&) = default;

    ////////////////////////////////////////////////////////////////////////
    // Input range
    ////////////////////////////////////////////////////////////////////////

    [[nodiscard]] output_type get() const {
      return _inner->get();
    }

    void next() {
      _inner->next();
    }

    [[nodiscard]] bool at_end() const {
      return _inner->at_end();
    }

    [[nodiscard]] size_t size_hint() const {
      return _inner->size_hint();
    }

   private:
    void init_inner() {
      _state = std::make_shared<State>(_kb_orig, Depth);
      _inner.emplace(
          _builder.template add_steps<0>(InputRange(_input_orig), _state));
    }
  };  // class PedersenPestovRange

  template <size_t Depth, typename Word, typename RewritingSystem>
  class PedersenPestov : public detail::SubwordsSettings {
   private:
    using Settings = detail::SubwordsSettings;

    KnuthBendix<Word, RewritingSystem> _kb;

   public:
    explicit PedersenPestov(KnuthBendix<Word, RewritingSystem> const& kb)
        : Settings(), _kb(kb) {
      min_length(2);
      max_length(6);
      proper(true);
    }

    ////////////////////////////////////////////////////////////////////////
    // Settings
    ////////////////////////////////////////////////////////////////////////

    using Settings::max_length;
    using Settings::min_length;
    using Settings::proper;

    PedersenPestov& max_length(size_t val) {
      Settings::max_length(val);
      return *this;
    }

    PedersenPestov& min_length(size_t val) {
      // TODO throw if val < 2
      Settings::min_length(val);
      return *this;
    }

    PedersenPestov& proper(bool val) {
      Settings::proper(val);
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // Main event
    ////////////////////////////////////////////////////////////////////////

    template <typename InputRange,
              typename = std::enable_if_t<rx::is_input_or_sink_v<InputRange>>>
    [[nodiscard]] auto operator()(InputRange&& input) const {
      return PedersenPestovRange<Depth,
                                 std::decay_t<InputRange>,
                                 Word,
                                 RewritingSystem>(
          std::forward<InputRange>(input), _kb, *this);
    }

    [[nodiscard]] auto operator()(Presentation<Word> const& input) const {
      return operator()(Singleton(input));
    }
  };  // class PedersenPestov

  template <size_t Depth, typename Word, typename RewritingSystem>
  auto pedersen_pestov(KnuthBendix<Word, RewritingSystem> const& kb) {
    return PedersenPestov<Depth, Word, RewritingSystem>(kb);
  }

  template <typename InputRange>
  class AllRuleOrdersRange {
    using Word =
        typename std::decay_t<typename InputRange::output_type>::word_type;

    InputRange         _input;
    uint32_t           _max_perm;
    uint32_t           _perm;
    Presentation<Word> _presentation;

   public:
    ////////////////////////////////////////////////////////////////////////
    // Aliases
    ////////////////////////////////////////////////////////////////////////
    static constexpr bool is_finite     = true;
    static constexpr bool is_idempotent = true;
    using output_type                   = Presentation<Word> const&;

    explicit AllRuleOrdersRange(InputRange&& input)
        : _input(std::move(input)), _max_perm(), _perm(0), _presentation() {
      if (!_input.at_end()) {
        _presentation = _input.get();
        LIBSEMIGROUPS_ASSERT(_presentation.rules.size() <= 62);
        _max_perm = 1 << (_presentation.rules.size() / 2);
      }
    }

    explicit AllRuleOrdersRange(InputRange const& input)
        : AllRuleOrdersRange(InputRange(input)) {}

    [[nodiscard]] output_type get() const {
      return _presentation;
    }

    void next() {
      ++_perm;
      if (_perm < _max_perm) {
        // A permutation that undoes the previous permutation, and applies the
        // current permutation
        uint32_t incremental_perm = (_perm - 1) ^ _perm;

        for (size_t i = 0; i < _presentation.rules.size() / 2; ++i) {
          // Swap the ith lhs and rhs if the ith bit of the incremental
          // permutation tells us to do so.
          if (incremental_perm & (1 << i)) {
            std::swap(_presentation.rules[2 * i],
                      _presentation.rules[(2 * i) + 1]);
          }
        }
        return;
      }
      _input.next();
      if (!_input.at_end()) {
        _presentation = _input.get();
        LIBSEMIGROUPS_ASSERT(_presentation.rules.size() <= 62);
        _max_perm = 1 << (_presentation.rules.size() / 2);
        _perm     = 0;
      }
    }

    [[nodiscard]] bool at_end() const {
      return _input.at_end();
    }

    [[nodiscard]] size_t size_hint() const {
      return 0;
    }
  };

  struct AllRuleOrders {
    template <typename InputRange,
              typename = std::enable_if_t<rx::is_input_or_sink_v<InputRange>>>
    [[nodiscard]] auto operator()(InputRange&& input) const {
      return AllRuleOrdersRange(std::forward<InputRange>(input));
    }

    template <typename Word>
    [[nodiscard]] auto operator()(Presentation<Word> const& input) const {
      return operator()(Singleton(input));
    }
  };

}  // namespace libsemigroups

#include "tietze.tpp"
#endif  // LIBSEMIGROUPS_TIETZE_HPP_

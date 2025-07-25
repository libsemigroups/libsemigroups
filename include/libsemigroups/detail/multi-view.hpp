//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021-2025 James D. Mitchell + Maria Tsalakou
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

// TODO(later)
// - more benchmarks
// - make a tpp file

#ifndef LIBSEMIGROUPS_DETAIL_MULTI_VIEW_HPP_
#define LIBSEMIGROUPS_DETAIL_MULTI_VIEW_HPP_

#include <algorithm>  // for equal, lexicographical_compare
#include <cstddef>    // for size_t, etc
#include <cstdint>    // for uint32_t, etc
#include <iterator>   // for reverse_iterator
#include <numeric>    // for accumulate
#include <string>     // for string
#include <utility>    // for make_pair
#include <vector>     // for vector

#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION

#include "iterator.hpp"  // for ConstIteratorStateful, ConstIteratorTraits
#include "string.hpp"    // for is_prefix

namespace libsemigroups {
  namespace detail {
    // TODO(v4) remove default value
    template <typename Thing = std::string>
    class View {
     public:
      ////////////////////////////////////////////////////////////////////////
      // View - type aliases - public
      ////////////////////////////////////////////////////////////////////////

      using view_of_type           = Thing;
      using const_iterator_view_of = typename view_of_type::const_iterator;

      View(const_iterator_view_of first, const_iterator_view_of last)
          : _begin(first), _length(last - first) {
        LIBSEMIGROUPS_ASSERT(first <= last);
      }

      uint32_t const& size() const noexcept {
        return _length;
      }

      uint32_t& size() noexcept {
        return _length;
      }

      bool empty() const noexcept {
        return _length == 0;
      }

      char operator[](size_t pos) const {
        return *(begin() + pos);
      }

      const_iterator_view_of& begin() noexcept {
        return _begin;
      }

      const_iterator_view_of const& begin() const noexcept {
        return _begin;
      }

      const_iterator_view_of end() const noexcept {
        return begin() + size();
      }

     private:
      const_iterator_view_of _begin;
      uint32_t               _length;
    };  // class View

    ////////////////////////////////////////////////////////////////////////
    // ViewContainer
    ////////////////////////////////////////////////////////////////////////

    // TODO remove default value?
    template <typename Thing = std::string>
    class ViewContainer {
      using view_of_type = Thing;
      using const_iterator_view_of =
          typename View<view_of_type>::const_iterator_view_of;

     public:
      ViewContainer() : _is_long(false) {}

      ViewContainer(ViewContainer const& that) : _is_long(that._is_long) {
        if (!_is_long) {
          _data._short = that._data._short;
        } else {
          _data.activate_long();
          _data._long = that._data._long;
        }
      }

      ViewContainer(ViewContainer&& that) : _is_long(that._is_long) {
        if (!_is_long) {
          _data._short = std::move(that._data._short);
        } else {
          _data.activate_long();
          _data._long = std::move(that._data._long);
        }
      }

      ViewContainer& operator=(ViewContainer const& that) {
        if (is_long()) {
          if (that.is_long()) {
            _data._long = that._data._long;
          } else {
            _data.activate_short();
            _data._short = that._data._short;
          }
        } else {
          if (that.is_long()) {
            _data.activate_long();
            _data._long = that._data._long;
          } else {
            _data._short = that._data._short;
          }
        }
        _is_long = that.is_long();
        return *this;
      }

      ViewContainer& operator=(ViewContainer&& that) {
        if (is_long()) {
          if (that.is_long()) {
            _data._long = std::move(that._data._long);
          } else {
            _data.activate_short();
            _data._short = std::move(that._data._short);
          }
        } else {
          if (that.is_long()) {
            _data.activate_long();
            _data._long = std::move(that._data._long);
          } else {
            _data._short = std::move(that._data._short);
          }
        }
        _is_long = that.is_long();
        return *this;
      }

      ~ViewContainer() {
        if (!is_long()) {
          _data.destroy_short();
        } else {
          _data.destroy_long();
        }
      }

      class Short {
       public:
        Short() : _begin1(), _begin2(), _length1(0), _length2(0) {}
        Short(Short const&)            = default;
        Short(Short&&)                 = default;
        Short& operator=(Short const&) = default;
        Short& operator=(Short&&)      = default;

        const_iterator_view_of const& begin(size_t i) const {
          if (i == 0) {
            return _begin1;
          } else {
            LIBSEMIGROUPS_ASSERT(i == 1);
            return _begin2;
          }
        }

        const_iterator_view_of& begin(size_t i) {
          if (i == 0) {
            return _begin1;
          } else {
            LIBSEMIGROUPS_ASSERT(i == 1);
            return _begin2;
          }
        }

        const_iterator_view_of end(size_t i) const {
          if (i == 0) {
            return _begin1 + _length1;
          } else {
            LIBSEMIGROUPS_ASSERT(i == 1);
            return _begin2 + _length2;
          }
        }

        size_t number_of_views() const noexcept {
          return (_length2 != 0 ? 2 : _length1 != 0);
          // It can happen that _length2 !=0 but _length1 == 0 in the middle
          // of an erase before we've moved the second position to the first.
        }

        uint32_t size() const {
          return _length1 + _length2;
        }

        uint32_t& size(size_t i) {
          if (i == 0) {
            return _length1;
          } else {
            LIBSEMIGROUPS_ASSERT(i == 1);
            return _length2;
          }
        }

        uint32_t const& size(size_t i) const {
          if (i == 0) {
            return _length1;
          } else {
            LIBSEMIGROUPS_ASSERT(i == 1);
            return _length2;
          }
        }

        bool empty(size_t i) const noexcept {
          LIBSEMIGROUPS_ASSERT(i < 2);
          return (i == 0 && _length1 == 0) || (i == 1 && _length2 == 0);
        }

        void emplace_back(const_iterator_view_of first,
                          const_iterator_view_of last) {
          LIBSEMIGROUPS_ASSERT(first < last);
          if (number_of_views() == 0) {
            _begin1  = first;
            _length1 = last - first;
          } else {
            LIBSEMIGROUPS_ASSERT(number_of_views() == 1);
            _begin2  = first;
            _length2 = last - first;
          }
        }

        void clear() {
          _length1 = 0;
          _length2 = 0;
        }

        bool empty() const {
          return size() == 0;
        }

        void pop_back() {
          if (number_of_views() == 2) {
            _length2 = 0;
          } else if (number_of_views() == 1) {
            _length1 = 0;
          }
          // do nothing if empty
        }

        size_t insert(size_t pos, View<view_of_type>&& sv) {
          LIBSEMIGROUPS_ASSERT(pos < 2);
          LIBSEMIGROUPS_ASSERT(number_of_views() != 2);
          if (pos == 0) {
            if (number_of_views() == 1) {
              _begin2  = _begin1;
              _length2 = _length1;
            }
            _begin1  = sv.begin();
            _length1 = sv.size();
          } else if (pos == 1) {
            LIBSEMIGROUPS_ASSERT(number_of_views() == 1);
            _begin2  = sv.begin();
            _length2 = sv.size();
          }
          return pos;
        }

        size_t erase(size_t first, size_t last) {
          if (empty()) {
            // It can happen that one of the next assertions is false because
            // if we are part way through a call to MultiView::erase
            // because number_of_views() is determined by examining _length1
            // and _length2, which might already have been updated.
            return 0;
          }
          LIBSEMIGROUPS_ASSERT(first <= last);
          LIBSEMIGROUPS_ASSERT(last <= number_of_views());
          if (first == 0 && last == 1) {
            if (number_of_views() == 1) {
              _length1 = 0;
              return 0;
            } else {
              LIBSEMIGROUPS_ASSERT(number_of_views() == 2);
              _begin1  = _begin2;
              _length1 = _length2;
              _length2 = 0;
              return 1;
            }
          } else if (first == 0 && last == 2) {
            LIBSEMIGROUPS_ASSERT(number_of_views() == 2);
            clear();
            return 0;
          } else if (first == 1 && last == 2) {
            LIBSEMIGROUPS_ASSERT(number_of_views() == 2);
            pop_back();
            return 1;
          }
          LIBSEMIGROUPS_ASSERT(false);
          // unreachable
          return -1;
        }

       private:
        const_iterator_view_of _begin1;
        const_iterator_view_of _begin2;
        uint32_t               _length1;
        uint32_t               _length2;
      };  // class Short

      class Long {
       public:
        ~Long() {}
        const_iterator_view_of const& begin(size_t i) const {
          LIBSEMIGROUPS_ASSERT(i < _views.size());
          return _views[i].begin();
        }

        const_iterator_view_of& begin(size_t i) {
          LIBSEMIGROUPS_ASSERT(i < _views.size());
          return _views[i].begin();
        }

        const_iterator_view_of end(size_t i) const {
          LIBSEMIGROUPS_ASSERT(i < _views.size());
          return _views[i].end();
        }

        size_t number_of_views() const noexcept {
          return _views.size();
        }

        size_t size() const {
          return std::accumulate(
              _views.cbegin(),
              _views.cend(),
              0,
              [](size_t result, View<view_of_type> const& sv) {
                return result + sv.size();
              });
        }

        uint32_t const& size(size_t i) const {
          LIBSEMIGROUPS_ASSERT(i < _views.size());
          return _views[i].size();
        }

        uint32_t& size(size_t i) {
          LIBSEMIGROUPS_ASSERT(i < _views.size());
          return _views[i].size();
        }

        bool empty(size_t i) const {
          LIBSEMIGROUPS_ASSERT(i < _views.size());
          return _views[i].empty();
        }

        bool empty() const {
          return _views.empty();
        }

        void emplace_back(const_iterator_view_of first,
                          const_iterator_view_of last) {
          LIBSEMIGROUPS_ASSERT(first < last);
          _views.emplace_back(first, last);
        }

        void clear() {
          _views.clear();
        }

        void pop_back() {
          if (!empty()) {  // for consistency with Short
            _views.pop_back();
          }
        }

        size_t insert(size_t pos, View<view_of_type>&& sv) {
          auto it = _views.insert(_views.begin() + pos, sv);
          return it - _views.cbegin();
        }

        void erase(size_t first, size_t last) {
          if (first <= last && !empty()) {
            _views.erase(_views.begin() + first, _views.begin() + last);
          }
        }

       private:
        std::vector<View<view_of_type>> _views;
      };  // class Long

      ////////////////////////////////////////////////////////////////////////
      // ViewContainer - public
      ////////////////////////////////////////////////////////////////////////

      inline bool is_long() const noexcept {
        return _is_long;
      }

      const_iterator_view_of const& begin(size_t i) const {
        if (!is_long()) {
          return _data._short.begin(i);
        } else {
          return _data._long.begin(i);
        }
      }

      const_iterator_view_of& begin(size_t i) {
        if (!is_long()) {
          return _data._short.begin(i);
        } else {
          return _data._long.begin(i);
        }
      }

      const_iterator_view_of end(size_t i) const {
        if (!is_long()) {
          return _data._short.end(i);
        } else {
          return _data._long.end(i);
        }
      }

      bool empty(size_t i) const {
        if (!is_long()) {
          return _data._short.empty(i);
        } else {
          return _data._long.empty(i);
        }
      }

      bool empty() const {
        if (!is_long()) {
          return _data._short.empty();
        } else {
          return _data._long.empty();
        }
      }

      size_t size() const {
        if (!is_long()) {
          return _data._short.size();
        } else {
          return _data._long.size();
        }
      }

      uint32_t const& size(size_t i) const {
        LIBSEMIGROUPS_ASSERT(i < number_of_views());
        if (!is_long()) {
          return _data._short.size(i);
        } else {
          return _data._long.size(i);
        }
      }

      uint32_t& size(size_t i) {
        LIBSEMIGROUPS_ASSERT(i < number_of_views());
        if (!is_long()) {
          return _data._short.size(i);
        } else {
          return _data._long.size(i);
        }
      }

      size_t number_of_views() const {
        if (!is_long()) {
          return _data._short.number_of_views();
        } else {
          return _data._long.number_of_views();
        }
      }

      void emplace_back(const_iterator_view_of first,
                        const_iterator_view_of last) {
        if (last <= first) {
          return;
        }
        if (!is_long()) {
          if (_data._short.number_of_views() < 2) {
            _data._short.emplace_back(first, last);
          } else {
            activate_long();
            emplace_back(first, last);
          }
        } else {
          _data._long.emplace_back(first, last);
        }
      }

      void clear() {
        if (!is_long()) {
          _data._short.clear();
        } else {
          _data._long.clear();
        }
      }

      void pop_back() {
        if (!is_long()) {
          _data._short.pop_back();
        } else {
          _data._long.pop_back();
        }
      }

      size_t insert(size_t pos, View<view_of_type>&& sv) {
        if (!is_long()) {
          if (_data._short.number_of_views() < 2) {
            return _data._short.insert(pos, std::move(sv));
          } else {
            activate_long();
            return insert(pos, std::move(sv));
          }
        } else {
          return _data._long.insert(pos, std::move(sv));
        }
      }

      void erase(size_t first, size_t last) {
        if (!is_long()) {
          _data._short.erase(first, last);
        } else {
          _data._long.erase(first, last);
        }
      }

     private:
      void activate_long() {
        LIBSEMIGROUPS_ASSERT(!is_long());
        LIBSEMIGROUPS_ASSERT(_data._short.number_of_views() == 2);
        Long l;
        l.emplace_back(_data._short.begin(0), _data._short.end(0));
        l.emplace_back(_data._short.begin(1), _data._short.end(1));
        _data.activate_long();
        _data._long = l;
        _is_long    = true;
      }

      union ShortOrLong {
        ShortOrLong() : _short() {}
        ~ShortOrLong() {}

        void activate_long() {
          destroy_short();
          new (&_long) Long;
        }

        void activate_short() {
          destroy_long();
          new (&_short) Short;
        }

        void destroy_short() {
          _short.~Short();
        }

        void destroy_long() {
#pragma GCC diagnostic push
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
          _long.~Long();
#pragma GCC diagnostic pop
        }

        Short _short;
        Long  _long;
      };  // class ShortOrLong

      ////////////////////////////////////////////////////////////////////////
      // MultiView - data - private
      ////////////////////////////////////////////////////////////////////////
      ShortOrLong _data;
      uint8_t     _is_long;
    };  // class ViewContainer

    template <typename Thing = std::string>
    class MultiView {
     public:
      ////////////////////////////////////////////////////////////////////////
      // MultiView - type aliases - public
      ////////////////////////////////////////////////////////////////////////
      using view_of_type           = Thing;
      using const_iterator_view_of = typename view_of_type::const_iterator;

     private:
      ////////////////////////////////////////////////////////////////////////
      // MultiView - iterator traits - private
      ////////////////////////////////////////////////////////////////////////

      struct IteratorTraits : detail::ConstIteratorTraits<view_of_type> {
        using const_iterator_view_of = typename view_of_type::const_iterator;
        using value_type             = typename view_of_type::value_type;
        using const_reference        = value_type const&;
        using reference              = value_type&;
        using const_pointer          = value_type const*;
        using pointer                = value_type*;

        // state_type.second is the index in _container into which the iterator
        // currently points
        using state_type = std::pair<MultiView const*, size_t>;

        struct PrefixIncrement {
          void operator()(state_type& st, const_iterator_view_of& it) const {
            MultiView const* ptr        = st.first;
            size_t&          view_index = st.second;

            LIBSEMIGROUPS_ASSERT(ptr->_container.begin(view_index) <= it);
            LIBSEMIGROUPS_ASSERT(it < ptr->_container.end(view_index)
                                 || ptr->_container.empty(view_index));

            ++it;
            // move to next View if necessary
            if (it >= ptr->_container.end(view_index)
                && view_index < ptr->number_of_views() - 1) {
              ++view_index;
              it = ptr->_container.begin(view_index);
            }
          }
        };

        struct PrefixDecrement {
          void operator()(state_type& st, const_iterator_view_of& it) const {
            MultiView const* ptr        = st.first;
            size_t&          view_index = st.second;

            LIBSEMIGROUPS_ASSERT(ptr->_container.begin(view_index) <= it);
            LIBSEMIGROUPS_ASSERT(it <= ptr->_container.end(view_index)
                                 || ptr->_container.empty(view_index));

            --it;
            // move to next View if necessary
            if (it < ptr->_container.begin(view_index) && view_index > 0) {
              --view_index;
              it = ptr->_container.end(view_index) - 1;
            }
          }
        };

        struct EqualTo {
          bool operator()(state_type const&            st1,
                          const_iterator_view_of const it1,
                          state_type const&            st2,
                          const_iterator_view_of const it2) const {
            return st1.second == st2.second && it1 == it2;
          }
        };

        struct NotEqualTo {
          bool operator()(state_type const&            st1,
                          const_iterator_view_of const it1,
                          state_type const&            st2,
                          const_iterator_view_of const it2) const {
            return !EqualTo()(st1, it1, st2, it2);
          }
        };

        struct AddAssign {
          void operator()(state_type&             st,
                          const_iterator_view_of& it,
                          size_t                  n) {
            MultiView const* ptr = st.first;
            if (ptr->empty() || n == 0) {
              return;
            }
            size_t& view_index = st.second;
            it += n;
            while (it >= ptr->_container.end(view_index)
                   && view_index < ptr->_container.number_of_views() - 1) {
              n = (it - ptr->_container.end(view_index));
              ++view_index;
              it = ptr->_container.begin(view_index) + n;
            }
          }
        };

        // TODO(later) SubtractAssign

        struct Less {
          bool operator()(state_type const&            st1,
                          const_iterator_view_of const it1,
                          state_type const&            st2,
                          const_iterator_view_of const it2) {
            LIBSEMIGROUPS_ASSERT(st1.first == st2.first);
            return st1.second < st2.second
                   || (st1.second == st2.second && it1 < it2);
          }
        };

        struct LessOrEqualTo {
          bool operator()(state_type const&            st1,
                          const_iterator_view_of const it1,
                          state_type const&            st2,
                          const_iterator_view_of const it2) {
            LIBSEMIGROUPS_ASSERT(st1.first == st2.first);
            return Less()(st1, it1, st2, it2) || EqualTo()(st1, it1, st2, it2);
          }
        };

        struct More {
          bool operator()(state_type const&            st1,
                          const_iterator_view_of const it1,
                          state_type const&            st2,
                          const_iterator_view_of const it2) {
            LIBSEMIGROUPS_ASSERT(st1.first == st2.first);
            return !LessOrEqualTo()(st1, it1, st2, it2);
          }
        };

        struct Difference {
          using difference_type =
              typename const_iterator_view_of::difference_type;

          difference_type operator()(state_type const&            st1,
                                     const_iterator_view_of const it1,
                                     state_type const&            st2,
                                     const_iterator_view_of const it2) {
            LIBSEMIGROUPS_ASSERT(st1.first == st2.first);
            if (st1.second == st2.second) {
              return it1 - it2;
            } else if (st1.second < st2.second) {
              return -this->operator()(st2, it2, st1, it1);
            }

            difference_type result
                = (st1.first->_container.end(st2.second) - it2)
                  + (it1 - st1.first->_container.begin(st1.second));
            for (size_t i = st2.second + 1; i < st1.second; ++i) {
              result += st1.first->_container.size(i);
            }
            return result;
          }
        };
      };

      friend struct IteratorTraits;

     public:
      using const_iterator         = ConstIteratorStateful<IteratorTraits>;
      using const_reverse_iterator = std::reverse_iterator<const_iterator>;

      ////////////////////////////////////////////////////////////////////////
      // MultiView - constructors + destructor - public
      ////////////////////////////////////////////////////////////////////////

      MultiView()                                 = default;
      MultiView(MultiView const& that)            = default;
      MultiView(MultiView&& that)                 = default;
      MultiView& operator=(MultiView&& that)      = default;
      MultiView& operator=(MultiView const& that) = default;

      // construct from view_of_type::const_iterators
      MultiView(const_iterator_view_of first, const_iterator_view_of last)
          : MultiView() {
        if (first < last) {
          _container.emplace_back(first, last);
        }
      }

      // construct from MultiView::const_iterators
      MultiView(const_iterator first, const_iterator last)
          : MultiView(*first.get_state().first) {
        LIBSEMIGROUPS_ASSERT(first.get_state().first == last.get_state().first);
        MultiView const* p = first.get_state().first;
        erase(cbegin() + (last - p->cbegin()), cend());
        erase(cbegin(), cbegin() + (first - p->cbegin()));
      }

      explicit MultiView(view_of_type const& s)
          : MultiView(s.cbegin(), s.cend()) {}

      ////////////////////////////////////////////////////////////////////////
      // MultiView - attributes - public
      ////////////////////////////////////////////////////////////////////////

      size_t size() const {
        return _container.size();
      }

      size_t number_of_views() const noexcept {
        return _container.number_of_views();
      }

      bool empty() const noexcept {
        return _container.empty();
      }

      ////////////////////////////////////////////////////////////////////////
      // MultiView - modifiers - public
      ////////////////////////////////////////////////////////////////////////

      void clear() {
        _container.clear();
      }

      char pop_front() {
        if (empty()) {
          LIBSEMIGROUPS_EXCEPTION("cannot pop_front, MultiView is empty!");
        }
        char result = (*this)[0];
        erase(cbegin());
        LIBSEMIGROUPS_ASSERT(_container.empty() || !_container.empty(0));
        return result;
      }

      void append(const_iterator_view_of first, const_iterator_view_of last) {
        if (first < last) {
          if (!empty()
              && first == _container.end(_container.number_of_views() - 1)) {
            _container.size(_container.number_of_views() - 1) += last - first;
          } else {
            _container.emplace_back(first, last);
          }
        }
      }

      MultiView& append(const_iterator first, const_iterator last) {
        if (first < last) {
          LIBSEMIGROUPS_ASSERT(first.get_state().first
                               == last.get_state().first);
          MultiView const* ptr         = first.get_state().first;
          size_t           first_index = first.get_state().second;
          size_t const     last_index  = last.get_state().second;
          if (first_index == last_index) {
            // append so that we concatenate adjacent ranges!
            append(first.get_wrapped_iter(), last.get_wrapped_iter());
          } else {
            // append so that we concatenate adjacent ranges!
            append(first.get_wrapped_iter(), ptr->_container.end(first_index));
            for (size_t pos = first_index + 1; pos < last_index; ++pos) {
              LIBSEMIGROUPS_ASSERT(!ptr->_container.empty(pos));
              _container.emplace_back(ptr->_container.begin(pos),
                                      ptr->_container.end(pos));
            }
            _container.emplace_back(ptr->_container.begin(last_index),
                                    last.get_wrapped_iter());
          }
        }
        return *this;
      }

      void erase(const_iterator it) {
        return erase(it, it + 1);
      }

      void erase(const_iterator first, const_iterator last) {
        LIBSEMIGROUPS_ASSERT(first.get_state().first == last.get_state().first);
        LIBSEMIGROUPS_ASSERT(first.get_state().first == this);
        if (last <= first) {
          return;
        }

        auto view_first = view_iterator(first);
        auto view_last  = view_iterator(last);
        if (view_first == view_last) {
          if (first.get_wrapped_iter() == _container.begin(view_first)) {
            auto old_end_first = _container.end(view_first);
            _container.size(view_first)
                -= (last.get_wrapped_iter() - _container.begin(view_first));
            _container.begin(view_first) = last.get_wrapped_iter();
            if (last.get_wrapped_iter() == old_end_first
                && _container.is_long()) {
              // The reduction in the size and moving begin above effectively
              // pop_back when _container is Short.
              _container.pop_back();  // first, last invalidated
            }
            return;
          } else if (last.get_wrapped_iter() == _container.end(view_first)) {
            LIBSEMIGROUPS_ASSERT(
                view_first
                == first.get_state().first->_container.number_of_views() - 1);
            _container.size(view_first)
                -= (_container.end(view_first) - first.get_wrapped_iter());
            LIBSEMIGROUPS_ASSERT(!_container.empty(view_first));
            return;
          } else {
            // No iterator that points at anything not in _container.back()
            // ever points at view_last.end() !!
            LIBSEMIGROUPS_ASSERT(last.get_wrapped_iter()
                                 != _container.end(view_last));
            View<view_of_type> sv(last.get_wrapped_iter(),
                                  _container.end(view_first));
            _container.size(view_first)
                -= (_container.end(view_first) - first.get_wrapped_iter());
            LIBSEMIGROUPS_ASSERT(!_container.empty(view_first));
            LIBSEMIGROUPS_ASSERT(!sv.empty());
            _container.insert(view_first + 1, std::move(sv));
            return;
          }
        } else {
          LIBSEMIGROUPS_ASSERT(view_first < view_last);
          _container.size(view_first)
              -= (_container.end(view_first) - first.get_wrapped_iter());
          _container.size(view_last)
              -= (last.get_wrapped_iter() - _container.begin(view_last));
          _container.begin(view_last) = last.get_wrapped_iter();
          if (_container.empty(view_first)) {
            view_first--;  // FIXME(1) this underflows if view_first == 0
          }
          if (_container.empty(view_last)) {
            view_last++;
          }

          _container.erase(view_first + 1, view_last);
          return;
        }
      }

      ////////////////////////////////////////////////////////////////////////
      // MultiView - iterators - public
      ////////////////////////////////////////////////////////////////////////

      const_iterator cbegin() const {
        if (empty()) {
          return const_iterator(std::make_pair(this, 0),
                                null_const_reference().cbegin());
        }
        LIBSEMIGROUPS_ASSERT(!_container.empty(0));
        return const_iterator(std::make_pair(this, 0), _container.begin(0));
      }

      const_iterator cend() const {
        if (empty()) {
          return const_iterator(std::make_pair(this, 0),
                                null_const_reference().cbegin());
        }
        return const_iterator(
            std::make_pair(this, _container.number_of_views() - 1),
            _container.end(_container.number_of_views() - 1));
      }

      const_reverse_iterator crbegin() const {
        return const_reverse_iterator(cend());
      }

      const_reverse_iterator crend() const {
        return const_reverse_iterator(cbegin());
      }

      const_iterator begin() const {
        return cbegin();
      }

      const_iterator end() const {
        return cend();
      }

      ////////////////////////////////////////////////////////////////////////
      // MultiView - operators - public
      ////////////////////////////////////////////////////////////////////////

      operator view_of_type() const {
        view_of_type result = "";
        for (size_t i = 0; i < _container.number_of_views(); ++i) {
          result.append(_container.begin(i), _container.end(i));
        }
        return result;
      }

      bool operator==(MultiView const& other) const {
        if (size() != other.size()) {
          return false;
        }
        return std::equal(cbegin(), cend(), other.cbegin());
      }

      bool operator!=(MultiView const& other) const {
        return !(*this == other);
      }

      bool operator<(MultiView const& other) const {
        return std::lexicographical_compare(
            cbegin(), cend(), other.cbegin(), other.cend());
      }

      bool operator>(MultiView const& other) const {
        return other < *this;
      }

      bool operator<=(MultiView const& other) const {
        return *this < other || other == *this;
      }

      bool operator>=(MultiView const& other) const {
        return *this > other || other == *this;
      }

      void operator+=(MultiView const& other) {
        for (size_t i = 0; i < other.number_of_views(); ++i) {
          append(other._container.begin(i), other._container.end(i));
        }
      }

      MultiView operator+(MultiView const& other) const {
        MultiView result(*this);
        result += other;
        return result;
      }

      char operator[](size_t pos) const {
        LIBSEMIGROUPS_ASSERT(pos < size());
        size_t i = 0;
        for (; pos >= _container.size(i); ++i) {
          pos -= _container.size(i);
        }
        return *(_container.begin(i) + pos);
      }

     private:
      ////////////////////////////////////////////////////////////////////////
      // MultiView - helpers - private
      ////////////////////////////////////////////////////////////////////////

      // returns _container.size() if it does not point inside any view
      size_t view_iterator(const_iterator it) {
        return it.get_state().second;
      }

      static view_of_type const& null_const_reference() noexcept {
        static const view_of_type nll_strng;
        return nll_strng;
      }

      ViewContainer<view_of_type> _container;
    };  // class MultiView

    // template <>
    // MultiView(char const*) -> MultiView<std::string>;

    ////////////////////////////////////////////////////////////////////////
    // libsemigroups comparison operators
    ////////////////////////////////////////////////////////////////////////

    template <typename Thing>
    bool is_prefix(Thing const& word, MultiView<Thing> const& possible_prefix) {
      return is_prefix(word.cbegin(),
                       word.cend(),
                       possible_prefix.cbegin(),
                       possible_prefix.cend());
    }

    template <typename Thing>
    MultiView<Thing> maximum_common_suffix(MultiView<Thing> const& first,
                                           MultiView<Thing> const& second) {
      auto p = maximum_common_suffix(
          first.cbegin(), first.cend(), second.cbegin(), second.cend());
      return MultiView<Thing>(p.first, first.cend());
    }
  }  // namespace detail
}  // namespace libsemigroups

namespace std {

  ////////////////////////////////////////////////////////////////////////
  // std comparison operators
  ////////////////////////////////////////////////////////////////////////

  template <typename Thing>
  bool operator==(Thing const&                                   x,
                  libsemigroups::detail::MultiView<Thing> const& y) {
    return std::equal(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  template <typename Thing>
  bool operator==(libsemigroups::detail::MultiView<Thing> const& x,
                  Thing const&                                   y) {
    return y == x;
  }
}  // namespace std
#endif  // LIBSEMIGROUPS_DETAIL_MULTI_VIEW_HPP_

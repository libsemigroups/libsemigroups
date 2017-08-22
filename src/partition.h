//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 Michael Torpey
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

#ifndef LIBSEMIGROUPS_SRC_PARTITION_H_
#define LIBSEMIGROUPS_SRC_PARTITION_H_

#include <algorithm>
#include <vector>

#include "libsemigroups-debug.h"

namespace libsemigroups {
  //! Class for partitions of a set used by Congruence::nontrivial_classes.
  //!
  //! This class is for representing a partition of a set, i.e. an equivalence
  //! relation on that set.  The template parameter \p T is the type of objects,
  //! pointers to which are stored in the partition.
  template <typename T> class Partition {
    // TODO: do something else instead of using this object?

   public:
    //! A constructor.
    //!
    //! This constructor returns a partition with \p nr_parts empty parts.
    explicit Partition(size_t nr_parts = 0)
        : _parts(new std::vector<std::vector<T*>*>()) {
      for (size_t i = 0; i < nr_parts; i++) {
        _parts->push_back(new std::vector<T*>());
      }
    }

    //! A constructor.
    //!
    //! This constructor returns the Partition defined by the given vector,
    //! which is not copied, and is deleted by the destructor of this class.
    // FIXME delete if not used
    explicit Partition(std::vector<std::vector<T*>*>* parts) : _parts(parts) {}

    //! A default destructor.
    //!
    //! Deletes the vector and all of its contents passed to the constructor,
    //! if any.
    ~Partition() {
      for (std::vector<T*>* block : *_parts) {
        for (T* elm : *block) {
          delete elm;
        }
        delete block;
      }
      delete _parts;
    }

    //! The assignment operator is deleted for Partition to avoid unintended
    //! copying.
    Partition& operator=(Partition const& part) = delete;

    //! The copy constructor is deleted for Partition to avoid unintended
    //! copying.
    Partition(Partition const& copy) = delete;

    //! Returns the number of parts in the partition.
    // FIXME rename this to nr_parts
    inline size_t size() const {
      return _parts->size();
    }

    //! Returns the part with index \p part_index.
    //!
    //! This method asserts that the parameter \p part_index is less than the
    //! number of parts.
    inline std::vector<T*>* operator[](size_t part_index) const {
      LIBSEMIGROUPS_ASSERT(part_index < size());
      return (*_parts)[part_index];
    }

    //! Returns the part with index \p part_index.
    //!
    //! This uses the \c at method of the underlying std::vector, and so
    //! performs out-of-bounds checks.
    inline std::vector<T*>* at(size_t part_index) const {
      return _parts->at(part_index);
    }

    //! Returns the element with index \p elm_nr in part \p part_index.
    T* at(size_t part_index, size_t elm_nr) const {
      return _parts->at(part_index)->at(elm_nr);
    }

   private:
    std::vector<std::vector<T*>*>* _parts;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_PARTITION_H_

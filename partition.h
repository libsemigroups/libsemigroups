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

#ifndef LIBSEMIGROUPS_PARTITION_H_
#define LIBSEMIGROUPS_PARTITION_H_

#include <assert.h>

#include <algorithm>
#include <vector>

namespace libsemigroups {

  // Non-abstract
  // @T type of the objects contained in the parts of the partition (via
  // pointers)
  //
  // This class is for representing a partition of a set, i.e. an equivalence
  // relation on that set.  The template parameter <T> is the type of objects,
  // pointers to which are stored in the partition.

  template <typename T> class Partition {
    // TODO: do something else instead of using this object?

   public:
    // 1 parameters (size_t)
    // @nr_parts a size_t (defaults to 0)
    //
    // This constructor returns a partition with <nr_parts> empty parts.
    explicit Partition(size_t nr_parts = 0)
        : _parts(new std::vector<std::vector<T*>*>()) {
      for (size_t i = 0; i < nr_parts; i++) {
        _parts->push_back(new std::vector<T*>());
      }
    }

    // 1 parameter (pointer)
    // @parts pointer to a vector describing the parts of the partition
    //
    // This constructor returns the partition defined by the given vector, by
    // storing all the pointers it contains.  The pointers will all be deleted
    // by the destructor when this Partition object is deleted.
    //
    // FIXME delete if not used
    explicit Partition(std::vector<std::vector<T*>*>* parts) : _parts(parts) {}

    //
    // A default destructor.
    ~Partition() {
      for (std::vector<T*>* block : *_parts) {
        for (T* elm : *block) {
          delete elm;
        }
        delete block;
      }
      delete _parts;
    }

    // deleted
    // @part const reference to a partition that cannot be assigned.
    //
    // The Partition class does not support an assignment contructor to avoid
    // accidental copying. A copy constructor is provided in case such a copy
    // should be required anyway.
    //
    // @return nothing it is deleted.
    Partition& operator=(Partition const& part) = delete;

    // const
    //
    // @return the number of parts in the partition.
    inline size_t size() const {
      return _parts->size();
    }

    // const
    // @part_index an integer
    //
    // @return the vector containing the elements of the given part of the
    // partition.
    inline std::vector<T*>* operator[](size_t part_index) const {
      assert(part_index < size());
      return (*_parts)[part_index];
    }

    // const
    // @part_index index of a part in the partition
    //
    // @return the vector containing the elements of the given part.
    inline std::vector<T*>* at(size_t part_index) const {
      return _parts->at(part_index);
    }

    // const
    // @part_index index of a part in the partition
    // @elm_nr index of an object in the specified part
    //
    // @return the element in the given position in the given part.
    T* at(size_t part_index, size_t elm_nr) const {
      return _parts->at(part_index)->at(elm_nr);
    }

   private:
    std::vector<std::vector<T*>*>* _parts;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_PARTITION_H_

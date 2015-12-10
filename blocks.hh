/*******************************************************************************
 * Semigroups++
 *
 * This file contains everything for blocks of a bipartition.
 *
 *******************************************************************************/


#ifndef SRC_BLOCKS_H_
#define SRC_BLOCKS_H_

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Header includes . . .
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <functional>
#include <vector>
#include <algorithm>

// Blocks are stored internally as a list consisting of:
//
// [ nr of blocks, internal rep of blocks, transverse blocks ]
//
// <nr of blocks> is a non-negative integer, <internal rep of blocks>[i] = j if
// <i> belongs to the <j>th block, <transverse blocks>[j] = true if block <j> is
// transverse and false if it is not.

class Blocks {

 public:
    Blocks () :
      _blocks(nullptr),
      _lookup(nullptr),
      _nr_blocks(0),
      _rank(0)
    { }

    Blocks (std::vector<u_int32_t>* blocks,
            std::vector<bool>*      lookup) :
      _blocks(blocks),
      _lookup(lookup),
      _nr_blocks(),
      _rank(UNDEFINED)
    {
      _nr_blocks = *(std::max_element(_blocks->begin(),
                                      _blocks->end())) + 1;
      assert(blocks->size() != 0);
      assert(_nr_blocks == _lookup->size());
    }

    Blocks (std::vector<u_int32_t>* blocks,
            std::vector<bool>*      lookup,
            u_int32_t               nr_blocks) :
      _blocks(blocks),
      _lookup(lookup),
      _nr_blocks(nr_blocks),
      _rank(UNDEFINED)
    {
      assert(blocks->size() != 0);
      assert(_nr_blocks == _lookup->size());
    }

    Blocks& operator= (Blocks const&) = delete;
    Blocks (Blocks const&) = delete;

    ~Blocks () {
      delete _blocks;
      delete _lookup;
    }

    bool operator == (const Blocks& that) const {
      if (this->_nr_blocks != that._nr_blocks) {
        return false;
      }
      return (*(this->_blocks) == *(that._blocks) &&
              *(this->_lookup) == *(that._lookup));
    }

    bool operator< (const Blocks& that) const {
      if (this->degree() != that.degree()) {
        return (this->degree() < that.degree());
      }
      for (size_t i = 0; i < this->degree(); i++) {
        if (((*this->_blocks)[i] != (*that._blocks)[i])) {
          return (*this->_blocks)[i] < (*that._blocks)[i];
        }
      }
      for (size_t i = 0; i < this->nr_blocks(); i++) {
        if (((*this->_lookup)[i] && !(*that._lookup)[i])) {
          return true;
        } else if ((!(*this->_lookup)[i] && (*that._lookup)[i])) {
          return false;
        }
      }
      return false;
    }

    inline u_int32_t degree () const {
      if (_nr_blocks == 0) {
        return 0;
      }
      return _blocks->size();
    }

    inline u_int32_t block (size_t pos) const {
      assert(pos < _blocks->size());
      return (*_blocks)[pos];
    }

    inline bool is_transverse_block (size_t index) const {
      assert(index < _lookup->size());
      return (*_lookup)[index];
    }

    inline std::vector<bool>* lookup () const {
      return _lookup;
    }

    inline u_int32_t nr_blocks () const {
      return _nr_blocks;
    }

    inline u_int32_t rank () {
      if (_rank == UNDEFINED) {
        _rank = 0;
        for (auto val: *_lookup) {
          if (val) _rank++;
        }
      }
      return _rank;
    }

    inline size_t hash_value () const {
      if (_nr_blocks == 0) {
        return 0;
      }
      size_t seed = 0;
      for (auto index: *_blocks) {
        seed = ((seed * _blocks->size()) + index);
      }
      for (auto val: *_lookup) {
        seed = ((seed * _blocks->size()) + val);
      }
      return seed;
    }

    inline typename std::vector<u_int32_t>::iterator begin () const {
      return _blocks->begin();
    }

    inline typename std::vector<u_int32_t>::iterator end () const {
      return _blocks->end();
    }

 private:

    std::vector<u_int32_t>* _blocks;
    std::vector<bool>*      _lookup;
    u_int32_t               _nr_blocks;
    u_int32_t               _rank;
    static u_int32_t        UNDEFINED;

};

#endif

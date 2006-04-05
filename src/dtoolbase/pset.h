// Filename: pset.h
// Created by:  drose (05Jun01)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) 2001 - 2004, Disney Enterprises, Inc.  All rights reserved
//
// All use of this software is subject to the terms of the Panda 3d
// Software license.  You should have received a copy of this license
// along with this source code; you will also find a current copy of
// the license at http://etc.cmu.edu/panda3d/docs/license/ .
//
// To contact the maintainers of this program write to
// panda3d-general@lists.sourceforge.net .
//
////////////////////////////////////////////////////////////////////

#ifndef PSET_H
#define PSET_H

#include "dtoolbase.h"
#include "pallocator.h"
#include "stl_compares.h"

#include <set>
#ifdef HAVE_STL_HASH
#include <hash_set>
#endif

#ifndef USE_STL_ALLOCATOR
// If we're not using custom allocators, just use the standard class
// definition.
#define pset set
#define pmultiset multiset

#ifdef HAVE_STL_HASH
#define phash_set hash_set
#define phash_multiset hash_multiset
#else  // HAVE_STL_HASH
#define phash_set set
#define phash_multiset multiset
#endif  // HAVE_STL_HASH

#else  // USE_STL_ALLOCATOR

////////////////////////////////////////////////////////////////////
//       Class : pset
// Description : This is our own Panda specialization on the default
//               STL set.  Its main purpose is to call the hooks
//               for MemoryUsage to properly track STL-allocated
//               memory.
////////////////////////////////////////////////////////////////////
template<class Key, class Compare = less<Key> >
class pset : public set<Key, Compare, pallocator_single<Key> > {
public:
  typedef set<Key, Compare, pallocator_single<Key> > base_class;
  pset() : base_class() { }
  pset(const pset<Key, Compare> &copy) : base_class(copy) { }
  pset(const Compare &comp) : base_class(comp) { }

#ifdef USE_TAU
  std::pair<TYPENAME base_class::iterator, bool>
  insert(const TYPENAME base_class::value_type &x) {
    TAU_PROFILE("pset::insert(const value_type &)", " ", TAU_USER);
    return base_class::insert(x);
  }

  TYPENAME base_class::iterator
  insert(TYPENAME base_class::iterator position, 
         const TYPENAME base_class::value_type &x) {
    TAU_PROFILE("pset::insert(iterator, const value_type &)", " ", TAU_USER);
    return base_class::insert(position, x);
  }

  void
  erase(TYPENAME base_class::iterator position) {
    TAU_PROFILE("pset::erase(iterator)", " ", TAU_USER);
    base_class::erase(position);
  }

  TYPENAME base_class::size_type
  erase(const TYPENAME base_class::key_type &x) {
    TAU_PROFILE("pset::erase(const key_type &)", " ", TAU_USER);
    return base_class::erase(x);
  }
  
  void
  clear() {
    TAU_PROFILE("pset::clear()", " ", TAU_USER);
    base_class::clear();
  }

  TYPENAME base_class::iterator
  find(const TYPENAME base_class::key_type &x) {
    TAU_PROFILE("pset::find(x)", " ", TAU_USER);
    return base_class::find(x);
  }

  TYPENAME base_class::const_iterator
  find(const TYPENAME base_class::key_type &x) const {
    TAU_PROFILE("pset::find(x)", " ", TAU_USER);
    return base_class::find(x);
  }
#endif  // USE_TAU
};

////////////////////////////////////////////////////////////////////
//       Class : pmultiset
// Description : This is our own Panda specialization on the default
//               STL multiset.  Its main purpose is to call the hooks
//               for MemoryUsage to properly track STL-allocated
//               memory.
////////////////////////////////////////////////////////////////////
template<class Key, class Compare = less<Key> >
class pmultiset : public multiset<Key, Compare, pallocator_single<Key> > {
public:
  pmultiset() : multiset<Key, Compare, pallocator_single<Key> >() { }
  pmultiset(const pmultiset<Key, Compare> &copy) : multiset<Key, Compare, pallocator_single<Key> >(copy) { }
  pmultiset(const Compare &comp) : multiset<Key, Compare, pallocator_single<Key> >(comp) { }
};

#ifdef HAVE_STL_HASH
////////////////////////////////////////////////////////////////////
//       Class : phash_set
// Description : This is our own Panda specialization on the default
//               STL hash_set.  Its main purpose is to call the hooks
//               for MemoryUsage to properly track STL-allocated
//               memory.
////////////////////////////////////////////////////////////////////
template<class Key, class Compare = method_hash<Key, less<Key> > >
class phash_set : public hash_set<Key, Compare, pallocator_single<Key> > {
public:
  phash_set() : hash_set<Key, Compare, pallocator_single<Key> >() { }
  phash_set(const phash_set<Key, Compare> &copy) : hash_set<Key, Compare, pallocator_single<Key> >(copy) { }
  phash_set(const Compare &comp) : hash_set<Key, Compare, pallocator_single<Key> >(comp) { }
};

////////////////////////////////////////////////////////////////////
//       Class : phash_multiset
// Description : This is our own Panda specialization on the default
//               STL hash_multiset.  Its main purpose is to call the hooks
//               for MemoryUsage to properly track STL-allocated
//               memory.
////////////////////////////////////////////////////////////////////
template<class Key, class Compare = method_hash<Key, less<Key> > >
class phash_multiset : public hash_multiset<Key, Compare, pallocator_single<Key> > {
public:
  phash_multiset() : hash_multiset<Key, Compare, pallocator_single<Key> >() { }
  phash_multiset(const phash_multiset<Key, Compare> &copy) : hash_multiset<Key, Compare, pallocator_single<Key> >(copy) { }
  phash_multiset(const Compare &comp) : hash_multiset<Key, Compare, pallocator_single<Key> >(comp) { }
};

#else // HAVE_STL_HASH
#define phash_set pset
#define phash_multiset pmultiset
#endif  // HAVE_STL_HASH

#endif  // USE_STL_ALLOCATOR
#endif

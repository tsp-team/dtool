// Filename: memoryHook.h
// Created by:  drose (28Jun07)
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

#ifndef MEMORYHOOK_H
#define MEMORYHOOK_H

#include "dtoolbase.h"
#include "numeric_types.h"
#include "atomicAdjust.h"
#include "mutexImpl.h"
#include <map>

class DeletedBufferChain;

////////////////////////////////////////////////////////////////////
//       Class : MemoryHook
// Description : This class provides a wrapper around the various
//               possible malloc schemes Panda might employ.  It also
//               exists to allow the MemoryUsage class in Panda to
//               insert callback hooks to track the size of allocated
//               pointers.
//
//               The PANDA_MALLOC_* and PANDA_FREE_* macros are
//               defined to vector through through this class (except
//               in production builds) to facilitate that.  Every
//               memory allocation call in Panda should therefore use
//               these macros instead of direct calls to malloc or
//               free.  (C++ new and delete operators may be employed
//               for classes which inherit from MemoryBase; otherwise,
//               use the PANDA_MALLOC macros.)
////////////////////////////////////////////////////////////////////
class EXPCL_DTOOL MemoryHook {
public:
  MemoryHook();
  MemoryHook(const MemoryHook &copy);
  virtual ~MemoryHook();

  virtual void *heap_alloc_single(size_t size);
  virtual void heap_free_single(void *ptr);

  virtual void *heap_alloc_array(size_t size);
  virtual void *heap_realloc_array(void *ptr, size_t size);
  virtual void heap_free_array(void *ptr);

  INLINE void inc_heap(size_t size);
  INLINE void dec_heap(size_t size);

  bool heap_trim(size_t pad);

  virtual void *mmap_alloc(size_t size, bool allow_exec);
  virtual void mmap_free(void *ptr, size_t size);
  INLINE size_t get_page_size() const;
  INLINE size_t round_up_to_page_size(size_t size) const;

  virtual void mark_pointer(void *ptr, size_t orig_size, ReferenceCount *ref_ptr);

  DeletedBufferChain *get_deleted_chain(size_t buffer_size);

private:
  INLINE static size_t inflate_size(size_t size);
  INLINE static void *alloc_to_ptr(void *alloc, size_t size);
  INLINE static void *ptr_to_alloc(void *ptr, size_t &size);

#ifdef DO_MEMORY_USAGE
protected:
  TVOLATILE PN_int32 _total_heap_single_size;
  TVOLATILE PN_int32 _total_heap_array_size;
  TVOLATILE PN_int32 _requested_heap_size;
  TVOLATILE PN_int32 _total_mmap_size;
#endif

private:
  size_t _page_size;

  typedef map<size_t, DeletedBufferChain *> DeletedChains;
  DeletedChains _deleted_chains;

  MutexImpl _lock;
};

#include "memoryHook.I"

#endif
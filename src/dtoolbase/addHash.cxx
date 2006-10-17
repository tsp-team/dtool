// Filename: addHash.cxx
// Created by:  drose (01Sep06)
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

#include "addHash.h"

////////////////////////////////////////////////////////////////////
//     Function: AddHash::add_hash
//       Access: Public, Static
//  Description: Adds a linear sequence of bytes to the hash.
////////////////////////////////////////////////////////////////////
size_t AddHash::
add_hash(size_t start, const PN_uint8 *bytes, size_t num_bytes) {
  size_t num_words = num_bytes >> 2;
  size_t remaining_bytes = num_bytes - (num_words << 2);
  size_t hash = (size_t)hashword((const PN_uint32 *)bytes, num_words, (PN_uint32)start);

  switch (remaining_bytes) {
  case 3:
    {
      PN_uint32 remaining;
      remaining = (bytes[num_bytes - 3] << 16) | (bytes[num_bytes - 2] << 8) | (bytes[num_bytes - 1]);
      hash = (size_t)hashword(&remaining, 1, (PN_uint32)hash);
    }
    break;

  case 2:
    {
      PN_uint32 remaining;
      remaining = (bytes[num_bytes - 2] << 8) | (bytes[num_bytes - 1]);
      hash = (size_t)hashword(&remaining, 1, (PN_uint32)hash);
    }
    break;

  case 1:
    {
      PN_uint32 remaining;
      remaining = (bytes[num_bytes - 1]);
      hash = (size_t)hashword(&remaining, 1, (PN_uint32)hash);
    }
    break;

  default:
    break;
  }
  return hash;
}
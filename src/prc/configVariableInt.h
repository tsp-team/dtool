// Filename: configVariableInt.h
// Created by:  drose (20Oct04)
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

#ifndef CONFIGVARIABLEINT_H
#define CONFIGVARIABLEINT_H

#include "dtoolbase.h"
#include "configVariable.h"

////////////////////////////////////////////////////////////////////
//       Class : ConfigVariableInt
// Description : This is a convenience class to specialize
//               ConfigVariable as an integer type.
////////////////////////////////////////////////////////////////////
class EXPCL_DTOOLCONFIG ConfigVariableInt : public ConfigVariable {
PUBLISHED:
  INLINE ConfigVariableInt(const string &name);
  INLINE ConfigVariableInt(const string &name, int default_value,
                           const string &description = string(), 
                           int flags = 0);
  INLINE ConfigVariableInt(const string &name, const string &default_value,
                           const string &description = string(), 
                           int flags = 0);

  INLINE void operator = (int value);
  INLINE operator int () const;

  INLINE int size() const;
  INLINE int operator [] (int n) const;

  INLINE void set_value(int value);
  INLINE int get_value() const;
  INLINE int get_default_value() const;

  INLINE int get_word(int n) const;
  INLINE void set_word(int n, int value);

private:
  void set_default_value(int default_value);

private:
  PN_int32 _local_modified;
  int _cache;
};

#include "configVariableInt.I"

#endif

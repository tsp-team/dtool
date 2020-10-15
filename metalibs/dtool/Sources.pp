// DIR_TYPE "metalib" indicates we are building a shared library that
// consists mostly of references to other shared libraries.  Under
// Windows, this directly produces a DLL (as opposed to the regular
// src libraries, which don't produce anything but a pile of OBJ files
// under Windows).

#define DIR_TYPE metalib

#begin metalib_target
  #define TARGET dtool

  #define BUILDING_DLL BUILDING_DTOOL
  #define COMPONENT_LIBS dtoolutil dtoolbase

  #define SOURCES dtool.cxx
#end metalib_target

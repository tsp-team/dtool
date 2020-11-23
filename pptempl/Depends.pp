//
// Depends.pp
//
// This file is loaded and run after each Sources.pp is read.  It
// defines the inter-directory dependencies, which is useful for
// determining build order.
//

#if $[eq $[DIR_TYPE], toplevel]
  #define DEPENDABLE_HEADERS $[CONFIG_HEADER]

#elif $[or $[eq $[DIR_TYPE], src],$[eq $[DIR_TYPE], metalib], $[eq $[DIR_TYPE], module]]
#if $[eq $[DEPEND_DIRS],]

  // DEPENDABLE_HEADERS is supposed to be the primary purpose of this
  // file: here we generate the list of source files that might be
  // included in some other source file, and hence is relevant to the
  // automatically-generated dependency chain.

  // We generate this variable by walking through all of the targets
  // and building it up.
  #define DEPENDABLE_HEADERS

  // We will also need to sort out the types files we will actually
  // compile within each directory, as well as the lists of files that
  // go into each composite.

  // We define $[c_sources], $[cxx_sources], $[yxx_sources], and
  // $[lxx_sources] within each target, which lists original files as
  // well as synthetic composite files.  We also now add
  // $[cxx_interrogate_sources], which are the C++ files generated by
  // interrogate.  We want to compile and link these files into a
  // Python module, separate from the C++ code.

  // Finally, there's $[compile_sources], which is the union of all
  // the others: any source files that are actually compiled and
  // result in a generated .o (or .obj) file.

  // Finally, we build up $[composite_list] out here to list all of
  // the composite files generated for all targets.

  // This is done at this point, within Depends.pp, so that the
  // various Template.*.pp files will be able to reliably access
  // $[compile_sources] from the different directories.
  #define composite_list
  // Tag all the static libraries by defining the "lib_is_static" variable.
  #forscopes static_lib_target
    #define lib_is_static 1
  #end static_lib_target
  #if $[WINDOWS_PLATFORM]
    #forscopes ss_lib_target
      #define lib_is_static 1
    #end ss_lib_target
  #endif
  #if $[LINK_ALL_STATIC]
    #forscopes python_target python_module_target metalib_target ss_lib_target lib_target
      #define lib_is_static 1
    #end python_target python_module_target metalib_target ss_lib_target lib_target
  #endif

  // Set up some needed stuff for Python modules.

  #forscopes python_module_target
    #define BUILD_TARGET $[and $[HAVE_INTERROGATE], $[INTERROGATE_PYTHON_INTERFACE]]
    #define OTHER_LIBS $[OTHER_LIBS] interrogatedb
  #end python_module_target

  #forscopes python_target
    #define BUILD_TARGET $[HAVE_PYTHON]
  #end python_target

  #forscopes python_target python_module_target
    #define USE_PACKAGES python
    #define LIB_PREFIX
    #define DYNAMIC_LIB_EXT $[PYTHON_MODULE_EXT]
    
    // Link Python modules as bundles on OSX.
    #if $[OSX_PLATFORM]
      #define LINK_AS_BUNDLE 1
      #define BUNDLE_EXT $[PYTHON_MODULE_EXT]
    #endif
    
    #define install_lib_dir $[install_py_module_dir]
    #define OUTPUT $[TARGET:panda3d.%=%]
    #define dllext
  #end python_target python_module_target

  #forscopes interface_target
    #define is_interface 1
  #end interface_target

  #forscopes interface_target python_target python_module_target metalib_target lib_target noinst_lib_target test_lib_target static_lib_target dynamic_lib_target ss_lib_target bin_target noinst_bin_target test_bin_target

    ///////////////////////////////////////////////////////////////////////
    // We can optimize quite a bit by evaluating now several of the key
    // deferred variables defined in Globals.pp.  This way they won't need
    // to get repeatedly reevaluated as each directory examines each
    // other.

    #define build_directory $[build_directory]
    #define build_target $[build_target]
    #define active_local_libs $[active_local_libs]
    #define active_component_libs $[active_component_libs]
    #define active_igate_libs $[active_igate_libs]
    #define active_libs $[active_libs]
    #define get_sources $[get_sources]

    // Non-empty for component libs that need to be interrogated
    #define get_igatescan $[get_igatescan]
    #define get_igateext $[get_igateext]
    #define get_igateoutput $[get_igateoutput]
    #define get_igatecode $[get_igatecode]
    #define get_igatedb $[get_igatedb]

    // Non-empty if this is a Python module with interrogated
    // components (defined using IGATE_LIBS).
    #define get_igatemscan $[get_igatemscan]
    #define get_igatemout $[get_igatemout]
    #define get_igatemcode $[get_igatemcode]

    #define get_output_name $[get_output_name]
    #define get_output_file $[get_output_file]
    #define get_output_bundle_file $[get_output_bundle_file]

    ///////////////////////////////////////////////////////////////////////

    // Report a warning for nonexisting dependencies.
    #define nonexisting $[unmapped all_libs,$[LOCAL_LIBS]]
    #if $[ne $[nonexisting],]
      #print Warning: Lib(s) $[nonexisting], referenced in $[DIRNAME]/$[TARGET], not found.
    #endif

    #set DEPENDABLE_HEADERS $[DEPENDABLE_HEADERS] $[filter %.hpp %.h %.I %.T %_src.cxx,$[get_sources]]

    // Now compute the source files.
    #define c_sources $[filter-out %_src.c,$[filter %.c,$[get_sources]]]
    #define mm_sources $[filter %.mm,$[get_sources]]
    #define cxx_sources $[filter-out %_src.cxx %_ext.cxx,$[filter %.cxx %.cpp,$[get_sources]]]

    #define yxx_sources $[filter %.yxx,$[get_sources]]
    #define lxx_sources $[filter %.lxx,$[get_sources]]

    // Define what the object files are.
    #foreach file $[c_sources] $[mm_sources] $[cxx_sources] $[yxx_sources] $[lxx_sources]
      #define $[file]_obj $[patsubst %.c %.cxx %.mm %.cpp %.yxx %.lxx,$[ODIR]/$[obj_prefix]%$[OBJ],$[notdir $[file]]]
      #push 1 $[file]_obj
    #end file

    #if $[should_composite_sources]
      // Put the C++ files specified in the $[COMPOSITE_SOURCES] variable
      // into a single C++ file that gets compiled.
      #define composite_file $[ODIR]/$[get_output_name]_composite.cxx
      #set composite_list $[composite_list] $[composite_file]
      #define $[composite_file]_sources $[composite_sources]
      #define $[composite_file]_obj $[ODIR]/$[get_output_name]_composite$[OBJ]
      #push 2 $[composite_file]_sources
      #push 2 $[composite_file]_obj
      #set cxx_sources $[cxx_sources] $[composite_file]
      #set get_sources $[get_sources] $[composite_file]

      // Now remove the composited source files from the list
      #set cxx_sources $[filter-out $[composite_sources], $[cxx_sources]]
      #set get_sources $[filter-out $[composite_sources], $[get_sources]]
    #endif

    // Add the bison- and flex-generated .cxx files, as well as the
    // interrogate-generated files, to the compile list, too.  These
    // never get added to composite files, though, mainly because they
    // tend to be very large files themselves.
    #foreach source_file $[yxx_sources] $[lxx_sources]
      #define generated_file $[patsubst %.yxx %.lxx,%.cxx,$[source_file]]
      #define $[generated_file]_obj $[patsubst %.yxx %.lxx,$[ODIR]/$[get_output_name]_%$[OBJ],$[source_file]]
      #define $[generated_file]_sources $[source_file]
      #push 1 $[generated_file]_obj
      #set cxx_sources $[cxx_sources] $[generated_file]
      #set get_sources $[get_sources] $[generated_file]
    #end source_file

    // If this is a Python module, then it should have a list of
    // interrogate-generated .cxx files and user-created extensions for
    // all interrogated components that we are linking into the module.
    // Add all those, as well as the module code, to the cxx_sources so
    // they are compiled and linked into our module.
    #if $[and $[build_directory],$[or $[get_igatemout],$[get_igatemcode]]]
      #define py_module_sources $[get_igatemout] $[get_igatemcode]
      #foreach igate_code $[py_module_sources]
        #define generated_file $[igate_code]
        #define $[generated_file]_obj $[patsubst %.cxx,$[ODIR]/%$[OBJ], $[notdir $[generated_file]]]
        #push 2 $[generated_file]_obj
        #set cxx_sources $[cxx_sources] $[generated_file]
        #set get_sources $[get_sources] $[generated_file]
      #end igate_code
    #endif

    #define compile_sources $[c_sources] $[mm_sources] $[cxx_sources]

  #end interface_target python_target python_module_target metalib_target lib_target noinst_lib_target test_lib_target static_lib_target dynamic_lib_target ss_lib_target bin_target noinst_bin_target test_bin_target

  // Allow the user to define additional EXTRA_DEPENDS targets in each
  // Sources.pp.
  #define DEPEND_DIRS \
    $[sort $[EXTRA_DEPENDS] $[all_libs $[DIRNAME],$[get_depend_libs]]]
  #set DEPENDABLE_HEADERS $[sort $[DEPENDABLE_HEADERS] $[EXTRA_DEPENDABLE_HEADERS]]
#endif

#endif // DIR_TYPE

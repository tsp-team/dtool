// Filename: interfaceMakerPythonSimple.cxx
// Created by:  drose (01Oct01)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) 2001, Disney Enterprises, Inc.  All rights reserved
//
// All use of this software is subject to the terms of the Panda 3d
// Software license.  You should have received a copy of this license
// along with this source code; you will also find a current copy of
// the license at http://www.panda3d.org/license.txt .
//
// To contact the maintainers of this program write to
// panda3d@yahoogroups.com .
//
////////////////////////////////////////////////////////////////////

#include "interfaceMakerPythonSimple.h"
#include "interrogateBuilder.h"
#include "interrogate.h"
#include "functionRemap.h"
#include "parameterRemapUnchanged.h"
#include "typeManager.h"

#include "interrogateDatabase.h"
#include "interrogateType.h"
#include "interrogateFunction.h"
#include "cppFunctionType.h"

////////////////////////////////////////////////////////////////////
//     Function: InterfaceMakerPythonSimple::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
InterfaceMakerPythonSimple::
InterfaceMakerPythonSimple(InterrogateModuleDef *def) :
  InterfaceMakerPython(def)
{
}

////////////////////////////////////////////////////////////////////
//     Function: InterfaceMakerPythonSimple::Destructor
//       Access: Public, Virtual
//  Description:
////////////////////////////////////////////////////////////////////
InterfaceMakerPythonSimple::
~InterfaceMakerPythonSimple() {
}

////////////////////////////////////////////////////////////////////
//     Function: InterfaceMakerPythonSimple::write_prototypes
//       Access: Public, Virtual
//  Description: Generates the list of function prototypes
//               corresponding to the functions that will be output in
//               write_functions().
////////////////////////////////////////////////////////////////////
void InterfaceMakerPythonSimple::
write_prototypes(ostream &out) {
  Functions::iterator fi;
  for (fi = _functions.begin(); fi != _functions.end(); ++fi) {
    Function *func = (*fi);
    write_prototype_for(out, func);
  }

  out << "\n";
  InterfaceMakerPython::write_prototypes(out);
}

////////////////////////////////////////////////////////////////////
//     Function: InterfaceMakerPythonSimple::write_functions
//       Access: Public, Virtual
//  Description: Generates the list of functions that are appropriate
//               for this interface.  This function is called *before*
//               write_prototypes(), above.
////////////////////////////////////////////////////////////////////
void InterfaceMakerPythonSimple::
write_functions(ostream &out) {
  Functions::iterator fi;
  for (fi = _functions.begin(); fi != _functions.end(); ++fi) {
    Function *func = (*fi);
    write_function_for(out, func);
  }

  InterfaceMakerPython::write_functions(out);
}

////////////////////////////////////////////////////////////////////
//     Function: InterfaceMakerPythonSimple::write_module
//       Access: Public, Virtual
//  Description: Generates whatever additional code is required to
//               support a module file.
////////////////////////////////////////////////////////////////////
void InterfaceMakerPythonSimple::
write_module(ostream &out, InterrogateModuleDef *def) {
  InterfaceMakerPython::write_module(out, def);

  out << "static PyMethodDef python_simple_funcs[] = {\n";

  Functions::iterator fi;
  for (fi = _functions.begin(); fi != _functions.end(); ++fi) {
    Function *func = (*fi);
    Function::Remaps::const_iterator ri;
    for (ri = func->_remaps.begin(); ri != func->_remaps.end(); ++ri) {
      FunctionRemap *remap = (*ri);
      out << "  { \"" << remap->_unique_name << "\", &" 
          << remap->_wrapper_name << ", METH_VARARGS },\n";
    }
  }  
  out << "  { NULL, NULL }\n"
      << "};\n\n"

      << "#ifdef _WIN32\n"
      << "extern \"C\" __declspec(dllexport) void init" << def->library_name << "();\n"
      << "#else\n"
      << "extern \"C\" void init" << def->library_name << "();\n"
      << "#endif\n\n"
    
      << "void init" << def->library_name << "() {\n"
      << "  Py_InitModule(\"" << def->library_name
      << "\", python_simple_funcs);\n"
      << "}\n\n";
}

////////////////////////////////////////////////////////////////////
//     Function: InterfaceMakerPythonSimple::synthesize_this_parameter
//       Access: Public, Virtual
//  Description: This method should be overridden and redefined to
//               return true for interfaces that require the implicit
//               "this" parameter, if present, to be passed as the
//               first parameter to any wrapper functions.
////////////////////////////////////////////////////////////////////
bool InterfaceMakerPythonSimple::
synthesize_this_parameter() {
  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: InterfaceMakerPythonSimple::get_wrapper_prefix
//       Access: Protected, Virtual
//  Description: Returns the prefix string used to generate wrapper
//               function names.
////////////////////////////////////////////////////////////////////
string InterfaceMakerPythonSimple::
get_wrapper_prefix() {
  return "_inP";
}

////////////////////////////////////////////////////////////////////
//     Function: InterfaceMakerPythonSimple::get_unique_prefix
//       Access: Protected, Virtual
//  Description: Returns the prefix string used to generate unique
//               symbolic names, which are not necessarily C-callable
//               function names.
////////////////////////////////////////////////////////////////////
string InterfaceMakerPythonSimple::
get_unique_prefix() {
  return "p";
}

////////////////////////////////////////////////////////////////////
//     Function: InterfaceMakerPythonSimple::record_function_wrapper
//       Access: Protected, Virtual
//  Description: Associates the function wrapper with its function in
//               the appropriate structures in the database.
////////////////////////////////////////////////////////////////////
void InterfaceMakerPythonSimple::
record_function_wrapper(InterrogateFunction &ifunc, 
                        FunctionWrapperIndex wrapper_index) {
  ifunc._python_wrappers.push_back(wrapper_index);
}

////////////////////////////////////////////////////////////////////
//     Function: InterfaceMakerPythonSimple::write_prototype_for
//       Access: Private
//  Description: Writes the prototype for the indicated function.
////////////////////////////////////////////////////////////////////
void InterfaceMakerPythonSimple::
write_prototype_for(ostream &out, InterfaceMaker::Function *func) {
  Function::Remaps::const_iterator ri;

  for (ri = func->_remaps.begin(); ri != func->_remaps.end(); ++ri) {
    FunctionRemap *remap = (*ri);
    if (!output_function_names) {
      // If we're not saving the function names, don't export it from
      // the library.
      out << "static ";
    } else {
      out << "extern \"C\" ";
    }
    out << "PyObject *"
        << remap->_wrapper_name << "(PyObject *self, PyObject *args);\n";
  }
}

////////////////////////////////////////////////////////////////////
//     Function: InterfaceMakerPythonSimple::write_function_for
//       Access: Private
//  Description: Writes the definition for a function that will call
//               the indicated C++ function or method.
////////////////////////////////////////////////////////////////////
void InterfaceMakerPythonSimple::
write_function_for(ostream &out, InterfaceMaker::Function *func) {
  Function::Remaps::const_iterator ri;

  for (ri = func->_remaps.begin(); ri != func->_remaps.end(); ++ri) {
    FunctionRemap *remap = (*ri);
    write_function_instance(out, func, remap);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: InterfaceMakerPythonSimple::write_function_instance
//       Access: Private
//  Description: Writes out the particular function that handles a
//               single instance of an overloaded function.
////////////////////////////////////////////////////////////////////
void InterfaceMakerPythonSimple::
write_function_instance(ostream &out, InterfaceMaker::Function *func,
                        FunctionRemap *remap) {
  out << "/*\n"
      << " * Python simple wrapper for\n"
      << " * ";
  remap->write_orig_prototype(out, 0);
  out << "\n"
      << " */\n";

  if (!output_function_names) {
    // If we're not saving the function names, don't export it from
    // the library.
    out << "static ";
  }

  out << "PyObject *\n"
      << remap->_wrapper_name << "(PyObject *, PyObject *args) {\n";
    
  //  write_spam_message(def_index, out);

  string format_specifiers;
  string parameter_list;
  string container;
  vector_string pexprs;

  // Make one pass through the parameter list.  We will output a
  // one-line temporary variable definition for each parameter, while
  // simultaneously building the ParseTuple() function call and also
  // the parameter expression list for call_function().

  int pn;
  for (pn = 0; pn < (int)remap->_parameters.size(); pn++) {
    indent(out, 2);
    CPPType *orig_type = remap->_parameters[pn]._remap->get_orig_type();
    CPPType *type = remap->_parameters[pn]._remap->get_new_type();
    string param_name = remap->get_parameter_name(pn);
    
    // This is the string to convert our local variable to the
    // appropriate C++ type.  Normally this is just a cast.
    string pexpr_string =
      "(" + type->get_local_name(&parser) + ")" + param_name;
    
    if (remap->_parameters[pn]._remap->new_type_is_atomic_string()) {
      if (TypeManager::is_char_pointer(orig_type)) {
        out << "char *" << param_name;
        format_specifiers += "s";
        parameter_list += ", &" + param_name;
        
      } else {
        out << "char *" << param_name
            << "_str; int " << param_name << "_len";
        format_specifiers += "s#";
        parameter_list += ", &" + param_name
          + "_str, &" + param_name + "_len";
        pexpr_string = "basic_string<char>(" +
          param_name + "_str, " +
          param_name + "_len)";
      }
      
    } else if (TypeManager::is_bool(type)) {
      out << "PyObject *" << param_name;
      format_specifiers += "O";
      parameter_list += ", &" + param_name;
      pexpr_string = "(PyObject_IsTrue(" + param_name + ")!=0)";

    } else if (TypeManager::is_integer(type)) {
      out << "int " << param_name;
      format_specifiers += "i";
      parameter_list += ", &" + param_name;

    } else if (TypeManager::is_float(type)) {
      out << "double " << param_name;
      format_specifiers += "d";
      parameter_list += ", &" + param_name;

    } else if (TypeManager::is_char_pointer(type)) {
      out << "char *" << param_name;
      format_specifiers += "s";
      parameter_list += ", &" + param_name;

    } else if (TypeManager::is_pointer(type)) {
      out << "int " << param_name;
      format_specifiers += "i";
      parameter_list += ", &" + param_name;

    } else {
      // Ignore a parameter.
      out << "PyObject *" << param_name;
      format_specifiers += "O";
      parameter_list += ", &" + param_name;
    }

    out << ";\n";
    if (remap->_has_this && pn == 0) {
      // The "this" parameter gets passed in separately.
      container = pexpr_string;
    }
    pexprs.push_back(pexpr_string);
  }

  out << "  if (PyArg_ParseTuple(args, \"" << format_specifiers
      << "\"" << parameter_list << ")) {\n";
  
  if (track_interpreter) {
    out << "    in_interpreter = 0;\n";
  }
  
  if (!remap->_void_return && 
      remap->_return_type->new_type_is_atomic_string()) {
    // Treat strings as a special case.  We don't want to format the
    // return expression.
    string return_expr = remap->call_function(out, 4, false, container, pexprs);
    
    CPPType *type = remap->_return_type->get_orig_type();
    out << "    ";
    type->output_instance(out, "return_value", &parser);
    out << " = " << return_expr << ";\n";
    
    if (track_interpreter) {
      out << "    in_interpreter = 1;\n";
    }
    
    return_expr = manage_return_value(out, 4, remap, "return_value");
    test_assert(out, 4);
    pack_return_value(out, 4, remap, return_expr);
    
  } else {
    string return_expr = remap->call_function(out, 4, true, container, pexprs);
    if (return_expr.empty()) {
      if (track_interpreter) {
        out << "    in_interpreter = 1;\n";
      }
      test_assert(out, 4);
      out << "    return Py_BuildValue(\"\");\n";
      
    } else {
      CPPType *type = remap->_return_type->get_temporary_type();
      out << "    ";
      type->output_instance(out, "return_value", &parser);
      out << " = " << return_expr << ";\n";
      if (track_interpreter) {
        out << "    in_interpreter = 1;\n";
      }
      
      return_expr = manage_return_value(out, 4, remap, "return_value");
      test_assert(out, 4);
      pack_return_value(out, 4, remap, remap->_return_type->temporary_to_return(return_expr));
    }
  }
  
  out << "  }\n";
  
  out << "  return (PyObject *)NULL;\n";
  out << "}\n\n";
}

////////////////////////////////////////////////////////////////////
//     Function: InterfaceMakerPythonSimple::pack_return_value
//       Access: Private
//  Description: Outputs a command to pack the indicated expression,
//               of the return_type type, as a Python return value.
////////////////////////////////////////////////////////////////////
void InterfaceMakerPythonSimple::
pack_return_value(ostream &out, int indent_level,
                  FunctionRemap *remap, string return_expr) {
  CPPType *orig_type = remap->_return_type->get_orig_type();
  CPPType *type = remap->_return_type->get_new_type();

  if (remap->_return_type->new_type_is_atomic_string()) {
    if (TypeManager::is_char_pointer(orig_type)) {
      indent(out, indent_level)
        << "return PyString_FromString(" << return_expr << ");\n";

    } else {
      indent(out, indent_level)
        << "return PyString_FromStringAndSize("
        << return_expr << ".data(), " << return_expr << ".length());\n";
    }

  } else if (TypeManager::is_integer(type)) {
    indent(out, indent_level)
      << "return PyInt_FromLong(" << return_expr << ");\n";

  } else if (TypeManager::is_float(type)) {
    indent(out, indent_level)
      << "return PyFloat_FromDouble(" << return_expr << ");\n";

  } else if (TypeManager::is_char_pointer(type)) {
    indent(out, indent_level)
      << "return PyString_FromString(" << return_expr << ");\n";

  } else if (TypeManager::is_pointer(type)) {
    indent(out, indent_level)
      << "return PyInt_FromLong((int)" << return_expr << ");\n";

  } else {
    // Return None.
    indent(out, indent_level)
      << "return Py_BuildValue(\"\");\n";
  }
}
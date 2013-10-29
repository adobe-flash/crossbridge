/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2013 Stephen Kelly <steveire@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/

#ifndef cmTargetCompileDefinitionsCommand_h
#define cmTargetCompileDefinitionsCommand_h

#include "cmTargetPropCommandBase.h"

class cmTargetCompileDefinitionsCommand : public cmTargetPropCommandBase
{
public:
  /**
   * This is a virtual constructor for the command.
   */
  virtual cmCommand* Clone()
    {
    return new cmTargetCompileDefinitionsCommand;
    }

  /**
   * This is called when the command is first encountered in
   * the CMakeLists.txt file.
   */
  virtual bool InitialPass(std::vector<std::string> const& args,
                           cmExecutionStatus &status);

  /**
   * The name of the command as specified in CMakeList.txt.
   */
  virtual const char* GetName() const { return "target_compile_definitions";}

  /**
   * Succinct documentation.
   */
  virtual const char* GetTerseDocumentation() const
    {
    return
      "Add compile definitions to a target.";
    }

  /**
   * More documentation.
   */
  virtual const char* GetFullDocumentation() const
    {
    return
      "  target_compile_definitions(<target> "
      "<INTERFACE|PUBLIC|PRIVATE> [items1...]\n"
      "    [<INTERFACE|PUBLIC|PRIVATE> [items2...] ...])\n"
      "Specify compile definitions to use when compiling a given target.  "
      "The named <target> must have been created by a command such as "
      "add_executable or add_library and must not be an IMPORTED target.  "
      "The INTERFACE, PUBLIC and PRIVATE keywords are required to specify "
      "the scope of the following arguments.  PRIVATE and PUBLIC items will "
      "populate the COMPILE_DEFINITIONS property of <target>.  PUBLIC and "
      "INTERFACE items will populate the INTERFACE_COMPILE_DEFINITIONS "
      "property of <target>.   "
      "The following arguments specify compile definitions.  "
      "Repeated calls for the same <target> append items in the order called."
      "\n"
      "Arguments to target_compile_definitions may use \"generator "
      "expressions\" with the syntax \"$<...>\".  "
      CM_DOCUMENT_COMMAND_GENERATOR_EXPRESSIONS
      ;
    }

  cmTypeMacro(cmTargetCompileDefinitionsCommand, cmTargetPropCommandBase);

private:
  virtual void HandleImportedTarget(const std::string &tgt);
  virtual void HandleMissingTarget(const std::string &name);

  virtual void HandleDirectContent(cmTarget *tgt,
                                   const std::vector<std::string> &content,
                                   bool prepend, bool system);
  virtual std::string Join(const std::vector<std::string> &content);
};

#endif

/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2000-2009 Kitware, Inc., Insight Software Consortium

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#ifndef cmFindBase_h
#define cmFindBase_h

#include "cmFindCommon.h"

/** \class cmFindBase
 * \brief Base class for most FIND_XXX commands.
 *
 * cmFindBase is a parent class for cmFindProgramCommand, cmFindPathCommand,
 * and cmFindLibraryCommand, cmFindFileCommand
 */
class cmFindBase : public cmFindCommon
{
public:
  cmFindBase();
  /**
   * This is called when the command is first encountered in
   * the CMakeLists.txt file.
   */
  virtual bool ParseArguments(std::vector<std::string> const& args);
  cmTypeMacro(cmFindBase, cmFindCommon);

  virtual const char* GetFullDocumentation() const;

protected:
  virtual void GenerateDocumentation();
  void PrintFindStuff();
  void ExpandPaths();
  void AddPathSuffixes();

  // see if the VariableName is already set in the cache,
  // also copy the documentation from the cache to VariableDocumentation
  // if it has documentation in the cache
  bool CheckForVariableInCache();

  cmStdString GenericDocumentation;
  // use by command during find
  cmStdString VariableDocumentation;
  cmStdString VariableName;
  std::vector<std::string> Names;
  bool NamesPerDir;
  bool NamesPerDirAllowed;

  // CMAKE_*_PATH CMAKE_SYSTEM_*_PATH FRAMEWORK|LIBRARY|INCLUDE|PROGRAM
  cmStdString EnvironmentPath; // LIB,INCLUDE

  bool AlreadyInCache;
  bool AlreadyInCacheWithoutMetaInfo;
private:
  // Add pieces of the search.
  void AddCMakeEnvironmentPath();
  void AddCMakeVariablePath();
  void AddSystemEnvironmentPath();
  void AddCMakeSystemVariablePath();
  void AddUserHintsPath();
  void AddUserGuessPath();

  // Helpers.
  void AddCMakePrefixPath(const char* variable);
  void AddEnvPrefixPath(const char* variable);
  void AddPrefixPaths(std::vector<std::string> const& in_paths,
                      PathType pathType);
};



#endif

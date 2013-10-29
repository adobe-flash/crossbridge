/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2000-2009 Kitware, Inc., Insight Software Consortium

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#include "cmGlobalVisualStudio6Generator.h"
#include "cmLocalVisualStudio6Generator.h"
#include "cmMakefile.h"
#include "cmake.h"
#include "cmGeneratedFileStream.h"

// Utility function to make a valid VS6 *.dsp filename out
// of a CMake target name:
//
std::string GetVS6TargetName(const std::string& targetName)
{
  std::string name(targetName);

  // Eliminate hyphens. VS6 cannot handle hyphens in *.dsp filenames...
  // Replace them with underscores.
  //
  cmSystemTools::ReplaceString(name, "-", "_");

  return name;
}

cmGlobalVisualStudio6Generator::cmGlobalVisualStudio6Generator()
{
  this->FindMakeProgramFile = "CMakeVS6FindMake.cmake";
}

void cmGlobalVisualStudio6Generator
::EnableLanguage(std::vector<std::string>const& lang,
                 cmMakefile *mf,
                 bool optional)
{
  cmGlobalVisualStudioGenerator::AddPlatformDefinitions(mf);
  mf->AddDefinition("CMAKE_GENERATOR_RC", "rc");
  mf->AddDefinition("CMAKE_GENERATOR_NO_COMPILER_ENV", "1");
  this->GenerateConfigurations(mf);
  this->cmGlobalGenerator::EnableLanguage(lang, mf, optional);
}

void cmGlobalVisualStudio6Generator::GenerateConfigurations(cmMakefile* mf)
{
  std::string fname= mf->GetRequiredDefinition("CMAKE_ROOT");
  const char* def= mf->GetDefinition( "MSPROJECT_TEMPLATE_DIRECTORY");
  if(def)
    {
    fname = def;
    }
  else
    {
    fname += "/Templates";
    }
  fname += "/CMakeVisualStudio6Configurations.cmake";
  if(!mf->ReadListFile(mf->GetCurrentListFile(), fname.c_str()))
    {
    cmSystemTools::Error("Cannot open ", fname.c_str(),
                         ".  Please copy this file from the main "
                         "CMake/Templates directory and edit it for "
                         "your build configurations.");
    }
  else if(!mf->GetDefinition("CMAKE_CONFIGURATION_TYPES"))
    {
    cmSystemTools::Error("CMAKE_CONFIGURATION_TYPES not set by ",
                         fname.c_str(),
                         ".  Please copy this file from the main "
                         "CMake/Templates directory and edit it for "
                         "your build configurations.");
    }
}

std::string cmGlobalVisualStudio6Generator
::GenerateBuildCommand(const char* makeProgram,
                       const char *projectName,
                       const char *projectDir,
                       const char* additionalOptions,
                       const char *targetName,
                       const char* config,
                       bool ignoreErrors,
                       bool)
{
  // Visual studio 6 doesn't need project dir
  (void) projectDir;
  // Ingoring errors is not implemented in visual studio 6
  (void) ignoreErrors;

  // now build the test
  std::vector<std::string> mp;
  mp.push_back("[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio"
               "\\6.0\\Setup;VsCommonDir]/MSDev98/Bin");
  cmSystemTools::ExpandRegistryValues(mp[0]);
  std::string originalCommand = makeProgram;
  std::string makeCommand =
    cmSystemTools::FindProgram(makeProgram, mp);
  if(makeCommand.size() == 0)
    {
    std::string e = "Generator cannot find Visual Studio 6 msdev program \"";
    e += originalCommand;
    e += "\" specified by CMAKE_MAKE_PROGRAM cache entry.  ";
    e += "Please fix the setting.";
    cmSystemTools::Error(e.c_str());
    return "";
    }
  makeCommand = cmSystemTools::ConvertToOutputPath(makeCommand.c_str());

  // if there are spaces in the makeCommand, assume a full path
  // and convert it to a path with no spaces in it as the
  // RunSingleCommand does not like spaces
#if defined(_WIN32) && !defined(__CYGWIN__)
  if(makeCommand.find(' ') != std::string::npos)
    {
    cmSystemTools::GetShortPath(makeCommand.c_str(), makeCommand);
    }
#endif
  makeCommand += " ";
  makeCommand += projectName;
  makeCommand += ".dsw /MAKE \"";
  bool clean = false;
  if ( targetName && strcmp(targetName, "clean") == 0 )
    {
    clean = true;
    targetName = "ALL_BUILD";
    }
  if (targetName && strlen(targetName))
    {
    makeCommand += targetName;
    }
  else
    {
    makeCommand += "ALL_BUILD";
    }
  makeCommand += " - ";
  if(config && strlen(config))
    {
    makeCommand += config;
    }
  else
    {
    makeCommand += "Debug";
    }
  if(clean)
    {
    makeCommand += "\" /CLEAN";
    }
  else
    {
    makeCommand += "\" /BUILD";
    }
  if ( additionalOptions )
    {
    makeCommand += " ";
    makeCommand += additionalOptions;
    }
  return makeCommand;
}

///! Create a local generator appropriate to this Global Generator
cmLocalGenerator *cmGlobalVisualStudio6Generator::CreateLocalGenerator()
{
  cmLocalGenerator *lg = new cmLocalVisualStudio6Generator;
  lg->SetGlobalGenerator(this);
  return lg;
}


void cmGlobalVisualStudio6Generator::Generate()
{
  // first do the superclass method
  this->cmGlobalVisualStudioGenerator::Generate();

  // Now write out the DSW
  this->OutputDSWFile();
}

// Write a DSW file to the stream
void cmGlobalVisualStudio6Generator
::WriteDSWFile(std::ostream& fout,cmLocalGenerator* root,
               std::vector<cmLocalGenerator*>& generators)
{
  // Write out the header for a DSW file
  this->WriteDSWHeader(fout);

  // Collect all targets under this root generator and the transitive
  // closure of their dependencies.
  TargetDependSet projectTargets;
  TargetDependSet originalTargets;
  this->GetTargetSets(projectTargets, originalTargets, root, generators);
  OrderedTargetDependSet orderedProjectTargets(projectTargets);

  for(OrderedTargetDependSet::const_iterator
        tt = orderedProjectTargets.begin();
      tt != orderedProjectTargets.end(); ++tt)
    {
    cmTarget* target = *tt;
    // Write the project into the DSW file
    const char* expath = target->GetProperty("EXTERNAL_MSPROJECT");
    if(expath)
      {
      std::string project = target->GetName();
      std::string location = expath;
      this->WriteExternalProject(fout, project.c_str(),
                                 location.c_str(), target->GetUtilities());
      }
    else
      {
      std::string dspname = GetVS6TargetName(target->GetName());
      std::string dir = target->GetMakefile()->GetStartOutputDirectory();
      dir = root->Convert(dir.c_str(), cmLocalGenerator::START_OUTPUT);
      this->WriteProject(fout, dspname.c_str(), dir.c_str(), *target);
      }
    }

  // Write the footer for the DSW file
  this->WriteDSWFooter(fout);
}

void cmGlobalVisualStudio6Generator
::OutputDSWFile(cmLocalGenerator* root,
                std::vector<cmLocalGenerator*>& generators)
{
  if(generators.size() == 0)
    {
    return;
    }
  std::string fname = root->GetMakefile()->GetStartOutputDirectory();
  fname += "/";
  fname += root->GetMakefile()->GetProjectName();
  fname += ".dsw";
  std::ofstream fout(fname.c_str());
  if(!fout)
    {
    cmSystemTools::Error("Error can not open DSW file for write: ",
                         fname.c_str());
    cmSystemTools::ReportLastSystemError("");
    return;
    }
  this->WriteDSWFile(fout, root, generators);
}

// output the DSW file
void cmGlobalVisualStudio6Generator::OutputDSWFile()
{
  std::map<cmStdString, std::vector<cmLocalGenerator*> >::iterator it;
  for(it = this->ProjectMap.begin(); it!= this->ProjectMap.end(); ++it)
    {
    this->OutputDSWFile(it->second[0], it->second);
    }
}

// Write a dsp file into the DSW file,
// Note, that dependencies from executables to
// the libraries it uses are also done here
void cmGlobalVisualStudio6Generator::WriteProject(std::ostream& fout,
                                                  const char* dspname,
                                                  const char* dir,
                                                  cmTarget& target)
{
  fout << "#########################################################"
    "######################\n\n";
  fout << "Project: \"" << dspname << "\"="
       << dir << "\\" << dspname << ".dsp - Package Owner=<4>\n\n";
  fout << "Package=<5>\n{{{\n}}}\n\n";
  fout << "Package=<4>\n";
  fout << "{{{\n";
  VSDependSet const& depends = this->VSTargetDepends[&target];
  for(VSDependSet::const_iterator di = depends.begin();
      di != depends.end(); ++di)
    {
    const char* name = di->c_str();
    fout << "Begin Project Dependency\n";
    fout << "Project_Dep_Name " << GetVS6TargetName(name) << "\n";
    fout << "End Project Dependency\n";
    }
  fout << "}}}\n\n";

  UtilityDependsMap::iterator ui = this->UtilityDepends.find(&target);
  if(ui != this->UtilityDepends.end())
    {
    const char* uname = ui->second.c_str();
    fout << "Project: \"" << uname << "\"="
         << dir << "\\" << uname << ".dsp - Package Owner=<4>\n\n";
    fout <<
      "Package=<5>\n{{{\n}}}\n\n"
      "Package=<4>\n"
      "{{{\n"
      "Begin Project Dependency\n"
      "Project_Dep_Name " << dspname << "\n"
      "End Project Dependency\n"
      "}}}\n\n";
      ;
    }
}


// Write a dsp file into the DSW file,
// Note, that dependencies from executables to
// the libraries it uses are also done here
void cmGlobalVisualStudio6Generator::WriteExternalProject(std::ostream& fout,
                               const char* name,
                               const char* location,
                               const std::set<cmStdString>& dependencies)
{
 fout << "#########################################################"
    "######################\n\n";
  fout << "Project: \"" << name << "\"="
       << location << " - Package Owner=<4>\n\n";
  fout << "Package=<5>\n{{{\n}}}\n\n";
  fout << "Package=<4>\n";
  fout << "{{{\n";


  std::set<cmStdString>::const_iterator i, end;
  // write dependencies.
  i = dependencies.begin();
  end = dependencies.end();
  for(;i!= end; ++i)
  {
    fout << "Begin Project Dependency\n";
    fout << "Project_Dep_Name " << GetVS6TargetName(*i) << "\n";
    fout << "End Project Dependency\n";
  }
  fout << "}}}\n\n";
}



// Standard end of dsw file
void cmGlobalVisualStudio6Generator::WriteDSWFooter(std::ostream& fout)
{
  fout << "######################################################"
    "#########################\n\n";
  fout << "Global:\n\n";
  fout << "Package=<5>\n{{{\n}}}\n\n";
  fout << "Package=<3>\n{{{\n}}}\n\n";
  fout << "#####################################################"
    "##########################\n\n";
}


// ouput standard header for dsw file
void cmGlobalVisualStudio6Generator::WriteDSWHeader(std::ostream& fout)
{
  fout << "Microsoft Developer Studio Workspace File, Format Version 6.00\n";
  fout << "# WARNING: DO NOT EDIT OR DELETE THIS WORKSPACE FILE!\n\n";
}

//----------------------------------------------------------------------------
std::string
cmGlobalVisualStudio6Generator::WriteUtilityDepend(cmTarget* target)
{
  std::string pname = target->GetName();
  pname += "_UTILITY";
  pname = GetVS6TargetName(pname.c_str());
  std::string fname = target->GetMakefile()->GetStartOutputDirectory();
  fname += "/";
  fname += pname;
  fname += ".dsp";
  cmGeneratedFileStream fout(fname.c_str());
  fout.SetCopyIfDifferent(true);
  fout <<
    "# Microsoft Developer Studio Project File - Name=\""
       << pname << "\" - Package Owner=<4>\n"
    "# Microsoft Developer Studio Generated Build File, Format Version 6.00\n"
    "# ** DO NOT EDIT **\n"
    "\n"
    "# TARGTYPE \"Win32 (x86) Generic Project\" 0x010a\n"
    "\n"
    "CFG=" << pname << " - Win32 Debug\n"
    "!MESSAGE \"" << pname << " - Win32 Debug\""
    " (based on \"Win32 (x86) Generic Project\")\n"
    "!MESSAGE \"" << pname << " - Win32 Release\" "
    "(based on \"Win32 (x86) Generic Project\")\n"
    "!MESSAGE \"" << pname << " - Win32 MinSizeRel\" "
    "(based on \"Win32 (x86) Generic Project\")\n"
    "!MESSAGE \"" << pname << " - Win32 RelWithDebInfo\" "
    "(based on \"Win32 (x86) Generic Project\")\n"
    "\n"
    "# Begin Project\n"
    "# Begin Target\n"
    "# Name \"" << pname << " - Win32 Debug\"\n"
    "# Name \"" << pname << " - Win32 Release\"\n"
    "# Name \"" << pname << " - Win32 MinSizeRel\"\n"
    "# Name \"" << pname << " - Win32 RelWithDebInfo\"\n"
    "# End Target\n"
    "# End Project\n"
    ;
  return pname;
}

//----------------------------------------------------------------------------
void cmGlobalVisualStudio6Generator
::GetDocumentation(cmDocumentationEntry& entry)
{
  entry.Name = cmGlobalVisualStudio6Generator::GetActualName();
  entry.Brief = "Generates Visual Studio 6 project files.";
  entry.Full = "";
}

//----------------------------------------------------------------------------
void
cmGlobalVisualStudio6Generator
::AppendDirectoryForConfig(const char* prefix,
                           const char* config,
                           const char* suffix,
                           std::string& dir)
{
  if(config)
    {
    dir += prefix;
    dir += config;
    dir += suffix;
    }
}

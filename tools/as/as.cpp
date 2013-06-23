// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <ctype.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <string>
#include <vector>

#define CHUNK (256 * 1024)

extern "C" char *SetFlasccSDKLocation(const char *suffix);

// quote any non-"word" [a-zA-Z0-9_] chars
static std::string quotemeta(const std::string &s)
{
  char stackBuf[4096];
  size_t len = s.length();
  char *buf = ((len * 2) > sizeof(stackBuf)) ?  new char[len * 2] : stackBuf;
  char *cur = buf;
  const char *data = s.data();

  while(len--)
  {
    char c = *data++;
    if(!isalnum(c) && c != '_')
      *cur++ = '\\';
    *cur++ = c;
  }

  std::string result(buf, cur - buf);

  if(buf != stackBuf)
    delete buf;

  return result;
}

// execute a command and optionally capture stdout and stderr
static int runCmd(std::string *output, const std::vector<std::string> &argv)
{
  std::string cmd;
  std::vector<std::string>::const_iterator i = argv.begin(), e = argv.end();

  if(i != e)
    for(;;)
    {
      cmd += quotemeta(*i);
      i++;
      if(i == e)
        break;
      cmd += ' ';
    }

  cmd += " 2>&1 ";

//fprintf(stderr, "popen(\"%s\")\n", cmd.c_str());
  FILE *pipe = popen(cmd.c_str(), "r");

  if(!pipe)
    return -1;

  for(;;)
  {
    char buf[CHUNK];
    size_t nRead = fread(buf, 1, sizeof(buf), pipe);

    if(nRead == 0)
      break;
    if(nRead < 0)
    {
      pclose(pipe);
      return (int)nRead;
    }
    if(output)
      output->append(buf, nRead);
    else
      fwrite(buf, 1, nRead, stdout);
  }
  return pclose(pipe);
}

// are we running under cygwin?
static bool isCygwin()
{
  static bool sChecked = false;
  static bool sCygwin;

  if(!sChecked)
  {
    struct utsname name;

    sCygwin = !uname(&name) && !strncmp(name.sysname, "CYGWIN", 6);
    sChecked = true;
  }
  return sCygwin;
}

// get a "universal" path -- friendly to cygwin and non-cygwin
// no-op outside of cygwin
static std::string unipath(const std::string path)
{
  if(!isCygwin())
    return path;

  std::vector<std::string> args;
  std::string result;

  args.push_back("cygpath");
  args.push_back("-m");
  args.push_back(path);
  runCmd(&result, args);

  size_t len = result.length();
  const char *rdata = result.data() + len - 1;

  while(len && *rdata == '\n')
  {
    len--;
    rdata--;
  }
  result.resize(len);
  return result;
}

// read a single line from a FILE into a std::string
static bool readline(std::string *out, FILE *f)
{
  char buf[CHUNK];

  for(;;)
  {
    if(!fgets(buf, sizeof(buf), f))
      return false;

    int len = strlen(buf);

    out->append(buf, len);
    if(buf[len - 1] == '\n')
      break;
  }
  return true;
}

#ifdef _WIN32
// Only used by the windows variant of as.cpp
static char apptempdir[MAX_PATH] = {0};
#endif

// create and open a temporary file
static FILE *tmpfile(std::string *pathOut, const std::string &ext)
{
  #ifdef _WIN32
    if(apptempdir[0] == 0) {
      char wintmpdir[MAX_PATH];
      if (!GetTempPath(MAX_PATH, wintmpdir)) {
        fprintf(stderr, "Can't determine temporary directory\n");
        exit(-1);
      }
      sprintf(apptempdir, "%s\\alcas_%u\\", wintmpdir, unsigned(GetCurrentProcessId()));
      // If this directory wasn't release the last time this process id was used we need to flush it
      RemoveDirectory(apptempdir);
    }

    // Create a unique temp file inside the app temp dir
    char tempfilepath[MAX_PATH];
    unsigned id = GetTempFileName(apptempdir, "alc", 0, tempfilepath);
    if(id == 0) {
      fprintf(stderr, "Can't create temporary file\n");
      exit(-1);
    }

    // Append the ".as" suffix to the temp file name
    char tempfilepathex[MAX_PATH];
    sprintf(tempfilepathex, "%s.as", tempfilepath);

    // Now go and create the real temp file
    HANDLE h = CreateFile(tempfilepathex, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) {
      fprintf(stderr, "Can't create temporary file handle\n");
      exit(-1);
    }
    // Close the windows file handle, we want to re-open it with a posix call
    CloseHandle(h);

    // finally get the FILE* to the temp file
    return fopen(tempfilepathex, "wb+");
  #else
    char buf[PATH_MAX];

    strcpy(buf, "/tmp/alcasXXXXXXXX");
    int suffixLen = 0;
    if (ext.length() > 0) {
        strcat(buf, ".");
        strcat(buf, ext.c_str());
        suffixLen = ext.length() + 1;
    }

    int fd = mkstemps(buf, suffixLen);

    if(fd < 0) {
      perror("mkstemps");
      fprintf(stderr, "Can't open temporary file: %s\n", buf);
      exit(-1);
    }
    
    pathOut->append(buf);
    return fdopen(fd, "wb+");
  #endif
}


// exit with an error
static void error(const char *fmt, ...)
{
  char buf[CHUNK];
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  buf[sizeof(buf)-1] = 0;
  va_end(ap);
  fprintf(stderr, "Error: %s\n", buf);
  exit(-1);
}

// unlink a path on destruct
class Unlinker
{
  std::string _path;
public:
  Unlinker(const std::string &path) : _path(path) {}
  ~Unlinker()
  {
//    fprintf(stderr, "unlink(\"%s\")\n", _path.c_str());
    unlink(_path.c_str());
  }
};

// wraps libgen dirname
static std::string dirname(const std::string &s)
{
  char buf[PATH_MAX];

  strncpy(buf, s.c_str(), sizeof(buf));
  buf[sizeof(buf)-1] = 0;
  return dirname(buf);
}

// wraps libgen basename
static std::string basename(const std::string &s)
{
  char buf[PATH_MAX];

  strncpy(buf, s.c_str(), sizeof(buf));
  buf[sizeof(buf)-1] = 0;
  return basename(buf);
}

int main(int argc, char **argv)
{
  static const char *defOutput = "a.out";

  bool targetPlayer = false;
  bool useLegacyAsc = false;
  FILE *input = NULL;
  FILE *output = NULL;
  std::vector<std::string> jargs;

  jargs.push_back("java");

  for(int i = 1; i < argc; i++)
  {
    std::string arg(argv[i]);

    if(arg == "-o")
    {
      if(i >= argc - 1)
        error("-o with no output file");
      if(output)
        error("Multiple output files specified");
      if(!(output = fopen(argv[++i], "wb+")))
        error("Failed to open %s for output", argv[i]);
    }
    else if(arg == "--")
    {
      if(input)
        error("Multiple input files specified");
      input = stdin;
    }
    else if(arg == "--target-player")
      targetPlayer = true;
    else if(arg == "--use-legacy-asc")
      useLegacyAsc = true;
    else if(arg == "--jvmopt")
    {
      if(i >= argc - 1)
        error("--jvmopt with no parameter");
      jargs.push_back(argv[++i]);
    }
    else
    {
      if(input)
        error("Multiple input files specified: %s", argv[i]);
      if(!(input = fopen(argv[i], "r")))
        error("Failed to open %s for input", argv[i]);
    }
  }
  if(!input)
    input = stdin;
  if(!output && !(output = fopen(defOutput, "wb+")))
    error("Failed to open %s for output", defOutput);

  std::string tmp1Path;
  FILE *tmp1 = tmpfile(&tmp1Path, ".as");

  if(!tmp1)
    error("Couldn't create temp file 1");

  Unlinker l1(tmp1Path);
  std::string tmp2Path;
  FILE *tmp2 = tmpfile(&tmp2Path, ".as");

  if(!tmp2)
    error("Couldn't create temp file 2");

  Unlinker l2(tmp2Path);
  FILE *cur = tmp1;

  for(;;)
  {
    std::string line;

    if(!readline(&line, input))
      break;

    if(line.at(0) == '#')
    {
      if(!line.compare(1, 8, "---SPLIT"))
      {
        if(cur == tmp2)
          error("Multiple split directives encountered");
        fclose(tmp1);
        tmp1 = NULL;
        cur = tmp2;
      }
    }
    else
    {
      size_t len = line.length();
      const char *data = line.data();

      while(len)
      {
        int written = fwrite(data, 1, len, cur);

        if(written <= 0)
          error("Failed to write to output file");

        len -= written;
        data += written;
      }
    }
  }
  if(tmp1)
    fclose(tmp1);
  fclose(tmp2);

  std::string outTmpPath;
  FILE *outTmp = tmpfile(&outTmpPath, ".abc");
  fclose(outTmp);

  if(!outTmp)
    error("Couldn't create temp file 2");

  Unlinker l3(outTmpPath);

  std::string libPath = SetFlasccSDKLocation("/../../");
  libPath = unipath(libPath + "/usr/lib");

  if(jargs.size() == 1) // default is 1 arg (just "java")
  {
    jargs.push_back("-Xms512M");
    jargs.push_back("-Xmx2048M");
    jargs.push_back("-ea");
  }
  if(useLegacyAsc)
  {
    jargs.push_back("-classpath");
    jargs.push_back(libPath + "/asc.jar");
    jargs.push_back("macromedia.asc.embedding.ScriptCompiler");
  }
  else
  {
    jargs.push_back("-jar");
    jargs.push_back(libPath + "/asc2.jar");
    jargs.push_back("-merge");
    jargs.push_back("-md");
  }
  jargs.push_back("-abcfuture");
  jargs.push_back("-AS3");

  jargs.push_back("-import");
  jargs.push_back(libPath + "/builtin.abc");
  jargs.push_back("-import");
  if(targetPlayer)
    jargs.push_back(libPath + "/playerglobal.abc");
  else
    jargs.push_back(libPath + "/shell_toplevel.abc");
  jargs.push_back("-import");
  jargs.push_back(libPath + "/BinaryData.abc");
  jargs.push_back("-import");
  jargs.push_back(libPath + "/Exit.abc");
  jargs.push_back("-import");
  jargs.push_back(libPath + "/LongJmp.abc");
  jargs.push_back("-import");
  jargs.push_back(libPath + "/ISpecialFile.abc");
  jargs.push_back("-import");
  jargs.push_back(libPath + "/IBackingStore.abc");

  if(targetPlayer) {
    jargs.push_back("-import");
    jargs.push_back(libPath + "/InMemoryBackingStore.abc");
  }

  jargs.push_back("-import");
  jargs.push_back(libPath + "/IVFS.abc");
  jargs.push_back("-import");
  jargs.push_back(libPath + "/CModule.abc");

  if(useLegacyAsc) {
    jargs.push_back(unipath(tmp1Path));
    jargs.push_back(unipath(tmp2Path));
  } else {
    jargs.push_back(unipath(tmp2Path));
    jargs.push_back(unipath(tmp1Path));
  }

  jargs.push_back("-outdir");
  jargs.push_back(unipath(dirname(outTmpPath)));
  
  std::string outNoExt = basename(outTmpPath);

  outNoExt = outNoExt.substr(0, outNoExt.length() - 4); // trim the ".abc"! -- ugh!

  jargs.push_back("-out");
  jargs.push_back(outNoExt);

  std::string cmdoutput;
  if(runCmd(&cmdoutput, jargs))
    error("Failed to execute compiler: %s", cmdoutput.c_str());

  outTmp = fopen(outTmpPath.c_str(), "r");
  for(;;)
  {
    char buf[CHUNK];
    int nRead = fread(buf, 1, sizeof(buf), outTmp);

    if(nRead < 0)
      error("Failed to read from temporary output");
    if(nRead == 0)
      break;
    if(nRead != fwrite(buf, 1, nRead, output))
      error("Failed to write to final output");
  }
  fclose(output);
  fclose(outTmp);

  #ifdef _WIN32
    DeleteFile(apptempdir);
  #endif
  
  return 0;
}

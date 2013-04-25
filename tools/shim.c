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

// simple utility that logs commands before executing them
// to use it, just create a symlink to it

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/stat.h>

#if __MACH__
#include <mach-o/dyld.h>

static int statSelf(struct stat *dst)
{
  char path[1024];
  uint32_t pathSize = sizeof(path)-1;
  int result;

  if((result = _NSGetExecutablePath(path, &pathSize)))
    return result;
  path[pathSize] = 0;
  return stat(path, dst);
}
#else
static int statSelf(struct stat *dst)
{
  char path[256];
  sprintf(path, "/proc/%d/exe", getpid());
  return stat(path, dst);
}
#endif

static void error(const char *tmpl, ...)
{
  int len;
  char buf[4096];
  va_list ap;
  va_start(ap, tmpl);
  len = vsnprintf(buf, sizeof(buf)-1, tmpl, ap);
  va_end(ap);
  buf[len] = 0;
  fprintf(stderr, "error: %s\n", buf);
  exit(-1);
}

// logs an execution
static void logExec(char **argv)
{
  FILE *flog;
  char *log = getenv("SHIM_LOG"); // try env var first
  char buf[16384];
  char *cur = buf, *end = buf + sizeof(buf) - 1; // always room for trailing nl
  char *arg = *argv;
  if(!log)
    log = "/tmp/shim.log"; // default log
  flog = fopen(log, "a");
  if(!flog)
    error("couldn't open log (%d)", errno);
  while(arg)
  {
    char c;
    while((c = *arg++))
    {
      if((c == ' ' || c == '\\' || c == '\n') && cur != end)
        *cur++ = '\\';
      if(cur != end)
        *cur++ = (c == '\n') ? 'n' : c;
    }
    if((arg = *++argv) && cur != end)
      *cur++ = ' ';
  }
  *cur++ = '\n'; // always ok since end allows for this!
  if(fwrite(buf, 1, cur-buf, flog) != cur-buf)
    error("couldn't write to log (%d)", errno);
  fclose(flog);
}

static void handleWhich(int rd, int wr, char **argv)
{
  FILE *whichPipe = fdopen(rd, "r");
  struct stat self;
  char buf[2048];

  if(!whichPipe)
    error("fdopen failed (%d)", errno);
  if(statSelf(&self))
    error("self stat failed (%d)", errno);
  close(wr); // not writing anything!
  while(fgets(buf, sizeof(buf), whichPipe))
  {
    struct stat other;
    char *nl = strrchr(buf, '\n');

    if(nl)
      *nl = 0;
    if(stat(buf, &other))
      error("stat failed (%s:%d)", buf, errno);
    if(other.st_dev != self.st_dev ||
        other.st_ino != self.st_ino)
    {
      fclose(whichPipe);
      logExec(argv);
      argv[0] = buf; // TODO switch?
      execv(buf, argv);
      error("exec failed (%d)", errno);
    }
  }
  error("couldn't find exe");
}

int main(int argc, char **argv)
{
  int childStdin[2];
  int childStdout[2];
  int result;

  if(pipe(childStdin))
    error("which stdin pipe creation failed (%d)", errno);
  if(pipe(childStdout))
    error("which stdout pipe creation failed (%d)", errno);
  if((result = fork()) == -1)
    error("fork failed (%d)", errno);
  if(!result) // child
  {
    char *lastSlash = strrchr(argv[0], '/');
    char *exeName = lastSlash ? lastSlash + 1 : argv[0];
    close(0);
    if(dup(childStdin[0]) != 0)
      error("stdin pipe dup failed (%d)", errno);
    close(1);
    if(dup(childStdout[1]) != 1)
      error("stdout pipe dup failed (%d)", errno);
    execl("/usr/bin/which", "/usr/bin/which", "-a", exeName, NULL);
    error("which execl failed (%d)", errno);
  }
  close(childStdin[0]);
  close(childStdout[1]);
  handleWhich(childStdout[0], childStdin[1], argv);
  return -1;
}

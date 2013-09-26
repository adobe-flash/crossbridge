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

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#if defined(__CYGWIN__) || defined(__MINGW32__)
#include <windows.h>
#include <stdio.h>

#ifdef SPECIAL_MALLOC
#define specialmalloc xmalloc
#else
#define specialmalloc malloc
#endif

static char* getResultFromCmd(const char* cmd);

static char* getResultFromCmd(const char* cmd)
{
	char buffer[PATH_MAX];
	FILE* pipe = popen(cmd, "r");
	int pos = PATH_MAX;
	char* result = NULL;
	if (!pipe) {
		fprintf(stderr, "Failed to execute command '%s' (errno: %d) (path: %s)\n", cmd, errno, getenv("PATH"));
		abort();
	}

	memset(buffer, sizeof(buffer), 0);
	fgets(buffer, PATH_MAX, pipe);
	pclose(pipe);

	pos = strlen(&buffer[0]);
	while(--pos > 0) {
		char c = buffer[pos];
		if(!(c == ' ' || c == '\t' || c == '\f' || c == '\v' || c == '\n' || c == '\r' || c == '\0')) {
			break;
		}
	}
	
	if(pos >= 0) {
		result = (char*)specialmalloc(pos+2);
		strncpy(result, &buffer[0], pos+1);
		result[pos+1] = 0;
	}
	return result;
}

static char* nativepath(const char *path);

static char* nativepath(const char *path)
{
	char buffer[PATH_MAX+32];
	char *result = NULL;
	sprintf(&buffer[0], "cygpath -at unix \"%s\"", path);
    result = getResultFromCmd(&buffer[0]);
	return result;
}

static char* javapath(const char *path);

static char* javapath(const char *path)
{
	char buffer[PATH_MAX+32];
	char *result = NULL;
	sprintf(&buffer[0], "cygpath -m \"%s\"", path);
    result = getResultFromCmd(&buffer[0]);
	return result;
}
#endif


#ifdef __cplusplus
extern "C" {
#endif
    
#if defined(__APPLE__)
extern int _NSGetExecutablePath(char* buf, unsigned int* bufsize);
#endif

#ifdef __IN_FILE_USE__
#define INFILEUSE static
#else
#define INFILEUSE
#endif

INFILEUSE void GetAppPath(char *dest, unsigned int *sz);
 
INFILEUSE void GetAppPath(char *dest, unsigned int *sz) {
  #if defined(__CYGWIN__) || defined(__MINGW32__)
    char *winpath = NULL;
    /* This should work for normal exes, but will most likely fail
       if called from a DLL on windows. */
    if((*sz = GetModuleFileNameA(0, dest, *sz)) != 0) {
      winpath = nativepath(dest);
      strncpy(dest, winpath, strlen(winpath));
    }
  #elif defined(__APPLE__)
    char macpath[PATH_MAX];
    if(_NSGetExecutablePath(&macpath[0], sz) != -1)
      realpath(&macpath[0], dest);
  #else
    char linuxpath[PATH_MAX]; memset(linuxpath, 0, PATH_MAX);
    ssize_t len = readlink("/proc/self/exe", linuxpath, sizeof(linuxpath));
    strncpy(dest, linuxpath, strlen(linuxpath));
  #endif
  *sz = strlen(dest);
}

INFILEUSE char *SetFlasccSDKLocation(const char *suffix);

INFILEUSE char *SetFlasccSDKLocation(const char *suffix) {
  static char FlasccSDKLocation[PATH_MAX] = "";
  char *dest = &FlasccSDKLocation[0];
  int slen;
  if(!FlasccSDKLocation[0]) {
    const char *sdkenv = getenv("FLASCC_INTERNAL_SDK_ROOT");
    if(sdkenv) {
      strncpy(dest, sdkenv, PATH_MAX);
    } else {
      unsigned int sz = PATH_MAX;
      GetAppPath(dest, &sz);
      /* Append the suffix to the exe path to give the final flascc sdk path */
      char *lastslash = strrchr(dest, '/');
      slen = strlen(suffix);
      if(sz+slen+1 > PATH_MAX)
        return 0;
      strncpy(lastslash, suffix, slen+1);
        
      /* so that we can access this easily from subprocesses/plugins */
      #ifdef __MINGW32__
      // can we get away with not doing this if we only need llvm compiled with mingw?
      #else
      setenv("FLASCC_INTERNAL_SDK_ROOT", &FlasccSDKLocation[0], 0);
      #endif
    }
  }
  return &FlasccSDKLocation[0];
}

#ifdef __cplusplus
}
#endif

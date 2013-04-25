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
#include <string.h>
#include <stdlib.h>

/* trivial flascc-specific uname replacement */

/* option flags */
enum
{
  kM = 1,
  kN = 2,
  kP = 4,
  kR = 8,
  kS = 16,
  kV = 32
};

void usage(const char *prog, int badOpt)
{
  const char *lastSlash = strrchr(prog, '/');

  if(lastSlash)
    prog = lastSlash + 1;
  if(badOpt)
    fprintf(stderr, "%s: illegal option -- %c\n", prog, badOpt);
  fprintf(stderr, "usage: %s [-amnprsv]\n", prog);
  exit(1);
}

void printUname(int flags)
{
  int needSep = 0;

  /* order: snrvmp */
  if(flags & kS)
    printf("%s%s", needSep++ ? " " : "", "FreeBSD");
  if(flags & kN)
    printf("%s%s", needSep++ ? " " : "", "localhost.localdomain");
  if(flags & kR)
    printf("%s%s", needSep++ ? " " : "", "8.2-RELEASE");
  if(flags & kV)
    printf("%s%s", needSep++ ? " " : "", "FreeBSD 8.2-RELEASE (compatible; Adobe Flascc 2.0)");
  if(flags & kM)
    printf("%s%s", needSep++ ? " " : "", "AVM2");
  if(flags & kP)
    printf("%s%s", needSep++ ? " " : "", "AVM2");
  printf("\n");
}

int main(int argc, char **argv)
{
  int flags = 0;

  if(argc == 1)
    flags = kS;
  else if(argc == 2)
  {
    const char *opt = argv[1];

    if(*opt++ != '-')
      usage(argv[0], 0);

    while(*opt)
    {
      switch(*opt)
      {
        case 'a': flags |= kM|kN|kR|kS|kV; break;
        case 'm': flags |= kM; break;
        case 'n': flags |= kN; break;
        case 'p': flags |= kP; break;
        case 'r': flags |= kR; break;
        case 's': flags |= kS; break;
        case 'v': flags |= kV; break;
        default: usage(argv[0], *opt); break;
      }
      opt++;
    }
  }
  else
    usage(argv[0], 0);

  printUname(flags);

  return 0;
}

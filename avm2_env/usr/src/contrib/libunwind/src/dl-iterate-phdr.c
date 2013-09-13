/* libunwind - a platform-independent unwind library
   Copyright (C) 2003-2005 Hewlett-Packard Co
	Contributed by David Mosberger-Tang <davidm@hpl.hp.com>

This file is part of libunwind.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  */

#include <limits.h>
#include <stdio.h>

#include "libunwind_i.h"
#include "os-linux.h"
#include "dl-iterate-phdr.h"

int
dl_iterate_phdr (int (*callback) (struct dl_phdr_info *info, size_t size, void *data),
                 void *data)
{
        struct map_iterator mi;
        int found = 0, rc;
        unsigned long hi;
        unsigned long low;

  if (maps_init (&mi, getpid()) < 0)
    return -1;

  unsigned long offset;
  while (maps_next (&mi, &low, &hi, &offset)) {
    struct dl_phdr_info info;
    info.dlpi_name = mi.path;
    info.dlpi_addr = low;

    Elf_W(Ehdr) *ehdr;

  struct elf_image ei;
  ei.image = (void*)info.dlpi_addr;
  ei.size = hi - low;

  //if (!elf_w(valid_object) (&ei))
  //  continue;

  ehdr = ei.image;
  Elf_W(Phdr) *phdr = (Elf_W(Phdr) *) ((char *) ei.image + ehdr->e_phoff);
  info.dlpi_phdr = phdr;
  info.dlpi_phdr = ehdr->e_phnum;
  callback(&info, sizeof(info), data);


  }
  maps_close (&mi);

  return rc;
}

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

// generates an ELF object file with this program's symbols in it
int main()
{
  void *elf;
  int elfLen;
  FILE *fout;

  __asm__("import flascc.ELF");
  __asm__("import flash.utils.ByteArray");
  
  __asm__("var mod = CModule.getModuleVector()[0]");
  __asm__("var syms:Array = mod.getSymsUsingMD()");
  __asm__("var sects:Object = mod.getSections()");
  __asm__("var elf:ByteArray = ELF.createFromSyms(syms, sects)");

  __asm__("%0 = elf.length" : "=r"(elfLen));

  elf = malloc(elfLen);

  __asm__("ram.position = %0; ram.writeBytes(elf)" : : "r"(elf));
  
  fout = fopen("foo.elf", "w");
  fwrite(elf, 1, elfLen, fout);
  return 0;
}

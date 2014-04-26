/* LLVM LOCAL begin (ENTIRE FILE!)  */
/* Definition of the oFILEstream class
Copyright (C) 2005 Free Software Foundation, Inc.
Contributed by Chris Lattner (sabre@nondot.org)

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

#ifndef LLVM_FILE_OSTREAM_H
#define LLVM_FILE_OSTREAM_H

#include <ostream>
#include <ext/stdio_filebuf.h>

/// oFILEstream - An ostream that outputs all of its data to the specified C
/// FILE* file stream.
///
/// FIXME: eliminate G++ dependency.
class oFILEstream : public std::ostream {
  __gnu_cxx::stdio_filebuf<char> sb;
public:
  typedef char           char_type;
  typedef int            int_type;
  typedef std::streampos pos_type;
  typedef std::streamoff off_type;
  
  explicit oFILEstream(FILE *F)
    : std::ostream(&sb), sb(F, std::ios_base::out) { }
};

#endif

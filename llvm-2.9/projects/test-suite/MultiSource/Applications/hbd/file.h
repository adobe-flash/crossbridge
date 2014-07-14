/* file.h */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#ifndef FILE_H
#define FILE_H

/* Various macros to read and copy from the files,
   and maintain tabs on where in the files we are at. */

inline u8 get8(FILE *f, int *f_pos) {
  *f_pos++;
  return getc(f);
}

inline u16 get16(FILE *f, int *f_pos) {
  u16 t1 = get8(f, f_pos); u16 t2 = get8(f, f_pos);
  return (u16)((t1 << 8) | t2);
}

inline u32 get32(FILE *f, int *f_pos) {
  u32 t1 = get16(f, f_pos); u32 t2 = get16(f, f_pos);
  return (u32)((t1 << 16) | t2);
}

#define getstr(str, size, f) ((f##_pos+=size), \
                             fread(str,size,1,f))

#define put8(f, v) ((f##_pos++),(putc((v), f)))
#define put16(f, v) (put8(f, (v) >> 8), put8(f, (v)))
#define putstr(outf, size, str) ((outf##_pos+=size), \
                                fwrite(str,size,1,outf))

#define copy8(inf, outf) ((outf##_pos++),(inf##_pos++), \
                                   putc(getc(inf), outf))
#define copy16(inf, outf) (u16)( \
                           (((u16)copy8(inf,outf)) << 8) \
                           | (u16)copy8(inf,outf))
#define copy32(inf, outf) (u32)( \
                           (((u32)copy16(inf,outf)) << 16) \
                           | (u32)copy16(inf,outf))
#define copystr(str, size, inf, outf) ((outf##_pos+=size), \
                                      (inf##_pos+=size), \
                                      fread(str,size,1,inf), \
                                      fwrite(str,size,1,outf))

#endif

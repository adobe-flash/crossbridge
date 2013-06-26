/* cp.cpp */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#include "general.h"
#include "cp.h"
#include "file.h"
#include "err.h"
#include "class.h"
#include "consts.h"

void ConstPool::read(Classfile *c, u16 *imports_count) {
  if ((constant_pool = new cp_info[(constant_pool_count = get16(c->infile, &c->infile_pos))]) == 0) memerr();
  constant_pool->tag = 0;
  for (int j = 1, i = constant_pool_count - 1; i--;) {
    cp_info *cpi = &constant_pool[j++];
    cpi->tag = (unsigned char)get8(c->infile, &c->infile_pos);
    unsigned short size;
    D(fprintf(c->outfile, "\npos: 0x%05X\tindex: %4d\t",c->infile_pos,j-1))
      switch(cpi->tag) {
        case CONSTANT_Utf8:
          if ((cpi->chp = new char[(size = get16(c->infile, &c->infile_pos)) + 1]) == 0) memerr();
          getstr(cpi->chp, size, c->infile);
          cpi->chp[size] = '\0';
          D(fprintf(c->outfile, "UTF8: %s\t", cpi->chp))
          break;
        case CONSTANT_Unicode: D(fprintf(c->outfile, "Unicode\t")) /* unused */
          break;
        case CONSTANT_Integer:
          cpi->i = get32(c->infile, &c->infile_pos);
          D(fprintf(c->outfile, "32-bit int: 0x%8lX\t", cpi->i))
          break;
        case CONSTANT_Float:
          cpi->i = get32(c->infile, &c->infile_pos);
          D(fprintf(c->outfile, "32-bit float: %.25G\t", cpi->f))
          break;
        case CONSTANT_Long:
          cpi->i = get32(c->infile, &c->infile_pos);
          cpi = &constant_pool[j++];
          cpi->tag = 0;
          cpi->i = get32(c->infile, &c->infile_pos);
          D(((cpi - 1)->i) ? fprintf(c->outfile, "64-bit int: 0x%lX%08lX", (cpi - 1)->i, cpi->i) : fprintf(c->outfile, "64-bit int: 0x%lX", cpi->i))
          if (i--) continue;
          break;
        case CONSTANT_Double:
          *(((unsigned long *)&cpi->i) + 1) = get32(c->infile, &c->infile_pos);
          cpi->i = get32(c->infile, &c->infile_pos);
          D(fprintf(c->outfile, "64-bit float: %.25G\t",(float)*(double*)(&cpi->i))) constant_pool[j++].tag = 0;
          if (i--) continue;
          break;
        case CONSTANT_Class:
          imports_count++;
          cpi->i = get16(c->infile, &c->infile_pos);
          D(fprintf(c->outfile, "Class: name = index %d\t", (int)cpi->i))
          break;
        case CONSTANT_String:
          cpi->i = get16(c->infile, &c->infile_pos);
          D(fprintf(c->outfile, "String: index %d\t", (int)cpi->i))
          break;
        case CONSTANT_Fieldref:
        case CONSTANT_Methodref:
        case CONSTANT_InterfaceMethodref:
           if ((cpi->p = new Ref) == 0) memerr();
           ((Ref*)cpi->p)->class_index = get16(c->infile, &c->infile_pos);
           ((Ref*)cpi->p)->name_and_type = get16(c->infile, &c->infile_pos);
           D(fprintf(c->outfile, "Ref: class_index %d, n&t_index %d\t", ((Ref*)cpi->p)->class_index, ((Ref*)cpi->p)->name_and_type))
           break;
        case CONSTANT_NameAndType:
          imports_count++;
          if ((cpi->p = new NameAndType) == 0) memerr();
          ((NameAndType*)cpi->p)->name_index = get16(c->infile, &c->infile_pos);
          ((NameAndType*)cpi->p)->signature_index = get16(c->infile, &c->infile_pos);
          D(fprintf(c->outfile, "Name&Type: name_index %d, sig_index %d\t", ((NameAndType*)cpi->p)->name_index, ((NameAndType*)cpi->p)->signature_index))
          break;
        default:
          fprintf(stderr, "Error reading constant pool entry %d of %d at file pos 0x%08x!\n", j, constant_pool_count, c->infile_pos);
          fatalerror(CP_ERR);
      }
  }
}

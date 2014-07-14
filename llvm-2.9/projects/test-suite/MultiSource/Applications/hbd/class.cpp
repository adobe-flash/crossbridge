/* class.cpp */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#include <string.h>
#include <ctype.h>
#include "general.h"
#include "options.h"
#include "cp.h"
#include "access.h"
#include "field.h"
#include "exp.h"
#include "method.h"
#include "version.h"
#include "class.h"
#include "file.h"
#include "err.h"
#include "consts.h"

#define JAVA_CLASSFILE_MAGIC              0xCafeBabeL

char *progname;

Classfile::Classfile(int argc, char **argv) {
  functoinsert = 0;
  outfile = stdout; infile = stdin;
  progname = *argv++;
  options = (CL_Options)0;
  if (strcmp(progname + strlen(progname) - 3, "hbt") == 0) {
    for (; (--argc) && (**argv == '-'); argv++) {
      options = (CL_Options)0;
      switch (toupper((*argv)[1])) {
        case 'D': *(int *)&options |= (int)OPT_DEBUG; break;
        case 'I': functoinsert = &((*argv)[2]); break;
        default:
          fprintf(stderr, "Unknown flag: %s\n", argv[1]);
          fatalerror(COMMAND_LINE_ERR_HBT, progname);
      }
    }
    char *tmpstr;
    switch (argc) {
      case 1:
        tmpstr = new char[strlen(argv[0])+5];
        strcpy(tmpstr, argv[0]);
        strcat(tmpstr, ".bak");
        rename(argv[0], tmpstr);
        if ((infile = fopen(tmpstr, "rb"))==NULL) {
          fprintf(stderr, "Could not open file %s\n", argv[0]);
          goto defaultcase_hbt;
        }
        if ((outfile = fopen(argv[0], "wb"))==NULL) {
          fprintf(stderr, "Could not open file %s\n", argv[1]);
          goto defaultcase_hbt;
        }
        delete [] tmpstr;
        break;
      default: defaultcase_hbt:
        fatalerror(COMMAND_LINE_ERR_HBT, progname);
    }
    if (functoinsert == 0)
      fatalerror(COMMAND_LINE_ERR_HBT, progname);
    outfile_pos = 0;
  } else {
    for (; (--argc) && (**argv == '-'); argv++) {
      options = (CL_Options)0;
      switch (toupper((*argv)[1])) {
        case 'O': *(int *)&options |= (int)OPT_DECOMPILE_OFF; break;
        case 'D': *(int *)&options |= (int)OPT_DEBUG; break;
        default:
          fprintf(stderr, "Unknown flag: %s\n", argv[1]);
          fatalerror(COMMAND_LINE_ERR_HBD, progname);
      }
    }
    switch (argc) {
      case 2:
        if ((outfile = fopen(argv[1], "wb"))==NULL) {
          fprintf(stderr, "Could not open file %s\n", argv[1]);
          goto defaultcase_hbd;
        }
      case 1:
        if ((infile = fopen(argv[0], "rb"))==NULL) {
          fprintf(stderr, "Could not open file %s\n", argv[0]);
          goto defaultcase_hbd;
        }
        break;
      default: defaultcase_hbd:
       fatalerror(COMMAND_LINE_ERR_HBD, progname);
    }
  }
  infile_pos = 0;
}

void Classfile::read() {
  u16 numimports;

  if (get32(infile, &infile_pos) != JAVA_CLASSFILE_MAGIC) fatalerror(NOT_A_CLASS_ERR);

  version.read(this);
  fprintf(stderr, "Classfile version %d.%d\n", version.major_version, version.minor_version);

  imports_count = 0;
  cp.read(this, &imports_count);
  access_flags = get16(infile, &infile_pos);

  this_class = get16(infile, &infile_pos);
  super_class = get16(infile, &infile_pos);
  if (((interfaces = new u16[interfaces_count = get16(infile, &infile_pos)]) == 0) && interfaces_count) memerr();
  u16 i,j;
  for (j = 0, i = interfaces_count; i--;) {
    interfaces[j++] = get16(infile, &infile_pos);
  }
  if (((fields = new field_info_ptr[fields_count = get16(infile, &infile_pos)]) == 0) && fields_count) memerr();
  for (j = 0, i = fields_count; i--;) {
    field_info *fip = fields[j++] = new field_info;
    if (!fip) memerr();
    field_info &fi = *fip;
    fi.isconstant = 0;
    fi.access_flags = get16(infile, &infile_pos);
    fi.name = cp[get16(infile, &infile_pos)]->chp;
    fi.sig = cp[get16(infile, &infile_pos)]->chp;
    for (int l = get16(infile, &infile_pos); l--;) {
      u16 attribute_name_index = get16(infile, &infile_pos);
      u32 attribute_size = get32(infile, &infile_pos);
      char *attribute_name = cp[attribute_name_index]->chp;
      if (!strcmp(attribute_name,"ConstantValue")) {
        if (attribute_size != 2) {
          fprintf(stderr, "Bad size on ConstantValue Attribute - should be 2!\n");
          exit(1);
        } else {
          fi.isconstant = 1;
          fi.constval_index = get16(infile, &infile_pos);
        }
      } else {
        fprintf(stderr, "Skipping Unknown Field Attribute: %s (size %ld)\n", attribute_name, attribute_size);
        for (u32 m = attribute_size; m--;) get8(infile, &infile_pos);
      }
    }
  }
  if (((methods = new method_info_ptr[methods_count = get16(infile, &infile_pos)]) == 0) && methods_count) memerr();
  imports_count += methods_count;
  numimports = 0;
  if ((imports = new char_ptr[imports_count]) == 0) memerr();
  char *tmpstr1, *tmpstr2, *tmpstr;
  int package_name_length;
  tmpstr = cp(this_class)->chp;
  if ((tmpstr1 = strchr(tmpstr,'/')) != 0) {
    int l;
    while (tmpstr1) {
      l = tmpstr1 - tmpstr;
      tmpstr1 = strchr(tmpstr1 + 1, '/');
    }
    if ((package_name = new char[l+1]) == 0) memerr();
    strncpy(package_name, tmpstr, l);
    package_name[l] = '\0';
    if ((this_class_name = new char[strlen(tmpstr + l + 1) + 1]) == 0) memerr();
    strcpy(this_class_name, cp(this_class)->chp = tmpstr + l + 1);
    tmpstr = package_name;
    package_name_length = strlen(tmpstr);
    while ((tmpstr = strchr(tmpstr,'/')) != 0) *tmpstr++ = '.';
  } else {
    package_name = 0;
    package_name_length = 0;
    if ((this_class_name = new char[strlen(tmpstr) + 1]) == 0) memerr();
    strcpy(this_class_name, tmpstr);
  }
  for (i16 l = cp.count(); --l >= 0;) {
    cp_info tmpcpi = *(cp[l]);
    if (tmpcpi.tag == CONSTANT_Class) {
      char **chap = &(cp(l)->chp);
      tmpstr = *chap;
      if (!strncmp(tmpstr, "java/lang/", 10)) *chap = tmpstr + 10;
      else while ((tmpstr = strchr(tmpstr,'/')) != 0) *tmpstr++ = '.';
      tmpstr = *chap;
      if (package_name && !strncmp(tmpstr, package_name, package_name_length)) *chap = tmpstr += package_name_length + 1;
      if ((numimports!=imports_count)&&(tmpstr2 = strrchr(tmpstr,'.')) != 0) {
        imports[numimports++] = tmpstr;
        for (int tint = numimports - 2; tint>=0; tint--) {
          if (!strcmp(imports[tint], tmpstr)) {
            --numimports;
            break;
          }
        }
        *chap = tmpstr2 + 1;
      }
    } else if(tmpcpi.tag == CONSTANT_NameAndType) {
      //      int tmpindex;
      tmpstr = cp[/*tmpindex = */((NameAndType*)tmpcpi.p)->signature_index]->chp;
        char *copytmpstr = tmpstr2 = strdup(tmpstr); if (!copytmpstr) memerr();
      char *srcstr = tmpstr2, *deststr = tmpstr, *tmpstr3, *tmpstr4;
      while ((*deststr++ = *srcstr++) != '\0') {
        if (*(srcstr-1) == 'L') {
          tmpstr3 = strchr(srcstr, ';'); if (!tmpstr3) fatalerror(UNKNOWN_ERR);
          if (!strncmp(srcstr, "java/lang/", 10)) srcstr += 10;
          else {
            tmpstr2 = srcstr;
            while (((tmpstr2 = strchr(tmpstr2,'/')) != 0)&&(tmpstr2 < tmpstr3)) *tmpstr2++ = '.';
          }
          if (package_name && !strncmp(srcstr, package_name, package_name_length)) srcstr += package_name_length + 1;
          if (numimports!=imports_count && (tmpstr4 = strchr(srcstr, '.')) != 0 && tmpstr4 < tmpstr3) {
            while (tmpstr4 != 0 && tmpstr4 < tmpstr3) { tmpstr2 = tmpstr4+1; tmpstr4 = strchr(tmpstr2,'.'); }
            int tint = tmpstr3 - srcstr;
            char *tstr = imports[numimports++] = new char[tint+1];
            if (!tstr) memerr();
            tstr = strncpy(tstr, srcstr, tint);
            tstr[tint] = '\0';
            for (tint = numimports - 2; tint>=0; tint--) {
              if (!strcmp(imports[tint], tstr)) {
                --numimports;
                delete [] tstr;
                break;
              }
            }
            srcstr = tmpstr2;
          }
          while ((*deststr++ = *srcstr++) != ';') ;
        }
      }
    }
  }
  for (j = 0, i = methods_count; i--;) {
    method_info *mip = methods[j++] = new method_info;
    if (!mip) memerr();
    method_info &mi = *mip;
    mi.access_flags = get16(infile, &infile_pos);
    char *tmpstr;
    mi.name = cp[get16(infile, &infile_pos)]->chp;
    mi.sig = cp[get16(infile, &infile_pos)]->chp;

    //    int tmpindex;
    tmpstr = mi.sig;
    char *copytmpstr = tmpstr2 = strdup(tmpstr); if (!copytmpstr) memerr();
    char *srcstr = tmpstr2, *deststr = tmpstr, *tmpstr3, *tmpstr4;
    while ((*deststr++ = *srcstr++) != '\0') {
      if (*(srcstr-1) == 'L') {
        tmpstr3 = strchr(srcstr, ';'); if (!tmpstr3) fatalerror(UNKNOWN_ERR);
        if (!strncmp(srcstr, "java/lang/", 10)) srcstr += 10;
        else {
          tmpstr2 = srcstr;
          while (((tmpstr2 = strchr(tmpstr2,'/')) != 0)&&(tmpstr2 < tmpstr3)) *tmpstr2++ = '.';
        }
        if (package_name && !strncmp(srcstr, package_name, package_name_length)) srcstr += package_name_length + 1;
        if (numimports!=imports_count && (tmpstr4 = strchr(srcstr, '.')) != 0 && tmpstr4 < tmpstr3) {
          while (tmpstr4 != 0 && tmpstr4 < tmpstr3) { tmpstr2 = tmpstr4+1; tmpstr4 = strchr(tmpstr2,'.'); }
          int tint = tmpstr3 - srcstr;
          char *tstr = imports[numimports++] = new char[tint+1];
          if (!tstr) memerr();
          tstr = strncpy(tstr, srcstr, tint);
          tstr[tint] = '\0';
          for (tint = numimports - 2; tint>=0; tint--) {
            if (!strcmp(imports[tint], tstr)) {
              --numimports;
              delete [] tstr;
              break;
            }
          }
          srcstr = tmpstr2;
        }
      }
    }
    mi.num_throws = 0;
    mi.local_variable_table_length = 0;
    mi.line_number_table_length = 0;
    for (int l = get16(infile, &infile_pos); l--;) {
      u16 attribute_name_index = get16(infile, &infile_pos);
      u32 attribute_size = get32(infile, &infile_pos);
      char *attribute_name = cp[attribute_name_index]->chp;
      if (!strcmp(attribute_name,"Code")) {
        mi.max_stack = (u8)get16(infile, &infile_pos);
        mi.max_locals = (u8)get16(infile, &infile_pos);
        if ((mi.code = new u8[mi.code_length = get32(infile, &infile_pos)]) == 0) memerr();
        getstr(mi.code, mi.code_length, infile);
        if (((mi.exception_table = new ExceptionTableEntry[mi.exception_table_length = get16(infile, &infile_pos)]) == 0) && mi.exception_table_length) memerr();
        for (u16 n = 0, m = mi.exception_table_length; m--;) {
          mi.exception_table[n].tag = TRY;
          mi.exception_table[n].start_pc = get16(infile, &infile_pos);
          mi.exception_table[n].end_pc = get16(infile, &infile_pos);
          mi.exception_table[n].handler_pc = get16(infile, &infile_pos);
          mi.exception_table[n++].catch_type = get16(infile, &infile_pos);
        }
        //        getstr(mi.exception_table, mi.exception_table_length << 3, infile);
        for (int l2 = get16(infile, &infile_pos); l2--;) {
          u16 attribute_name_index2 = get16(infile, &infile_pos);
          u32 attribute_size2 = get32(infile, &infile_pos);
          char *attribute_name2 = cp[attribute_name_index2]->chp;
          if (!strcmp(attribute_name2,"LineNumberTable")) {
            if ((mi.line_number_table = new LineNumberTableEntry[mi.line_number_table_length = get16(infile, &infile_pos)]) == 0) memerr();
            getstr(mi.line_number_table, mi.line_number_table_length << 2, infile);
          } else if (!strcmp(attribute_name2,"LocalVariableTable")) {
            if ((mi.local_variable_table = new LocalVariableTableEntry[mi.local_variable_table_length = get16(infile, &infile_pos)]) == 0) memerr();
            getstr(mi.local_variable_table, mi.local_variable_table_length * 10, infile);
            if ((mi.local_names = new char*[2*mi.local_variable_table_length]) == 0) memerr();
            int o;
            for (o = mi.local_variable_table_length; o--;) {
              char *tmpstr = cp[mi.local_variable_table[o].name_index]->chp;
              if ((mi.local_names[mi.local_variable_table[o].slot] = new char[strlen(tmpstr) + 1]) == 0) memerr();
              strcpy(mi.local_names[mi.local_variable_table[o].slot], tmpstr);
            }
            if ((mi.local_sigs = new char*[2*mi.local_variable_table_length]) == 0) memerr();
            for (o = mi.local_variable_table_length; o--;) {
              char *tmpstr = cp[mi.local_variable_table[o].signature_index]->chp;
              if ((mi.local_sigs[mi.local_variable_table[o].slot] = new char[strlen(tmpstr) + 1]) == 0) memerr();
              strcpy(mi.local_sigs[mi.local_variable_table[o].slot], tmpstr);
            }
          } else {
            fprintf(stderr, "Skipping Unknown Code Attribute: %s (size %ld)\n", attribute_name2, attribute_size2);
            for (int m = attribute_size2; m--;) get8(infile, &infile_pos);
          }
        }
      } else if (!strcmp(attribute_name,"Exceptions")) {
        int *tmpintptr;
        if ((tmpintptr = mi.throws = new int[mi.num_throws = get16(infile, &infile_pos)]) == 0) memerr();
        for (int m = mi.num_throws;m--;) *tmpintptr++ = get16(infile, &infile_pos); //*tmpstr++ = constant_pool[constant_pool[get16(infile)].i].cp;
      } else {
        fprintf(stderr, "Skipping Unknown Method Attribute: %s (size %ld)\n", attribute_name, attribute_size);
        //      char *ti = new int[attribute_size], *tip = ti;
        for (unsigned int m = 0; m++!=attribute_size;) printf("%02x%c", /**tip++ =*/ get8(infile, &infile_pos), (m%8)?' ':(m%16?'\n':'\t'));
        printf("\n");
        //      int tc = ti[0]<<8+ti[1]
      }
    }
  }
  imports_count = numimports;
  for (j = 0, i = get16(infile, &infile_pos); i--;) {
    u16 attribute_name_index = get16(infile, &infile_pos);
    u32 attribute_size = get32(infile, &infile_pos);
    char *attribute_name = cp[attribute_name_index]->chp;
    if (!strcmp(attribute_name,"SourceFile")) {
      if (attribute_size != 2) {
        fprintf(stderr, "Bad size on SourceFile Attribute - should be 2!\n");
        exit(1);
      } else {
        char *tmpstr = cp[get16(infile, &infile_pos)]->chp;
        if ((source_name = new char[strlen(tmpstr) + 1]) == 0) memerr();
        strcpy(source_name, tmpstr);
      }
    } else {
      fprintf(stderr, "Skipping Unknown Attribute: %s (size %ld)\n", attribute_name, attribute_size);
      for (u32 k = attribute_size; k--;) get8(infile, &infile_pos);
    }
  }
}

void Classfile::print() {
  int i,j;
  fprintf(stderr, "Compiled from %s\n", source_name);
  fprintf(outfile, "/*\n** Compiled from %s - COPYRIGHT UNKNOWN.\n**\n"
      "** Decompiled using the HomeBrew Decompiler\n"
      "** Copyright (c) 1994-2003 Widget (aka Pete Ryland).\n"
      "** Available under GPL from http://pdr.cx/hbd/\n*/\n\n", source_name);
  if (package_name) fprintf(outfile, "package %s;\n\n", package_name);
  char **strptr = imports;
  for (i = imports_count; i--;) {
    fprintf(outfile, "import %s;\n", *strptr++);
  }
  fprintf(outfile, "\n");
  char *tmpstr = new char[access_flags.strlen() + 1];
  fprintf(outfile, "%sclass %s ", access_flags.toString(tmpstr), this_class_name);
  delete [] tmpstr;
  if (super_class) {
    if (!strcmp(tmpstr = cp(super_class)->chp, "Object")) {
      super_class_name = "Object";
    } else {
      if ((super_class_name = new char[strlen(tmpstr) + 1]) == 0) memerr();
      strcpy(super_class_name, tmpstr);
      fprintf(outfile, "extends %s ", super_class_name);
    }
  }
  if (interfaces_count) {
    fprintf(outfile, "implements ");
    for (j = 0, i = interfaces_count - 1; i--;) {
      fprintf(outfile, "%s, ", cp(interfaces[j])->chp);
    }
    fprintf(outfile, "%s ", cp(interfaces[j])->chp);
  }
  fprintf(outfile, "{");
  for (j = 0, i = fields_count; i--;) {
    field_info &fi = *(fields[j++]);
    tmpstr = new char[fi.access_flags.strlen() + 1];
    fprintf(outfile, "\n  %s", fi.access_flags.toString(tmpstr));
    delete [] tmpstr;
    tmpstr = fi.sig;
    printsigname(this, outfile, tmpstr, fi.name, 0);
    if (fi.isconstant) {
      fprintf(outfile, " = ");
      char *chptr = fi.sig;
      switch(*chptr++) {
        case SIGNATURE_INT:
          fprintf(outfile, "0x%lX", cp[fi.constval_index]->i);
          break;
        case SIGNATURE_LONG:
          if(cp[fi.constval_index]->i) {
            fprintf(outfile, "0x%lX%08lXL", cp[fi.constval_index]->i,
                cp[(u16)((i16)fi.constval_index + 1)]->i);
          } else {
            fprintf(outfile, "0x%lXL", cp[(u16)((i16)fi.constval_index + 1)]->i);
          }
          break;
        case SIGNATURE_FLOAT:
          fprintf(outfile, "%#.100Gf", cp[fi.constval_index]->f);
          break;
        case SIGNATURE_DOUBLE:
          fprintf(outfile, "%#.100Gd",(float)*(double*)(cp[fi.constval_index]->i));
          break;
        default:
          fprintf(stderr, "Bad type for constant\n");
      }
    }
    fprintf(outfile, ";");
  }
  for (j = 0, i = methods_count; i--;) {
    if (decompileblock(this, methods[j++])) fprintf(outfile, "/* Decompilation Error.  Continuing... */");
  }
  fprintf(outfile, "\n}");
  return;
}

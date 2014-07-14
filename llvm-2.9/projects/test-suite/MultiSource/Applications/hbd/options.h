/* options.h */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#ifdef debug_optional
#define DEBUG_ON 1
#else
#define DEBUG_ON 0
#endif

/* The command-line options */
enum CL_Options {
  OPT_DEBUG = DEBUG_ON,
  OPT_DECOMPILE_OFF = 2
};

//void parse_cmdline(FILE **infile_ptr, FILE **outfile_ptr,
//                CL_Options *options, int argc, char **argv);

#endif

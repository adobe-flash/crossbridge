/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON

namespace halfmoon {

//debug routines that are potentially useful throughout halfmoon

/**
 * if someone debugging JIT sets this env variable we interpret it as
 * a list of instruction names to generate calls to stubs for rather than
 * properly compile.
 */
const bool debug_stubs   = VMPI_getenv("DEBUG_STUB_LIST") != NULL;

/**
 * print debug contributions to the list of interested env variables
 */
void debugWelcome() {
  printf("Halfmoon Debug environment:\n");
  printf("  DEBUG_STUB_LIST %d  compile calls to listed stubs instead of generate code\n", debug_stubs);
}

bool stub_out[HR_MAX];

/**
 *
 * @param tok query string.. does it name an instruction?
 * @param found_kind OUT param. InstrKind of found instruction
 * @return return true if instr_attrs contains an instruction named tok.
 */
static bool isInInstrAttrs(const char *tok, InstrKind &found_kind) {
  for(int ix=0; ix< HR_MAX; ix++) {
    if (strcmp(instr_attrs[ix].name, tok) == 0) {
      //printf("initStubList skip list contains %-15s, stub_out[%d] = true\n", tok,ix);fflush(NULL);
      found_kind = InstrKind(ix);
      return true;
    }
  }
  return false;
}


/**
 *
 * @param env_var list, presumably from an environement variable, to tokensize
 * @param separator_chars string containing strings that demarkate tokens
 * @param bitmap out parm -- the bitmap that is being initialized
 *
 * tokenize env_var, look for instruction names, and set corresponding  bits.
 */
static void initStubList(const char *env_var, const char *separator_chars, bool bitmap[] )
{
  const unsigned int BUF_SIZE = 16384; //say 1000 instruction of 10 chars each
  char buf[BUF_SIZE];
  AvmAssert( strlen(env_var) < BUF_SIZE );
  strncpy(buf, env_var, BUF_SIZE);
  char *bp = buf; //the skip list
  char *tok;
//  char *saveptr = NULL; //carries strtok_r state
//  tok = strtok_r(bp, separator_chars, &saveptr); //initialize strtok
  tok = strtok(bp, separator_chars); //initialize strtok
  while ( tok != NULL) {
    //printf("%s skip list finds token: %s\n", env_var_name, tok);fflush(NULL);
    AvmAssert (strlen(tok) != 0);
    InstrKind ix = InstrKind(-1); //something invalid
    if (isInInstrAttrs(tok, ix)) {
       bitmap[ix] = true;
    }else{
      //these are debug options, so an assert shouldn't shock or awe.
      AvmAssert(false &&
          "instr named in debug env skip list doesn't exist in instr_attrs");
    }
//    tok = strtok_r(NULL, separator_chars, &saveptr);
    tok = strtok(NULL, separator_chars);
  }//while tok
}

/**
 * init the debug stuff. intended to be called early from somewhere in hm-main.
 */
void debugInit() {
  if(debug_stubs) {
    const char *env_var = VMPI_getenv("DEBUG_STUB_LIST");
    if (env_var == NULL) {
      return;
    }
    initStubList(env_var, " ;,\t\n", stub_out );
  }
}

/**
 * Originally written so that the inliner could be made to skip inlining certain candidates.
 * (This helps debugging as one can narrow down which inlining decision is causing problems)
 * @param env_var_name value something like ;foo;bar;baz;
 * @param query_string
 * @return true if the env variable named by env_var_name contains query_string
 */
bool debugIsInSkiplist(const char *env_var_name, String *query_string ){
  const char *env_var = VMPI_getenv(env_var_name);
  if ( env_var == NULL ) {
    return false;
  }
  char buf[1024];
  strncpy(buf, env_var, 1024);
  char *bp = buf; //the skip list
  char *tok;
  const char *separator_chars = " ;,\t\n";
//  char *saveptr = NULL; //carries strtok_r state
//  tok = strtok_r(bp, separator_chars, &saveptr); //initialize strtok
  tok = strtok(bp, separator_chars); //initialize strtok
  while ( tok != NULL) {
    AvmAssert (strlen(tok) != 0);
    if ( query_string->equalsLatin1(tok,(int32_t)strlen(tok)) ) {
      //printf("skip list returning true for %s\n", tok);     fflush(NULL);
      return true;
    }
//    tok = strtok_r(NULL, separator_chars, &saveptr);
    tok = strtok(NULL, separator_chars);
  }//while tok
  return false;
}


}//namespace halfmoon
#endif // VMCFG_HALFMOON

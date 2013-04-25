/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Debug assistance code.

#ifndef HM_DEBUG_H_
#define HM_DEBUG_H_

namespace halfmoon {

//debug routines that are potentially useful throughout halfmoon

extern const bool debug_stubs;      // call stubs rather than generate code
extern bool stub_out[];             // call stubs rather than generate code

/**
 * print available debug flags
 */
void debugWelcome();

/**
 * initialize debug tools
 */
void debugInit();

/**
 * Originally written so that the inliner could be made to skip inlining certain candidates.
 * idea is that user set env var to contain list of ; separated names. this function
 * @param env_var_name value something like ;foo;bar;baz;
 * @param query_string
 * @return true if tokens within env variable named by env_var_name contains query_string.
 * idea is that user sets env var to contain list of ; separated names. this function returns true
 * if query_string is one of those tokens.
 */
extern bool debugIsInSkiplist(const char *env_var_name, String *query_string );

} //namespace halfmoon

#endif /* HM_DEBUG_H_ */

/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "VMPI.h"

/*************************************************************************/
/******************************* Debugging *******************************/
/*************************************************************************/

// old SDK can not see these? comment out for now
//#include "e32def.h"
//#include "e32std.h"

void VMPI_debugLog(const char* /*message*/)
{
//  VMPI_log( message ); //also log to standard output
}

void VMPI_debugBreak()
{
//  ASSERT( false );
}

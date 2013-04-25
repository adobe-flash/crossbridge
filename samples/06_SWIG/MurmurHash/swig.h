/*
** ADOBE SYSTEMS INCORPORATED
** Copyright 2012 Adobe Systems Incorporated
** All Rights Reserved.
**
** NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the
** terms of the Adobe license agreement accompanying it.  If you have received this file from a
** source other than Adobe, then your use, modification, or distribution of it requires the prior
** written permission of Adobe.
*/

#ifdef SWIG
%{
#include <stdint.h>   
#include "MurmurHash3.h"
%}

// This directive tells swig to pull in type information from its library
// for the types declared in stdint.h.
%include "stdint.i"

// This include causes swig to parse and emit wrappers for the functions
// declared in the header file.
%include "MurmurHash3.h"
#endif


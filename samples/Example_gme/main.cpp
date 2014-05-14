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
#include "AS3/AS3.h"

int main()
{
    // We still need a main function for the SWC. this function must be called
    // so that all the static init code is executed before any library functions
    // are used.
    //
    // The main function for a library must throw an exception so that it does
    // not return normally. Returning normally would cause the static
    // destructors to be executed leaving the library in an unuseable state.

    AS3_GoAsync();
}

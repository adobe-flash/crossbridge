/*
** ADOBE SYSTEMS INCORPORATED
** Copyright 2012 Adobe Systems Incorporated. All Rights Reserved.
**
** NOTICE:  Adobe permits you to use, modify, and distribute this file in
** accordance with the terms of the Adobe license agreement accompanying it.
** If you have received this file from a source other than Adobe, then your use,
** modification, or distribution of it requires the prior written permission of Adobe.
*/

#ifndef AS3xx_H
#define AS3xx_H

#include <string.h>
#include <stdarg.h>
#include <string>

namespace AS3
{
/** Types from this namespace wrap objects in the current worker
*/
namespace local
{
#define AS3_LOCAL
#define AS3_NS AS3::local
#include <AS3/AS3++.impl.h>
#undef AS3_NS
#undef AS3_LOCAL
}

/** Types from this namespace wrap objects on the main UI worker
*/
namespace ui
{
#define AS3_UI
#define AS3_NS AS3::ui
#include <AS3/AS3++.impl.h>
#undef AS3_NS
#undef AS3_UI
}

/**
* Turns a null-terminated char* into a std::string and frees the memory it used in the process.
*/
inline std::string sz2stringAndFree(const char *s)
{
    std::string result(s);
    free((void *)s);
    return result;
}
};

#endif

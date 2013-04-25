/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmshell_DescribeTypeClass__
#define __avmshell_DescribeTypeClass__


namespace avmplus
{
    class DescribeTypeClass
    {
    public:
        static Atom describeTypeJSON(ScriptObject* self, Atom o, uint32_t flags);
        static Stringp getQualifiedClassName(ScriptObject* self, Atom v);
        static Stringp getQualifiedSuperclassName(ScriptObject* self, Atom v);
    };
}

#endif /* __avmshell_DescribeTypeClass__ */

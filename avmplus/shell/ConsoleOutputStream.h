/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmshell_ConsoleOutputStream__
#define __avmshell_ConsoleOutputStream__


namespace avmshell
{
    /**
     * ConsoleOutputStream is a subclass of OutputStream
     * that writes output to the standard output.
     *
     * This is used for routing print() statements and
     * exception messages to stdout in the AVM+
     * command-line shell.
     */
    class ConsoleOutputStream : public avmplus::GCOutputStream
    {
    public:
        ConsoleOutputStream(avmplus::AvmCore* core) : buffer(core) {}
        void write(const char* utf8);
        void writeN(const char* utf8, size_t count);
    private:
        avmplus::StringBuffer buffer;
    };
}

#endif /* __avmshell_ConsoleOutputStream__ */

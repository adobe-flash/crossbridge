/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmshell_FileClass__
#define __avmshell_FileClass__


namespace avmshell
{
    /**
     * A simple class that has some native methods.
     * Included as an example for writers of native methods,
     * and also to provide some useful QA instrumentation.
     */
    class FileClass : public avmplus::ClassClosure
    {
    public:
        FileClass(avmplus::VTable* cvtable);

        /**
         * Implementation of File.exists
         * AS usage: exists = File.exists(filename);
         * Tests whether file "filename" exists.
         */
        bool exists(avmplus::Stringp filename);

        /**
         * Implementation of File.read
         * AS usage: data = File.read(filename);
         * Reads the file "filename" into memory and returns
         * it as a string
         */
        avmplus::Stringp read(avmplus::Stringp filename);

        /**
         * Implementation of File.write
         * AS usage: File.write(filename, data);
         * Writes the text "data" to the file "filename"
         */
        void write(avmplus::Stringp filename, avmplus::Stringp data);

        avmplus::ByteArrayObject* readByteArray(avmplus::Stringp filename);
        bool writeByteArray(avmplus::Stringp filename, avmplus::ByteArrayObject* bytes);


        DECLARE_SLOTS_FileClass;
    };
}

#endif /* __avmshell_FileClass__ */

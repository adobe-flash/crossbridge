/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmshell_FileInputStream__
#define __avmshell_FileInputStream__


namespace avmshell
{
    class File;

    /**
     * InputStream is an abstract base class for input streams.
     * Concrete subclasses must implement the available and read
     * methods to do appropriate read operations.
     */
    class InputStream
    {
    public:
        virtual ~InputStream() {}
        virtual int64_t available() = 0;
        virtual size_t read(void *buffer, size_t count) = 0;
    };

    /**
     * FileInputStream is a concrete subclass of InputStream
     * that implements file input.
     */
    class FileInputStream : public InputStream
    {
    public:
        FileInputStream(const char *filename);

        bool valid() const;
        ~FileInputStream();
        int64_t available();
        int64_t length() const { return len; }
        void seek(int64_t offset);
        size_t read(void *buffer, size_t count);

    private:
        File *file;
        int64_t len;
    };
}

#endif /* __avmshell_FileInputStream__ */

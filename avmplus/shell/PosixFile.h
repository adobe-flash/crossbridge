/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmshell_PosixFile__
#define __avmshell_PosixFile__

#include "File.h"

namespace avmshell
{
    class PosixFile : public File
    {
    public:
        PosixFile() : file(0) {}
        virtual ~PosixFile();

        virtual bool open(const char* filename, File::OpenAttribute flags);
        virtual void close();
        virtual size_t read(void* buffer, size_t bytesToRead);
        virtual size_t write(const void* buffer, size_t bytesToWrite);
        virtual int64_t getPosition() const;
        virtual bool setPosition(int64_t pos);
        virtual int64_t size() const;
        virtual bool isEOF() const;

    private:
        FILE* file;
    };
}

#endif /* __avmshell_PosixFile__ */

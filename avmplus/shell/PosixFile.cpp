/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "PosixFile.h"

namespace avmshell
{
    PosixFile::~PosixFile()
    {
        if(file)
        {
            fclose(file);
        }
    }

    bool PosixFile::open(const char* filename, File::OpenAttribute flags)
    {
        const char* fileAttributes[] = { "r", "w", "a", "rb", "wb", "ab" };

        file = fopen(filename, fileAttributes[flags] );

        return file != 0;
    }

    void PosixFile::close()
    {
        fclose(file);
        file = 0;
    }

    size_t PosixFile::read(void* buffer, size_t bytesToRead)
    {
        return fread(buffer, 1, bytesToRead, file);
    }

    size_t PosixFile::write(const void* buffer, size_t bytesToWrite)
    {
        return fwrite(buffer, 1, bytesToWrite, file);
    }

    int64_t PosixFile::getPosition() const
    {
        return ftell(file);
    }

    bool PosixFile::setPosition(int64_t pos)
    {
        return fseek(file, pos, SEEK_SET) == 0;
    }

    int64_t PosixFile::size() const
    {
        fseek(file, 0L, SEEK_END);
        int64_t pos = ftell(file);
        fseek (file, 0L, SEEK_SET);

        return pos;
    }

    bool PosixFile::isEOF() const
    {
        return feof(file) != 0;
    }

}

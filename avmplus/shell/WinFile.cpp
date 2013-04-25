/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "WinFile.h"

namespace avmshell
{
    WinFile::~WinFile()
    {
        if(file)
        {
            fclose(file);
        }
    }

    bool WinFile::open(const char* filename, File::OpenAttribute flags)
    {
        const char* fileAttributes[] = { "r", "w", "a", "rb", "wb", "ab" };

        file = fopen(filename, fileAttributes[flags] );

        return file != NULL;
    }

    void WinFile::close()
    {
        fclose(file);
        file = NULL;
    }

    size_t WinFile::read(void* buffer, size_t bytesToRead)
    {
        return fread(buffer, 1, bytesToRead, file);
    }

    size_t WinFile::write(const void* buffer, size_t bytesToWrite)
    {
        return fwrite(buffer, 1, bytesToWrite, file);
    }

    int64_t WinFile::getPosition() const
    {
    #if defined (UNDER_CE)
        return ftell(file);
    #else
        return _ftelli64(file);
    #endif
    }

    bool WinFile::setPosition(int64_t pos)
    {
    #if defined (UNDER_CE)
        return fseek(file, (long)pos, SEEK_SET) == 0;
    #else
        return _fseeki64(file, pos, SEEK_SET) == 0;
    #endif
    }

    int64_t WinFile::size() const
    {
    #if defined (UNDER_CE)
        fseek(file, 0L, SEEK_END);
        int64_t pos = ftell(file);
        fseek (file, 0L, SEEK_SET);
    #else
        _fseeki64(file, 0LL, SEEK_END);
        int64_t pos = _ftelli64(file);
        _fseeki64 (file, 0LL, SEEK_SET);
    #endif

        return pos;
    }

    bool WinFile::isEOF() const
    {
        return feof(file) != 0;
    }
}

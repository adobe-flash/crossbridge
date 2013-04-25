/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "PosixPartialPlatform.h"
#include "PosixFile.h"

namespace avmshell
{
    void PosixPartialPlatform::exit(int code)
    {
        ::exit(code);
    }

    File* PosixPartialPlatform::createFile()
    {
        return mmfx_new( PosixFile() );
    }

    void PosixPartialPlatform::destroyFile(File* file)
    {
        mmfx_delete( file );
    }

    void PosixPartialPlatform::initializeLogging(const char* filename)
    {
        FILE *f = freopen(filename, "w", stdout);
        if (!f)
            avmplus::AvmLog("freopen %s failed.\n",filename);
    }

    int PosixPartialPlatform::logMessage(const char* message)
    {
        return fprintf(stdout, "%s", message);
    }

    char* PosixPartialPlatform::getUserInput(char* buffer, int bufferSize)
    {
        fflush(stdout);
        if (fgets(buffer, bufferSize, stdin) == NULL) {
            // EOF or error
            *buffer = 0;
            return NULL;
        }
        return buffer;
    }
}

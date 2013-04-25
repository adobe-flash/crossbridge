/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmshell.h"

namespace avmshell
{
    FileInputStream::FileInputStream(const char *filename)
    {
        file = Platform::GetInstance()->createFile();
        if(file)
        {
            if(file->open(filename, File::OPEN_READ_BINARY))
            {
                len = file->size();
            }
            else
            {
                Platform::GetInstance()->destroyFile(file);
                file = NULL;
            }
        }
    }

    bool FileInputStream::valid() const
    {
        return file != NULL;
    }

    FileInputStream::~FileInputStream()
    {
        Platform::GetInstance()->destroyFile(file);
    }

    void FileInputStream::seek(int64_t offset)
    {
        file->setPosition(offset);
    }

    int64_t FileInputStream::available()
    {
        int64_t pos = file->getPosition();
        if(pos >= 0 )
        {
            return len - pos;
        }
        return 0;
    }

    size_t FileInputStream::read(void *buffer,
                              size_t count)
    {
        return file->read(buffer, count);
    }
}

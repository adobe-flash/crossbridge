/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmshell_File__
#define __avmshell_File__

#include "avmshell.h"

namespace avmshell
{
    /**
    * Abstract base class for performing platform-specific file operations
    * This class needs to be derived and its methods implemented by platform to enable the shell to perform file I/O
    */
    class File
    {
    public:
        /**
        *  An enum defining the open modes for a file
        */
        typedef enum
        {
            OPEN_READ = 0,
            OPEN_WRITE,
            OPEN_APPEND,
            OPEN_READ_BINARY,
            OPEN_WRITE_BINARY,
            OPEN_APPEND_BINARY
        }OpenAttribute;

        /**
        * Virtual Destructor
        */
        virtual ~File() {}

        /**
        * Method to open a file on platform's file system
        * The implementation should open the file with the given name and attributes for open mode
        * @param filename represents the path and name of the file to be opened.  filename is UTF-8
        * @param flags one of the values from OpenAttribute enumeration indicating the  mode of file
        * @return true if file was opened successfully, false otherwise
        * @see enum OpenAttribute
        */
        virtual bool open(const char* filename, OpenAttribute flags) = 0;

        /**
        * Method to close an opened file.
        * If the file was not opened this method should do nothing
        * @return none
        */
        virtual void close() = 0;

        /**
        * Method to read a chunk of data from the file
        * This method can return 0 in event of a read error or if the file marker was at end-of-file.
        * So a return value of zero should not be treated as an error condition by default.
        * Instead method isEOF() should be used to detect a end-of-file .
        * @param buffer buffer to read the data into
        * @param bytesToRead number of bytes to read
        * @return number of bytes actually read.  0 or less than bytesToRead in case of error or end-of-file was reached
        */
        virtual size_t read(void* buffer, size_t bytesToRead) = 0;

        /**
        * Method to write a chunk of data to the file
        * @param buffer buffer containing the data to write to file
        * @param bytesToWrite number of bytes to write from the buffer
        * @return number of bytes actually written.  If this value is less than bytesToWrite then that indicates an error
        */
        virtual size_t write(const void* buffer, size_t bytesToWrite) = 0;

        /**
        * Method to get the current position of the file head
        * @return returns the current byte position of the file head, -1 if an error occurred
        */
        virtual int64_t getPosition() const = 0;

        /**
        * Method to move the file head to a certain position
        * @param pos number indicating the byte position in file to move the file head to
        * @return returns true if the operation was successful, false if an error occurred
        */
        virtual bool setPosition(int64_t pos) = 0;

        /**
        * Method to get the size of file
        * @return size of the file, -1 if an error occurred
        */
        virtual int64_t size() const = 0;

        /**
        * Method to check if the end-of-file has reached
        * @return true if end if reached, false otherwise
        */
        virtual bool isEOF() const = 0;
    };
}

#endif /* __avmshell_File__ */

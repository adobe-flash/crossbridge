// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

package com.adobe.flascc.vfs {

	/**
	* This interface is used by the VFS for special files that must be implemented via code
	* rather than a fixed ByteArray. The default Console implementation uses the ISpecialFile
	* interface to handle read/write requests to /dev/tty which corresponds with C/C++ code
	* reading/writting to stdin/stdout.
	*
	* You might also use this interface to implement other special files like "/dev/null" or "/dev/random".
	*/
	public interface ISpecialFile {
		/**
		* This matches the signature of read from the IKernel interface.
		* @param fileDescriptor The file descriptor being read from
		* @param bufPtr A Pointer to the buffer you are expected to fill with data for this read.
		* @param nbyte The size of the buffer pointed to by bufPtr
		* @param errnoPtr A pointer to the location of the errno global variable
		* @return an integer indicating the success or failure of the syscall, see the BSD documentation for expected values
		*/
		function read(fileDescriptor:int, bufPtr:int, nbyte:int, errnoPtr:int):int

		/**
		* This matches the signature of write from the IKernel interface.
		* @param fileDescriptor The file descriptor being written to
		* @param bufPtr A Pointer to the buffer containing data to be written to this file descriptor.
		* @param nbyte The size of the buffer pointed to by bufPtr
		* @param errnoPtr A pointer to the location of the errno global variable
		* @return an integer indicating the success or failure of the syscall, see the BSD documentation for expected values
		*/
		function write(fileDescriptor:int, bufPtr:int, nbyte:int, errnoPtr:int):int

		/**
		* This matches the signature of fcntl from the IKernel interface.
		* @param fileDescriptor The file descriptor being manipulated
		* @param cmd An fcntl command
		* @param data An argument for the given command
		* @param errnoPtr A pointer to the location of the errno global variable
		* @return an integer indicating the success or failure of the syscall, see the BSD documentation for expected values
		*/
		function fcntl(fileDescriptor:int, cmd:int, data:int, errnoPtr:int):int

		/**
		* This matches the signature of ioctl from the IKernel interface.
		* @param fileDescriptor The file descriptor being manipulated
		* @param request An ioctl request
		* @param data An argument for the given command
		* @param errnoPtr A pointer to the location of the errno global variable
		* @return an integer indicating the success or failure of the syscall, see the BSD documentation for expected values
		*/
		function ioctl(fileDescriptor:int, request:int, data:int, errnoPtr:int):int
	}
}


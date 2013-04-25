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

/* This class does not depend on the rest of flascc, so it can be safely
 * used in isolation, such as in a preloader.
 */
package com.adobe.flascc.vfs {
    import flash.utils.ByteArray;

    /**
     * This interface describes how an underlying data store interacts with
     * the flascc virtual filesystem. Implement this interface to customize
     * the backing store for the VFS. Backing stores are added to the VFS
     * via the IBackingStore.addBackingStore method.
     */ 
    public interface IBackingStore {
        /**
         * Adds a file into the filesystem with a given path and contents.
         * Ownership of the ByteArray is assumed to transfer to the BackingStore with this method.
         * 
         * @param path The path to the file. 
         * @param data The contents of the file.
         */
        function addFile(path:String, data:ByteArray):void;

        /**
         * Deletes a file from the filesystem.
         * 
         * @param path The path to the file. 
         */
        function deleteFile(path:String):void;

        /**
         * Retrieves all of the files paths contained in this BackingStore.
         *
         * @return A Vector of all file paths contained in this BackingStore.
         */
        function getPaths():Vector.<String>;

        /**
         * Retrieves the contents of a file from this BackingStore.
         * 
         * @param path The path to the file. 
         *
         * @return The contents of the file.
         */
        function getFile(path:String):ByteArray;

        /**
         * Determines whether a path refers to a file or directory.
         * 
         * @param path The path to be checked.  
         *
         * @return true if the path refers to a file or a directory, false otherwise.
         */
        function pathExists(path:String):Boolean;
        
        /**
         * Determines whether a path refers to a directory.
         * 
         * @param path The path to be checked.  
         *
         * @return true if the path refers to a directory, false otherwise.
         */
        function isDirectory(path:String):Boolean;

        /**
         * Writes any changes to the underlying store.
         */
        function flush():void; 

        /**
         * Determines whether the filesystem may be changed.
         * 
         * @return true if the fileystem is read only, false otherwise.
         */
        function get readOnly():Boolean;
    };
}

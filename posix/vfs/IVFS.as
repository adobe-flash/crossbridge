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
    import flash.errors.EOFError;
    import flash.utils.ByteArray;
    import com.adobe.flascc.vfs.IBackingStore;
    import com.adobe.flascc.vfs.ISpecialFile;

    /**
     * This class represents open files available to an flascc application. It should be used by implementations of the
     * IKernel interface but not by end-user code.
     */
    public class FileHandle {
        
        /**
        * @private
        */
        public function FileHandle(
                __backingStore:IBackingStore = null,
                __backingStoreRelativePath:String = null,
                __bytes:ByteArray = null,
                __callback:ISpecialFile = null,
                __readable:Boolean = true,
                __writeable:Boolean = false,
                __appending:Boolean = false,
                __isDirectory:Boolean = false,
                __path:String = null,
                __position:uint = 0
        )
        {
            _backingStore = __backingStore
            _backingStoreRelativePath = __backingStoreRelativePath
            _bytes = __bytes
            _callback = __callback
            readable = __readable
            writeable = __writeable
            appending = __appending
            _isDirectory = __isDirectory
            _path = __path
            position = __position
        }

        /**
        * @private
        */
        public static function makeSpecialFile(specialFile:ISpecialFile):FileHandle
        {
            var fh:FileHandle = new FileHandle()
            fh.writeable = true
            fh.readable = true;
            fh._callback = specialFile
            return fh;
        }

        /**
        * @private
        */
        public static function makeRegularFile(__path:String, __backingStoreRelativePath:String, __backingStore:IBackingStore, __bytes:ByteArray, __isDirectory:Boolean):FileHandle
        {
            var fh:FileHandle = new FileHandle()
            fh._path = __path
            fh._backingStore = __backingStore
            fh._backingStoreRelativePath = __backingStoreRelativePath
            fh._bytes = __bytes
            fh._isDirectory = __isDirectory
            fh.writeable = !__backingStore.readOnly;
            fh.readable = true;
            fh.position = 0;
            return fh;
        }
        
        /**
        * The BackingStore that owns this FDEntry.
        */
        public function get backingStore():IBackingStore { return _backingStore }
        private var _backingStore:IBackingStore = null;

        /**
        * The relative path to the underlying file in the VFS.
        */
        public function get backingStoreRelativePath():String { return _backingStoreRelativePath }
        private var _backingStoreRelativePath:String = null;


        /**
        * If this file descriptor refers to a normal file this will contain the contents of the file.
        */
        public function get bytes():ByteArray { return _bytes }
        private var _bytes:ByteArray = null;

        /**
        * If this file descriptor refers to a special file this references an object implementing the ISpecialFile interface that will be used to handle read/write requests.
        */
        public function get callback():ISpecialFile { return _callback }
        private var _callback:ISpecialFile = null;

        /**
        * True if this file descriptor was opened with O_RDWR or if it was not opened as O_WRONLY.
        */
        public var readable:Boolean;

        /**
        * True if this file descriptor was opened with O_WRONLY or O_RDWR.
        */
        public var writeable:Boolean;

        /**
        * True if this file descriptor was opened with O_APPEND.
        */
        public var appending:Boolean;

        /**
        * True if this file descriptor referes to a directory.
        */
        public function get isDirectory():Boolean { return _isDirectory }
        private var _isDirectory:Boolean = false;

        /**
        * The full path to the file referenced by this file descriptor
        */
        public function get path():String { return _path }
        private var _path:String = null;

        /**
        * The position within the file that read/write operations will occur at.
        */
        public var position:uint;
    }

    /**
     * This enum class contains the results determined by the checkPath method.
     * It should be used by implemetations of the IKernel interface but not by end-user code.
     */
    public class CheckPath {

        public static const PATH_VALID:String = "pathValid";

        public static const PATH_COMPONENT_DOES_NOT_EXIST:String = "pathComponentDoesNotExist";

        public static const PATH_COMPONENT_IS_NOT_DIRECTORY:String = "pathComponentIsNotDirectory";
    }

    /**
     * IVFS describes the interface to the flascc virtual filesystem, which
     * controls how the filesystem appears to an flascc application. It is
     * possible for user code to implement this interface, though there
     * is a default implementation, DefaultVFS. The CModule.getVFS method
     * returns the current VFS.
     */
    public interface IVFS {
        /**
         * Mounts a backing store as part of the VFS.
         * Once this method returns, files contained within the backing store
         * will be available to the application.
         *
         * @param backingStore The backing store containing the files to add.
         * @param mountPath The path to the mount point for this backing store.
         *                  This must be an empty directory.
         */
        function addBackingStore(backingStore:IBackingStore, mountPath:String):void;
        
        /**
         * Adds a file to the filesystem.
         * The file must not already exist.
         *
         * @param path The absolute path to the file to be added.
         * @param data The contents of the file.
         */
        function addFile(path:String, data:ByteArray):void;

        /**
         * Adds an empty directory to the filesystem.
         * The directory must not already exist.
         *
         * @param path The absolute path to the directory to be added.
         */
        function addDirectory(path:String):void;

        /**
         * Deletes a file (or directory) from the filesystem.
         * The file must exist, and if it is a directory, it must be empty.
         *
         * @param path The absolute path to the file to be deleted.
         */
        function deleteFile(path:String):void;

        /**
         * Set the console for the filesystem.
         * This associates the file "/dev/tty" with the console object.
         * When the applications writes to stdout and stderr, the console
         * object will be called back.
         *
         * @param c The console object.
         */
        function set console(c:ISpecialFile):void;

        /**
         * Get the current console for the filesystem.
         * This is the special file "/dev/tty".
         *
         * @return The currently assigned console object, or null if there isn't one.
         */
        function get console():ISpecialFile;

        /**
         * Add a special file to the filesystem.
         * Special files are different from regular files in that they
         * don't have any stored contents. Instead calls to read and write
         * for a special file are routed to a callback object. Special files
         * are never persisted across different runs of the application.
         *
         * @param path The absolute path to the special file.
         * @param specialFile The callback object for the file.
         */
        function addSpecialFile(path:String, specialFile:ISpecialFile):void;

        /**
         * Retrieves the contents of a directory.
         *
         * @param dirpath The absolute path to the directory.
         * 
         * @return An array of FileHandle instances, one per file in
         *              the directory, or null if the directory does not
         *              exist.
         */
        function getDirectoryEntries(dirpath:String):Vector.<FileHandle>;

        /**
         * Checks a proposed path for a new file.
         *
         * @param path The absolute path to the file.
         * 
         * @return A value from the CheckPath enum class describing any problems with the
         *              proposed file name, if any.
         */
        function checkPath(path:String):String;

        /**
         * Opens a file and assigns it a file descriptor.
         * A file descriptor is a non-negative integer, which remains
         * valid until closeFile is called.
         *
         * @param fileHandle An FileHandle instance for the file to be opened.
         * @param fileDescriptor If specified, the file descriptor to assign. Must not
         *              already be a valid file descriptor.
         * 
         * @return The file descriptor assigned to this file.
         */
        function openFile(fileHandle:FileHandle, fileDescriptor:int=-1):int;

        /**
         * Closes a file.
         * Once this method returns, the file descriptor is no longer valid.
         *
         * @param fileDescriptor A file descriptor returned from openFile.
         */
        function closeFile(fileDescriptor:int):void;

        /**
         * Determines whether the argument is a valid file descriptor.
         *
         * @param fileDescriptor A integer that may or may not be a file descriptor.
         * 
         * @return true if the file descriptor is a valid file descriptor
         *          corresponding to an open file, false otherwise.
         */
        function isValidFileDescriptor(fileDescriptor:int):Boolean;

        /**
         * Looks up the FileHandle instance for a file descriptor.
         *
         * @param path The absolute path to the file to be retrieved.
         * 
         * @return The FileHandle corresponding to the path,
         *          or null if the path cannot be retrieved.
         */
        function getFileHandleFromPath(path:String):FileHandle;

        /**
         * Looks up the FileHandle instance for a file descriptor.
         *
         * @param fileDescriptor A file descriptor.
         * 
         * @return The FileHandle corresponding to the file descriptor,
         *          or null if the file descriptor is not valid.
         */
        function getFileHandleFromFileDescriptor(fileDescriptor:int):FileHandle;
    };
};


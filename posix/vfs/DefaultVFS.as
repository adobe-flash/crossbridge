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
    import com.adobe.flascc.vfs.InMemoryBackingStore;
    import com.adobe.flascc.vfs.ISpecialFile;

    /**
    * The default implementation of the IVFS interface
    */
    public class DefaultVFS implements IVFS {

        private var _bslist:Array = [];
        private var _fdTable:Array = [null, null, null];
        private var _specialFileTable:Object = {};

        /**
        * The default implementation of the IVFS interface
        */
        public function DefaultVFS() {
            addBackingStore(new InMemoryBackingStore(), null);
        }

        private function getBackingStoreForPath(path:String):Object {
            path = PathUtils.toCanonicalPath(path);
            for (var i:int = 0; i < _bslist.length; i++) {
                if (_bslist[i].prefix.substr(1) ==
                        path.substr(1, _bslist[i].prefix.length - 1)) {
                    return {'store': _bslist[i].store, 
                            'path': PathUtils.toCanonicalPath(
                                path.substr(_bslist[i].prefix.length))};
                }
            }
            return null;
        }

        private function getPrefixForBackingStore(store:IBackingStore):String {
            for (var i:int = 0; i < _bslist.length; i++) {
                if (_bslist[i].store === store) {
                    return _bslist[i].prefix;
                }
            }
            return null;
        }

        /**
         * Mounts a backing store as part of the VFS.
         * Once this method returns, files contained within the backing store
         * will be available to the application.
         *
         * @param bs The backing store containing the files to add.
         * @param prefix The path to the mount point for this backing store.
         *                  This must be an empty directory.
         */
        public function addBackingStore(bs:IBackingStore, 
                                        prefix:String):void {
            if (!prefix) {
                prefix = "";
            }
            prefix = PathUtils.toCanonicalPath(prefix);

            // Special case for the root directory: don't require an existing
            // empty directory (otherwise we couldn't ever create anything!)
            var bsdesc:Object = {'prefix': prefix, 'store': bs};
            if (prefix != "/") { 
                var a:Vector.<FileHandle> = getDirectoryEntries(prefix); 
                if (!a || a.length) {
                    throw new Error("prefix '" + prefix + "' must be an empty directory");
                }
                _bslist.push(bsdesc);
            } else {
                _bslist = [bsdesc];
            }
            
            // Keep the list sorted by the length of the prefix
            // (getBackingStoreForPath relies on this to match a path
            // with the proper backing store).
            _bslist.sort(function(a:*, b:*):* { return b.prefix.length - a.prefix.length });

            if (!getFileHandleFromPath(prefix)) {
                addDirectory(prefix);
            }
        }

        /**
         * Adds a file to the filesystem.
         * The file must not already exist.
         *
         * @param path The absolute path to the file to be added.
         * @param data The contents of the file.
         */
        public function addFile(path:String, data:ByteArray):void {
            if (!data) {
                throw new Error("data may not be null");
            }
            addFileImpl(path, data);
        }

        /**
         * Adds an empty directory to the filesystem.
         * The directory must not already exist.
         *
         * @param path The absolute path to the directory to be added.
         */
        public function addDirectory(path:String):void {
            addFileImpl(path, null);
        }

        private function addFileImpl(path:String, data:ByteArray):void {
            path = PathUtils.toCanonicalPath(path);
            if (getFileHandleFromPath(path)) {
                throw new Error("cannot create '" + path + "', file exists");
            }
            var lookup:Object = getBackingStoreForPath(path);

            if(data)
                lookup.store.addFile(lookup.path, data);
            else
                lookup.store.addDirectory(lookup.path);
        }

        /**
         * Deletes a file (or directory) from the filesystem.
         * The file must exist, and if it is a directory, it must be empty.
         *
         * @param path The absolute path to the file to be deleted.
         */
        public function deleteFile(path:String):void {
            var ent:FileHandle = getFileHandleFromPath(path);
            if (!ent) {
                throw new Error("cannot delete '" + path + 
                    "', file does not exist");
            } else if (ent.isDirectory && getDirectoryEntries(path).length) {
                throw new Error("cannot delete '" + path +
                    "', directory not empty");
            }
            ent.backingStore.deleteFile(ent.backingStoreRelativePath);
        }

        private const consolePath:String = "/dev/tty"

        /**
         * Set the console for the filesystem.
         * This associates the file "/dev/tty" with the console object.
         * When the applications writes to stdout and stderr, the console
         * object will be called back.
         *
         * @param c The console object.
         */
        public function set console(_console:ISpecialFile):void {
            if (_fdTable[0]) {
                closeFile(0);
            }
            if (_fdTable[1]) {
                closeFile(1);
            }
            if (_fdTable[2]) {
                closeFile(2);
            }
            addSpecialFile(consolePath, _console)
            var entry:FileHandle = getFileHandleFromPath(consolePath)
            _fdTable[0] = entry;
            _fdTable[1] = entry;
            _fdTable[2] = entry;
        }

        public function get console():ISpecialFile {
            var entry:FileHandle = getFileHandleFromPath(consolePath)
            return entry.callback
        }

        /**
         * Add a special file to the filesystem.
         * Special files are different from regular files in that they
         * don't have any stored contents. Instead calls to read and write
         * for a special file are routed to a callback object. Special files
         * are never persisted across different runs of the application.
         *
         * @param path The absolute path to the special file.
         * @param specialFile The callback object for the file.
         * 
         * @return A table entry describing the new file.
         */
        public function addSpecialFile(path:String, specialFile:ISpecialFile):void {
            var entry:FileHandle = FileHandle.makeSpecialFile(specialFile);
            _specialFileTable[path] = entry;
        }

        /**
         * Retrieves the contents of a directory.
         *
         * @param dirpath The absolute path to the directory.
         * 
         * @return A Vector of FileHandles, one per file in
         *              the directory, or null if the directory does not
         *              exist.
         */
        public function getDirectoryEntries(dirpath:String):Vector.<FileHandle> {
            var entry:FileHandle = getFileHandleFromPath(dirpath);
            if (!entry || !entry.isDirectory) {
                return null;
            }
            var filemap:Vector.<String> = entry.backingStore.getPaths();
            var ret:Vector.<FileHandle> = new Vector.<FileHandle>();
            var entDirName:String = entry.backingStoreRelativePath + "/";
            var prefix:String = getPrefixForBackingStore(entry.backingStore);
            for each(var f:String in filemap) {
                if (f && f.substr(0, entDirName.length) == entDirName &&
                        f.length > entDirName.length) {

                    // The name of this entry relative to the directory
                    // in question, without a leading '/' character.
                    var dirRelName:String = f.substr(
                        entry.backingStoreRelativePath.length + 1);

                    // Any remaining '/' characters indicate this file
                    // is in a subdirectory and should not be included
                    // in the output.
                    if (dirRelName.indexOf('/') == -1) {
                        ret.push(getFileHandleFromPath(prefix + '/' + f));
                    }
                }
            }
            return ret;
        }

        /**
         * Resolves a path to a file.
         *
         * @param path The absolute path to the file.
         * 
         * @return An FileHandle instance for the file, or null if
         *              the file does not exist.
         */
        public function getFileHandleFromPath(path:String):FileHandle {
            path = PathUtils.toCanonicalPath(path);

            if (_specialFileTable.hasOwnProperty(path)) {
                return _specialFileTable[path];
            }

            var lookup:Object = getBackingStoreForPath(path);
            var store:IBackingStore = lookup.store;
            if (!lookup) {
                return null;
            }
            if (store.pathExists(lookup.path)) {
                return FileHandle.makeRegularFile(path, lookup.path, store, store.getFile(lookup.path), store.isDirectory(lookup.path))
            } else {
                return null;
            }
        }

        /**
         * Checks a proposed path for a new file.
         *
         * @param path The absolute path to the file.
         * 
         * @return A CheckPathResult describing any problems with the
         *              proposed file name, if any.
         */
        public function checkPath(path:String):String {
            var ret:String = CheckPath.PATH_VALID;
            path = PathUtils.toCanonicalPath(path);

            while (path != "/") {
                path = PathUtils.getDirectory(path);
                var ent:FileHandle = getFileHandleFromPath(path);
                if (!ent) {
                    ret = CheckPath.PATH_COMPONENT_DOES_NOT_EXIST;
                    break;
                } else if (!ent.isDirectory) {
                    ret = CheckPath.PATH_COMPONENT_IS_NOT_DIRECTORY;
                    break;
                }
            }

            return ret;
        }

        /**
         * Opens a file and assigns it a file descriptor.
         * A file descriptor is a non-negative integer, which remains
         * valid until closeFile is called.
         *
         * @param fileHandle A FileHandle instance for the file to be opened.
         * @param fileDescriptor If specified, the file descriptor to assign. Must not
         *              already be a valid file descriptor.
         * 
         * @return The file descriptor assigned to this file.
         */
        public function openFile(fileHandle:FileHandle, fileDescriptor:int=-1):int {
            if (fileDescriptor == -1) {
                return _fdTable.push(fileHandle) - 1; 
            } else {
                _fdTable[fileDescriptor] = fileHandle;
                return fileDescriptor;
            }
        }

        /**
         * Closes a file.
         * Once this method returns, the file descriptor is no longer valid.
         *
         * @param fileDescriptor A file descriptor returned from openFile.
         */
        public function closeFile(fileDescriptor:int):void {
            if (_fdTable[fileDescriptor].backingStore) {
                _fdTable[fileDescriptor].backingStore.flush();
            }
            _fdTable[fileDescriptor] = null;
        }

        /**
         * Determines whether the argument is a valid file descriptor.
         *
         * @param fileDescriptor A integer that may or may not be a file descriptor.
         * 
         * @return true if the file descriptor is a valid file descriptor
         *          corresponding to an open file, false otherwise.
         */
        public function isValidFileDescriptor(fileDescriptor:int):Boolean {
            return fileDescriptor >= 0 && fileDescriptor < _fdTable.length && _fdTable[fileDescriptor];
        }

        /**
         * Looks up the FileHandle instance for a file descriptor.
         *
         * @param fileDescriptor A file descriptor.
         * 
         * @return The FileHandle corresponding to the file descriptor,
         *          or null if the file descriptor is not valid.
         */
        public function getFileHandleFromFileDescriptor(fileDescriptor:int):FileHandle {
            if (isValidFileDescriptor(fileDescriptor)) {
                return _fdTable[fileDescriptor];
            } else {
                return null;
            }
        }
    }

    /**
    * A collection of path handling related functions
    */
    public class PathUtils {

        /**
        * Return a path equivalent to the argument, with these properties:
        * 1. is absolute (will always have a leading '/')
        * 2. doesn't contain '.' or '..' 
        * 3. never has a trailing '/', even if the argument path did
        * 4. never has two or more "/" characters in a row
        * TODO: Relative paths are assumed to be relative to the 
        * current working directory.
        */
        public static function toCanonicalPath(path:String):String {
            var x:String = path.replace(/\//, "/");
            while(x != path)
            {
                path = x;
                x = path.replace(/\//, "/");
            }
            var segs:Array = path.split("/");
            var ret:Array = [];
            for (var i:uint = 0; i < segs.length; i++) {
                if (segs[i] == "..") {
                    if (ret.length > 0) {
                        ret.pop(); 
                    }
                } else if (segs[i].length != 0 && segs[i] != ".") {
                    ret.push(segs[i]);
                }
            }
            return "/" + ret.join("/");
        }

        /**
        * Return everything except the last component of the supplied path.
        */
        public static function getDirectory(path:String):String {
            path = PathUtils.toCanonicalPath(path);
            var i:uint = path.lastIndexOf("/");
            if (!i) {
                return "/";
            } else {
                return path.substring(0, i);
            }
        }
    }
}

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
    import flash.utils.ByteArray;
    import flash.events.EventDispatcher;

    /**
    * The InMemoryBackingStore implements <code>IBackingStore</code> with a memory resident
    * mapping between paths and file data stored as a ByteArray.
    *
    * <p>This class does not depend on the rest of flascc, so it can be safely
    * used in isolation, such as in a preloader.</p>
    */
    public class InMemoryBackingStore extends EventDispatcher 
                                      implements IBackingStore {

        /**
        * Construct a new InMemoryBackingStore
        */
        public function InMemoryBackingStore() {}

        private var filemap:Object = {}

        /**
        * This BackingStore is read-write, so this always returns false.
        */
        public function get readOnly():Boolean {
            return false
        }

        /**
         * Retrieves all of the files paths contained in this BackingStore.
         *
         * @return A Vector of all file paths contained in this BackingStore.
         */
        public function getPaths():Vector.<String> {
            var paths:Vector.<String> = new Vector.<String>()
            for (var f:String in filemap) {
                paths.push(f)
            }
            return paths
        }

        /**
        * Commits any outstanding changes to the underlying datastore. This is a no-op in this class,
        * but could do something useful in a BackingStore that would benefit from buffering some of its
        * actual write operations.
        */
        public function flush():void {}
        
        /**
        * Given an array of strings representing file paths, this method removes
        * the reference to that file from the filemap.
        */
        public function deleteFile(path:String):void {
            delete filemap[path]
        } 

        /**
        * Insert a mapping from path to data into the filemap.
        */
        public function addFile(path:String, data:ByteArray):void {
            //trace("[InMemoryBackingStore] addFile(" + path + ")")
            filemap[path] = data
        }

        /**
        * Insert a mapping representing a directory.
        */
        public function addDirectory(path:String):void {
            if(path.lastIndexOf("/") == path.length-1 && path.length > 1) {
                path = path.slice(0, path.length-1)
            }
            //trace("[InMemoryBackingStore] addDirectory(" + path + ")")
            filemap[path] = null
        }
        
        /**
        * Given a file object returned from makeFileObject this method extracts the contents of that file as a <code>ByteArray</code>
        */
        public function getFile(path:String):ByteArray {
            //trace("[InMemoryBackingStore] getFile(" + path + ")")
            return filemap[path]
        }

        /**
        * Checks if a path is a directory.
        */
        public function isDirectory(path:String):Boolean {
            var result:Boolean = filemap[path] == null
            //trace("[InMemoryBackingStore] isDirectory(" + path + ") == " + result)
            return result
        }

        /**
        * Checks if a path exists.
        */
        public function pathExists(path:String):Boolean {
            var result:Boolean = filemap.hasOwnProperty(path)
            //trace("[InMemoryBackingStore] pathExists(" + path + ") == " + result)
            return result
        }
    };
}

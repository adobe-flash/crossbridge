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
    import flash.net.SharedObject;

    /**
    * Extends the InMemoryBackingStore with one that stores and retrieves its data from a Flash
    * Local Shared object (<a href="http://www.adobe.com/security/flashplayer/articles/lso/" target="_new">http://www.adobe.com/security/flashplayer/articles/lso/</a>).
    * 
    * <p>The contents of this VFS will persist until the user clears their LSO cache (which might happen when they clear their browser cache.)</p>
    */
    public class LSOBackingStore extends InMemoryBackingStore {

        private var _so:SharedObject;

        /**
        * Construct an LSOBackingStore using a named local shared object.
        * @name the name of the Local Shared Object to be used, the property "filemap" will be used to store the VFS.
        */
        public function LSOBackingStore(name:String):void {
            _so = SharedObject.getLocal(name);
            if (_so && _so.data.hasOwnProperty("filemap")) {
                for(var f:String in _so.data.filemap) {
                    addFile(f, _so.data.filemap[f])
                }
            }
        }

        /**
        * Writes all of the files back into the LSO, this might be slow so you need to decide when to call
        * this in your application to balance performance with reliability.
        */
        override public function flush():void {
            super.flush();
            _so.data.filemap = {}
            for each(var f:String in getPaths()) {
                _so.data.filemap[f] = getFile(f)
            }
            _so.flush();
        }
    }

}

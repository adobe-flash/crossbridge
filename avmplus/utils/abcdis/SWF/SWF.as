/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package SWF
{
    import SWF.Rect;
    import flash.utils.ByteArray
    
    
    public final class SWF
    {
    
        internal var uncompressedData : ByteArray;
        
        public var rect:Rect;
        public var frameRate : uint;
        public var frameCount : uint;
        public var tags : Array;
        
        public function SWF(uncompressedData : ByteArray)
        {
            this.uncompressedData = uncompressedData;
            tags = new Array();
        }
    }
}
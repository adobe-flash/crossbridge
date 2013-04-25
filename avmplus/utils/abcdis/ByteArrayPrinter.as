/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package
{
    import flash.utils.ByteArray;
    
    public class ByteArrayPrinter implements IPrinter
    {
        public function ByteArrayPrinter(byteArray : ByteArray)
        {
            m_byteArray = byteArray;
        }
        
        private var m_byteArray : ByteArray;
        
        public final function println(s : String) : void
        {
            m_byteArray.writeUTFBytes(s + "\n");
        }
    }
}
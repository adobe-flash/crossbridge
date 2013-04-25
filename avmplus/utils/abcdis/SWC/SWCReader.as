/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package SWC
{
    import flash.utils.ByteArray;
    
    import abc.IReader;
    import abc.Types.ABCFile;
    import zip.ZipFile;
    import SWF.SWFReader;
    
    public class SWCReader implements IReader
    {
        public function SWCReader(data:ByteArray)
        {
            this.data = data;
        }
        
        private var data : ByteArray;
    
        private namespace catalogNS = "http://www.adobe.com/flash/swccatalog/9"
    
        public function readABC():Array
        {
            var result : Array = new Array();
            decodeSWC(function (swfReader : SWFReader, libraryPath : String) : void
                {
                    var swfABCs : Array = swfReader.readABC()
                    var count : uint = 0;
                    for each (var swfABC : ABCFile in swfABCs)
                    {
                        swfABC.abcName = libraryPath + "(" + swfABC.abcName + ")"
                        ++count
                        result.push(swfABC)
                    }
                });
            return result;
        }
        
        public function readABCBytes() : Array
        {
            var result : Array = []
            decodeSWC(function (swfReader : SWFReader, libraryPath : String) : void
                {
                    var swfABCBytes : Array = swfReader.readABCBytes()
                    result.concat(swfABCBytes)
                });
            return result;
        }
        
        public function readLibraries() : Array
        {
            var result : Array = []
            decodeSWC(function (swfReader : SWFReader, libraryPath : String) : void
                {
                    result.push({swf : swfReader, path : libraryPath})
                });
            return result;
        }
        
        private function decodeSWC(libraryHandler : Function) : void
        {
            var zf : ZipFile = new ZipFile(data)
            var catalogBytes : ByteArray = zf.getEntryByName("catalog.xml");
            var catalogStr : String = catalogBytes.readUTFBytes(catalogBytes.length)
            var catalogXML : XML = new XML(catalogStr)
            var result : Array = new Array();
            for each ( var libraryElement : Object in catalogXML.catalogNS::libraries.catalogNS::library)
            {
                var libraryPath : String = libraryElement.@path;
                var libraryBytes : ByteArray = zf.getEntryByName(libraryPath)
                if (libraryBytes != null)
                {
                    var swfReader : SWFReader = new SWFReader(libraryBytes)
                    libraryHandler(swfReader, libraryPath);
                }
            }

        }
        
        public function supportsContent():Boolean
        {
            return ZipFile.isZipFile(data);
        }
    }
}

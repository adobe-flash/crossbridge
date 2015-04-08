//
// =BEGIN MIT LICENSE
//
// The MIT License (MIT)
//
// Copyright (c) 2014 The CrossBridge Team
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =END MIT LICENSE
//
package com.adobe.flascc.vfs {
import flash.events.AsyncErrorEvent;
import flash.events.Event;
import flash.events.IOErrorEvent;
import flash.events.ProgressEvent;
import flash.events.SecurityErrorEvent;
import flash.net.URLLoader;
import flash.net.URLLoaderDataFormat;
import flash.net.URLRequest;
import flash.utils.ByteArray;
import flash.utils.Endian;

/**
 * TBD
 */
public class URLLoaderVFS extends InMemoryBackingStore {
    /**
     * @private
     */
    private var bytesLoaded:uint;
    /**
     * @private
     */
    private var percentComplete:uint;
    /**
     * @private
     */
    private var currentLoader:URLLoader;
    /**
     * @private
     */
    private var currentVPath:String;
    /**
     * @private
     */
    private var currentUrls:Array = [];
    /**
     * @private
     */
    private var currentContents:ByteArray;
    /**
     * @private
     */
    private var urlLoader:URLLoader;
    /**
     * @private
     */
    private var vfsFiles:Array;
    /**
     * Constructor
     */
    public function URLLoaderVFS() {
    }
    /**
     * TBD
     */
    public function dispose():void {
        // TODO: implement
        // free memory
    }
    //----------------------------------
    //  Manifest Loader
    //----------------------------------
    /**
     * TBD
     */
    public function loadManifest(path:String = "manifest"):void {
        urlLoader = new URLLoader();
        urlLoader.addEventListener(Event.COMPLETE, onManifestComplete, false, 0, true);
        urlLoader.addEventListener(IOErrorEvent.IO_ERROR, onManifestError, false, 0, true);
        urlLoader.addEventListener(SecurityErrorEvent.SECURITY_ERROR, onManifestError, false, 0, true);
        urlLoader.load(new URLRequest(path));
    }
    /**
     * @private
     */
    private function onManifestComplete(event:Event):void {
        var temp1:Array = event.target.data.split(/\r/);
        var temp2:Array;
        vfsFiles = new Array();
        for each (var tempPath1:String in temp1) {
            temp2 = tempPath1.split(/\n/);
            for each (var tempPath2:String in temp2) {
                vfsFiles.push(tempPath2);
            }
        }
        removeManifestLoaderListeners();
        startNewFile();
    }
    /**
     * @private
     */
    private function onManifestError(event:Event):void {
        trace(this, "onManifestError", event);
        removeManifestLoaderListeners();
        this.dispatchEvent(event);
    }
    /**
     * @private
     */
    private function removeManifestLoaderListeners():void {
        urlLoader.removeEventListener(Event.COMPLETE, onManifestComplete);
        urlLoader.removeEventListener(IOErrorEvent.IO_ERROR, onManifestError);
        urlLoader.removeEventListener(SecurityErrorEvent.SECURITY_ERROR, onManifestError);
    }
    //----------------------------------
    //  Caching
    //----------------------------------
    /**
     * @private
     */
    private function startNewFile():void {
        if (currentVPath == null) {
            var newfile:String = vfsFiles.shift()
            if (newfile == null) {
                // All files finished
                this.dispatchEvent(new Event(Event.COMPLETE));
                return
            }

            var paths:Array = newfile.split(" ");
            var filterFunc:Function = function (path:String, index:int, array:Array):Boolean {
                return (path != "");
            };
            paths = paths.filter(filterFunc);

            var realPath:String;
            var calculatedPath:String;
            if (paths.length > 1) {
                realPath = paths[0];
                calculatedPath = paths[1];
            }
            else {
                realPath = paths[0];
                calculatedPath = realPath;
            }

            currentVPath = calculatedPath;
            currentContents = new ByteArray()
            currentContents.endian = Endian.LITTLE_ENDIAN
            currentContents.position = 0
            currentUrls.length = 0
            currentUrls.push(realPath);
        }

        startNewDownload();
    }
    //----------------------------------
    //  Content Loader
    //----------------------------------
    /**
     * @private
     */
    private function startNewDownload():void {
        var url:String = currentUrls.shift()
        if (url == null) {
            addFile(currentVPath, currentContents);
            currentVPath = null;
            startNewFile();
            return;
        }
        currentLoader = new URLLoader(new URLRequest(url));
        currentLoader.dataFormat = URLLoaderDataFormat.BINARY;
        currentLoader.addEventListener(AsyncErrorEvent.ASYNC_ERROR, onURLLoaderError, false, 0, true);
        currentLoader.addEventListener(SecurityErrorEvent.SECURITY_ERROR, onURLLoaderError, false, 0, true);
        currentLoader.addEventListener(IOErrorEvent.IO_ERROR, onURLLoaderError, false, 0, true);
        currentLoader.addEventListener(Event.COMPLETE, onURLLoaderComplete, false, 0, true);
        currentLoader.addEventListener(ProgressEvent.PROGRESS, onURLLoaderProgress, false, 0, true);
    }
    /**
     * @private
     */
    private function removeListeners():void {
        currentLoader.removeEventListener(AsyncErrorEvent.ASYNC_ERROR, onURLLoaderError);
        currentLoader.removeEventListener(SecurityErrorEvent.SECURITY_ERROR, onURLLoaderError);
        currentLoader.removeEventListener(IOErrorEvent.IO_ERROR, onURLLoaderError);
        currentLoader.removeEventListener(Event.COMPLETE, onURLLoaderComplete);
        currentLoader.removeEventListener(ProgressEvent.PROGRESS, onURLLoaderProgress);
    }
    /**
     * @private
     */
    private function onURLLoaderComplete(event:Event):void {
        bytesLoaded += currentLoader.data.length;
        currentContents.writeBytes(currentLoader.data);
        removeListeners();
        startNewDownload();
    }
    /**
     * @private
     */
    private function onURLLoaderError(event:Event):void {
        trace(this, "onURLLoaderError", event);
        removeListeners();
        this.dispatchEvent(event);
    }
    /**
     * @private
     */
    private function onURLLoaderProgress(event:ProgressEvent):void {
        percentComplete = (event.bytesLoaded / event.bytesLoaded * 100);
    }
}
}

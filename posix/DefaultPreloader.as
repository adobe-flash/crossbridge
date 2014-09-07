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

package com.adobe.flascc.preloader {
import com.adobe.flascc.CModule;
import com.adobe.flascc.Console;
import com.adobe.flascc.vfs.HTTPBackingStore;

import flash.display.MovieClip;
import flash.display.StageAlign;
import flash.display.StageScaleMode;
import flash.events.Event;
import flash.events.IOErrorEvent;
import flash.events.ProgressEvent;

/**
 * By default, this preloader is injected into every flascc-generated SWF. After detecting that
 * the SWF has fully loaded, it constructs a new Console object, which should cause the SWF to execute.
 *
 * <p>If a suitable HTTPBackingStore has been linked into the SWF, it will be be created and the SWF will only
 * start once the contents have been downloaded.</p>
 *
 * <p>To replace the default preloader, you should compile this class to a SWF and tell gcc/g++ to use
 * it with the argument "-swf-preloader=foo.swf". Alternatively, if you don't want a preloader, use "-no-swf-preloader".</p>
 */
public class DefaultPreloader extends MovieClip {
    /**
     * @private
     */
    protected var webFS:Object;

    /**
     * @private
     */
    protected var webFSPercent:Number = 0;

    /**
     * @private
     */
    protected var swfPercent:Number = 0;

    /**
     * @private
     */
    protected var barHeight:uint = 40;

    /**
     * @private
     */
    protected var isWebFSFinished:Boolean;

    /**
     * @private
     */
    protected var isWebFSFailed:Boolean;

    /**
     * Construct the flascc preloader that is injected into every SWF by default.
     */
    public function DefaultPreloader() {
        initialize();
    }

    /**
     * @private
     */
    protected function initialize() {
        CONFIG::debug { trace("DefaultPreloader::initialize"); }
        try {
            if (!webFS) {
                webFS = new HTTPBackingStore();
                webFS.addEventListener(ProgressEvent.PROGRESS, onWebFSProgress, false, 0, true);
                webFS.addEventListener(Event.COMPLETE, onWebFSComplete, false, 0, true);
                webFS.addEventListener(IOErrorEvent.IO_ERROR, onWebFSError, false, 0, true);
            }
        } catch (error:*) {
            // If an HTTPBackingStore wasn't linked into this exe then ignore it.
        }

        if (stage) {
            initStage();
            loaderInfo.addEventListener(ProgressEvent.PROGRESS, onProgress, false, 0, true);
            loaderInfo.addEventListener(Event.COMPLETE, onPreloaderComplete, false, 0, true);
        } else {
            addEventListener(Event.ADDED_TO_STAGE, onPreloaderComplete, false, 0, true);
        }
    }

    /**
     * @private
     */
    protected function dispose(event:Event = null) {
        CONFIG::debug { trace("DefaultPreloader::dispose"); }
        removeListeners();
        removeWebFSListeners();
    }

    /**
     * @private
     */
    protected function removeListeners() {
        loaderInfo.removeEventListener(ProgressEvent.PROGRESS, onProgress);
        loaderInfo.removeEventListener(Event.COMPLETE, onPreloaderComplete);
        removeEventListener(Event.ADDED_TO_STAGE, onPreloaderComplete);
        removeEventListener(Event.REMOVED_FROM_STAGE, dispose);
    }

    /**
     * @private
     */
    protected function removeWebFSListeners() {
        if (webFS) {
            webFS.removeEventListener(ProgressEvent.PROGRESS, onWebFSProgress);
            webFS.removeEventListener(Event.COMPLETE, onWebFSComplete);
            webFS.removeEventListener(IOErrorEvent.IO_ERROR, onWebFSError);
        }
    }
    
    /**
     * @private
     */
    protected function initStage():void {
        //CONFIG::debug { trace("DefaultPreloader::initStage"); }
        stage.frameRate = 60;
        stage.scaleMode = StageScaleMode.NO_SCALE;
        stage.align = StageAlign.TOP_LEFT;
    }

    /**
     * @private
     */
    protected function onWebFSProgress(event:ProgressEvent):void {
        webFSPercent = (event.bytesLoaded / event.bytesTotal);
        render();
    }

    /**
     * @private
     */
    protected function onWebFSComplete(event:Event):void {
        isWebFSFinished = true;
        onPreloaderComplete(null);
    }

    /**
     * @private
     */
    protected function onWebFSError(event:Event):void {
        CONFIG::debug {
            trace("DefaultPreloader::onWebFSError: " + event);
        }
        removeWebFSListeners();
        isWebFSFailed = true;
    }

    /**
     * @private
     */
    protected function render():void {
        const sw:int = stage.stageWidth;
        const sh:int = stage.stageHeight;

        graphics.clear();
        graphics.beginFill(0);
        graphics.drawRect(0, 0, sw, sh);
        graphics.endFill();

        var barColor:uint = isWebFSFailed ? 0xFF0000 : 0xFFFFFF;

        // Swf progress bar
        var barWidth:int = sw * 0.75;

        graphics.lineStyle(1, barColor);
        graphics.drawRect((sw - barWidth) * 0.5, (sh * 0.5) - (barHeight * 0.5), barWidth, barHeight);

        graphics.beginFill(barColor);
        graphics.drawRect((sw - barWidth) * 0.5 + 5, (sh * 0.5) - (barHeight * 0.5) + 5, (barWidth - 10) * swfPercent, barHeight - 10);
        graphics.endFill();

        // WebsFS progress bar
        if (webFS) {
            barColor = isWebFSFailed ? 0xFF0000 : 0xAAAAAA;
            graphics.lineStyle(1, barColor);
            graphics.drawRect((sw - barWidth) * 0.5, (sh * 0.5) - (barHeight * 0.5) + 5 + barHeight, barWidth, barHeight);

            graphics.beginFill(barColor);
            graphics.drawRect((sw - barWidth) * 0.5 + 5, (sh * 0.5) - (barHeight * 0.5) + 10 + barHeight, (barWidth - 10) * webFSPercent, barHeight - 10);
            graphics.endFill();
        }
    }

    /**
     * @private
     */
    protected function onProgress(event:ProgressEvent):void {
        swfPercent = event.bytesLoaded / event.bytesTotal;
        render();
    }

    /**
     * @private
     */
    protected function onPreloaderComplete(event:Event):void {
        // wait until the webFS is complete
        if (webFS && isWebFSFinished == false) {
            return;
        }
        CONFIG::debug { trace("DefaultPreloader::onPreloaderComplete"); }
        // remove event listeners
        removeListeners();
        // add dispose listeners
        addEventListener(Event.REMOVED_FROM_STAGE, dispose, false, 0, true);
        // step to main frame
        graphics.clear();
        gotoAndStop(2);
        // cache web file system
        if (webFS) {
            CModule.vfs.addBackingStore(webFS, null);
            removeWebFSListeners();
            webFS = null;
        }
        // create console
        new Console(this);
    }
}
}
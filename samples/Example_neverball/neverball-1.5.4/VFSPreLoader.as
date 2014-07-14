////////////////////////////////////////////////////////////////////////////////
//
//  ADOBE SYSTEMS INCORPORATED
//  Copyright 2011 Adobe Systems Incorporated
//  All Rights Reserved.
//
//  NOTICE: Adobe permits you to use, modify, and distribute this file
//  in accordance with the terms of the license agreement accompanying it.
//
////////////////////////////////////////////////////////////////////////////////

package com.adobe.flascc.preloader
{
    import flash.display.MovieClip;
    import flash.events.Event;
    import flash.events.ProgressEvent;
    import flash.text.TextField;
    import flash.display.Graphics;
    import flash.events.AsyncErrorEvent;
    import flash.events.HTTPStatusEvent;
    import flash.events.IOErrorEvent;
    import flash.events.SecurityErrorEvent;
    import flash.net.URLLoader;
    import flash.net.URLLoaderDataFormat;
    import flash.net.URLRequest;
    import flash.net.URLRequest;
    import flash.system.ApplicationDomain;
    import flash.text.TextFormat;
    import flash.text.TextFormatAlign;
    import flash.utils.ByteArray;

    import com.adobe.flascc.Console;
    
    public class VFSPreLoader extends MovieClip
    {
      var fail = false
      private var datazips:Array = [];
      private var engineLoaded:Boolean = false;

      var vfscomplete:Boolean, enginecomplete:Boolean

      public function VFSPreLoader() 
      {
          addEventListener(Event.ADDED_TO_STAGE, onAddedToStage);
      }

      private function onAddedToStage(e:*)
      {
        stage.frameRate = 60
        
        loaderInfo.addEventListener(ProgressEvent.PROGRESS, onProgress4)

        var datazip1 = new URLLoader(new URLRequest("data1.zip"));
        datazip1.dataFormat = URLLoaderDataFormat.BINARY;
        datazip1.addEventListener(AsyncErrorEvent.ASYNC_ERROR, onError)
        datazip1.addEventListener(SecurityErrorEvent.SECURITY_ERROR, onError)
        datazip1.addEventListener(IOErrorEvent.IO_ERROR, onError)
        datazip1.addEventListener(Event.COMPLETE, onComplete)
        datazip1.addEventListener(ProgressEvent.PROGRESS, onProgress1)

        var datazip2 = new URLLoader(new URLRequest("data2.zip"));
        datazip2.dataFormat = URLLoaderDataFormat.BINARY;
        datazip2.addEventListener(AsyncErrorEvent.ASYNC_ERROR, onError)
        datazip2.addEventListener(SecurityErrorEvent.SECURITY_ERROR, onError)
        datazip2.addEventListener(IOErrorEvent.IO_ERROR, onError)
        datazip2.addEventListener(Event.COMPLETE, onComplete)
        datazip2.addEventListener(ProgressEvent.PROGRESS, onProgress2)

        var datazip3 = new URLLoader(new URLRequest("data3.zip"));
        datazip3.dataFormat = URLLoaderDataFormat.BINARY;
        datazip3.addEventListener(AsyncErrorEvent.ASYNC_ERROR, onError)
        datazip3.addEventListener(SecurityErrorEvent.SECURITY_ERROR, onError)
        datazip3.addEventListener(IOErrorEvent.IO_ERROR, onError)
        datazip3.addEventListener(Event.COMPLETE, onComplete)
        datazip3.addEventListener(ProgressEvent.PROGRESS, onProgress3)
    }

    private var p1:int,p2:int,p3:int,p4:int;

    private function onProgress1(e:ProgressEvent):void {
        p1 = uint(e.bytesLoaded / e.bytesTotal * 100)
        render()
    }

    private function onProgress2(e:ProgressEvent):void {
        p2 = uint(e.bytesLoaded / e.bytesTotal * 100)
        render()
    }

    private function onProgress3(e:ProgressEvent):void {
        p3 = uint(e.bytesLoaded / e.bytesTotal * 100)
        render()
    }
    
    private function onProgress4(e:ProgressEvent):void {
        p4 = uint(e.bytesLoaded / e.bytesTotal * 100)
        render()
    }

    private function onComplete(e:Event):void {
        datazips.push(e.target.data);
        if(datazips.length == 3)
        this.addEventListener(Event.ENTER_FRAME, enterFrame);
        render()
    }

    private function render():void {
        graphics.clear()
        graphics.beginFill(0)
        graphics.drawRect(0, 0, stage.stageWidth, stage.stageHeight)
        graphics.endFill()

        var pct:Number = ((p1+p2+p3+p4) / 4.0) / 100.0

        var barColor = fail ? 0xFF0000 : 0xFFFFFF;

            // progress bar
            var barHeight:int = 40
            var barWidth:int = stage.stageWidth * 0.75

            graphics.lineStyle(1, barColor)
            graphics.drawRect((stage.stageWidth - barWidth) / 2, (stage.stageHeight/2) - (barHeight/2), barWidth, barHeight)

            graphics.beginFill(barColor)
            graphics.drawRect((stage.stageWidth - barWidth) / 2 + 5, (stage.stageHeight/2) - (barHeight/2) + 5, (barWidth - 10) * pct, barHeight - 10)
            graphics.endFill()
        }

        private function onError(e:Event):void
        {
            fail = true
            render()
        }

        private function enterFrame(e:Event):void {
            if(fail || datazips.length < 3)
                return;

            this.removeEventListener(Event.ENTER_FRAME, enterFrame);
            gotoAndStop(2)
            graphics.clear()

            var f = ApplicationDomain.currentDomain.getDefinition("com.adobe.flascc.addVFSZip");
            for(var i in datazips)
               f(datazips[i])
            datazips = null

            new Console(this)
        }
    }
}

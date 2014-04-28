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

package
{
	import flash.display.MovieClip;
	import flash.events.Event;
	import flash.events.ProgressEvent;
	import flash.text.TextField;
    import flash.text.TextFormat;
    import flash.text.TextFormatAlign;
    import flash.display.Loader;
    import flash.net.URLRequest;
    import flash.events.AsyncErrorEvent;
    import flash.events.SecurityErrorEvent;
    import flash.events.HTTPStatusEvent;
    import flash.events.IOErrorEvent;
    import flash.display.LoaderInfo;
    import flash.display.Graphics;
    import flash.system.ApplicationDomain;
    import flash.system.LoaderContext;
    import flash.utils.ByteArray;
    import flash.net.URLRequest;
    import flash.net.URLLoader;
    import flash.net.URLLoaderDataFormat;
    import flash.system.Worker;
    
    import com.adobe.flascc.vfs.HTTPBackingStore;
	import com.adobe.flascc.CModule;
	import com.adobe.flascc.Console;

	public class VFSPreLoader extends MovieClip
	{
		private var webfs:Object = null
		private var webfsPct:Number = 0, swfPct:Number = 0
		private var webfsFinished:Boolean = false
		private var failed:Boolean = false

		public function VFSPreLoader() 
		{
			if(!Worker.current.isPrimordial) {
				gotoAndStop(2)
				new Console(this)
				return
			}

			webfs = new URLLoader(new URLRequest("dosboxvfs.zip"));
			webfs.dataFormat = URLLoaderDataFormat.BINARY;
			webfs.addEventListener(AsyncErrorEvent.ASYNC_ERROR, webfsError)
			webfs.addEventListener(SecurityErrorEvent.SECURITY_ERROR, webfsError)
			webfs.addEventListener(IOErrorEvent.IO_ERROR, webfsError)
			webfs.addEventListener(Event.COMPLETE, webfsComplete)
			webfs.addEventListener(ProgressEvent.PROGRESS, webfsProgress)

			if(stage) {
				stage.frameRate = 60
				loaderInfo.addEventListener(ProgressEvent.PROGRESS, onProgress)
				loaderInfo.addEventListener(Event.COMPLETE, onPreloaderComplete)
			} else {
				addEventListener(Event.ADDED_TO_STAGE, onPreloaderComplete)
			}
		}

		private function webfsProgress(e:ProgressEvent):void {
			webfsPct = (e.bytesLoaded / e.bytesTotal)
			render()
		}

		private function webfsComplete(e:Event):void {
			trace("preloader 4")
			webfsFinished = true
			webfs = webfs.data
			onPreloaderComplete(null)
		}

		private function webfsError(e:Event):void {
			trace(e)
			failed = true
			webfsFinished = true
		}

		private function render():void {
			graphics.clear()
			graphics.beginFill(0)
			graphics.drawRect(0, 0, stage.stageWidth, stage.stageHeight)
			graphics.endFill()

			var barColor:uint = failed ? 0xFF0000 : 0xFFFFFF;

			// Swf progress bar
			var barHeight:int = 40
			var barWidth:int = stage.stageWidth * 0.75

			graphics.lineStyle(1, barColor)
      		graphics.drawRect((stage.stageWidth - barWidth) / 2, (stage.stageHeight/2) - (barHeight/2), barWidth, barHeight)

      		graphics.beginFill(barColor)
      		graphics.drawRect((stage.stageWidth - barWidth) / 2 + 5, (stage.stageHeight/2) - (barHeight/2) + 5, (barWidth - 10) * swfPct, barHeight - 10)
			graphics.endFill()

			// WebsFS progress bar
			if(webfs) {
				barColor = failed ? 0xFF0000 : 0xAAAAAA;
				graphics.lineStyle(1, barColor)
				graphics.drawRect((stage.stageWidth - barWidth) / 2, (stage.stageHeight/2) - (barHeight/2) + 5 + barHeight, barWidth, barHeight)

				graphics.beginFill(barColor)
				graphics.drawRect((stage.stageWidth - barWidth) / 2 + 5, (stage.stageHeight/2) - (barHeight/2) + 10 + barHeight, (barWidth - 10) * webfsPct, barHeight - 10)
				graphics.endFill()
			}
		}

		private function onProgress(e:ProgressEvent):void 
		{
			swfPct = e.bytesLoaded / e.bytesTotal
			render()
		}
		
		private function onPreloaderComplete(e:Event):void 
		{
			if(webfs && webfsFinished == false)
				return; // wait until the webfs is complete

			gotoAndStop(2)
			graphics.clear()

			new Console(this,webfs as ByteArray)
			webfs = null
		}		
	}
}

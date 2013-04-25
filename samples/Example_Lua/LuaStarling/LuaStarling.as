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

package
{
	import flash.display.Sprite;
	import flash.display.StageAlign;
	import flash.display.StageQuality;
	import flash.display.StageScaleMode;
	import flash.geom.Rectangle;
	import flash.system.Capabilities;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.ProgressEvent;
	import flash.events.SecurityErrorEvent;
	import flash.events.IOErrorEvent;
	import flash.events.AsyncErrorEvent;
	import flash.net.URLRequest;
	import flash.net.URLLoader;
	import flash.system.Security;
	import flash.external.ExternalInterface;
	
	import starling.core.Starling;

	import sample.lua.CModule;
	
	[SWF(width="640", height="960", frameRate="60", backgroundColor="#ffffff")]
	public class LuaStarling extends Sprite
	{
		private var _width:Number = 640
		private var _height:Number = 960
		private var mStarling:Starling
		private var scriptloader:URLLoader
		
		include "../../TestingCode.as";

		public function write(fd:int, buf:int, nbyte:int, errno_ptr:int):int
		{
			var str:String = CModule.readString(buf, nbyte)
			trace(str)
			return nbyte
		}

		public function LuaStarling()
		{
			trace("LuaStarling ctor");
			addEventListener(Event.ADDED_TO_STAGE, onAdded)
		}

		private function onAdded(e:*):void 
		{
			stage.frameRate = 60
			
			// Initialize the flascc world
			CModule.startAsync(this)

			if (ExternalInterface.available) {
				try {
					ExternalInterface.addCallback("newLuaScript", newLuaScript);
					Security.allowDomain("*") // Awwww yeah!
					trace("External interface callback added.");
				} catch (error:Error) {
					trace("An Error occurred: " + error.message + "\n");
				}
			} else {
				trace("External interface is not available for this container.");
				scriptloader = new URLLoader(new URLRequest("game.lua"))
				scriptloader.addEventListener(AsyncErrorEvent.ASYNC_ERROR, onError)
				scriptloader.addEventListener(SecurityErrorEvent.SECURITY_ERROR, onError)
				scriptloader.addEventListener(IOErrorEvent.IO_ERROR, onError)
				scriptloader.addEventListener(Event.COMPLETE, onComplete)
			}

			// Initialize the Starling world
			stage.scaleMode = StageScaleMode.NO_SCALE
			stage.align = StageAlign.TOP_LEFT
			stage.quality = StageQuality.LOW
			if (Capabilities.manufacturer.toLowerCase().indexOf("ios") != -1 || 
				Capabilities.manufacturer.toLowerCase().indexOf("android") != -1)
			{
				_width = Capabilities.screenResolutionX
				_height = Capabilities.screenResolutionY
			}

			Starling.multitouchEnabled = true
			initTesting();
		}

		private function onComplete(e:*):void
		{
			LuaGame.luascript = scriptloader.data
			scriptloader = null
			initStarling()
		}

		private function initStarling():void
		{
			if(mStarling) {
				mStarling.stop()
				mStarling.dispose()
			}
		
			mStarling = new Starling(LuaGame, stage, new Rectangle(0, 0, _width, _height))
			mStarling.simulateMultitouch = true
			mStarling.enableErrorChecking = false
			mStarling.start()
		}
		
		private function newLuaScript(value:String):void
		{
			LuaGame.luascript = value
			initStarling();
		}
		
		public static function onError(e:*):void
		{
			if(ExternalInterface.available) {
				ExternalInterface.call("reportError", e.toString())
			} else {
				trace(e)
			}
		}
	}
}

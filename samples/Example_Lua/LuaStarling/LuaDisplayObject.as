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
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display3D.*;
	import flash.utils.Dictionary;
	import flash.utils.getTimer;
	import flash.text.TextField;
	import flash.text.TextFieldAutoSize;
	import flash.text.TextFormat;
	import flash.text.TextFormatAlign;
	import flash.events.MouseEvent;
	import flash.geom.Rectangle;

	import starling.core.Starling;
	import starling.core.RenderSupport;
	import starling.display.DisplayObject;
	import starling.display.Image;
	import starling.display.Sprite;
	import starling.events.Event;
	import starling.textures.Texture;

	import sample.lua.CModule;
	import sample.lua.__lua_objrefs;
	import Lua;
	
	public class LuaDisplayObject extends DisplayObject
	{
		private var currentGame:LuaGame
		private var renderFunc:String

		public function LuaDisplayObject(currentGame:LuaGame, renderFunc:String)
		{
			trace("LuaDisplayObject ctor");
			super()
			touchable = false
			this.currentGame = currentGame
			this.renderFunc = renderFunc
		}

		public override function getBounds(targetSpace:DisplayObject, resultRect:Rectangle=null):Rectangle {
			return null;
		}

		private function push_objref(o:*):void
		{
			var udptr:int = Lua.push_flashref(currentGame.luastate)
			sample.lua.__lua_objrefs[udptr] = o
		}

		public override function render(support:RenderSupport, alpha:Number):void {
			try {
				Lua.lua_getglobal(currentGame.luastate, renderFunc)
				push_objref(support)
				Lua.lua_callk(currentGame.luastate, 1, 0, 0, null)
			} catch(e:*) {
				currentGame.onError("Error during LuaDisplayObjectGame.render (luastate: "+currentGame.luastate+", func: "+renderFunc+"): " + e.toString())
			}
		}
	}
}

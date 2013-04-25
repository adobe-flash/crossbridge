/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: t; tab-width: 4 -*- */
/*
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
package {
    import avmplus.*;
    import flash.utils.ByteArray;

    public namespace flash_proxy = "flashproxy";

    public class Stats {
        public static var instructions=0;
        public static var timecycles=0;
        public static function toString():String {
            return " instructions:"+instructions+"\n"+
                   " timecycles  :"+timecycles;
        }
    }

    public class Screen {
    
        public static var screenData:Array=new Array();
        public static function logToScreen(msg):void {
            if (screenData.length==0)
                screenData.push(msg);
            else if (screenData.length>0 && screenData[screenData.length-1]!=msg) {
                screenData.push(msg);
            }
        }
        public static function getScreen():String {
            var ret="";
            for (var i=0;i<screenData.length;i++) {
                ret+=screenData[i];
                if (i<screenData.length-1)
                    ret+="\n";
            }
            return ret;
        }
        public static function resetScreen():void {
            screenData=new Array();
        }
     }
     public interface IEventDispatcher {
        function addEventListener(type:String, listener:Function, useCapture:Boolean = false, priority:int = 0, useWeakReference:Boolean = false):void;
        function dispatchEvent(evt:Event):Boolean;
        function hasEventListener(type:String):Boolean;
        function removeEventListener(type:String,listener:Function,useCapture:Boolean=false):void;
        function willTrigger(type:String):Boolean;
    }

    [Event(name="deactivate", type="flash.events.Event")]
    public class EventDispatcher implements IEventDispatcher
    {
        public static var textfields:Array=new Array();
        private var _target:IEventDispatcher;
	    public function EventDispatcher(target:IEventDispatcher=null) {
             _target=target;
        }
  	    public function toString():String
	    {
		     return Object.prototype.toString.call(this);
	    }
        public var _listeners:Array=new Array();
        public var _listenersNames:Array=new Array();
        public var _listenersCalls:Array=new Array();
        public static var _objectlist:Array=new Array();
        public static var _objectmap:Array=new Array();
        public function addEventListener(type:String, listener:Function, useCapture:Boolean=false, priority:int=0, useWeakReference:Boolean=false):void {
            var obj=this;
//            print(""+this+" EventDispatcher.addEventListener(type="+type+" object="+obj+" listener="+listener+")");
            if (_listeners[type]==undefined)
                _listeners[type]=new Array();
            _listeners[type].push(listener);
            var fnd=-1;
            for (var i=0;i<_listenersNames.length;i++) {
                if (type==_listenersNames[i]) { fnd=i; break; }
            }
            if (fnd==-1) {
                _listenersNames.push(type);
                _listenersCalls[type]=0;
            }
            if (obj==null) obj=this;
            var fnd=-1;
            for (var i=0;i<_objectlist.length;i++) {
                if (_objectmap[i]==obj) {
                   fnd=i;
                   break;
                }
            }
            if (fnd==-1) {
                _objectlist.push(""+obj);
                _objectmap.push(obj);
            }
        }
        public function removeEventListener(type:String, listener:Function, useCapture:Boolean=false):void {
            var obj=this;
//            print("EventDispatcher.removeEventListener("+type+") this="+obj);
            try {
                if (_listeners[type]!=null) {
                    for (var i=0;i<_listeners[type].length;i++) {
                        if (_listeners[type][i]==listener) {
//                      print("removing event index "+i);
//                      print("1-"+_listeners[type]);
                        _listeners[type].splice(i,1);
//                      print("2-"+_listeners[type]);
//                      print("3-"+_listeners[type].length);
                        if (_listeners[type].length==0) {
//                          print("1-"+_listenersNames);
                            for (var j=0;j<_listenersNames.length;j++) {
                                if (_listenersNames[j]==type) {
                                    _listenersNames.splice(j,1);
                                    _listenersCalls[type]=-1;
                                    break;
                                }
                            }
//                          print("2-"+_listenersNames+" "+_listenersNames.length);
                        }
                        break;
                    }
                    }
                }
            } catch (e) {
                print("exception in EventDispatcher.removeEventListener: "+e);
                print(e.getStackTrace());
            }
        }
	    public function dispatchEvent(event:Event):Boolean	{
 		    if (event.target && event.target!=null) {
			    return dispatchEventFunction (event.clone());
		    } else {
			    return dispatchEventFunction (event);
		    }
	    }
        public function hasEventListener(type:String):Boolean {
//          print(""+this+" EventDispatcher.hasEventListener()");
            return _listenersCalls[type]!=null;
        }
        public function willTrigger(type:String):Boolean {
//            print(""+this+" EventDispatcher.willTrigger("+type+")");
            return true;
        }
        private function dispatchEventFunction(event:Event):Boolean {
//            print(""+this+" EventDispatcher.dispatchEventFunction("+event.type+")");
            var arr:Array=_listeners[event.type];
            try { _listenersCalls[event.type]++; } catch(e) {}
            if (arr==null) {
  //            print("no listeners for event "+event.type);
            } else {
//            print(""+this+" "+arr.length+" function(s) listening to event "+event.type);
            var arr2=new Array();
            for (var i:int=0;i<arr.length;i++) {
                arr2[i]=arr[i];
            }
            for (var i:int=0;i<arr2.length;i++) {
                if (arr2[i]==null || event==null) {
                    if (arr2[i]==null)
                        print("ERROR: dispatching event "+event.type+" function is null");
                    if (event==null)
                        print("ERROR: event is null");
                } else {  
                    var fnc=arr2[i];
//                    print(""+this+" dispatching event "+event.type+" #"+i+" to "+fnc);
                    try {
                        fnc(event);
                    } catch (e) {
                        print("ERROR dispatching event "+e.getStackTrace());
                    }            
//                    print(""+this+" finished dispatching event "+event.type+" #"+i+" to "+fnc);
                }
            }
//            print(""+this+" finished dispatching "+event.type+" events");
            }
        }
	    public function get listeners():Array {
//            print(""+this+" EventDispatcher.get listeners()");
            return _listeners;
        }
        public function showTextFields() {
            print("showtextfields: "+EventDispatcher.textfields.length);
            for (t in EventDispatcher.textfields) {
                print("  "+t+" "+EventDispatcher.textfields[t]+" text='"+EventDispatcher.textfields[t].text+"'");
            }
        }
        public function showEvents() {
            print("showevents:");
            var elist=EventDispatcher._objectlist;
            var emap=EventDispatcher._objectmap;
            print("events with listeners "+elist.length);
            for (var i=0;i<elist.length;i++) {
                print("["+i+"]="+elist[i]);
                for (var j=0;j<emap[i]._listenersNames.length;j++) {
                    print("  "+emap[i]._listenersNames[j]+
                          " (#func): "+emap[i]._listeners[emap[i]._listenersNames[j]].length+
                          " (#call): "+emap[i]._listenersCalls[emap[i]._listenersNames[j]]+
                         "");
                }
            }
        }
        public function showObjects() {
            print("showobjects:");
            var n:int=0;
            for (var n=0;n<EventDispatcher.eventobjectslist.length;n++) {
                print("  "+n+" = "+EventDispatcher.eventobjectslist[n]);
           }
        }
        public function sendEvent(cmds) {
            var index=cmds[1];
            var name=cmds[2];
            var type=null;
            if (cmds.length>3) {
                type=cmds[3];
            }
            var n=1;
            if (cmds.length>4) {
                n=parseInt(cmds[4]);
            }
            var elist=EventDispatcher._objectlist;
            var emap=EventDispatcher._objectmap;
            var evt=new Event(name);
            if (type!=null) {
                if (type=="timerEvent") {
                    evt=new TimerEvent(name);
                }
            }
            if (emap[index]==undefined) {
                print("ERROR: object "+index+" is undefined");
            } else {
                for (var i=0;i<n;i++) {
                    print("dispatching event "+name+" to "+elist[index]+" "+(i+1)+"/"+n);
                    if (type=="timerEvent")
                        Stats.timecycles++;
                    emap[index].dispatchEvent(evt);
                    if (Screen.getScreen().length>0)
                        print(Screen.getScreen());
                    Screen.resetScreen();
                }
            }
        }
    }
    public class Event {
        public static var COMPLETE="complete";
        public var target;
        public var type;
        public function Event(type,target=null) {
            this.type=type;
            this.target=target;
        }
        public function clone():Event {
            return new Event(type,target);
        }
    }
    public class DisplayObject extends EventDispatcher {
        public var displaylist=new Array();
        public function addChild(obj) {
//        print("addChild: "+obj);
            displaylist.push(obj);
        }
    }
    public class Stage extends DisplayObject {
        public static function getInstance() {
            return stage;
        }
    }
    public class Bitmap {
        public var bitmap;
        public function Bitmap(bitmap) {
            this.bitmap=bitmap;
        }
    }
    public class BitmapData {
        var width,height,b,c;
        public function BitmapData(width,height,b,c) {
//            print("BitmapData width="+width+" height="+height+" b="+b+" c="+c);    
            this.width=width;
            this.height=height;
            this.b=b;
            this.c=c;
        }   
        public function setPixel32(x,y,color) {
        }
        public function lock() {
//            print(""+this+" lock");
        }
        public function unlock() {
//            print(""+this+" unlock");
        }
        public function fillRect(rect:Rectangle,color:uint):void {
//            if (color>0)
//               print(""+this+" fillRect rect="+rect+" color="+color);
        }
        public function toString() {
            return "[object BitmapData width="+width+" height="+height+" b="+b+" c="+c+"]";
        }
    }
    public class Rectangle {
        public function Rectangle(x:Number=0,y:Number=0,width:Number=0,height:Number=0) {
        }
        public var x;
        public var y;
        public var width;
        public var height;
        public function toString() {
            return "[object Rectangle x="+x+" y="+y+" width="+width+" height="+height+"]";
        }
    }
    public class TimerEvent extends Event {
        public static var TIMER="timer";
        public function TimerEvent(type) {
            super(type);
        }
        public function updateAfterEvent():void {
//            print("TimerEvent: updateAfterEvent");
        }
    }
    public class Timer extends EventDispatcher {
        public var running=false;
        public function Timer(t1,t2) {
//           print("Timer constructor "+t1+" "+t2);
        }
        public function start() {
//           print("[timer] start");
           running=true;
        }
        public function stop() {
//           print("[timer] stop");
           running=false;
        }
    }
    public class TextField {
        var _textColor:int;
        var _height:int;
        var _width:int;
        var _x:int;
        var _y:int;
        var _wordWrap:Boolean;
        var _text:String='';
        public function TextField() {
//            print("TextField constructor");
            EventDispatcher.textfields.push(this);
        }
        public function set textColor(color:int):void {
            _textColor=color;
        }
        public function set height(height:int):void {
            _height=height;
        }
        public function set width(width:int):void {
            _width=width;
        }
        public function set x(x:int):void {
            _x=x;
        }
        public function set y(y:int):void {
            _y=y;
        }
        public function set wordWrap(wordWrap:Boolean):void {
            _wordWrap=wordWrap;
        }
        public function set text(value:String):void {
            _text=value;
            //print("set text="+_text);                      
        }
        public function get text():String {
            return _text;
        }
    }
    public class URLLoader extends EventDispatcher {
        public var dataFormat;
        public function load(request:URLRequest) {
//            print("loading "+request.url);
            var ba:ByteArray=File.readByteArray(request.url);
            var evt=new Event("complete");
            evt.target=new Object();
            evt.target.data=ba;
            dispatchEvent(evt);
        }
    }
    public class URLRequest {
        public var url;
        public function URLRequest(url) {
            this.url=url;
        }
    }
    public class URLLoaderDataFormat {
        public static var BINARY="binary";
    }
    public class Proxy {
        public function setProperty(name:*,value:*):void {
        }
        public function getProperty(name:*):* {
        }
        public function callProperty(name,...rest):* {
        }
    }
    public class Sprite extends DisplayObject {
    }
//  main
    var stage:Stage=new Stage();
}
package flash.events {
    public class KeyboardEvent extends Event {
        public var keyCode;
        public static var KEY_UP="up";
        public static var KEY_DOWN="down";
    }
}
package flash.ui {
    public class Keyboard {
        public static const BACKSPACE:uint       = 8;
        public static const CAPS_LOCK:uint        = 20;
        public static const CONTROL:uint         = 17;
        public static const DELETE:uint       = 46;
        public static const DOWN:uint            = 40;
        public static const END:uint             = 35;
        public static const ENTER:uint           = 13;
        public static const ESCAPE:uint          = 27;
        public static const F1:uint              = 112;
        public static const F2:uint              = 113;
        public static const F3:uint              = 114;
        public static const F4:uint              = 115;
        public static const F5:uint              = 116;
       	public static const F6:uint              = 117;
        public static const F7:uint              = 118;
        public static const F8:uint              = 119;
        public static const F9:uint              = 120;
        public static const F10:uint             = 121;
        public static const F11:uint             = 122;
        public static const F12:uint             = 123;
        public static const F13:uint             = 124;
        public static const F14:uint             = 125;
        public static const F15:uint             = 126;
        public static const HOME:uint            = 36;
        public static const INSERT:uint          = 45;
        public static const LEFT:uint            = 37;
        public static const NUMPAD_0:uint        = 96;
        public static const NUMPAD_1:uint        = 97;
        public static const NUMPAD_2:uint        = 98;
        public static const NUMPAD_3:uint        = 99;
        public static const NUMPAD_4:uint        = 100;
        public static const NUMPAD_5:uint        = 101;
        public static const NUMPAD_6:uint        = 102;
        public static const NUMPAD_7:uint        = 103;
        public static const NUMPAD_8:uint        = 104;
        public static const NUMPAD_9:uint        = 105;
        public static const NUMPAD_ADD:uint      = 107;
        public static const NUMPAD_DECIMAL:uint  = 110;
        public static const NUMPAD_DIVIDE:uint   = 111;
        public static const NUMPAD_ENTER:uint    = 108;
        public static const NUMPAD_MULTIPLY:uint = 106;
        public static const NUMPAD_SUBTRACT:uint = 109;
        public static const PAGE_DOWN:uint            = 34;
        public static const PAGE_UP:uint            = 33;
        public static const RIGHT:uint           = 39;
        public static const SHIFT:uint           = 16;
        public static const SPACE:uint           = 32;
        public static const TAB:uint             = 9;
        public static const UP:uint              = 38;
    }
}

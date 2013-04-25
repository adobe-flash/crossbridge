/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package {
    import flash.display.*;
    import flash.net.*;
    import flash.system.*;
    import flash.text.TextField;
    import flash.events.MouseEvent;
    import flash.events.TimerEvent;
    import flash.utils.Timer;
    import flash.utils.*;
    include "num.as";
    use namespace async;


    var workerlist=new Array();
    var promiselist=new Array();
    var promiselist2=new Array();

    public class MySprite extends Sprite {
        public var line:int=0;
        public var xdef:int=25;
        public var ydef:int=25;
        public var hdef:int=20;
        public var wdef:int=800;
        public function addText(text:String):TextField {
            var tf:TextField=addTextField(xdef,ydef+(line*hdef),wdef,hdef,text);
            line++;
            return tf;
        }
        public function addButton(text:String,callback:Function):TextField {
            var button:TextField=addText(text);
            button.border=true;
            button.selectable=false;
            stage.addEventListener(MouseEvent.CLICK,callback,true);
            return button;
        }
        public function addSimpleButton(x:int,y:int,width:int,height:int,text:String,fn:Function):TextField {
            var button:TextField;
            button.border=true;
            button.selectable=false;
            button.addEventListener(MouseEvent.CLICK,fn,true);
            return button;
        }
        public function addTextField(x:int,y:int,width:int,height:int,text:String):TextField {
            var tf:TextField=new TextField();
            tf.x=x;
            tf.y=y;
            tf.width=width;
            tf.height=height;
            tf.text=text;
            addChild(tf);
            return tf;
        }
    }

    public class Overhead extends MySprite {
        public function Overhead() {
            if (Worker.current.isPrimordial()) {
                addText("Information:");
                addText("OS      : "+Capabilities.os);
                addText("screen  : "+Capabilities.screenResolutionX+","+Capabilities.screenResolutionY);
                var debug:String=Capabilities.isDebugger?"debugger":"";
                addText("version : "+Capabilities.version+" "+debug+" "+Capabilities.playerType);
                addText("date    : "+new Date());
                addText("workers : "+nworkers);

                trace("Information:");
                trace("OS      : "+Capabilities.os);
                trace("screen  : "+Capabilities.screenResolutionX+","+Capabilities.screenResolutionY);
                trace("version : "+Capabilities.version+" "+debug+" "+Capabilities.playerType);
                trace("date    : "+new Date());
                trace("workers : "+nworkers);
                trace("startup: "+flash.utils.getTimer());
                for (var i:uint=0;i<nworkers;i++) {
                    workerlist[i]=WorkerDomain.current.createWorkerFromPrimordial();
                    promiselist[i]=workerlist[i].start();
                }
                trace("start: "+flash.utils.getTimer());
                for (var i:uint=0;i<nworkers;i++) {
                    promiselist2[i]=promiselist[i].echo("hello");
//                    promiselist2[i].when(done);
                    var res=promiselist2[i].receive();
                    trace("got result for "+i+": "+res);
                }
                trace("got results: "+flash.utils.getTimer());

/*
                var timer:Timer = new Timer(500,10);
                timer.addEventListener(TimerEvent.TIMER,function(event:TimerEvent):void {
                });
                timer.addEventListener(TimerEvent.TIMER_COMPLETE,function(event:TimerEvent):void {
                    trace("timer complete")
                    for (var i:uint=0;i<nworkers;i++) {
                        trace("[worker "+i+"] stop");
                        workerlist[i].stop();
                    }
                    trace("System.totalMemory: "+System.totalMemory);
                    trace("finished");
                });
                timer.start();
*/
                for (var i:uint=0;i<nworkers;i++) {
                    workerlist[i].stop();
                }
                trace("stopped workers: "+flash.utils.getTimer());
                trace("finished");
            } else {
                trace("created worker "+Worker.current);
            }
        }
        public function done(res:Object,state:uint) {
            trace("done: "+res+" "+state);
        }        
    }
    public function timer_complete(event:TimerEvent):void {
    }
    public function echo(o:Object):Object {
        trace("echo "+Worker.current+" "+o);
        return o;
    }
}
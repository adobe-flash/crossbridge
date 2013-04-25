/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package {
    import flash.utils.ByteArray;
    import flash.net.Socket;
    import flash.display.*;
    import flash.system.*;
    import flash.text.*;
    import flash.events.*;

    var tf;
    var linectr:int=0;
    var contents:String="";
    public function print(...s):void {
        for (var i=0;i<s.length;i++) {
            trace("[flashrunner]"+s[i]);
            contents+=s[i]+"\n";
            if (linectr<50)
                tf.text+=s[i]+"\n";
            linectr++;
        }
    }
    public class template extends Sprite {
        var loader:Loader;
        var context;
        var sock:Socket;
        public function template():void {
            setup();
            loadProgram();
        }
        public function setup():void {
            var versionString=Capabilities.playerType+" "+Capabilities.version;
            loader=new Loader();
            loader.contentLoaderInfo.addEventListener(Event.INIT,initHandler);
            loader.contentLoaderInfo.addEventListener(Event.COMPLETE,completeHandler);
            loader.contentLoaderInfo.addEventListener(IOErrorEvent.IO_ERROR, ioErrorHandler);
            tf=new TextField();
            tf.x=0;tf.y=0;
            tf.width=550;tf.height=400;
            tf.text=versionString+"\n";
            addChild(tf);
        }
        public function sendresults() {
            var server:String="localhost";
            var serverPort:uint=1189;
            var ok=true;
            try {
                sock=new Socket();
                sock.addEventListener(Event.CONNECT,onConnect);
                sock.addEventListener(ProgressEvent.SOCKET_DATA, onData);
                sock.connect(server,serverPort);
            } catch (e) {
                ok=false;
                print("error sending results: "+e)
            }
    //     if (ok) print("sent results successfully");
        }
        private function onConnect(event:Event):void {
            contents="player version: "+Capabilities.version+"\n"+contents;
            if (contents.length>4096) {
                var ptr=0;
                while(ptr<contents.length) {
                    sock.writeUTFBytes(contents.substring(ptr,Math.min(ptr+1000,contents.length)));
                    ptr+=1000;
                }
            } else {
                sock.writeUTFBytes(contents);
            }
            sock.writeByte(13);
            sock.flush();
            sock.close();
    //      print("sent results over socket");
        }
        private function onData(event:ProgressEvent):void {
            message.text='onData event received at '+new Date();
        }
        public function initHandler(event:Event):void {
        }
        public function completeHandler(event:Event):void {
    //      print("loader finished, sending results");
            sendresults();
        }
        public function ioErrorHandler(event:Event):void {
            print("error thrown loading bytes")
        }
        public function loadProgram():void {
            // this line must match exact for code generation
            var bytes=[];//pointer
            var bytearray:ByteArray=new ByteArray();
            for (var i:uint=0;i<bytes.length;i++) {
                bytearray.writeByte(bytes[i]);
            }
            loader.loadBytes(bytearray);
        }
    }
}

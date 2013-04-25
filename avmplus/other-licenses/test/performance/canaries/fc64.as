/*
 *
 * Copyright notice
 *
 * (c) 2005-2006 Darron Schall, Claus Wahlers.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * CW: For your convenience, here are the compiler arguments i use
 *     to compile fc64 for debugging:
 *     -use-network=false 
 *     -default-background-color=0x000000 
 *     -default-frame-rate=120 
 *     -default-size=500,368
 */
package {
    import core.cpu.*;
    import core.memory.*;
    import core.screen.*;
    import core.events.*;
    import core.misc.*;
    import c64.memory.*;
    import c64.screen.*;
    import c64.events.*;

    import flash.events.*;
    import flash.net.*;
    import avmplus.*;

    public class fc64 extends Sprite
    {
        private var cpu:CPU6502;
        private var mem:MemoryManager;
        private var renderer:Renderer;
//      private var sidRenderer:SIDRenderer;
        
        private var loader:URLLoader;
        private var loadROMComplete:Function;
        private var loadROMCompleteID:int;
        private var loadROMCompleteAddr:uint;
        
        private var tf:TextField;
        private var tfBottom:TextField;


        public function fc64() {
            init();
        }


        private function init():void {
            mem = new MemoryManager();
            loadROMs();
        }
        
        private function loadROMs():void {
            loadROM( MemoryManager.MEMBANK_KERNAL, 0xe000, "fc64util/kernel/kernal.901227-03.bin", onKernelComplete);
        }
        
        private function onKernelComplete(event:Event):void {
            loadROM( MemoryManager.MEMBANK_BASIC, 0xa000, "fc64util/kernel/basic.901226-01.bin", onBasicComplete);
        }

        private function onBasicComplete(event:Event):void {
            loadROM( MemoryManager.MEMBANK_CHARACTER, 0xd000, "fc64util/kernel/characters.901225-01.bin", onCharacterComplete);
        }

        private function onCharacterComplete(event:Event):void {
            start();
        }


        private function start():void
        {
            // create and initialize cpu
            cpu = new CPU6502(mem);
            
            // $A483 is the main BASIC program loop
            // set a breakpoint here so we know when the C64 is ready for action
            cpu.setBreakpoint(0xA483, 255);
            
            // initialize keyboard
            mem.cia1.keyboard.initialize(cpu, stage);
            
            // initialize screen renderer
            renderer = new Renderer(cpu, mem);
            renderer.addEventListener("frameRateInfoInternal", onFrameRateInfo);
            renderer.addEventListener("stopInternal", onStop);
            renderer.addEventListener("rasterInternal", onRaster);
            addChild(renderer);
            
            // uncomment to enable sid
            // setupAudio();
            
            // debug info
            tf = new TextField();
            tf.textColor = 0xdddddd;
            tf.height = 284;
            tf.width = 95;
            tf.x = 405;
            addChild(tf);
            tfBottom = new TextField();
            tfBottom.wordWrap = true;
            tfBottom.textColor = 0xdddddd;
            tfBottom.height = 80;
            tfBottom.width = 500;
            tfBottom.y = 285;
            addChild(tfBottom);
            stage.addEventListener(flash.events.KeyboardEvent.KEY_DOWN, keyDownHandler);

            // start main display loop
            renderer.start();
        }
        

        private function loadROM(romid:int, address:uint, url:String, complete:Function):void {
            if (File.exists(url)==false)
                url="canaries/"+url;
            if (File.exists(url)==false)
                url="../../other-licenses/test/performance/"+url;
            if (File.exists(url)==false) {
                print("ERROR: cannot find "+url);
                System.exit(1);
            }
            print("loading ROM "+url);
            var request:URLRequest = new URLRequest(url);
            loadROMComplete = complete;
            loadROMCompleteID = romid;
            loadROMCompleteAddr = address;
            loader = new URLLoader();
            loader.dataFormat = URLLoaderDataFormat.BINARY;
            loader.addEventListener(Event.COMPLETE, onLoadROM);
            loader.load(request);
        }

        private function onLoadROM(event:Event):void {
            mem.setMemoryBank(loadROMCompleteID, loadROMCompleteAddr, event.target.data.length, event.target.data);
            loadROMComplete(event);
        }
        
        private function onRaster(e:RasterEvent):void {
        }
        
        private function onStop(e:DebuggerEvent):void {
            var d:String = "";
            switch(e.breakpointType) {
                case 255:
                    /*
                    // load a ROM
                    mem.addEventListener("complete", onROMLoaded);
                    mem.loadPRG("roms/some_rom.prg");
                    */
                    
                    /*
                    // or add your own test code
                    var asm:ASM6502 = new ASM6502(cpu, mem, 0x0801);
                    d += '.a $0801\n';
                    d += '.b 0F,08,0A,00,97,35,33,32,34,38,2C,32,34,00,1D,08,14,00,97,35,33,32,34,39,2C,35,30,00,32,08,1E,00,97,35,33,32,36,39,2C,31,3A,8F,45,4E,41,42,4C,45,00,4C,08,28,00,97,35,33,32,37,31,2C,31,3A,8F,45,58,50,41,4E,44,2D,56,45,52,54,00,64,08,32,00,97,35,33,32,37,35,2C,31,3A,8F,46,4F,52,45,2F,42,41,43,4B,00,7D,08,3C,00,97,35,33,32,37,36,2C,31,3A,8F,4D,55,4C,54,49,43,4F,4C,4F,52,00,97,08,46,00,97,35,33,32,37,37,2C,31,3A,8F,45,58,50,41,4E,44,2D,48,4F,52,5A,00,AC,08,50,00,97,35,33,32,38,35,2C,37,3A,8F,43,4F,4C,4F,52,31,00,C1,08,5A,00,97,35,33,32,38,36,2C,32,3A,8F,43,4F,4C,4F,52,32,00,D9,08,64,00,97,35,33,32,38,37,2C,38,3A,8F,43,4F,4C,4F,52,4D,41,49,4E,00,E7,08,C8,00,97,32,30,34,30,2C,31,32,38,00,FD,08,2C,01,81,49,B2,38,31,39,32,A4,38,31,39,32,AA,36,33,A9,33,00,07,09,36,01,97,49,2C,38,35,00,14,09,37,01,97,49,AA,31,2C,31,37,30,00,21,09,38,01,97,49,AA,32,2C,32,35,35,00,28,09,40,01,82,49,00,4D,09,90,01,81,49,B2,32,34,A4,31,30,30,3A,97,35,33,32,34,38,2C,49,3A,97,35,33,32,34,39,2C,49,AA,32,36,3A,82,00,75,09,95,01,81,49,B2,31,30,30,A4,32,34,A9,AB,31,3A,97,35,33,32,34,38,2C,49,3A,97,35,33,32,34,39,2C,49,AA,32,36,3A,82,00,7E,09,9A,01,89,34,30,30,00,00,00\n';
                    d += '.a $002B\n';
                    d += '.b 01,08,80,09,87,09,87,09,00,A0\n';
                    asm.compile(d);
                    */
                    
                    // clear the BASIC program loop breakpoint again
                    cpu.clearBreakpoint(0xA483);
                    // and restart renderer
                    renderer.start();
                    break;
                default:
                    d += "stopped (" + e.breakpointType + ")\n";
                    d += "at: $" + Convert.toHex(cpu.pc);
                    tf.text = d;
                    break;
            }
        }

        private function onROMLoaded(e:Event):void {
            //mem.removeEventListener("complete", onROMLoaded);
        }
        
        private function onFrameRateInfo(e:FrameRateInfoEvent):void {
            var d:String = e.frameTime + " ms/frame\n";
            d += e.fps + " fps\n";
            d += Math.round(System.totalMemory / 1024) + " kb";
            tf.text = d;
        }
        
        // Create the SID Renderer and let
        // the normal renderer tell it when to do its thing.
        private function setupAudio():void
        {
//          sidRenderer = new SIDRenderer(mem);         
//          renderer.sidRenderer = sidRenderer;
        }
        
        // dumps out some useful stuff
        // (just for debugging...)
        private function keyDownHandler(event:KeyboardEvent):void {
            switch(event.keyCode) {
                case Keyboard.F11:
                    var startOfBasic:uint = mem.readWord(43);
                    var endOfBasic:uint = mem.readWord(45);
                    var d:String = "";
                    d += "var asm:ASM6502 = new ASM6502(cpu, mem, 0x" + Convert.toHex(startOfBasic,4) + ");\n"
                    d += "d += '.a $" + Convert.toHex(startOfBasic,4) + "\\n';\n";
                    d += "d += '.b ";
                    var i:uint;
                    for(i = startOfBasic; i < endOfBasic; i++) {
                        if(i > startOfBasic) { d += ","; }
                        d += Convert.toHex(mem.read(i));
                    }
                    d += "\\n';\n";
                    d += "d += '.a $002B\\n';\n";
                    d += "d += '.b ";
                    for(i = 0; i < 5; i++) {
                        var w:uint = mem.readWord(0x2b + (i*2));
                        if(i > 0) { d += ","; }
                        d += Convert.toHex(w & 0xff) + ",";
                        d += Convert.toHex((w >> 8) & 0xff);
                    }
                    d += "\\n';\nasm.compile(d);\n";
                    tfBottom.text = d;
                    break;
                case Keyboard.F12:
                    tfBottom.text = mem.vic.toString();
                    tfBottom.appendText(Convert.toHex(mem.cia2.vicBaseAddr, 4));
                    break;
            }
        }
    }
    public function show(cmds) {
        var usage="show events";
        if (cmds.length<2) {
            print(usage);
            return;
        }
        if (cmds[1]=="events") {
            Stage.getInstance().showEvents();
        } else if (cmds[1]=="objects") {
            Stage.getInstance().showObjects();
        } else if (cmds[1]=="textfields") {
            Stage.getInstance().showTextFields();
        } else if (cmds[1]=="stats") {
            print(Stats.toString());
        } else {
            print("unknown command show "+cmds[1]+"\n"+usage);
        }
    }

    /*
     * parameters: 0=event 1=object# 2=name
    */
    public function event(cmds) {
        if (cmds.length<3) {
            print("incorrect usage: event object# eventName");
            return;
        }
        Stage.getInstance().sendEvent(cmds);
    }

    public function hex2dec( hex:String ) : String {
	    var bytes:Array = [];
	    while( hex.length > 2 ) {
		    var byte:String = hex.substr( -2 );
		    hex = hex.substr(0, hex.length-2 );
		    bytes.splice( 0, 0, int("0x"+byte) );
	    }
	    return bytes.join(" ");
    }

    public function d2h( d:int ) : String {
	    var c:Array = [ '0', '1', '2', '3', '4', '5', '6', '7', '8',
			    '9', 'A', 'B', 'C', 'D', 'E', 'F' ];
   	    if( d > 255 ) d = 255;
	    var l:int = d / 16;
	    var r:int = d % 16;
	    return c[l]+c[r];
    }

    public function dec2hex( dec:String ) : String {
	    var hex:String = "0x";
	    var bytes:Array = dec.split(" ");
	    for( var i:int = 0; i < bytes.length; i++ )
		    hex += d2h( int(bytes[i]) );
	    return hex;
    }
    public function startup() {
        var evt=new TimerEvent("timer");
        var object=EventDispatcher._objectmap[4];
        var screen:String="";
        var dots=true;
        var starttime=new Date();
        System.write("loading c64...");
        while (true) {
            object.dispatchEvent(evt);
            Stats.timecycles++;
            screen=Screen.getScreen();
            if (screen.length>0) {
                dots=false;
                print("\nscreen at time "+Stats.timecycles+":\n"+screen);
            }
            if (dots && Stats.timecycles%10==0) {
                System.write(".");
            }
            if (screen.indexOf("_")>-1) {
                break;
            }
            if (Stats.timecycles>200) 
               break;
            Screen.resetScreen();
        }
        if (screen.indexOf("_")>-1) {
            print("metric time "+(new Date()-starttime));
        } else {
            print("ERROR: timed out of 200 time cycles");
        }
    }
    public function main() {
        var f=new fc64();
        var usage="usage: startup, show [events],  event [object#,event,eventType,repeat] exit";
        if (System.argv<1 || System.argv[0]!="-shell") {
            startup();
        } else {
            while (true) {
                System.write("$ ");
                var line=System.readLine();
                cmds=line.split(' ');
                var cmd=cmds[0];
                if (cmd=="exit") {
                    print("exiting...");
                    System.exit(0);
                } else if (cmd=="show") {
                    show(cmds);
                } else if (cmd=="event") {
                    event(cmds);
                } else if (cmd=="startup") {
                    startup();
                } else {
                    print("unknown command: "+cmd+"\n"+usage);
                }
            }
        }
    }
}
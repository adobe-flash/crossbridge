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

package com.adobe.flascc
{
	import flash.utils.Dictionary;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	import com.adobe.flascc.CModule;
	import com.adobe.flascc.ELF;
	import C_Run.ram;
	import C_Run.eax;
	import C_Run.edx;
    import C_Run.st0;
	import C_Run.ESP;

	[ExcludeClass]
	/**
	* flascc debugger helper class
	* debugger relies on fully qualified name of this class
	*/
	public class AlcDbgHelper
	{
		private static var _singleton:AlcDbgHelper;
	
		private var _userGDBPacketPat:RegExp; // if non-null and packet matches, packet goes to user
		private var _userGDBResponse:*; // response to a user gdb packet
		private var _signal:String; // current signal string (defaults to S05)
		private var _contAction:String; // how "continue" should behave

		private var _symFilePrefix:String = "remote:";

        private var _savedPC:int = 0;
        private var _savedSP:int = 0;

        private var _inPrimordialWorker:Boolean;
        // the id of the isolate running this helper, according to fdb
        private var _fdbTID:int = 0; 

        private var _nonStop:Boolean = false;
        
        private var _libraryNotificationsPaused:Boolean = false;

		// transact w/ gdb
		private function gdb(request:*, packetPat:RegExp):*
		{
			if(_inDebugger)
				throw "can't 'gdb' while in debugger";
			_hook = function():AlcDbgContinuation
			{
				return new AlcDbgContinuation(function(dummy:*, dummy1:*):* { _userGDBPacketPat = packetPat; }, ["suspend"], ["gdbPacket", request]);
			}
			debugger();
			var r:* = _userGDBResponse;
			_userGDBResponse = undefined;
			return r;
		}

        private static function inPrimordialWorker():Boolean {
            var flashSysNS:Namespace = new Namespace("flash.system");
            var workerClass:Class = null;
            try {
                workerClass= flashSysNS::["Worker"];
            } catch (e:*) {}
            return !workerClass || workerClass.current.isPrimordial;
        }

        public static function pauseLibraryNotifications():void
        {
            _singleton._libraryNotificationsPaused = true;
        }

        public static function resumeLibraryNotifications():void
        {
            if (_singleton._libraryNotificationsPaused)
            {
                _singleton._libraryNotificationsPaused = false;
                _singleton.sendLibraryNotification();
            }
        }

		// perform a gdb transaction from user code
		public static function gdb(request:*, packetPat:RegExp):*
		{
			return _singleton ? _singleton.gdb(request, packetPat) : undefined;
		}

		// request to break into the debugger
		public static function debugBreak():Boolean
		{
			var result:Boolean = false;

			if(_singleton)
			{
				_singleton._hook = function():AlcDbgContinuation {
					// step out of debugBreak()
					return new AlcDbgContinuation(function(dummy:*):void { result = true; }, ["stepOut"]);
				};
				_singleton.debugger();
			}
			return result;
		}
		
		// have a debugger attached?
		public static function debuggerAttached():Boolean
		{
			return _singleton != null;
		}
			
		// query the debugger from user code
		public static function queryDebugger(request:String, ...args:Array):*
		{
			if(!_singleton)
				throw "no debugger attached";
			if(_singleton._inDebugger)
				throw "internal error: queryDebugger only works from user code";
			
			var response:*;
			
			args.unshift(request); // stick request name on the front
			// set us up the hook
			_singleton._hook = function():AlcDbgContinuation {
				return new AlcDbgContinuation(function(resp:*):void {
					response = resp;
				}, args);
			};
			_singleton.debugger(); // invoke the debugger
			return response;
		}

		public function AlcDbgHelper()
		{
			if(_singleton)
				throw "internal error: debugger already attached";
            _inPrimordialWorker = inPrimordialWorker();
			debugger();
		}

		private function eipFromLocation(fileID:int, lineNo:int):int
		{
			var mod:CModule = fileId2Module[fileID];
			var eip:uint = (mod && mod.getSections()[".text"]) ? 
			    (mod.getSections()[".text"][0] + lineNo) : 0;
            if (bpPtrMap.hasOwnProperty(eip)) {
              eip = bpPtrMap[eip];
            }
            return eip;
		}

        private function locationForAddress(addr:uint):Object
        {
			var mods:Vector.<CModule> = CModule.getModuleVector();
			for each(var mod:CModule in mods)
			{
				var textSect:Array = mod.getSections()[".text"];
				if (!textSect)
				    continue;
				var textStart:uint = textSect[0];
				var textLen:uint = textSect[1];
				
				if(addr >= textStart && addr < (textStart+textLen)) {
                    var ret:Object = {};
                    ret.line = addr - textStart;
                    ret.file = module2FileId[mod];
                    return ret;
                }
            }
            return null;
        }

		private function pathFromFileId(id:int):String
		{
			return queryDebugger("getFileIdPath", id);
		}

		private const fileId2Module:Dictionary = new Dictionary;
		private const module2FileId:Dictionary = new Dictionary;

        private function stopSignal():String
        {
            if (_nonStop) {
                return "Stop:T05thread:" + _fdbTID + ";";
            } else {
                return "S05";
            }
        }

        private function syncStopSignal():String
        {
            if (_nonStop) {
                return "T05thread:" + _fdbTID + ";";
            } else {
                return "S05";
            }
        }

        private var recurGuard:Boolean = false;
		private function notifyScriptInit(mod:CModule):void
		{
            if (_inPrimordialWorker && !recurGuard) {
                recurGuard = true;
                CModule.explicitlyInitAllModules();
            }

            if (module2FileId.hasOwnProperty(mod))
                return;

            var locs:Array = listToArray(queryDebugger("getStackLocationList"));

			module2FileId[mod] = -1; // default to seen, but invalid
            var i:int = 10;
            fileId2Module[int(locs[i])] = mod;
            module2FileId[mod] = int(locs[i]);
            sendLibraryNotification();
		}

		public static function notifyScriptInit(obj:CModule):void
		{
			if(_singleton)
				_singleton.notifyScriptInit(obj);
		}

        private function sendLibraryNotification():void
        {
            // tell gdb we've loaded something
            if (_inPrimordialWorker && !_libraryNotificationsPaused) {
                _signal = "T05library:1;";
                if (_nonStop) {
                    _signal = "Stop:T05thread:" + _fdbTID + ";library:1;";
                } 
                debugBreak();
            }
        }

		// get the debugger's attention
		private function debugger():void
		{
			// dummy throw to attract the debugger's attention
			try { throw new AlcDbgHookRequest; } catch(e:*) {}
		}

		private var _inDebugger:int;
		private var _hook:Function; // we registered for a hook!
		private var _version:String;
		
		// called by debugger
		//*** debugger calls this by name
		public function hook():*
		{
			_inDebugger++;
			try
			{
				if(!_singleton) // attaching
				{
					CModule.recordFixups = true;
					_singleton = this;
                    var rescode:String = _inPrimordialWorker ?
                        "suspend" : "resume";
					return new AlcDbgContinuation(function(ver:String, tid:int, nonStop:Boolean, dummy:*):void {
							_version = ver;
                            _fdbTID = tid;
                            setThreadDebugId(_fdbTID);
                            _nonStop = nonStop;
						},
						["getVersion"], ["getThisThread"], ["getNonStopMode"], 
                            [rescode]);
				}
				if(_hook != null)
				{
					var lhook:Function = _hook;
					_hook = null;
					return lhook();
				}
				// spurious hook (possibly a throw)
				return;
			}
			finally
			{
				_inDebugger--;
			}
		}

		// gdb-style binary data escaping
		private function escapeBA(ba:ByteArray, offset:int = 0, count:int = -1):ByteArray
		{
			var result:ByteArray = new ByteArray;
			if(offset >= ba.length)
				return result;
			ba.position = offset;
			while(count-- && ba.bytesAvailable)
			{
				var b:int = ba.readUnsignedByte();
				// need to escape these values
				if(b == 0x7d || b == 0x23 || b == 0x24 || b == 0x2a)
				{ 
					result.writeByte(0x7d);
					result.writeByte(b ^ 0x20);
				}
				else
					result.writeByte(b);
			}
			return result;
		}

		//*** debugger calls this by name
		public static function hexBA(ba:ByteArray, offset:int = 0, count:int = -1):String
		{
			var result:String = "";
			ba.position = offset;
			while(count-- && ba.bytesAvailable)
			{
				var b:String = ba.readUnsignedByte().toString(16);
				if(b.length == 1)
					result += "0"
				result += b
			}
			return result
		}

		public static function hexStr(str:String):String
		{
			var ba:ByteArray = new ByteArray;
			ba.writeUTFBytes(str);
			return hexBA(ba);
		}

		private function dehexBA(str:String):ByteArray
		{
			var ba:ByteArray = new ByteArray;

			var len:uint = str.length
			for (var i:uint = 0; i < len; i += 2) {
				var c1:int = str.charCodeAt(i)
				var c2:int = str.charCodeAt(i+1)
				var result:int = 0

				if(c1 < 58) result = c1 - 48
				else if(c1 < 71) result = 10 + (c1 - 65)
				else if(c1 < 103) result = 10 + (c1 - 97)

				result *= 16

				if(c2 < 58) result += c2 - 48
				else if(c2 < 71) result += 10 + (c2 - 65)
				else if(c2 < 103) result += 10 + (c2 - 97)
				
				ba.writeByte(result);
			}

			return ba;
		}

		private function dehexStr(str:String):String
		{
			var ba:ByteArray = dehexBA(str);
			ba.position = 0;
			return ba.readUTFBytes(ba.length);
		}

		private function hex(n:uint):String
		{
			return n.toString(16);
		}

        private function hexFloat(n:Number):String
        {
            var ba:ByteArray = new ByteArray();
            ba.endian = "littleEndian";
            ba.writeDouble(n);
            ba.position = 0;
            var ret:String = "";

            for (var i:int = 0; i < 8; i++) {
				var b:String = ba.readUnsignedByte().toString(16);
				if(b.length == 1)
					ret += "0"
                ret += b;
            }
            return ret;
        }

		private function arrayToList(a:Array):String
		{
		  return a.map(function(item:String, index:int, a:Array):String
		    { return item.replace("\\", "\\\\").replace(",", "\\,") } ).join(",");
		}
		
		private function listToArray(l:String):Array
		{
		  return l.match(/(?:\\.|[^,])+/g).map(function(item:String, index:int, a:Array) : String
		    { return item.replace(/\\(.)|([^\\])/g,
		        function(dummy:*, match1:*, match2:*, dummy2:*, dummy3:*):String { return match2 || match1 } ) } );
		}

		private function readReg(n:int):*
		{
			if(!CModule.regsInitted)
				return 0xdddddddd;

			try {
				switch(n) // dwarf numbering
				{
				case 0: // eax
					return eax;
				case 1: // esp (ecx in x86 numbering!)
                    return curStackEBPESPs[curStackOffset*2+1] as Number || ESP;
				case 2: // edx
					return edx;
				case 3: // ei32
					return 0xe132; // TODO
				case 4: // edp (esp in x86 numbering!)
					// debug frame is eip/ef64/ef32/ei32/ebp
					return 0xe2000000-(curStackDepth-curStackOffset)*20;
				case 5: // ebp (or esp if ebp isn't setup yet)
					return curStackEBPESPs[curStackOffset*2]as Number || curStackEBPESPs[curStackOffset*2+1] as Number || ESP;
				case 6: // ef32
					return 0xef32; // TODO
				case 7: // ef64
					return 0xef64; // TODO
				case 8: // eip
                    return eipFromLocation(curStackLocations[curStackOffset*2], curStackLocations[curStackOffset*2+1]);
				case 9: // flags
					return 0;
                case 16: //st0
                    return st0;
				}
			} catch (e:*) {
				// Naked functions might cause some of the above
				// register accesses to fail.
			}
			return 0xcccccccc;
		}

		private function readRegs():ByteArray
		{
			var ba:ByteArray = new ByteArray;
			ba.endian = Endian.LITTLE_ENDIAN;
			for(var n:uint = 0; n <= 9; n++)
				ba.writeInt(readReg(n));
			return ba;
		}

		private const hostIOFiles:Dictionary = new Dictionary;

		private function hostIOFileFromByteArray(ba:ByteArray):int
		{
			for(var i:int = 1; hostIOFiles[i]; i++) {}
			ba.position = 0;
			hostIOFiles[i] = ba;
			return i;
		}

		private function handleFilePacket(packet:String):*
		{
			var args:Array
			var ba:ByteArray

			if(packet.substr(0, 6) == "close:")
			{
				var id:int = int("0x"+packet.substr(6));
				hostIOFiles[id] = null;
				delete hostIOFiles[id];
				return "F0";
			}
			else if(packet.substr(0, 6) == "pread:")
			{
				// fd,count,offset
				args = packet.substr(6).split(",");
				ba = hostIOFiles[int("0x"+args[0])];

				if(!ba)
					return "F-1,2"; // TODO better error

				var pos:int;
				var esc:ByteArray = escapeBA(ba, pos = int("0x"+args[2]), Math.min(10240, int("0x"+args[1])));
				var response:ByteArray = new ByteArray;
				response.writeUTFBytes("F" + hex(ba.position - pos) + ";");
				response.writeBytes(esc);
				return response;
			}
			else if(packet.substr(0, 5) == "open:")
			{
				// name(hex),false,mode
				args = packet.substr(5).split(",");
				var path:String = dehexStr(args[0]);
				var symMatch:Array = /^([0-9]+)\.elf/.exec(path);

				if(symMatch) // "/DDD" where DDD is a decimal number is interpreted a request sym data for module DDD
				{
					var index:int = symMatch[1];
					var mods:Vector.<CModule> = CModule.getModuleVector();

					if(index >= 0 && index < mods.length)
					{
						var mod:CModule = mods[index];

						if(typeof(module2FileId[mod]) != "undefined") // mod.getSymsUsingMD is intrusive so wait until initted
						{
							ba = ELF.createFromSyms(mod.getSymsUsingMD(), mod.getSections(), ram, mod.getFixups());
							return "F"+hex(hostIOFileFromByteArray(ba));
						}
					}
				}
				return "F-1,2"; // error, ENOENT
			}
			return "";
		}

		// "v" packets
		private function handleGDBPacket_v(packet:String):*
		{
			if(packet.substr(0, 5) == "File:")
				return handleFilePacket(packet.substr(5));
            // TODO: should ask bridge if any other threads have stopped
            if(packet.substr(0, 7) == "Stopped")
                return "OK";
            if(packet.substr(0, 5) == "Cont?")
                return "vCont;c;C;s;S;t";
            /*
            if(packet.substr(0, 4) == "Cont") 
            {
                // TODO: clearing the signal here is too simplistic, since
                // gdb might be asking to continue a different thread, or
                // not continue at all
                _signal = null;
				return new AlcDbgContinuation(function(dummy:String):void {}, 
                    ["handleVCont", packet.substr(4)]);
            }
            */
			return "";
		}

		// read a word from the debug frame area
		private function readDFWord(ptr:uint):int
		{
			var index:uint = curStackDepth-((uint(0xe2000000)-ptr)/20);

			if(index < curStackDepth)
			{
				var offs:uint = 20-((uint(0xe2000000)-ptr)%20);
			
				switch(offs)
				{
				// EBP
				case 4: return curStackEBPESPs[index*2] as Number || curStackEBPESPs[index*2+1] as Number || ESP;
				// EI32
				case 8: return 0x2e132;
				// EF32
				case 12: return 0x2ef32;
				// EF64
				case 16: return 0x2ef64;
				// EIP
				case 20: return 1+eipFromLocation(curStackLocations[index*2], curStackLocations[index*2+1]); // add 1 because this is where gdb things it will restart after being returned to
				}
			}
			return 0;
		}

		private function readMem(ptr:uint, len:uint):ByteArray
		{
			if(!CModule.regsInitted)
				return new ByteArray;
			var ptrStart:uint = ptr & ~3; // align to 4
			var ptrCur:uint = ptrStart;
			var ptrEnd:uint = ptr+len;
			var ba:ByteArray = new ByteArray;

			ba.endian = "littleEndian";

			// don't read "text"
			if(ptrEnd > 0xf0000000)
			{
				ptrEnd = 0xf0000000;
				len = ptrEnd-ptr;
			}
			if(ptrCur >= ptrEnd)
				return ba;

			// copy any domainMemory requested
			if(ptrCur < ram.length)
			{
				var n:int = Math.min(ptrEnd-ptrCur, ram.length-ptrCur);
				ba.writeBytes(ram, ptrCur, n);
				ptrCur += n;
				while(ptrCur & 3)
				{
					ba.writeByte(0);
					ptrCur++;
				}
			}
			// ptrCur aligned

			// pad to debug frames
			while(ptrCur < ptrEnd && ptrCur < uint(0xe0000000))
			{
				ba.writeInt(0);
				ptrCur += 4;
			}
			// fill in debug frame info
			while(ptrCur < ptrEnd && ptrCur < uint(0xe2000000))
			{
				ba.writeInt(readDFWord(ptrCur));
				ptrCur += 4;
			}

			// build un-aligned result (only seems to happen when reading single bytes in practice)
			var result:ByteArray;

			if(ptr != ptrStart && ba.length)
			{
				result = new ByteArray;
				result.writeBytes(ba, ptr-ptrStart);
			}
			else
				result = ba;
			result.length = len;
			return result;
		}

		// write register
		private function handleGDBPacket_P(packet:String):*
		{
			var args:Array = packet.split(/=/);
			var n:uint = args[0];
			var r:uint = uint("0x"+args[1]);
			var val:ByteArray = dehexBA(args[1]);
			var result:String = "E01";
			
			// write register
            val.position = 0;
			val.endian = "littleEndian";
            var i:uint = val.readUnsignedInt();
            result = "OK";

            // eip
            if (n == 8) {
                _savedPC = i;
            } else if (n == 4) {
                _savedSP = i;
            }

			return result;
		}

		// write registers
		private function handleGDBPacket_G(packet:String):*
		{
			var result:String = "E01";
			
			// write registers

			return result;
		}

		// select thread
		private function handleGDBPacket_H(packet:String):*
		{
            var tid:int = int(packet.substr(1));
			var result:String = "OK";

            if (tid > 0) {
                var fdbtid:int = getThreadDebugId(tid);
                return new AlcDbgContinuation(
                    function (dummy:*):* {
                        return "OK";
                    }, ["setThread", fdbtid]); 
            }
			
			return result;
		}

		// write memory
		private function handleGDBPacket_M(packet:String):*
		{
			var args:Array = packet.split(/[,:]/);
			var ptr:uint = uint("0x"+args[0]);
			var len:uint = uint("0x"+args[1]);
			var val:ByteArray = dehexBA(args[2]);
			var result:String = "E01"; // TODO better error

			if(!CModule.regsInitted)
				return result;
			if(ptr < ram.length)
			{
				if((ptr + len) > ram.length)
					len = ram.length - ptr; // partial write => error (but still write!) according to some guy! http://www.it.uom.gr/teaching/gcc_manuals/onlinedocs/gdb_33.html
				else
					result = "OK";
				ram.position = ptr;
				ram.writeBytes(val, 0, len);
			}
			return result;
		}

		// read memory
		private function handleGDBPacket_m(packet:String):*
		{
			var args:Array = packet.split(",");
			var ptr:uint = uint("0x"+args[0]);
			var len:uint = uint("0x"+args[1]);
			return hexBA(readMem(ptr, len));
		}

		private function handleXferPacket(packet:String):*
		{
			if(packet.substr(0, 16)  == "libraries:read::")
			{
				var args:Array = packet.substr(16).split(",");
				var rxml:XML = <library-list/>;
				var mods:Vector.<CModule> = CModule.getModuleVector();

				for(var i:int = 0; i < mods.length; i++)
				{
					var mod:CModule = mods[i];
					if(typeof(module2FileId[mod]) != "undefined")
						rxml.library += <library name={_symFilePrefix+i+".elf"}><segment address="0x00000000"/></library>;
				}

				var ba:ByteArray = new ByteArray;
				ba.writeUTFBytes(rxml);

				var start:int = int("0x"+args[0]);

				if(start > ba.length)
					return "E01"; // TODO better error

				var result:ByteArray = new ByteArray;

				result.writeUTFBytes("m");
				result.writeBytes(escapeBA(ba, start, int("0x"+args[1])));
				return (result.length > 1) ? result : "l";
			}
			return "";
		}

		private function AS3BackTrace():String
		{
			var st:Array = ((new Error).getStackTrace()).split(/\n/s).reverse().slice(0, curStackDepth).reverse();
			st = st.map(function(s:String, i:int, a:Array):String {
				return s.replace(/^\s+at\s+/g, (i==curStackOffset)?"(*)":"   ");
			});
			return st.join("\n") + "\n";
		}

		// evaluate an expression
		private function eval(exp:String):String
		{
			return curFrameLocals[exp]; // XXX TODO
		}

		// handle an "eval" request
		private function handleEval(cmd:String):*
		{
			// build up list of thises
			if(!curStackThises)
				return new AlcDbgContinuation(
					function(thises:Array):* { curStackThises = thises; return handleEval(cmd); }, ["getStackThisArray"]);
			// ensure we have locals for the current frame
			if(!curFrameLocals)
				return new AlcDbgContinuation(
					function(argNameList:String, localNameList:String):* {
						var argNames:Array = listToArray(argNameList);
						var localNames:Array = listToArray(localNameList);
				return new AlcDbgContinuation(
					function(argValues:Array, localValues:Array):* {
						curFrameLocals = {};
						var i:int;
						for(i = 0; i < argNames.length; i++)
							curFrameLocals[argNames[i]] = argValues[i];
						for(i = 0; i < localNames.length; i++)
							curFrameLocals[localNames[i]] = localValues[i];
						curFrameLocals["this"] = curStackThises[curStackOffset];
						return handleEval(cmd);
					}, ["getFrameArgumentValueArray", curStackOffset, argNameList],
						["getFrameLocalValueArray", curStackOffset, localNameList]);
					}, ["getFrameArgumentNameList", curStackOffset], ["getFrameLocalNameList", curStackOffset]);
			return hexStr(eval(cmd) + "\n"); // TODO
		}

		private function handleRcmdPacket(cmd:String):*
		{
			var cmdStr:String = dehexStr(cmd);
			var m:Array;

			if((m = cmdStr.match(/^setSymFilePrefix (.*)/)))
				_symFilePrefix = m[1];
			else if(cmdStr == "exit")
			{
				try {
					var ns:Namespace = new Namespace("flash.system");
					var sclass:Object = ns::["System"];
					sclass.exit(-1);
				} catch(e:*) {
					return hexStr(e + "\n");
				}
				return "E01"; // shouldn't get here...
			}
			else if(cmdStr == "down")
			{
				curFrameLocals = null;
				if(--curStackOffset < 0)
				{
					curStackOffset = 0;
					return hexStr("Bottom (i.e., innermost) AS3 frame selected; you cannot go down.\n");
				}
			}
			else if(cmdStr == "up")
			{
				curFrameLocals = null;
				if(++curStackOffset >= curStackLocals.length)
				{
					curStackOffset = curStackLocals.length-1;
					return hexStr("Initial AS3 frame selected; you cannot go up.\n");
				}
			}
			else if((m = cmdStr.match(/^eval (.*)/)))
				return handleEval(m[1]);
			else if(cmdStr == "locals")
			{
				return new AlcDbgContinuation(function(nameList:String):AlcDbgContinuation {
					return new AlcDbgContinuation(function(typeList:String, values:Array):String {
						var r:String = "";
						var names:Array = listToArray(nameList);
						var types:Array = listToArray(typeList);
						for(var i:int = 0; i < names.length; i++)
						{
							var val:String = values[i];
							if(val && val.length > 80)
								val = "(use 'monitor eval " + names[i] + "' to see this local's value)";
							r += names[i] + ":" + types[i] + " = " + val + "\n";
						}
			
						return hexStr(r) || "OK";
					}, ["getFrameLocalTypeList", curStackOffset, nameList],
						["getFrameLocalValueArray", curStackOffset, nameList]);
				}, ["getFrameLocalNameList", curStackOffset]);
			}
			else if(cmdStr == "args")
			{
				return new AlcDbgContinuation(function(nameList:String):AlcDbgContinuation {
					return new AlcDbgContinuation(function(typeList:String, values:Array):String {
						var r:String = "";
						var names:Array = listToArray(nameList);
						var types:Array = listToArray(typeList);
						for(var i:int = 0; i < names.length; i++)
						{
							var val:String = values[i];
							if(val && val.length > 80)
								val = "(use 'monitor eval " + names[i] + "' to see this argument's value)";
							r += names[i] + ":" + types[i] + " = " + val + "\n";
						}
			
						return hexStr(r) || "OK";
					}, ["getFrameArgumentTypeList", curStackOffset, nameList],
						["getFrameArgumentValueArray", curStackOffset, nameList]);
				}, ["getFrameArgumentNameList", curStackOffset]);
			}
			else if(cmdStr == "bt")
				return hexStr(AS3BackTrace());
			else if(cmdStr == "setContinueActionOut")
				_contAction = "stepOut";
			else if(cmdStr == "setContinueActionOver")
				_contAction = "stepOver";
			else if(cmdStr == "setContinueActionInto")
				_contAction = "stepInto";
			else if(cmdStr == "setContinueActionResume")
				_contAction = "resume";
			else if(cmdStr == "setContinueActionNop")
				_contAction = "*nop*";
			else
				return "E01";
			return "OK";
		}

		private function handleGDBPacket_q(packet:String):*
		{
			if(packet.substr(0, 10) == "Supported:")
			{
				var args:Array = packet.substr(10).split(";");
				// TODO reply about args?
				return "qXfer:libraries:read+;QStartNoAckMode+;QNonStop+;PacketSize=2800";
			}
			else if(packet.substr(0, 5) == "Xfer:")
				return handleXferPacket(packet.substr(5));
			else if(packet.substr(0, 5) == "Rcmd,")
				return handleRcmdPacket(packet.substr(5));
            else if(packet.substr(0, 8) == "Attached")
            {
                return "0";
            }
            else if(packet.substr(0, 1) == "C")
            {
				return new AlcDbgContinuation(function(tid:*):String { return "QC" + tid; }, ["getThread"]);
            }
            else if(packet.substr(0, 7) == "TStatus") 
            {
                //return "T0";
            }
			return "";
		}

		private function handleGDBPacket_Q(packet:String):*
		{
			if(packet.substr(0, 14) == "StartNoAckMode")
			{
				return new AlcDbgContinuation(function(dummy:*):String { return "OK"; }, ["setNoAckMode"]);
			}
            else if(packet.substr(0, 7) == "NonStop")
            {
                if (packet.substr(8, 9) == "1") {
                    _nonStop = true;
                    return new AlcDbgContinuation(function(dummy:*):String { return "OK"; }, ["setNonStopMode"]);
                }
                return "OK";
            }
			return "";
		}

		// map breakpoint "ptrs" to locations
		private const bpMap:Dictionary = new Dictionary; // more than 1 "ptr" can map to the same location
		// map breakpoint gdb ptrs to actual ptrs
		private const bpPtrMap:Dictionary = new Dictionary; // more than 1 "ptr" can map to the same location
		// map locations to ref counts
		private const bpRC:Dictionary = new Dictionary; // refcount locations as more than 1 ptr can mape to the same one
		// lazy breakpoints to be set when module loads
		private const bpLazy:Dictionary = new Dictionary;

		// clear breakpoint
		private function handleGDBPacket_z(packet:String):*
		{
			var args:Array = packet.split(",");
			if(args[0] != "0")
				return "E01";
			if(args[2] != "1")
				return "E01";
			var addr:uint = uint("0x"+args[1]);
			var loc:* = bpMap[addr];
			if(!loc)
			{
				if(bpLazy[addr])
				{
					delete bpLazy[addr];
					return "OK";
				}
				return "E01";
			}
			delete bpMap[addr];
            var la:Array = listToArray(loc);
            var fileId:int = la[0];
            var lineNo:int = la[1];
			//var mod:CModule = (fileId2Module.hasOwnProperty(fileId)) ? 
            //    fileId2Module[fileId] : null;
			var mod:CModule = fileId2Module[fileId];
            var delAddr:int = 0;
            if (mod && mod.getSections().hasOwnProperty(".text")) {
                var textSect:Array = mod.getSections()[".text"];
                var textStart:uint = textSect[0];
                var act:uint = textStart + uint(la[1]);
                delAddr = act;
            }
			if(!--bpRC[loc])
			{
				delete bpRC[loc];
				return new AlcDbgContinuation(function(dummy:*):* { 
                    if (delAddr) {
                        return new AlcDbgContinuation(function(dummy:*):* {
                            return "OK";
                        }, ["clearBreakpointAddress", delAddr]);
                    } else {
                        return "OK";
                    }
                }, ["clearBreakpoint", fileId, lineNo]);
			}
			return "OK";
		}

		// set breakpoint
		private function handleGDBPacket_Z(packet:String):*
		{
			var args:Array = packet.split(",");
			if(args[0] != "0")
				return "E01";
			if(args[2] != "1")
				return "E01";
			var addr:uint = uint("0x"+args[1]);
			var mods:Vector.<CModule> = CModule.getModuleVector();
			for each(var mod:CModule in mods)
			{
				if(typeof(module2FileId[mod]) == "undefined") // skip uninitialized modules (breakpoint may become lazy)
					continue;
				var textSect:Array = mod.getSections()[".text"];
				if (!textSect)
				    continue;
				var textStart:uint = textSect[0];
				var textLen:uint = textSect[1];
				
				if(addr >= textStart && addr < (textStart+textLen))
					return new AlcDbgContinuation(function(loc:String):*
					{
						if(!loc)
							return "E01";
                        var la:Array = listToArray(loc);
                        var act:uint = textStart + uint(la[1]);
						bpMap[addr] = loc;
						bpRC[loc] = int(bpRC[loc]) + 1;
                        return new AlcDbgContinuation(function(dummy:*):* {
                            return "OK";
                        }, ["recordBreakpointAddress", addr, act]); 
					}, ["setBreakpoint", module2FileId[mod], addr-textStart]);
			}
			// record it to set later!
			bpLazy[addr] = 1;
			return "OK";

		}

		private var curStackDepth:uint;
		private var curStackOffset:uint;
		private var curStackLocations:Array;
		private var curStackEBPESPs:Array;
		private var curStackLocals:Array;
		private var curStackThises:Array;
		private var curFrameLocals:Object;

		// debugger calls to handle a break
		//*** debugger calls this by name
		public function handleBreak():*
		{
			// check for pending breakpoints
			// TODO OPTOPT this is really inefficient...
			var mods:Vector.<CModule> = CModule.getModuleVector();
			for each(var mod:CModule in mods)
			{
				if(typeof(module2FileId[mod]) == "undefined")
					continue;

				var textSect:Array = mod.getSections()[".text"];
				if (!textSect)
					continue;
				
				var textStart:uint = textSect[0];
				var textEnd:uint = textStart+textSect[1];

				for(var addrs:String in bpLazy)
					var addr:uint = uint(addrs);
					if(addr >= textStart && addr <= textEnd) // found one!
						return new AlcDbgContinuation(function(loc:String):* {
							delete bpLazy[addr];
							if(loc) // TODO any way to communicate failure here?
							{
								bpMap[addr] = loc;
                                var la:Array = listToArray(loc);
                                var act:uint = textStart + uint(la[1]);
								bpRC[loc] = int(bpRC[loc]) + 1;
                                return new AlcDbgContinuation(
                                        function(dummy:*):* {
                                    // continue w/ break handling
                                    return handleBreak();
                                    }, ["recordBreakpointAddress", addr, act]); 
							} else {
                                return handleBreak();
                            }
						}, ["setBreakpoint", module2FileId[mod], addr-textStart]);
			}
			// this info now invalid...
			curStackLocations = null;
			if(!_userGDBPacketPat)
			{
				// send TRAP signal
                var op:String = _nonStop ? "gdbNotify" : "gdbPacket";
				return new AlcDbgContinuation(function(dummy:*):* {},
					[op, (_signal || stopSignal())]);
			}
			return undefined; // do nothing for user transaction
		}

		// debugger calls to handle certain gdb remote commands
		//*** debugger calls this by name
		public function handleGDBPacket(packet:String):*
		{
			// deal w/ user gdb transaction
			if(_userGDBPacketPat && _userGDBPacketPat.test(packet))
			{
				_userGDBPacketPat = null;
				_userGDBResponse = packet;
				return new AlcDbgContinuation(function(dummy:*):void {}, ["resume"]);
			}

			try
			{
				_inDebugger++;
				// ensure we have up-to-date frame info
				if(!curStackLocations)
					return new AlcDbgContinuation(function(locs:String, ebpesps:Array, localCounts:String, bpAddrs:Array):* {
					curStackLocations = listToArray(locs);
					curStackEBPESPs = ebpesps;
					curStackLocals = listToArray(localCounts);
					curStackDepth = curStackLocals.length;
					curStackOffset = 0;
					curStackThises = null;
					curFrameLocals = null;

                    for (var i:int = 0; i < bpAddrs.length; i += 2) {
                        bpPtrMap[bpAddrs[i + 1]] = bpAddrs[i];
                    }

					return handleGDBPacket(packet); }, ["getStackLocationList"], ["getStackSpecificLocalArray", "ebp,esp"], ["getStackLocalCountList"], ["getBreakpointAddresses"]);
	
				switch(packet.substr(0, 1))
				{
					case "k":
						return undefined;
					case "v":
						return handleGDBPacket_v(packet.substr(1));
					case "c": // continue
					{
						var contAction:String = _contAction;
						_contAction = null;
                        if (_savedPC) {
                            var savedPC:int = _savedPC;
                            _savedPC = 0;
                            var loc:Object = locationForAddress(savedPC);
                            var locStr:String = loc.file + "," + loc.line;

                            var argsize:int = ESP - _savedSP;
                            CModule.alloca(argsize);

                            return new AlcDbgContinuation(
                                function(dummy:*):* { 
                                    ESP = _savedSP;

                                    return stopSignal();
                                }, ["callInferiorFunc", loc.file, loc.line,
                                    _savedSP]
                            );
                        }
						if(contAction == "*nop*") {
                            if (_nonStop) {
                                return new AlcDbgContinuation(
                                    function(dummy:*):* {},
                                    ["gdbNotify", stopSignal()]);
                            } else {
                                return stopSignal();
                            }
                        }
						_signal = null;
						return new AlcDbgContinuation(function(dummy:*):void {}, [contAction ? contAction : "resume"]);
					}
					case "s": // step
						if(contAction == "*nop*") {
                            if (_nonStop) {
                                return new AlcDbgContinuation(
                                    function(dummy:*):* {},
                                    ["gdbNotify", stopSignal()]);
                            } else {
                                return stopSignal();
                            }
                        }
						_signal = null;
						return new AlcDbgContinuation(function(dummy:*):void {}, ["stepInto"]);
                    case "t": // stop (part of vCont packet)
                        return stopSignal();
					case "?": // get last signal
						return (_signal || syncStopSignal()); // TRAP signal
					case "p": // read register
                        var reg:int = int("0x"+packet.substr(1));
                        if (reg == 16) {
                            return hexFloat(readReg(16));
                        } 
						return hex(readReg(reg));
					case "g": // read registers
						return hexBA(readRegs());
					case "P": // write register
						return handleGDBPacket_P(packet.substr(1));
					case "G": // write registers
						return handleGDBPacket_G(packet.substr(1));
                    case "H": // select thread
                        return handleGDBPacket_H(packet.substr(1));
					case "M": // write memory
						return handleGDBPacket_M(packet.substr(1));
					case "m": // read memory
						return handleGDBPacket_m(packet.substr(1));
					case "q": // queries
						return handleGDBPacket_q(packet.substr(1));
					case "Q": // sets
						return handleGDBPacket_Q(packet.substr(1));
					case "Z": // set bkpt
						return handleGDBPacket_Z(packet.substr(1));
					case "z": // clear bkpt
						return handleGDBPacket_z(packet.substr(1));
				}
				return "";
			}
			finally
			{
				_inDebugger--;
			}
		}

        private static function getThreadDebugId(alcThreadId:int):int {
            if (workerDomainClass) {
                for (var i:int = 0;
                        i < workerDomainClass.listWorkers().length;
                        i++) {
                    var w:* = workerDomainClass.listWorkers()[i];
                    var id:* = w.getSharedProperty(
                        "flascc.threadId");
                    if (id == alcThreadId) {
                        return w.getSharedProperty("flascc.fdbThreadId");
                    }
                }
            } 
            return -1;
        }

        private static function setThreadDebugId(tid:int):void {
            var flashSysNS:Namespace = new Namespace("flash.system");
            var workerClass:Class = null;
            try {
                workerClass= flashSysNS::["Worker"];
            } catch (e:*) {}
            if (workerClass) {
                workerClass.current.setSharedProperty("flascc.fdbThreadId", 
                                                        tid);
            }
        }

        private static function get workerDomainClass():* {
            try {
                var flashSysNS:Namespace = new Namespace("flash.system");
                return flashSysNS::["WorkerDomain"].current;
            } catch (e:*) {
                return null;
            }
        }
    
	}	
}

[ExcludeClass]
/**
* debugger relies on base name of this class
* @private
*/
class AlcDbgHookRequest {}

[ExcludeClass]
/**
* debugger relies on base name of this class
* @private
*/
class AlcDbgContinuation
{
    import com.adobe.flascc.AlcDbgHelper;
	public var fun:Function;
	public var queries:Array; //*** debugger accesses this by name
    public var queryString:String; //*** debugger accesses this by name

	public function AlcDbgContinuation(fun:Function, ...queries:Array)
	{
		this.fun = fun;
		this.queries = queries;
        this.queryString = makeQueryString(queries);
	}

    private static function makeQueryString(queries:Array):String
    {
        var str:String = "";
        for (var i:int = 0; i < queries.length; i++) {
            //str += queries[i][0] + '#!@#' + 'a#!@#';
            str += AlcDbgHelper.hexStr(queries[i][0]) + ':a:';
            for (var j:int = 1; j < queries[i].length; j++) {
                str += AlcDbgHelper.hexStr(queries[i][j]) + ':';
                //str += queries[i][j] + '#!@#';
            }
            str += 'b';
            str += '#';
        }
        return str;
    }
	
	//*** debugger calls this by name
	public function exec(...responses:Array):*
	{
		return fun.apply(null, responses);
	}
}


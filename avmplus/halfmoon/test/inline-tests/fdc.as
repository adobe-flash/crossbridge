/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//way cut down version of acceptance/as3/Definitions/Classes/ClassDef/FinalDefaultClass.as

//env INLINE=1 MODE=4 DEBUG_PRINT_INLINES=1 /home/mzaleski/adobe/workspace-halfmoon/halfmoon/objdir/shell/avmshell  -Dnodebugger fdc.abc
//Assertion failed: "((address < (char*)container + Size(container)))" ("../MMgc/WriteBarrier-inlines.h":199)

//env INLINE=1 MODE=4 DEBUG_PRINT_INLINES=1  DEBUG_INLINE_SKIP=FinalDefaultClass/testGetSetPrivStatString shell/avmshell  -Dnodebugger fdc.abc
//runs.

package FinalDefaultClassPackage {

    final class FinalDefaultClass {
		//TODO: jit segv's instead of mmgc assert if this line commented out
		static var statFunction:Function;           // Default Static property
		//TODO: jit segv's instead of mmgc assert if this line commented out
        internal static var internalStatFunction:Function;          // Internal Static property

        private static var privStatString:String;               // Private Static property

        private static function setPrivStatString(s:String) { /*trace("before setting privStatString in setPrivStatString()");*/ privStatString = s; }
        private static function getPrivStatString() : String { /*trace("before getting privStatString in getPrivStatString()");*/ return privStatString; }

        // inlining this guy causes problem
		// 
        public function testGetSetPrivStatString(s:String) : String {
			//trace("enter testGetSetPrivStatString")
            setPrivStatString(s);
			//trace("have returned from setPrivStatString.. never get here if inlined")
			//trace(privStatString);*/
			return getPrivStatString();
        }
    }
    
    public class FinalDefaultClassAccessor {
        private var Obj:FinalDefaultClass = new FinalDefaultClass();

        public function testGetSetPrivStatString(s:String) : String {
			//trace("about to call Obj.testGetSetPrivStatString(s) -- which asserts when inlined if setPrivStatString inlined also")
            var rc: String = Obj.testGetSetPrivStatString(s);
			//trace(rc)
			return rc
        }
		static public function failsInlining(){
			var fdca = new FinalDefaultClassAccessor();
			var str = "Test";
			fdca.testGetSetPrivStatString(str)
			trace("finished")
		}
    }
 
   
}


import FinalDefaultClassPackage.*;

// function failsInlining(){
// 	var Obj = new FinalDefaultClassAccessor();
// 	var str = "Test";
// 	Obj.testGetSetPrivStatString(str)
// 	}
	
//env INLINE=1 MODE=4 DEBUG_PRINT_INLINES=1 /home/mzaleski/adobe/workspace-halfmoon/halfmoon/objdir/shell/avmshell  -Dnodebugger fdc.abc
//Assertion failed: "((address < (char*)container + Size(container)))" ("../MMgc/WriteBarrier-inlines.h":199)

//env INLINE=1 MODE=4 DEBUG_PRINT_INLINES=1  DEBUG_INLINE_SKIP=FinalDefaultClass/testGetSetPrivStatString shell/avmshell  -Dnodebugger fdc.abc
//runs.
//failsInlining()

FinalDefaultClassAccessor.failsInlining()


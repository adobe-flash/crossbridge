/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package avmplus
{
    import flash.utils.ByteArray

    // this namespace exists solely so that nativegen.py will encounter them during shell building,
    // thus giving us a test case for parsing methods with custom namespaces in nativegen.py
    namespace ns_example = "http://www.example.com"

    // this interface exists solely so that nativegen.py will encounter them during shell building,
    // thus giving us a test case for interface parsing in nativegen.py
    interface ITest
    {
        function test();
    }
    class CTest //implements ITest
    {
        public function test() { }
    }

    // this class exists solely to test native classes that use MI.
    [native(cls="::avmshell::MIClass", instance="::avmshell::MIObject", methods="::avmshell::MIObjectImpl")]
    internal class MI
    {
        public native function plus(i:Number):Number;
    }

    [native(cls="::avmshell::SystemClass", classgc="exact", methods="auto", construct="none")]
    public class System
    {
        // this metadata exists solely so that nativegen.py will encounter it during shell building,
        // thus giving us a test case for metadata parsing in nativegen.py
        [foo]
        public native static function exit(status:int):void

        // this metadata exists solely so that nativegen.py will encounter it during shell building,
        // thus giving us a test case for metadata parsing in nativegen.py
        [bar(attr="whatever")]
        public native static function exec(command:String):int
	    /**
	    * Do nothing in this process for some time.
	    *
	    * @param ms  A timeout value in milliseconds. Negative values are taken to be zero.
	    */
	    public native static function sleep(ms:int): void;
        public native static function getAvmplusVersion():String
        public native static function getFeatures():String
        public native static function getRunmode():String
        public native static function trace(a:Array):void
        public native static function write(s:String):void
        public native static function debugger():void
        public native static function isDebugger():Boolean
        public native static function getNanosecondTimer():Number
        public native static function getTimer():uint
        private native static function getArgv():Array
        public static const argv:Array = getArgv();
        private native static function getEnviron():Array
        public static const environ:Array = getEnviron();
        public native static function readLine():String;

        // Amount of real memory we've aqcuired from the OS
        public native static function get totalMemory():Number;

        // Part of totalMemory we aren't using
        public native static function get freeMemory():Number;

        // process wide size of resident private memory
        public native static function get privateMemory():Number;

        // Initiate a garbage collection; future versions will not return before completed.
        public native static function forceFullCollection():void;

        // Queue a garbage collection request.
        public native static function queueCollection():void;

        // function exists solely to test native-methods with custom namespaces
        ns_example native static function nstest():void;

        // function exists solely to test ScriptObject::isGlobalObject
        public native static function isGlobal(o:Object):Boolean;

        // return the value passed to -swfversion at launch (or the default value, if -swfversion was not specified)
        public native static function get swfVersion():int;

        // return the value passed to -api at launch (or the default value, if -api was not specified)
        public native static function get apiVersion():int;

        public native static function disposeXML(xml:XML):void;

        // DEOPT: Diagnostic scaffolding.
        public native static function deopt(n:int):void;

        /**
         * Suggest to the garbage collector that if collector's imminence
         * exceeds imminence parameter then the collector should finish
         * the incremental collection cycle.  Doing so may incur a pause.
         *
         * @param imminence  A number between 0 and 1 where 0 means less imminent and 1 means most imminent
         *
         * @note The API is provided to allow the program to help the GC schedule GC pauses, thereby
         * working around a weakness in the garbage collector: though the GC is incremental, there is
         * still the chance of longer-than-desired pauses in some situations.  Scheduling those pauses
         * at points where the user won't notice or won't care (program phase transitions, say)
         * makes for a smoother user experience.
         */
        public static native function pauseForGCIfCollectionImminent(imminence:Number=0.75): void;

        // Support for VM tests that depend on representation of numeric atoms.
        // These are provided solely for testing purposes, as the semantics of an AS3 program
        // should not depend on the word size of the platform nor the internal encoding of atoms.

        // Return true if running on a 64-bit platform.
        public native static function is64bit(): Boolean;

        // Return true if the argument is an atom with tag kIntptrType.
        public native static function isIntptr(a:*): Boolean;
        public native static function runInSafepoint(code:Function):void;

        // Return argument value as a kIntptrType atom if the argument is a numeric
        // value that can be so represented, else return the argument unchanged.
        public native static function canonicalizeNumber(a:*):*;
    }

    [native(cls="::avmshell::FileClass", methods="auto", construct="none")]
    public class File
    {
        public native static function exists(filename:String):Boolean;
        public native static function read(filename:String):String;
        public native static function write(filename:String, data:String):void;

        public native static function readByteArray(filename:String):ByteArray;
        public native static function writeByteArray(filename:String, bytes:ByteArray):Boolean;
    }

    public function debugger()
    {
        System.debugger()
    }
}

package avmshell 
{
    // this class exists solely to test construct="abstract"
    [native(cls="::avmshell::AbstractBaseClass", instance="::avmshell::AbstractBaseObject", gc="exact", methods="auto", construct="abstract")]
    public class AbstractBase
    {
    }

    // this class exists solely to test construct="abstract"
    [native(cls="::avmshell::NativeSubclassOfAbstractBaseClass", instance="::avmshell::NativeSubclassOfAbstractBaseObject", gc="exact", methods="auto")]
    public class NativeSubclassOfAbstractBase extends AbstractBase
    {
    }

    // this class exists solely to test construct="abstract"
    public class SubclassOfAbstractBase extends AbstractBase
    {
    }

    // this class exists solely to test construct="restricted"
    [native(cls="::avmshell::RestrictedBaseClass", instance="::avmshell::RestrictedBaseObject", gc="exact", methods="auto", construct="restricted")]
    public class RestrictedBase
    {
    }

    // this class exists solely to test construct="restricted"
    [native(cls="::avmshell::NativeSubclassOfRestrictedBaseClass", instance="::avmshell::NativeSubclassOfRestrictedBaseObject", gc="exact", methods="auto")]
    public class NativeSubclassOfRestrictedBase extends RestrictedBase
    {
    }

    // this class exists solely to test construct="restricted"
    public class SubclassOfRestrictedBase extends RestrictedBase
    {
    }

    // this class exists solely to test construct="abstract-restricted"
    [native(cls="::avmshell::AbstractRestrictedBaseClass", instance="::avmshell::AbstractRestrictedBaseObject", gc="exact", methods="auto", construct="abstract-restricted")]
    public class AbstractRestrictedBase
    {
    }

    // this class exists solely to test construct="abstract-restricted"
    [native(cls="::avmshell::NativeSubclassOfAbstractRestrictedBaseClass", instance="::avmshell::NativeSubclassOfAbstractRestrictedBaseObject", gc="exact", methods="auto")]
    public class NativeSubclassOfAbstractRestrictedBase extends AbstractRestrictedBase
    {
    }

    // this class exists solely to test construct="abstract-restricted"
    public class SubclassOfAbstractRestrictedBase extends AbstractRestrictedBase
    {
    }

    // this class exists solely to test construct="check"
    [native(cls="::avmshell::CheckBaseClass", instance="::avmshell::CheckBaseObject", gc="exact", methods="auto", construct="check")]
    public class CheckBase
    {
    }

    // this class exists test construct="native" with a C++ class,
    // and also to test for correct calculation of slot offsets for
    // native classes with padding at the end (bugzilla 655300)
    [native(cls="::avmshell::NativeBaseClass", instance="::avmshell::NativeBaseObject", gc="exact", methods="auto", construct="native")]
    public class NativeBase
    {
    }

    // this class exists solely to test for correct calculation of slot offsets for
    // native classes with padding at the end (bugzilla 655300)
    public class NativeBaseExtender1 extends NativeBase
    {
        private var foo:int;
    }

    // this class exists solely to test for correct calculation of slot offsets for
    // native classes with padding at the end (bugzilla 655300)
    public class NativeBaseExtender2 extends NativeBase
    {
        private var foo:int;
        private var foo1:int;
        private var foo2:int;
    }

    // this class exists solely to test construct="native" with a pure-AS3 class
    [native(construct="native")]
    public class NativeBaseAS3
    {
    }

    // this class exists solely to test friend="someclass"
    [native(friend="avmshell::ShellCore")]
    public class ShellCoreFriend1
    {
        static private var foo:int = 42;
    }

    // this class exists solely to test friend="someclass"
    [native(friend="avmshell::ShellCore,avmshell::ShellToplevel")]
    public class ShellCoreFriend2
    {
        static private var bar:int = 101;
    }
}

// The flash.system package is present so identical ATS test media can be used
// in the command-line VM and the Player
package flash.system
{
    import avmplus.*;

    public final class Capabilities
    {
        public static function get playerType():String { return "AVMPlus"; }
        public static function get isDebugger():Boolean { return System.isDebugger(); }
    }
}

package {

    import avmplus.*

    public function getClassByName(name:String):Class
    {
        return Domain.currentDomain.getClass(name);
    }

    // nonstandard extensions to ECMAScript
    public function print(...s)
    {
        System.trace(s)
    }

    // nonstandard Flash Player extensions
    public function trace(...s)
    {
        System.trace(s)
    }

    public function getTimer():uint
    {
        return System.getTimer()
    }

    public function readLine():String
    {
        return System.readLine()
    }

    // DEOPT: Diagnostic scaffolding.
    public function deopt(k):void
    {
        System.deopt(k);
    }
}

// test library code

/*
 tests
 - unversioned names: are in all versions, as though version 0
 - versioned names: are not visible to smaller versions (bindings, not number)
 - multiple versioned names: are visible to all compatible versions
 - class, interface, method, accessor, slot, static names
 - running multiple active versions
*/

// API Versioning - Globals
// moved from "avmplus" to "avmshell" to avoid having avmplus be unversioned in builtin, but versioned in shell
package avmshell 
{
    include "../core/api-versions.as"
    
    // Shadow the declarations into public vars solely so that the acceptance tests
    // can read them; they can't simple include api-versions.as because the CONFIG
    // namespace is special and available only to builtin code (which the acceptance
    // tests aren't).
    // NB: 660 is a legacy wart due to existing ASC.jar expecting that as the first
    // version. It's a bit ugly but keeping it is easier than modifying asc.jar at this point.
    // (Since this magic value only affects builtin code we could change it in the future
    // without affecting existing content, though.)
    public const API_AIR_1_0 = CONFIG::AIR_1_0 - 660;
    public const API_FP_10_0 = CONFIG::FP_10_0 - 660;
    public const API_AIR_1_5 = CONFIG::AIR_1_5 - 660;
    public const API_AIR_1_5_1 = CONFIG::AIR_1_5_1 - 660;
    public const API_FP_10_0_32 = CONFIG::FP_10_0_32 - 660;
    public const API_AIR_1_5_2 = CONFIG::AIR_1_5_2 - 660;

    public var public_var:Boolean = true;
    [API(CONFIG::AIR_1_0)] public var public_var_AIR_1_0:Boolean = true;
    [API(CONFIG::FP_10_0)] public var public_var_FP_10_0:Boolean = true;
    [API(CONFIG::AIR_1_5)] public var public_var_AIR_1_5:Boolean = true;
    [API(CONFIG::AIR_1_5_1)] public var public_var_AIR_1_5_1:Boolean = true;
    [API(CONFIG::FP_10_0_32)] public var public_var_FP_10_0_32:Boolean = true;
    [API(CONFIG::AIR_1_5_2)] public var public_var_AIR_1_5_2:Boolean = true;
    [API(CONFIG::AIR_1_0,CONFIG::FP_10_0)] public var public_var_AIR_1_0_FP_10_0:Boolean = true;
    [API(CONFIG::AIR_1_5_1,CONFIG::FP_10_0,CONFIG::AIR_1_5_2)] public var public_var_AIR_1_5_1_FP_10_0_AIR_1_5_2:Boolean = true;
    [API(CONFIG::FP_10_0_32,CONFIG::AIR_1_0,CONFIG::FP_10_0)] public var public_var_FP_10_0_32_AIR_1_0_FP_10_0:Boolean = true;

    public const public_const:Boolean = true;
    [API(CONFIG::AIR_1_0)] public const public_const_AIR_1_0:Boolean = true;
    [API(CONFIG::FP_10_0)] public const public_const_FP_10_0:Boolean = true;
    [API(CONFIG::AIR_1_5)] public const public_const_AIR_1_5:Boolean = true;
    [API(CONFIG::AIR_1_5_1)] public const public_const_AIR_1_5_1:Boolean = true;
    [API(CONFIG::FP_10_0_32)] public const public_const_FP_10_0_32:Boolean = true;
    [API(CONFIG::AIR_1_5_2)] public const public_const_AIR_1_5_2:Boolean = true;
    [API(CONFIG::AIR_1_0,CONFIG::FP_10_0)] public const public_const_AIR_1_0_FP_10_0:Boolean = true;
    [API(CONFIG::AIR_1_5_1,CONFIG::FP_10_0,CONFIG::AIR_1_5_2)] public const public_const_AIR_1_5_1_FP_10_0_AIR_1_5_2:Boolean = true;
    [API(CONFIG::FP_10_0_32,CONFIG::AIR_1_0,CONFIG::FP_10_0)] public const public_const_FP_10_0_32_AIR_1_0_FP_10_0:Boolean = true;

    public function public_function():Boolean { return true; }
    [API(CONFIG::AIR_1_0)] public function public_function_AIR_1_0():Boolean { return true; }
    [API(CONFIG::FP_10_0)] public function public_function_FP_10_0():Boolean { return true; }
    [API(CONFIG::AIR_1_5)] public function public_function_AIR_1_5():Boolean { return true; }
    [API(CONFIG::AIR_1_5_1)] public function public_function_AIR_1_5_1():Boolean { return true; }
    [API(CONFIG::FP_10_0_32)] public function public_function_FP_10_0_32():Boolean { return true; }
    [API(CONFIG::AIR_1_5_2)] public function public_function_AIR_1_5_2():Boolean { return true; }
    [API(CONFIG::AIR_1_0,CONFIG::FP_10_0)] public function public_function_AIR_1_0_FP_10_0():Boolean { return true; }
    [API(CONFIG::AIR_1_5_1,CONFIG::FP_10_0,CONFIG::AIR_1_5_2)] public function public_function_AIR_1_5_1_FP_10_0_AIR_1_5_2():Boolean { return true; }
    [API(CONFIG::FP_10_0_32,CONFIG::AIR_1_0,CONFIG::FP_10_0)] public function public_function_FP_10_0_32_AIR_1_0_FP_10_0():Boolean { return true; }

    public function get public_getset():Boolean { return true; }
    [API(CONFIG::AIR_1_0)] public function get public_getset_AIR_1_0():Boolean { return true; }
    [API(CONFIG::FP_10_0)] public function get public_getset_FP_10_0():Boolean { return true; }
    [API(CONFIG::AIR_1_5)] public function get public_getset_AIR_1_5():Boolean { return true; }
    [API(CONFIG::AIR_1_5_1)] public function get public_getset_AIR_1_5_1():Boolean { return true; }
    [API(CONFIG::FP_10_0_32)] public function get public_getset_FP_10_0_32():Boolean { return true; }
    [API(CONFIG::AIR_1_5_2)] public function get public_getset_AIR_1_5_2():Boolean { return true; }
    [API(CONFIG::AIR_1_0,CONFIG::FP_10_0)] public function get public_getset_AIR_1_0_FP_10_0():Boolean { return true; }
    [API(CONFIG::AIR_1_5_1,CONFIG::FP_10_0,CONFIG::AIR_1_5_2)] public function get public_getset_AIR_1_5_1_FP_10_0_AIR_1_5_2():Boolean { return true; }
    [API(CONFIG::FP_10_0_32,CONFIG::AIR_1_0,CONFIG::FP_10_0)] public function get public_getset_FP_10_0_32_AIR_1_0_FP_10_0():Boolean { return true; }

    public function set public_getset(x:*):void {  }
    [API(CONFIG::AIR_1_0)] public function set public_getset_AIR_1_0(x:*):void {  }
    [API(CONFIG::FP_10_0)] public function set public_getset_FP_10_0(x:*):void {  }
    [API(CONFIG::AIR_1_5)] public function set public_getset_AIR_1_5(x:*):void {  }
    [API(CONFIG::AIR_1_5_1)] public function set public_getset_AIR_1_5_1(x:*):void {  }
    [API(CONFIG::FP_10_0_32)] public function set public_getset_FP_10_0_32(x:*):void {  }
    [API(CONFIG::AIR_1_5_2)] public function set public_getset_AIR_1_5_2(x:*):void {  }
    [API(CONFIG::AIR_1_0,CONFIG::FP_10_0)] public function set public_getset_AIR_1_0_FP_10_0(x:*):void {  }
    [API(CONFIG::AIR_1_5_1,CONFIG::FP_10_0,CONFIG::AIR_1_5_2)] public function set public_getset_AIR_1_5_1_FP_10_0_AIR_1_5_2(x:*):void {  }
    [API(CONFIG::FP_10_0_32,CONFIG::AIR_1_0,CONFIG::FP_10_0)] public function set public_getset_FP_10_0_32_AIR_1_0_FP_10_0(x:*):void {  }

}

// API Versioning - Classes
// moved from "avmplus" to "avmshell" to avoid having avmplus be unversioned in builtin, but versioned in shell
package avmshell 
{
    public class public_class 
    {

        public var public_var:Boolean = true;
        [API(CONFIG::AIR_1_0)] public var public_var_AIR_1_0:Boolean = true;
        [API(CONFIG::FP_10_0)] public var public_var_FP_10_0:Boolean = true;
        [API(CONFIG::AIR_1_5)] public var public_var_AIR_1_5:Boolean = true;
        [API(CONFIG::AIR_1_5_1)] public var public_var_AIR_1_5_1:Boolean = true;
        [API(CONFIG::FP_10_0_32)] public var public_var_FP_10_0_32:Boolean = true;
        [API(CONFIG::AIR_1_5_2)] public var public_var_AIR_1_5_2:Boolean = true;
        [API(CONFIG::AIR_1_0,CONFIG::FP_10_0)] public var public_var_AIR_1_0_FP_10_0:Boolean = true;
        [API(CONFIG::AIR_1_5_1,CONFIG::FP_10_0,CONFIG::AIR_1_5_2)] public var public_var_AIR_1_5_1_FP_10_0_AIR_1_5_2:Boolean = true;
        [API(CONFIG::FP_10_0_32,CONFIG::AIR_1_0,CONFIG::FP_10_0)] public var public_var_FP_10_0_32_AIR_1_0_FP_10_0:Boolean = true;

        public const public_const:Boolean = true;
        [API(CONFIG::AIR_1_0)] public const public_const_AIR_1_0:Boolean = true;
        [API(CONFIG::FP_10_0)] public const public_const_FP_10_0:Boolean = true;
        [API(CONFIG::AIR_1_5)] public const public_const_AIR_1_5:Boolean = true;
        [API(CONFIG::AIR_1_5_1)] public const public_const_AIR_1_5_1:Boolean = true;
        [API(CONFIG::FP_10_0_32)] public const public_const_FP_10_0_32:Boolean = true;
        [API(CONFIG::AIR_1_5_2)] public const public_const_AIR_1_5_2:Boolean = true;
        [API(CONFIG::AIR_1_0,CONFIG::FP_10_0)] public const public_const_AIR_1_0_FP_10_0:Boolean = true;
        [API(CONFIG::AIR_1_5_1,CONFIG::FP_10_0,CONFIG::AIR_1_5_2)] public const public_const_AIR_1_5_1_FP_10_0_AIR_1_5_2:Boolean = true;
        [API(CONFIG::FP_10_0_32,CONFIG::AIR_1_0,CONFIG::FP_10_0)] public const public_const_FP_10_0_32_AIR_1_0_FP_10_0:Boolean = true;

        public function public_function():Boolean { return true; }
        [API(CONFIG::AIR_1_0)] public function public_function_AIR_1_0():Boolean { return true; }
        [API(CONFIG::FP_10_0)] public function public_function_FP_10_0():Boolean { return true; }
        [API(CONFIG::AIR_1_5)] public function public_function_AIR_1_5():Boolean { return true; }
        [API(CONFIG::AIR_1_5_1)] public function public_function_AIR_1_5_1():Boolean { return true; }
        [API(CONFIG::FP_10_0_32)] public function public_function_FP_10_0_32():Boolean { return true; }
        [API(CONFIG::AIR_1_5_2)] public function public_function_AIR_1_5_2():Boolean { return true; }
        [API(CONFIG::AIR_1_0,CONFIG::FP_10_0)] public function public_function_AIR_1_0_FP_10_0():Boolean { return true; }
        [API(CONFIG::AIR_1_5_1,CONFIG::FP_10_0,CONFIG::AIR_1_5_2)] public function public_function_AIR_1_5_1_FP_10_0_AIR_1_5_2():Boolean { return true; }
        [API(CONFIG::FP_10_0_32,CONFIG::AIR_1_0,CONFIG::FP_10_0)] public function public_function_FP_10_0_32_AIR_1_0_FP_10_0():Boolean { return true; }

        public function get public_getset():Boolean { return true; }
        [API(CONFIG::AIR_1_0)] public function get public_getset_AIR_1_0():Boolean { return true; }
        [API(CONFIG::FP_10_0)] public function get public_getset_FP_10_0():Boolean { return true; }
        [API(CONFIG::AIR_1_5)] public function get public_getset_AIR_1_5():Boolean { return true; }
        [API(CONFIG::AIR_1_5_1)] public function get public_getset_AIR_1_5_1():Boolean { return true; }
        [API(CONFIG::FP_10_0_32)] public function get public_getset_FP_10_0_32():Boolean { return true; }
        [API(CONFIG::AIR_1_5_2)] public function get public_getset_AIR_1_5_2():Boolean { return true; }
        [API(CONFIG::AIR_1_0,CONFIG::FP_10_0)] public function get public_getset_AIR_1_0_FP_10_0():Boolean { return true; }
        [API(CONFIG::AIR_1_5_1,CONFIG::FP_10_0,CONFIG::AIR_1_5_2)] public function get public_getset_AIR_1_5_1_FP_10_0_AIR_1_5_2():Boolean { return true; }
        [API(CONFIG::FP_10_0_32,CONFIG::AIR_1_0,CONFIG::FP_10_0)] public function get public_getset_FP_10_0_32_AIR_1_0_FP_10_0():Boolean { return true; }

        public function set public_getset(x:*):void {  }
        [API(CONFIG::AIR_1_0)] public function set public_getset_AIR_1_0(x:*):void {  }
        [API(CONFIG::FP_10_0)] public function set public_getset_FP_10_0(x:*):void {  }
        [API(CONFIG::AIR_1_5)] public function set public_getset_AIR_1_5(x:*):void {  }
        [API(CONFIG::AIR_1_5_1)] public function set public_getset_AIR_1_5_1(x:*):void {  }
        [API(CONFIG::FP_10_0_32)] public function set public_getset_FP_10_0_32(x:*):void {  }
        [API(CONFIG::AIR_1_5_2)] public function set public_getset_AIR_1_5_2(x:*):void {  }
        [API(CONFIG::AIR_1_0,CONFIG::FP_10_0)] public function set public_getset_AIR_1_0_FP_10_0(x:*):void {  }
        [API(CONFIG::AIR_1_5_1,CONFIG::FP_10_0,CONFIG::AIR_1_5_2)] public function set public_getset_AIR_1_5_1_FP_10_0_AIR_1_5_2(x:*):void {  }
        [API(CONFIG::FP_10_0_32,CONFIG::AIR_1_0,CONFIG::FP_10_0)] public function set public_getset_FP_10_0_32_AIR_1_0_FP_10_0(x:*):void {  }
    }

    [API(CONFIG::AIR_1_0)] public class  public_class_AIR_1_0 {  }
    [API(CONFIG::FP_10_0)] public class  public_class_FP_10_0 {  }
    [API(CONFIG::AIR_1_5)] public class  public_class_AIR_1_5 {  }
    [API(CONFIG::AIR_1_5_1)] public class  public_class_AIR_1_5_1 {  }
    [API(CONFIG::FP_10_0_32)] public class  public_class_FP_10_0_32 {  }
    [API(CONFIG::AIR_1_5_2)] public class  public_class_AIR_1_5_2 {  }
    [API(CONFIG::AIR_1_0,CONFIG::FP_10_0)] public class  public_class_AIR_1_0_FP_10_0 {  }
    [API(CONFIG::AIR_1_5_1,CONFIG::FP_10_0,CONFIG::AIR_1_5_2)] public class  public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2 {  }
    [API(CONFIG::FP_10_0_32,CONFIG::AIR_1_0,CONFIG::FP_10_0)] public class  public_class_FP_10_0_32_AIR_1_0_FP_10_0 {  }

    public interface  public_interface {}
    [API(CONFIG::AIR_1_0)] public interface  public_interface_AIR_1_0 {  }
    [API(CONFIG::FP_10_0)] public interface  public_interface_FP_10_0 {  }
    [API(CONFIG::AIR_1_5)] public interface  public_interface_AIR_1_5 {  }
    [API(CONFIG::AIR_1_5_1)] public interface  public_interface_AIR_1_5_1 {  }
    [API(CONFIG::FP_10_0_32)] public interface  public_interface_FP_10_0_32 {  }
    [API(CONFIG::AIR_1_5_2)] public interface  public_interface_AIR_1_5_2 {  }
    [API(CONFIG::AIR_1_0,CONFIG::FP_10_0)] public interface  public_interface_AIR_1_0_FP_10_0 {  }
    [API(CONFIG::AIR_1_5_1,CONFIG::FP_10_0,CONFIG::AIR_1_5_2)] public interface  public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2 {  }
    [API(CONFIG::FP_10_0_32,CONFIG::AIR_1_0,CONFIG::FP_10_0)] public interface  public_interface_FP_10_0_32_AIR_1_0_FP_10_0 {  }

}

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

package com.adobe.flascc {

import flash.profiler.Telemetry;
import flash.utils.*;

import com.adobe.flascc.kernel.*;
import com.adobe.flascc.vfs.*;
import C_Run.*;

//CONFIG::debug { trace("CModule::loaded"); }
  
//--------------------------------------------------------------------------
//
//  ThunkMaker
//
//--------------------------------------------------------------------------

/**
* @private
*/
internal class ThunkMaker
{
  /**
  * @private
  */
  private var modPkgName:String;
  /**
  * @private
  */
  private var thunkSet:Dictionary;
  /**
  * @private
  */
  private var start:int;
  /**
  * @private
  */
  private var end:int;
  /**
  * @private
  */
  private var index:int;

  /**
  * Constructor
  */
  public function ThunkMaker(modPkgName:String, thunkSet:Dictionary, start:int, end:int, index:int):void
  {
	this.modPkgName = modPkgName;
	this.thunkSet = thunkSet;
	this.start = start;
	this.end = end;
	this.index = index;
  }
  
  /**
  * TBD
  */
  public function thunk():void
  {
    delete CModule.modThunks[modPkgName];
  //trace("thunking for " + modPkgName + " (" + index + ")");

    var m:CModule = CModule.getModuleByPackage(modPkgName);
  //      var removed:int = 0;

    m.getScript(); // init!
    // remove all remaining thunks for this range
    for(var j:int = start; j < end; j++)
      if(thunkSet[ptr2fun[j]])
      {
        delete thunkSet[ptr2fun[j]];
        ptr2fun[j] = null;
  //          removed++;
      }
  //trace("removed " + removed + " thunks");

    if(index >= 0)
      ptr2fun[index]();
  };
}

//--------------------------------------------------------------------------
//
//  CModule
//
//--------------------------------------------------------------------------

/**
* Contains convenience functions for reading and writting to domainMemory; also manages any flascc-specific global state (for example, the VFS and Posix interface implementations.)
*/
public class CModule
{
  /**
  * Returns the ByteArray object using as RAM
  */
  public static function get ram():ByteArray
  {
    return C_Run.ram;
  }

  /**
  * Returns a boolean indicating whether Flash out of memory conditions when growing the domainMemory ByteArray will throw an ActionScript exception. The default is false, malloc will return an appropriate failure and new/new[] will throw a std::badalloc C++ exception.
  */
  public static function get throwWhenOutOfMemory():Boolean
  {
    return C_Run.throwWhenOutOfMemory;
  }
  /**
  * Sets a boolean indicating whether Flash out of memory conditions when growing the domainMemory ByteArray will throw an ActionScript exception or cause malloc to fail. The default is false, malloc will return an appropriate failure and new/new[] will throw a std::badalloc C++ exception.
  */
  public static function set throwWhenOutOfMemory(x:Boolean):void
  {
    C_Run.throwWhenOutOfMemory = x;
  }

  /**
  * Returns true if the current environment supports
  * use of ActionScript Workers with flascc content
  */
  public static function get canUseWorkers():Boolean
  {
    return C_Run.workerClass != null;
  }

  /**
  * @private
  */
  static protected var weakResolvers:Vector.<Function> = new Vector.<Function>;

  /**
  * @private
  */
  public static function resolveWeaks(f:Function):void
  {
    weakResolvers.unshift(f);
    while(weakResolvers.length)
    {
      var resolver:Function = weakResolvers.pop();
      if(!resolver())
        weakResolvers.unshift(f);
    }
  }

  private static var workerSprite:* = null

  /**
  * Console should use this function to check if the SWF
  * has been launched as an isolate so it can act appropriately
  */
  public static function runningAsWorker():Boolean {
    var inWorker:Boolean = false

    if(!CModule.rootSprite)
      throw new Error("specify a rootSprite using \"CModule.rootSprite = ...\"");

    try {
      C_Run.workerSWFBytes = CModule.rootSprite.loaderInfo.bytes
      if(C_Run.workerClass["current"].getSharedProperty("flascc.ram"))
        inWorker = true;
    } catch(e:*) {
      inWorker = false;
    }

    if(inWorker) {
      workerSprite = new C_Run.AlcWorkerSprite();
    }
    return inWorker
  }

  /**
  * @private
  */
  public static var recordFixups:Boolean = false;

  /**
  * @private
  */
  public static var regsInitted:Boolean = false;

  /**
  * Lookup the specified publicly defined C/C++ symbol
  *
  * @param s The name of the symbol to lookup, for example, "malloc"
  *
  * @return The address of the symbol. If the symbol was a POD type you can read it's
  *         value using one of the read functions. If the symbol was a function then you can call it using callI.
  */
  public static function getPublicSymbol(name:String):int
  {
    namespace ns = "C_Run";
    return ns::["_"+name];
  }

  /**
  * relies on metadata -- won't return info in stripped builds
  * returns an Array of sym descriptors
  * sym descriptor is an Array of form:
  * [nmStyleType, symName(, sectName, value)]
  * @private
  */
  public static function getSymsUsingMD():Array
  {
    var esyms:Dictionary = mapCsymsWithExplicitValues();
    var result:Array = [];

    for each(var module:CModule in modules)
      result = result.concat(module.getSymsUsingMDWithExplicitSyms(esyms));
    return result;
  }

  /***
  * @private
  */
  internal static var preppedForThreadedExec:Boolean = false;

  /***
  * @private
  */
  public static function prepForThreadedExec():void
  {
    if(preppedForThreadedExec)
      return;

//trace('ptr2fun: ' + ptr2fun.constructor);
    if(typeof(ptr2fun) == "undefined" || ptr2fun is Array) // sparse function mapping? explicitly init every module...
    {
//trace("faulting in all modules: " + (ptr2fun_init.constructor) + ' (' + threadId + ')');
      explicitlyInitAllModules();
    }
    preppedForThreadedExec = true;
  }

  /***
  * @private
  */
  internal static var explicitlyInittedAllModules:Boolean = false;

  /***
  * @private
  */
  public static function explicitlyInitAllModules():void
  {
    if (explicitlyInittedAllModules)
      return;

    var resumeLibs:Boolean = false;
    try
    {
      AlcDbgHelper.pauseLibraryNotifications();
      resumeLibs = true;
    }
    catch (e:*) {}
    try
    {
      for each(var m:* in getModuleVector()) //type is * to avoid cyclic depend on CModule script..
      m.getScript();
    }
    finally
    {
      try
      {
        if (resumeLibs)
          AlcDbgHelper.resumeLibraryNotifications();
      }
      catch (e:*) {}
    }
    explicitlyInittedAllModules = true;
  }

  // package name => base thunk
  /***
  * @private
  */
  internal static const modThunks:Dictionary = new Dictionary;

  /***
  * @private
  */
  internal static function makeThunk(modPkgName:String, thunkSet:Dictionary, start:int, end:int, index:int):Function
  {
    return new ThunkMaker(modPkgName, thunkSet, start, end, index).thunk;
  }

  /**
  * note that this coexists peacefully w/ allocText-based function ptrs
  * because allocText always returns a negative number and setting
  * ptr2fun_init[someNegNumber] doesn't affect ptr2fun_init.length
  * @private
  */
  public static function allocFunPtrs(modPkgName:String, n:int, align:int):int
  {
    // TODO -- anything to do for threads here? should be totally deterministic, I guess!

    if(align < 1)
      align = 1;

    ptr2fun_init.length = int((ptr2fun_init.length + align - 1) / align) * align;
    var result:int = ptr2fun_init.length;
    ptr2fun_init.length += n;

    // real package (i.e., not SWIG allocing some temp stuff) + worker support? put a bunch of thunks in to fault in modules
    if(modPkgName != null && workerClass)
    {
      // set of all thunks
      var thunkSet:Dictionary = new Dictionary;

      for(var i:int = result; i < result + n; i++)
      {
        // thunk function that will forcibly init the script
        // and then call the right function
        var thunk:Function = makeThunk(modPkgName, thunkSet, result, result + n, i);
        thunkSet[thunk] = true;
        ptr2fun_init[i] = thunk;
      }
      var modThunk:Function = makeThunk(modPkgName, thunkSet, result, result + n, -1);
      modThunks[modPkgName] = modThunk;
      thunkSet[modThunk] = true;
    }
    return result;
  }

  /**
  * @private
  */
  static protected var text:int = int(0xf0000000); // 3.75G (don't clash w/ real data)

  /**
  * allocate a region in "text" space (i.e., code space)
  * @private
  */
  public static function allocTextSect(modPkgName:String, sectName:String, size:int):int
  {
    // TODO -- anything to do for threads here? should be totally deterministic, I guess!

    var result:int = (text + 15) & ~15; // align per module

    text  = result + size;
    return result;
  }

  /**
  * allocate space in domainMemory for data
  * @private
  */
  public static function allocDataSect(modPkgName:String, sectName:String, size:int, align:int):int
  {
    var result:int;

    if(workerClass)
    {
      var propName:String = "flascc.sect." + modPkgName + "." + sectName;
      result = workerClass.current.getSharedProperty(propName);
//trace("allocDataSect tid: " + threadId + " / " + modPkgName + "." + sectName + " = " + result);
    }
    if(!result)
    {
      if(align < 1)
        align = 1;

      result = sbrk(size, align);
    }
    return result;
  }

  /**
  * @private
  */
  public static function describeType(value:*):XML
  {
    try
    {
      var flashutilsNS:Namespace = new Namespace("flash.utils");

      return (flashutilsNS::["describeType"])(value);
    } catch(e:*) {}

    var avmplusNS:Namespace = new Namespace("avmplus");

    return (avmplusNS::["describeType"])(value, avmplusNS::["FLASH10_FLAGS"]);
  }

  /**
  * Write the low 8 bits of <i>value</i> into domainMemory.
  * @param ptr The address of the location in domainMemory to write to
  * @param value The value to write (only the low 8 bits are written)
  */
  public static function write8(ptr:int, value:int):void
  {
    ram_init.position = ptr;
    ram_init.writeByte(value);
  }

  /**
  * Write the low 16 bits of <i>value</i> into domainMemory.
  * @param ptr The address of the location in domainMemory to write to
  * @param value The value to write (only the low 16 bits are written)
  */
  public static function write16(ptr:int, value:int):void
  {
    ram_init.position = ptr;
    ram_init.writeShort(value);
  }

  /**
  * Write a 32 bit value into domainMemory
  * @param ptr The address of the location in domainMemory to write to
  * @param value The value to write
  */
  public static function write32(ptr:int, value:int):void
  {
    ram_init.position = ptr;
    ram_init.writeInt(value);
  }

  /**
  * Write a Number as a 32 bit float into domainMemory
  * @param ptr The address of the location in domainMemory to write to
  * @param value The value to write at the address (will be rounded to the nearest 32bit floating point value)
  */
  public static function writeFloat(ptr:int, value:Number):void
  {
    ram_init.position = ptr;
    ram_init.writeFloat(value);
  }

  /**
  * Write a Number as a 64 bit float into domainMemory
  * @param ptr The address of the location in domainMemory to write to
  * @param value The value to write at the address
  */
  public static function writeDouble(ptr:int, value:Number):void
  {
    ram_init.position = ptr;
    ram_init.writeDouble(value);
  }

  /**
  * Copy the contents of an object that implements the IDataInput interface (ByteArray, FileStream, Socket etc) into domainMemory.
  * @param ptr The address of the location in domainMemory to start writting to
  * @param length The number of bytes to copy from the source bytearray (starting from source.position)
  * @param source The IDataInput to copy data from (the current position in this IDataInput will be used, and will increase by length)
  */
  public static function writeBytes(ptr:int, length:uint, source:IDataInput):void
  {
    source.readBytes(ram_init, ptr, length);
  }

  /**
  * Read an 8 bit value from domainMemory with zero padding to extend it to a 32 bit signed integer. This will always result in positive integers.
  * @param ptr The address of the location in domainMemory to read from
  * @return An 8 bit integer zero padded to 32 bit.
  */
  public static function read8(ptr:int):int
  {
    ram_init.position = ptr;
    return ram_init.readUnsignedByte();
  }

  /**
  * Read a 16 bit value from domainMemory with zero padding to extend it to a 32 bit signed integer. This will always result in positive integers.
  * @param ptr The address of the location in domainMemory to read from
  * @return A 16 bit integer zero padded to 32 bit.
  */
  public static function read16(ptr:int):int
  {
    ram_init.position = ptr;
    return ram_init.readUnsignedShort();
  }

  /**
  * Read a 32 bit value from domainMemory.
  * @param ptr The address of the location in domainMemory to read from
  * @return A 32 bit integer.
  */
  public static function read32(ptr:int):int
  {
    ram_init.position = ptr;
    return ram_init.readInt();
  }

  /**
  * Read a 32 bit floating point value from domainMemory
  * @param ptr The address of the location in domainMemory to read from
  * @return A signed 32 bit floating point value
  */
  public static function readFloat(ptr:int):Number
  {
    ram_init.position = ptr;
    return ram_init.readFloat();
  }

  /**
  * Read a 64 bit floating point value from domainMemory
  * @param ptr The address of the location in domainMemory to read from
  * @return A signed 64 bit floating point value
  */
  public static function readDouble(ptr:int):Number
  {
    ram_init.position = ptr;
    return ram_init.readDouble();
  }

  /**
  * Copy data from domainMemory into an object implementing the IDataOutput interface (ByteArray, FileStream, Socket etc)
  * @param ptr The address of the location in domainMemory to start reading from.
  * @param length The number of bytes to copy from the domainMemory (starting from ptr)
  * @param dst The IDataOutput to copy data into (the current position in this IDataOutput will be respected and will increase by length)
  */
  public static function readBytes(ptr:int, length:uint, dst:IDataOutput):void
  {
    dst.writeBytes(ram_init, ptr, length);
  }

  /**
  * @private
  */
  public static function push32(value:int):void
  {
    ESP -= 4;
    write32(ESP, value);
  }

  /**
  * @private
  */
  public static function pop32():int
  {
    var result:int = read32(ESP);
    ESP += 4;
    return result;
  }

  /**
  * Allocate <i>size</i> bytes of data within the domainMemory ByteArray. The memory must be freed using <code>CModule.free()</code>.
  * @return A pointer to a region of memory within domainMemory <i>size</i> bytes long.
  *
  * Internally this calls the libc implementation of <code>malloc()</code> so the returned value should be treated as though it had been allocated in C with <code>malloc()</code>.
  */
  public static function malloc(size:int):int
  {
    var args:Vector.<int> = new Vector.<int>;
    args.push(size);
    return callI(_malloc, args);
  }

  /**
  * Free a pointer that was previously allocated using <code>CModule.malloc()</code>.
  *
  * Internally this calls the libc implementation of <code>free()</code> so it can be used to free pointers allocated in C with <code>malloc()</code>.
  */
  public static function free(ptr:int):void
  {
    var args:Vector.<int> = new Vector.<int>;
    args.push(ptr);
    callI(_free, args);
  }

  /**
  * should inline asm using me indicate that esp is clobbered?
  * @private
  */
  public static function alloca(size:int):int
  {
    ESP = int((ESP - size) / stackAlign) * stackAlign;
    return ESP;
  }

  /**
  * Write an actionscript string as UTF8 data into domainMemory
  * @param ptr The location in domainMemory where the string will be written. You must ensure that this
  *            points to a buffer in the flascc world that has enough space to hold the string
  *            data or this method will overwrite other data within your program. Consider using
  *            <code>mallocString</code> instead as it will allocate the correct amount of space.
  * @return The number of bytes written
  */
  public static function writeString(ptr:int, s:String):int
  {
    ram_init.position = ptr;
    ram_init.writeUTFBytes(s);
    return ram_init.position - ptr;
  }

  /**
  * Read UTF8 data from domainMemory and construct an Actionscript String
  * @param ptr The location in domainMemory to start reading from
  * @param length The number of bytes to read
  * @return A String constructed from the UTF8 data found between ptr and ptr+length
  */
  public static function readString(ptr:int, length:int):String
  {
    ram_init.position = ptr;
    var s:String = ram_init.readUTFBytes(length);
    //trace(s);
    return s;
  }

  /**
  * @private
  */
  public static function writeLatin1String(ptr:int, s:String):int
  {
    ram_init.position = ptr;
    ram_init.writeUTFBytes(s);
    //XXX ram_init.writeMultiByte(s, "iso-8859-1");
    return ram_init.position - ptr;
  }

  /**
  * @private
  */
  public static function readLatin1String(ptr:int, length:int):String
  {
    ram_init.position = ptr;
    var s:String = ram_init.readUTFBytes(length);//XXX ram_init.readMultiByte(length, "iso-8859-1");
    //trace(s);
    return s;
  }

  /**
  * Read <i>count</i> integers from the offset <i>ptr</i> in domainMemory into a Vector of ints.
  * @param ptr The offset in domainMemory to read from
  * @param count The number of 32 bit integers to read
  * @return A Vector of ints read from domainMemory
  */
  public static function readIntVector(ptr:int, count:int):Vector.<int>
  {
    var r:Vector.<int> = new Vector.<int>;
    ram_init.position = ptr;
    while(count--)
      r.push(ram_init.readInt());
    return r;
  }

  /**
  * Write a Vector of ints into domainMemory starting at offset <i>ptr</i>.
  * @param ptr The offset in domainMemory to start writing to
  * @param vec The Vector of ints to write into domainMemory
  */
  public static function writeIntVector(ptr:int, vec:Vector.<int>):void
  {
    ram_init.position = ptr;
    for(var i:int = 0; i < vec.length; i++)
      ram_init.writeInt(vec[i]);
  }

  /**
  * @private
  */
  static protected const scratchBA:ByteArray = new ByteArray;

  /**
  * Allocate a buffer in domainMemory and write an actionscript string as UTF8 data into that buffer.
  * @param s   The string you want to copy into domainMemory
  * @return The pointer to the buffer in domainMemory that contains the string. This must be manually
  *         freed by passing this pointer to "free()"
  */
  public static function mallocString(s:String):int
  {
    scratchBA.position = 0;
    scratchBA.writeUTFBytes(s);
    scratchBA.writeByte(0);
    var length:int = scratchBA.position;
    var ptr:int = malloc(length);
    ram_init.position = ptr;
    ram_init.writeBytes(scratchBA, 0, length);
    if(length > 0x1000)
      scratchBA.length = 0x1000;
    return ptr;
  }
  /**
  * @private
  */
  public static function allocaString(s:String):int
  {
    scratchBA.position = 0;
    scratchBA.writeUTFBytes(s);
    scratchBA.writeByte(0);
    var length:int = scratchBA.position;
    var ptr:int = alloca(length);
    ram_init.position = ptr;
    ram_init.writeBytes(scratchBA, 0, length);
    if(length > 0x1000)
      scratchBA.length = 0x1000;
    return ptr;
  }
  /**
  * @private
  */
  public static function mallocLatin1String(s:String):int
  {
    var length:int = s.length;
    var ptr:int = malloc(length+1);
    ram_init.position = ptr;
    ram_init.writeUTFBytes(s);//XXX ram_init.writeMultiByte(s, "iso-8859-1");
    ram_init[ptr+length] = 0;
    return ptr;
  }
  /**
  * @private
  */
  public static function allocaLatin1String(s:String):int
  {
    var length:int = s.length;
    var ptr:int = alloca(length+1);
    ram_init.position = ptr;
    ram_init.writeUTFBytes(s);//XXX ram_init.writeMultiByte(s, "iso-8859-1");
    ram_init[ptr+length] = 0;
    return ptr;
  }

  /**
  * @private
  */
  public static function regFun(ptr:int, f:Function):void
  {
    if(ptr == 0)
      throw new Error("Error: Attempting to register the null function pointer");

    // trace("regFun " + uint(ptr).toString(16));
    ptr2fun_init[ptr] = f;
  }

  /**
  * Low level API for calling a C/C++ function from Actionscript, returning a 32-bit value
  * @param functionPtr The address of the function. You can find the address of a C/C++
  *            function by passing the name of the function to <code>getPublicSym</code>.
  * @param args A Vector containing 32-bit values to pass on the stack to
  *             the function. These could either be primitive integer data, or pointers.
  *             To pass more complex data types (including AS3 types) you should use the
  *             function annotation syntax explained in the interop tutorial to give the
  *             function a mopre natural AS3 function signature.
  * @param stack An optional pointer to the base of the stack. A value of 0 means use the
  *              current ESP.
  * @param preserveStack Indicates whether the stack pointer should unwind back to the value it was
  *                      before the function was called.
  * @return The integer return value of the function. As above, to return complex (possibly AS3)
  *         types you should annotate the function as explained in the interop tutorial
  *         to give the function a more natural signature and return type.
  */
  public static function callI(functionPtr:int, args:Vector.<int>, stack:int = 0, preserveStack:Boolean = false):int
  {
    var esp:int = ESP;

    try
    {
      // optional stack param
      if(stack)
        ESP = stack;

      // align stack
      ESP = int(ESP / stackAlign) * stackAlign;
      // align for args
      var argAlign:int = (stackAlign - ((args.length * 4) % stackAlign)) % stackAlign;

      ESP -= argAlign;

      for(var i:int = args.length-1; i >= 0; i--)
        push32(args[i]);

      (ptr2fun_init[functionPtr])();
    }
    finally
    {
      if(!preserveStack)
        ESP = esp;
    }
    return eax;
  }

  /**
  * Low level API for calling a C/C++ function from Actionscript returning a 64-bit floating point value
  * @param functionPtr The address of the function. You can find the address of a C/C++
  *            function by passing the name of the function to <code>getPublicSym</code>.
  * @param args A Vector containing 32-bit values to pass on the stack to
  *             the function. These could ewither be primitive integer data, or pointers.
  *             To pass more complex data types (including AS3 types) you should use the
  *             function annotation syntax explained in the interop tutorial to give the
  *             function a mopre natural AS3 function signature.
  * @param stack An optional pointer to the base of the stack. A value of 0 means use the
  *              current ESP.
  * @param preserveStack Indicates whether the stack pointer should unwind back to the value it was
  *                      before the function was called.
  * @return The 64-bit floating point return value of the function. As above, to return complex (possibly AS3)
  *         types you should annotate the function as explained in the interop tutorial
  *         to give the function a more natural signature and return type.
  */
  public static function callN(functionPtr:int, args:Vector.<int>, stack:int = 0, preserveStack:Boolean = false):Number
  {
    callI(functionPtr, args, stack);
    return st0;
  }

  /**
  * @private
  */
  static protected const modules:Vector.<CModule> = new Vector.<CModule>;

  private static var seenModuleInit:Boolean;

  /**
  * @private
  */
  public static function regModule(sections:Object, init:Function, modSyms:Array, modPackage:String):CModule
  {
    if(seenModuleInit)
      trace("WARNING: module registration after module initialization has begun");
    // trace("registered module");
    var mod:CModule = new CModule(sections, init, modSyms, modPackage);
    modules.push(mod);
    return mod;
  }

  /**
  * @private
  */
  public static function getModuleByPackage(s:String):CModule
  {
    for each(var m:CModule in modules)
      if(m.modPackage == s)
        return m;
    return null;
  }

  /**
  * @private
  */
  public static function getModuleVector():Vector.<CModule>
  {
    return modules.concat();
  }

  private var sections:Object; // section map
  private var init:Function; // initialization function to force the module to init
  private var script:Object; // global/script object for this module
  private var modSyms:Array;
  private var modPackage:String;
  private var fixups:Vector.<int>;

  /**
  * @private
  */
  public const oncep:int;

  /**
  * @private
  */
  public function CModule(sections:Object, init:Function, modSyms:Array, modPackage:String)
  {
    this.sections = sections;
    this.init = init;
    this.modSyms = modSyms;
    this.modPackage = modPackage;
    // grab a byte to use as our "run once" state
    this.oncep = (workerClass ? workerClass.current.getSharedProperty("flascc." + modPackage + ".oncep") : 0) ||
      sbrk(1, 1);
    if(recordFixups)
      fixups = new Vector.<int>;
  }

  /**
  * @private
  */
  public function once(f:Function, ...args:Array):void
  {
    if(workerClass)
      threadArbMutexLock();
    try
    {
      if(!read8(oncep))
      {
        write8(oncep, 1);
        f.apply(undefined, args);
      }
    }
    finally
    {
      if(workerClass)
        threadArbMutexUnlock();
    }
  }

  private static var telemetrySupported:Boolean;
  try {
    if(Telemetry != null) {
      telemetrySupported = true;
    }
  } catch (e:*) {
    telemetrySupported = false;
  }

  /**
  * @private
  * Send a telemetry metric if Monocle is currently attached
  * See the Flash API reference for information about these parameters
  */
  public static function sendMetric(metric:String, value:*):void
  {
    try {
      if(telemetrySupported) {
        Telemetry.sendMetric(metric, value);
      }
    } catch (e:*) {
      trace("failed to send metric '"+metric+"' value '"+value+"'");
    }
  }

  /**
  * @private
  * Send a telemetry span metric if Monocle is currently attached
  * See the Flash API reference for information about these parameters
  */
  public static function sendSpanMetric(metric:String, startSpanMarker:Number, value:*=null):void
  {
    if(telemetrySupported) {
      Telemetry.sendMetric(metric, startSpanMarker, value);
    }
  }

  /**
  * @private
  */
  public function fixup32(ptr:int, value:int):void
  {
    if(fixups)
      fixups.push(ptr);
    write32(ptr, value);
  }

  /**
  * @private
  */
  public function fixup16(ptr:int, value:int):void
  {
    write16(ptr, value);
  }

  /**
  * @private
  */
  public function fixup8(ptr:int, value:int):void
  {
    write8(ptr, value);
  }

  /**
  * @private
  */
  public function getFixups():Vector.<int>
  {
    return fixups ? fixups.concat() : null;
  }

  /**
  * @private
  */
  public function getSections():Object
  {
    var o:Object = {};
    for(var s:String in sections)
      o[s] = sections[s].concat();
    return o;
  }

  /**
  * "sectName/value" => explicit value for Csym metadata in this module
  * and populate "map"
  */
  private function mapCsymsWithExplicitValues(map:Dictionary):void
  {
    var ns:Namespace
    var value:uint

    if(!script)
        return; // don't map anything for un-initted scripts

    if(modSyms) {
      for(var i:int=0; i<modSyms.length; i++) {
        var arr:Array = modSyms[i];
        if (arr.length < 4)
          continue

        var _type:String = arr[0]
        var _name:String = arr[1]
        var _sect:String = arr[2]
        var _off:uint = arr[3]
        if(_type == "t")
        {
          ns = new Namespace(modPackage);
          value = script.ns::[_name]; // value of the constant
        }
        else
        {
          namespace C_RunNS = "C_Run";
          value = script.C_RunNS::[_name]; // value of the constant
        }
        // ".text/12" => 5456/*explicit value*/ + 1234/*section offset*/
        map[_sect + "/" + value] = uint(_off + sections[_sect][0]);
      }
    } else {
      var scriptXML:XML = CModule.describeType(script);
      var csymsXML:XMLList = scriptXML..metadata.(@name == "Csym");
      for each(var s:XML in csymsXML)
      {
        var parent:XML = s.parent();

        if(!parent || parent.name() != "constant")
          continue;

        var argsXML:XMLList =  s..arg;

        if(argsXML.length() < 4) // [type, name, section, value]
          continue;

        var args:Array = [];

        for each(var a:XML in argsXML)
          args.push(String(a.@value));

        var cname:String = parent.@name;
        ns = new Namespace(String(parent.@uri));
        value = script.ns::[cname]; // value of the constant

        // ".text/12" => 5456/*explicit value*/ + 1234/*section offset*/
        map[args[2] + "/" + value] = uint(uint(args[3]) + sections[args[2]][0]);
      }
    }
  }

  private static var cachedCSyms:Dictionary = new Dictionary;
  private static var toBeMapped:Vector.<CModule> = modules;
  private static function mapCsymsWithExplicitValues():Dictionary
  {
    var module:CModule;
    var newToBeMapped:Vector.<CModule> = new Vector.<CModule>;
    for each (module in toBeMapped) {
      if (module.script) {
        module.mapCsymsWithExplicitValues(cachedCSyms);
      } else {
        newToBeMapped.push(module);
      }
    }
    toBeMapped = newToBeMapped;
    return cachedCSyms;
  }

  /**
  * relies on metadata -- won't return info in stripped builds
  * returns an Array of sym descriptors
  * sym descriptor is an Array of form:
  * [nmStyleType, symName(, sectName, value)]
  * @private
  */
  public function getSymsUsingMD():Array
  {
    var esyms:Dictionary = CModule.mapCsymsWithExplicitValues();

    return getSymsUsingMDWithExplicitSyms(esyms);
  }

  private function getSymsUsingMDWithExplicitSyms(esyms:Dictionary):Array
  {
    var script:Object = getScript();
    var result:Array = [];
    var ns:Namespace
    var value:uint
    var sv:String


    if(modSyms) {
      for(var i:int=0; i<modSyms.length; i++) {
        var arr:Array = modSyms[i];

        var _type:String = arr[0]
        var _name:String = arr[1]

        if(arr.length == 2) {
          result.push([_type, _name]);
        } else {
          var _sect:String = arr[2]
          if(_type == "t")
          {
            ns = new Namespace(modPackage);
            value = script.ns::[_name];
          }
          else
          {
            namespace C_RunNS = "C_Run";
            value = script.C_RunNS::[_name];
          }
          sv = _sect + "/" + value;

          if(typeof(esyms[sv]) != "undefined") // remap if explicitly specified
            value = esyms[sv];

          result.push([_type, _name, _sect, value]);
        }
      }
    } else {
      var scriptXML:XML = CModule.describeType(script);
      var csymsXML:XMLList = scriptXML..metadata.(@name == "Csym");

      // [Csym("T", "_myFunc")] is Csym MD format
      for each(var s:XML in csymsXML)
      {
        var args:Array = [];

        for each(var a:XML in s..arg)
          args.push(String(a.@value));

        var type:String = args[0];
        var name:String = args[1];
        var parent:XML = s.parent();

        // MD applied to a constant? it's a live sym we should be able to get
        // a value from
        if(parent && parent.name() == "constant")
        {
          var sectName:String = args[2];
          var cname:String = parent.@name;
          ns = new Namespace(String(parent.@uri));
          value = script.ns::[cname];
          sv = sectName + "/" + value;

          if(typeof(esyms[sv]) != "undefined") // remap if explicitly specified
            value = esyms[sv];

          result.push([type, name, sectName, value]);
        }
        else
          result.push([type, name]);
      }
    }

    return result;
  }

  /**
  * @private
  */
  public function notifyInit(script:Object):void
  {
    seenModuleInit = true;

    var thunk:Function = modThunks[modPackage];

    if(thunk != null)
      thunk(); // remove remaining thunks
    this.script = script;
    this.init = null;
    try
    { AlcDbgHelper.notifyScriptInit(this) }
    catch(e:*) {}
    try
    {
      namespace C_RunNS = "C_Run";
      if(script.C_RunNS::["ESP"])
        CModule.regsInitted = true;
    } catch(e:*) {}
  }

  /**
  * @private
  */
  public function getScript():Object
  {
    if(!script)
      init();
    return script;
  }

  /**
  * @private
  */
  public function getPublicSym(s:String):int
  {
    var script:Object = getScript();
    namespace C_RunNS = "C_Run";

    return script.C_RunNS::["_"+s];
  }

  /**
  * @private
  */
  public function runCtors():void
  {
     var ctorsSect:* = sections[".ctors"];
    if(ctorsSect)
    {
      var size:int = ctorsSect[1];
      var args:Vector.<int> = new Vector.<int>;

      if(size)
        getScript(); // ensure module is initialized
      for(var p:int = ctorsSect[0]; size >= 4; size -=4, p += 4)
      {
        var fun:int = read32(p);

        if(fun)
          callI(fun, args);
      }
    }
  }

  /**
  * @private
  */
  public function runDtors():void
  {
    var dtorsSect:* = sections[".dtors"];
    if(dtorsSect)
    {
      var size:int = dtorsSect[1];
      var p:int = dtorsSect[0];
      var args:Vector.<int> = new Vector.<int>;

      if(size)
        getScript(); // ensure module is initialized
      for(; size >= 4; size -=4, p += 4)
      {
        var fun:int = read32(p);

        if(fun)
          callI(fun, args);
      }
    }
  }

  /**
  * Some fake definitions so that the types look correct in the ASDocs
  * even though they are actually just "Object" due to us wanting the
  * same abc to work in both the shell and player.
  */
  CONFIG::asdocs
  {
    import flash.display.Sprite;

    /**
    * Return the current kernel implementation
    * @return An object that implements the Posix interface defined by PlayerKernel
    */
    public static function get kernel():IKernel {}

    /**
    * Set the current kernel implementation
    * @param kernel The object satisfying IKernel to act as the current flascc Kernel
    */
    public static function set kernel(kernel:IKernel):void {}

    /**
    * A reference to the current rootSprite so that flascc code has an easy way of getting to the stage regardless of what preloaders might be employed.
    */
    public static var rootSprite:Sprite;
  }

  /**
  * The actual definitions with duck typed parameters.
  */
  CONFIG::actual
  {
    public static var rootSprite:Object;


    private static var _kernel:Object = null;

    public static function get kernel():Object
    {
      if (!_kernel) {
        try {
          _kernel = new flash.utils.ShellPosix();
        } catch (e:ReferenceError) {
          _kernel = new com.adobe.flascc.kernel.PlayerKernel();
        }
      }
      return _kernel;
    }

    public static function set kernel(kernel:Object):void
    {
      _kernel = kernel
    }
  }

  /**
  * A reference to the current Console object, it is set when one of the start methods is called
  */
  public static var activeConsole:Object;


  private static var _vfs:Object = null;

  /**
  * The current VFS implementation
  * @return An object that implements the IVFS interface
  */
  public static function get vfs():IVFS
  {
    if (!_vfs) {
      _vfs = new com.adobe.flascc.vfs.DefaultVFS();
    }
    return _vfs;
  }

  public static function set vfs(fs:IVFS):void
  {
    _vfs = fs;
  }

  /**
  * Copies each string into a unique buffer in domainMemory
  * @param strs A Vector containing the Actionscript strings t be copied into domainMemory
  * @retun A Vector containing pointers to each of the allocated strings within domainMemory.
  *        As with mallocString() these are manually managed buffers and the caller is
  *        responsible for freeing each pointer using free().
  */
  private static function mallocStrings(strs:Vector.<String>):Vector.<int>
  {
    var result:Vector.<int> = new Vector.<int>;

    for(var i:int = 0; i < strs.length; i++)
      result.push(mallocString(strs[i]));
    return result;
  }

  /**
  * Copies each string into a unique buffer in domainMemory
  * @param strs A Vector containing the Actionscript strings t be copied into domainMemory
  * @retun A Vector containing pointers to each of the allocated strings within domainMemory.
  *        As with mallocString() these are manually managed buffers and the caller is
  *        responsible for freeing each pointer using free().
  */
  private static function mallocPtrArray(ptrs:Vector.<int>):int
  {
    if(ptrs.length == 0)
      return 0;

    var result:int = malloc(ptrs.length * 4);

    ram_init.position = result;
    for(var i:int = 0; i < ptrs.length; i++)
      ram_init.writeInt(ptrs[i]);
    return result;
  }

  /**
  * @private
  */
  private static function allocaStrings(strs:Vector.<String>):Vector.<int>
  {
    var result:Vector.<int> = new Vector.<int>;

    for(var i:int = 0; i < strs.length; i++)
      result.push(allocaString(strs[i]));
    return result;
  }

  /**
  * @private
  */
  private static function allocaPtrArray(ptrs:Vector.<int>):int
  {
    if(ptrs.length == 0)
      return 0;

    var result:int = alloca(ptrs.length * 4);

    ram_init.position = result;
    for(var i:int = 0; i < ptrs.length; i++)
      ram_init.writeInt(ptrs[i]);
    return result;
  }

  /**
  * @private
  */
  public static function runCtors():void
  {
    // run in reverse!
    for each (var mod:CModule in getModuleVector().reverse())
      mod.runCtors();
  }

  /**
  * @private
  */
  public static function runDtors():void
  {
    for each (var mod:CModule in getModuleVector())
      mod.runDtors();
  }

  /**
  * This method services any pending uiThunk requests that background threads
  * have queued up. A good place to call this would be in the <code>enterFrame</code> handler.
  */
  public static function serviceUIRequests():void
  {
    var fun:int = read32(_flascc_uiTickProc);

    if(fun)
      callI(fun, new <int>[]);
  }

  /**
  * Calls the libc <code>__start1</code> function which, in turn, causes <code>main()</code> to execute.
  * @param console The current Console object
  * @param args A vector of Strings that are used to fill the argv array given to main. The first String is typically used to specify the name of the application.
  * @param env A vector of Strings that are used to populate the environment variables accesible by <code>getenv()</code>. There should be an even number of Strings in this Vector as they are treated as name/value pairs.
  * @param preserveStack Indicates whether the C stack should be preserved after the call to <code>__start1</code>. If this is set to false the stack depth will return to the value when start() was called upon return.
  * @return The exit code of the C application
  */
  public static function start(console:Object, args:Vector.<String>, env:Vector.<String>, preserveStack:Boolean = true):int
  {
    CONFIG::debug { trace("CModule::start: " + arguments); }
    
    if(console)
    {
      if(activeConsole)
        throw new Error("calling start with a console with an already active console");
      activeConsole = console;
    }

    var esp:int = ESP;
    var result:int;

    try {
      // TODO: leaks if you invoke main multiple times...
      var eargs:Vector.<int> = allocaStrings(args);
      eargs.push(0);
      eargs = eargs.concat(allocaStrings(env));
      eargs.push(0);
      var argv:int = allocaPtrArray(eargs);

      esp = ESP;

      var fargs:Vector.<int> = new Vector.<int>;

      fargs.push(0, args.length, argv);

      result = callI(__start1, fargs);
    } catch(exitCode:Exit) {
      result = exitCode.code;
    }
    finally
    {
      if(!preserveStack)
        ESP = esp;
    }
    return result;
  }

  /**
  * Calls the libc <code>__start1</code> function which, in turn, causes <code>main()</code> to execute. Unlike <code>start()</code>, this method handles the case where the main C run loop has been broken by a call to "AS3_GoAsync" within the C code. This allows main to return without allowing C/C++ static initializers to run. This should be used when you want to run long running code with a broken run-loop, or run some code as a library.
  * @param console The current Console object
  * @param args A vector of Strings that are used to fill the argv array given to main. The first String is typically used to specify the name of the application.
  * @param env A vector of Strings that are used to populate the environment variables accesible by <code>getenv()</code>. There should be an even number of Strings in this Vector as they are treated as name/value pairs.
  * @param preserveStack Inidcates whether the C stack should be preserved after the call to <code>__start1</code>. If this is set to false the stack depth will return to the value when start() was called upon return.
  */
  public static function startAsync(console:Object = null, args:Vector.<String> = null, env:Vector.<String> = null, preserveStack:Boolean = true):void
  {
    CONFIG::debug { trace("CModule::startAsync: " + arguments); }
    
    if(activeConsole)
      throw new Error("calling startAsync with an active console");

    if(!args)
      args = new Vector.<String>

    if(!env)
      env = new Vector.<String>

    // try to set us as the ui thread
    /*try {
      C_Run.workerClass["current"].setSharedProperty("flascc.uiThread.threadId", realThreadId);
    } catch(e:*) {}*/
    try {
      CModule.start(console, args, env, preserveStack);
    } catch(e:GoingAsync) {
    } catch(e:*) {
      activeConsole = undefined;
      throw e;
    }
  }

  /**
  * Creates a background Worker and runs the libc function <code>__start1</code> within that Worker. This allows you to run main without breaking the run loop, but requires the SWF to be version 18 or higher (Flash Runtime 11.5).
  * @param console The current Console object
  * @param args A vector of Strings that are used to fill the argv array given to main. The first String is typically used to specify the name of the application.
  * @param env A vector of Strings that are used to populate the environment variables accesible by <code>getenv()</code>. There should be an even number of Strings in this Vector as they are treated as name/value pairs.
  * @param afterStackSize The amount of stack space to allocate for the ui thread (code that will run the uiThunk callbacks and also any calls to callI coming from the main Worker).
  */
  public static function startBackground(console:Object = null, args:Vector.<String> = null, env:Vector.<String> = null, afterStackSize:int = 65536):void
  {
    CONFIG::debug { trace("CModule::startBackground: " + arguments); }
    
    if(console)
    {
      if(activeConsole)
        throw new Error("calling startBackground with a console with an already active console");
      activeConsole = console;
    }

    if(afterStackSize < 8192 || afterStackSize % pageSize)
      throw new Error("invalid stack size");

    if(!args)
      args = new Vector.<String>

    if(!env)
      env = new Vector.<String>

    var esp:int = ESP;

    try
    {
      // TODO: leaks if you invoke main multiple times...
      var eargs:Vector.<int> = allocaStrings(args);
      eargs.push(0);
      eargs = eargs.concat(allocaStrings(env));
      eargs.push(0);
      var argv:int = allocaPtrArray(eargs);

      esp = ESP;

      var fargs:Vector.<int> = new Vector.<int>;

      fargs.push(0, args.length, argv);

      var tesp:int = esp;
      var w:* = newThread(0, tesp, __start1, fargs);
      threadId = realThreadId = 1; // change tid to 1 after newThread!
      w.setSharedProperty("flascc.uiThread.threadId", threadId);
      var stack:int = sbrk(afterStackSize, pageSize);
      esp = ESP_init = stack + afterStackSize;
      w.setSharedProperty("flascc.uiThread.esp_init", esp);
      w.setSharedProperty("flascc.uiThread.stackSize", afterStackSize);
      w.setSharedProperty("flascc.uiThread.tcbpp", stack);
      threadArbMutexLock();
      w.start();
      // serviceUIRequests shouldn't need much stack here as it will only be servicing posix requests
      // for very early startup, so we use 4k at the bottom
      ESP = stack + 4096; // give ourself a bit of stack for serviceUIRequests
      do // spin (servicing ui requests) until we get our tcbp
      {
        threadArbCondWait(1);
        serviceUIRequests();
      }
      while(!(tcbp = read32(stack)));
      // signal that we got it and are no longer relying on the mini-stack
      // (meaning serviceUIRequests can handle more arbitrary requests)
      write32(stack, 0);
      threadArbCondsNotify(-1);
      threadArbMutexUnlock();
    }
    finally
    {
      ESP = esp;
    }
  }

  // set up worker init stuff
  workerInits.push(function(worker:*):void
  {
    CONFIG::debug { trace("CModule::workerInit: " + threadId); }
    prepForThreadedExec();

    var v:Vector.<CModule> = getModuleVector();

    // set section data into worker's start args
    for each(var m:CModule in v)
    {
      var pkgName:String = m.modPackage;
      var sects:Object = m.getSections();

      for(var sname:String in sects)
        worker.setSharedProperty("flascc.sect." + pkgName + "." + sname, sects[sname][0]);
      worker.setSharedProperty("flascc." + pkgName + ".oncep", m.oncep);
    }
  });
  //CONFIG::debug { trace("CModule::workerInit added: " + threadId); }
}

//--------------------------------------------------------------------------
//
//  GoingAsync
//
//--------------------------------------------------------------------------

/**
* @private
*/
public class GoingAsync {}

//--------------------------------------------------------------------------
//
//  PtrLink
//
//--------------------------------------------------------------------------

/**
* @private
*/
internal class PtrLink
{
  public const ptr:int = 0;
  
  public var next:PtrLink;

  public function PtrLink(ptr:int)
  {
    this.ptr = ptr;
  }
}

try { new AlcDbgHelper } catch(e:*) {}

}

package C_Run
{
  import com.adobe.flascc.CModule;

  [Csym("U", "__start1")]
  [Csym("U", "_malloc")]
  [Csym("U", "_free")]
  [Csym("U", "_flascc_uiTickProc")]
  [Csym("D", "__DYNAMIC")]
  /**
  * no dyld yet
  * @private
  */
  public const __DYNAMIC:int = 0;

  /**
  * setjmp id for calling function (calling setjmp)
  * @private
  */
  public var sjid:int;

  /**
  * thread control block for current thread
  * @private
  */
  public var tcbp:int = -1;

  /**
  * @private
  */
  public function longjmp(ptr:int, retval:int):void {
    var id:int = CModule.read32(ptr+4);
    var esp:int = CModule.read32(ptr+8);
    //trace("longjmp: " + esp + "/" + retval);
    throw new LongJmp(esp, id, retval);
  }

  /**
  * @private
  */
  public function setjmp(ptr:int, id:int, esp:int):int {
    CModule.write32(ptr+4, id);
    CModule.write32(ptr+8, esp);
    //trace("setjmp: " + esp);
    return 0;
  }
}

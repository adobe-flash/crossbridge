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

package com.adobe.flascc.kernel
{
  import flash.utils.Proxy;
  import flash.utils.flash_proxy;
  import com.adobe.flascc.CModule;
  import C_Run.threadId;

  public dynamic class PlayerKernel extends Proxy
  {
    public function PlayerKernel(...args:Array)
    {
      trace("(" + threadId + ") Warning: PlayerKernel instantiated from shell! " + (new Error).getStackTrace());
    }

    flash_proxy override function callProperty(name:*, ...args:Array):*
    {
      if(name == "write")
      {
        var s:String = CModule.readString(args[1], args[2]);
        trace("(" + threadId + ") PlayerKernel.write(" + args[0] + "): " + s);
        return int(args[2]);
      }
/*
      trace("(" + threadId + ") Warning: PlayerKernel." + name + "(" + args + ") called from shell! " + (new Error).getStackTrace());
*/
      return -1;
    }
  }
}

import com.adobe.flascc.*;
import C_Run.*;
import flash.utils.ShellPosix;

// return argv in shell, null in player
function getShellArgv():Array
{
  try
  {
    var avmplusNS:Namespace = new Namespace("avmplus");

    return avmplusNS::["System"].argv;
  }
  catch(e:*) {}
  return null;
}

// return argv in shell, null in player
function getShellEnviron():Array
{
  try
  {
    var avmplusNS:Namespace = new Namespace("avmplus");

    return avmplusNS::["System"].environ;
  }
  catch(e:*) {}
  return null;
}

if(threadId == 0) // main thread
{
  var args:Vector.<String> = new Vector.<String>;
  var argv:Array = getShellArgv();
  try // TODO work around for shells w/o ShellPosix
  {
    args.push(ShellPosix.getAppPath());
  }
  catch(e:*) {}
  if(argv)
  {
    for(var i:int = 0; i < argv.length; i++)
      args.push(argv[i]);
  }
  var env:Vector.<String> = new Vector.<String>;
  var environ:Array = getShellEnviron();
  if(environ)
  {
    for(var j:int = 0; j < environ.length; j++)
      env.push(environ[j]);
  }
  
  if(argv)  {// running in the shell -- just go!
    import avmplus.System;
    System.exit(CModule.start(null, args, env));
  } else {
    throw "argv is null"
  }
}
else
{
  try
  {
    var entry:int = workerClass["current"].getSharedProperty("flascc.thread_entry");
    var fargs:Vector.<int> = workerClass["current"].getSharedProperty("flascc.thread_args").readObject();

//trace("thread starting: " + threadId + " => " + fargs.length);
    if(!(ptr2fun_init is Vector.<Function>)) // sparse function mapping? explicitly init every module...
    {
//trace("faulting in all modules: " + (ptr2fun_init.constructor));
      for each(var m:* in CModule.getModuleVector()) //type is * to avoid cyclic depend on CModule script...
        m.getScript();
    }
    CModule.callI(entry, fargs);
  }
  catch(e:*)
  {
    if(e is Exit)
    {
      import avmplus.System;
      System.exit(e.code); // TODO doesn't appear to actually exit!
    }
    else
      trace("Warning: Worker threw exception: " + threadId + " - " + e + " " + ((e is Error) ? e.getStackTrace() : ""));
  }
  trace("Warning: Worker returned without exiting: " + threadId);
}

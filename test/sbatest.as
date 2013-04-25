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

// tests atomicCompareAndSwapLength on a share ByteArray
package
{

import avm2.intrinsics.memory.*;
import flash.utils.*;
import flash.system.*;
import avmplus.*;

const tid:int = Worker.current.getSharedProperty("tid");
const sba:ByteArray = Worker.current.getSharedProperty("sba") || new ByteArray;

if(!tid) // first thread! create a bunch of clones
{
  trace("thread 0 initial setup");
  sba.length = 1024 * 1024;
  sba.shareable = true;
  sba.endian = "littleEndian";
  Domain.currentDomain.domainMemory = sba;
  // all workers will swap pointers using addr 0 of the sba
  si32(4, 0); // prime ptr swapping area
  for(var i:int = 1; i < 8; i++)
  {
    var w:* = WorkerDomain.current.createWorkerFromPrimordial();
    w.setSharedProperty("tid", i);
    w.setSharedProperty("sba", sba);
    w.start();
  }
}
else
{
  sba.endian = "littleEndian";
  Domain.currentDomain.domainMemory = sba;
}

const chunkSize:int = 16;

trace("thread " + tid + " beginning");

for(var j:int = 0; j < 1000000; j++)
{
  var p:int = sba.length;
  var pn:int;

  for(;;) // allocate chunk via atomic increment
  {
    pn = sba.atomicCompareAndSwapLength(p, p+chunkSize);

    if(pn == p)
      break;
    p = pn;
  }

  if(!(pn & (1024 * 1024 - 1)))
    trace(pn);

  var po:int = li32(0);

  // swap pointers with some other worker by atomic xchg on sba at addr 0
  for(;;)
  {
    pn = casi32(0, po, p);
    if(pn == po)
      break;
    po = pn;
  }
  try
  {
    // write to swapped pointer
    si32(0xdeadbeef, po);
  }
  catch(e:*)
  {
    trace("thread " + tid + " failed to write to " + po);
    throw e;
  }
}

}

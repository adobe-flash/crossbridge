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

//package utils
//{

import flash.utils.Dictionary;

// public
class ReferenceToken
{
  public function run():void {}
}

// public
final class ReferenceQueue
{
  // map objects to Refs with weak keys
  private var _obj2ref:Dictionary = new Dictionary(true);
  // Refs mapped to liveness tokens
  private var _refs:Dictionary = new Dictionary(false);

  // Refs still to enqueue: _newPairXXX incrementally move into _obj2ref and _newRefs
  // _newRefs incrementally move into _refs
  private const _newPairRefs:Vector.<ReferenceToken> = new Vector.<ReferenceToken>;
  private const _newPairObjs:Vector.<*> = new Vector.<*>;
  private const _newRefs:Vector.<ReferenceToken> = new Vector.<ReferenceToken>;

  // Refs of collected objects that are still in "_refs"
  // these are incrementally removed from _refs and moved to _q
  private const _deadRefs:Vector.<ReferenceToken> = new Vector.<ReferenceToken>;
  // Actual queue of dead objects
  private var _q:Vector.<ReferenceToken> = new Vector.<ReferenceToken>;

  // are we sweeping? (vs marking)
  private var _sweeping:Boolean;
  // current iterator
  private var _it:int;
  // current liveness token
  private var _liveToken:int = 1;
  // sub-RQ for the next generation
  private var _nextGen:ReferenceQueue;
  // let _nextGen work on odd _pollPhase
  private var _pollPhase:int;

  public function ReferenceQueue(gens:int = 2)
  {
    if(gens > 1)
      _nextGen = new ReferenceQueue(gens-1);
  }

  private function reset():void
  {
    // reset state
    _it = 0;
    _liveToken = 1;
    _sweeping = false;
    _deadRefs.length = 0;
    _newRefs.length = 0;
    _newPairRefs.length = 0;
    _newPairObjs.length = 0;
    _refs = new Dictionary(false);
    _obj2ref = new Dictionary(true);
    _pollPhase = 0;
  }

  // queue all currently tracked Refernce as if their objects
  // got collected
  public function flush():void
  {
    _q = _q.concat(_newRefs);
    _q = _q.concat(_newPairRefs);
    for(var aref:* in _refs)
      _q.push(aref);
    reset();
    if(_nextGen)
    {
      _nextGen.flush();
      _q = _q.concat(_nextGen._q);
      _nextGen._q.length = 0;
    }
  }

  // do some incremental work and return a dead ref is we have one
  public function poll():ReferenceToken
  {
    var ref:ReferenceToken;
    var obj:Object;
    var n:int;

    if(_sweeping) // we're iterating over _refs
    {
      n = _newPairRefs.length;
      // add some new objects to _obj2ref
      while(n--)
      {
        ref = _newPairRefs[n];
        // add to _obj2ref
        obj = _newPairObjs[n];
        _obj2ref[obj] = ref;
      }
      _newPairRefs.length = 0;
      _newPairObjs.length = 0;
      // work based on new object pressure
      n = _newRefs.length;
      // add dead refs to _dead
      while(n--)
      {
        _it = __xasm<int>(push(_refs), push(_it), hasnext);
        if(!_it)
        {
          // if we have a sub-generation
          // move live stuff there!
          if(_nextGen)
          {
            var it:int = 0;
            for(;;)
            {
              it = __xasm<int>(push(_obj2ref), push(it), hasnext);
              if(!it)
                break;
              ref = __xasm<Object>(push(_obj2ref), push(it), nextvalue);
              obj = __xasm<Object>(push(_obj2ref), push(it), nextname);
              _nextGen.add(ref, obj);
            }
            reset();
          }
          else
          {
            // finished sweep; back to marking
            _sweeping = false;
            // switch the liveness token
            _liveToken = -_liveToken;
          }
          break;
        }

        var token:int = __xasm<Object>(push(_refs), push(_it), nextvalue);
        if(token != _liveToken) // dead
        {
          var name:Object = __xasm<Object>(push(_refs), push(_it), nextname);
          _deadRefs.push(name);
        }
      }
    }
    else // we're iterating over _obj2ref
    {
      // add some objects to _refs
      n = _newRefs.length;
      while(n--)
        _refs[_newRefs[n]] = _liveToken;
      _newRefs.length = 0;
      // remove some objects from _refs
      n = _deadRefs.length;
      while(n--)
      {
        ref = _deadRefs[n];
        delete _refs[ref];
        _q.push(ref);
      }
      _deadRefs.length = 0;
      // work based on new object pressure
      n = _newPairRefs.length;
      // mark live refs live
      while(n--)
      {
        _it = __xasm<int>(push(_obj2ref), push(_it), hasnext);
        if(!_it)
        {
          // finished marking; sweep
          _sweeping = true;
          break;
        }
        ref = __xasm<Object>(push(_obj2ref), push(_it), nextvalue);
        _refs[ref] = _liveToken;
      }
    }
    if(_nextGen)
    {
      if(_pollPhase & 1)
      {
        ref = _nextGen.poll();
        if(ref)
          return ref;
      }
      _pollPhase++;
    }
    return _q.pop();
  }

  public function add(ref:ReferenceToken, obj:Object):void
  {
    if(_sweeping) // don't touch _refs
    {
      _newRefs.push(ref);
      _obj2ref[obj] = ref;
    }
    else // don't touch _obj2ref
    {
      _refs[ref] = _liveToken;
      _newPairRefs.push(ref);
      _newPairObjs.push(obj);
    }
  }
}

// private
class CleanupReference extends ReferenceToken
{
  private const fun:Function;

  public function CleanupReference(fun:Function)
  {
    this.fun = fun;
  }

  public override function run():void
  {
    fun();
  }
}

// public
class CleanupBase
{
  // call all cleanups
  public static function cleanNow():void
  {
    var rq:ReferenceQueue = cleanuprq;

    rq.flush();

    var ref:ReferenceToken;

    do
    {
      try
      {
        while((ref = rq.poll()))
          ref.run();
      }
      catch(e:*) {}
    }
    while(ref);
  }

  public function CleanupBase(f:Function)
  {
/*
    // control
    try { f(); } catch(e:*) {}
    return;
//*/

    var rq:ReferenceQueue = cleanuprq;
    var ref:ReferenceToken = new CleanupReference(f);

    rq.add(ref, this);
    // run up to 2 cleanups for every construction
    // to guarantee they don't back up
    if((ref = rq.poll()))
    {
      try { ref.run(); } catch(e:*) {}
      ref = rq.poll();
      if(ref)
        try { ref.run(); } catch(e:*) {}
    }
  }
}

//} // end package

// a reference queue for CleanupBase
const cleanuprq:ReferenceQueue = new ReferenceQueue;

//*** test
class Foo extends CleanupBase
{
  // MUST be static!! otherwise, the Function itself
  // will have a reference to the object
  static function createCleanup(s:String):Function
  {
    return function() { trace("dead: #" + s) };
  }

  public function Foo(s:String)
  {
    trace("new : #" + s);
    super(createCleanup(s));
  }
}

var fv:Vector.<Foo> = new Vector.<Foo>;
var i:int;
var startTime:Number = (new Date).time;
var lastTime:Number = startTime;
var lastCount:int;

for(var z:int = 0; z < 500000; z++)
{
  if(!(i % 1000))
  {
    var now:Number = (new Date).time;
    var diff:Number = now - lastTime;

    if(diff >= 1000)
    {
      lastTime = now;
      trace("live: " + fv.length);
      trace("rate: " + ((i-lastCount)*1000)/diff + " new/s (" + ((now - startTime)/1000) + " s elapsed)");
      lastCount = i;
    }
  }
  var foo:Foo = new Foo(String(i));
  if(Math.random() >= 0.5)
    fv.push(foo);
  i++;
}
CleanupBase.cleanNow();

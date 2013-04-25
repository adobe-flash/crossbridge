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

extern "C" void (*flascc_uiTickProc)();

  struct UIThunkReq
  {
    void *proc;
    void *args;
    int size;
    struct __avm2_retdata retdata;
  };

#if AS3_UI
  static struct UIRCQEntry
  {
    unsigned id;
    int rel; // neg rel is valid acquire
  } sUIRCQ[127];
  static unsigned sUIRCQLock = 0;
  static int sUIRCQSize = 0;
  static void (*sOldUITickProc)();

  // applies queued ref count adjustments
  static void *UIRCQProcess(void *arg)
  {
    avm2_lock(&sUIRCQLock);
//if(sUIRCQSize) inline_as3 ("trace('q process')");
    for(int n = 0; n < sUIRCQSize; n++)
    {
      UIRCQEntry &entry = sUIRCQ[n];
//inline_as3 ("trace('q rel: ' + %0 + ' ' + %1)" :: "r"(entry.id), "r"(entry.rel));
      inline_as3 ("as3_id2rcv[%0].release(%1)" : : "r"(entry.id), "r"(entry.rel));
    }
    sUIRCQSize = 0;
    avm2_unlock(&sUIRCQLock);
  }

  // run every frame in player
  static void UIRCQTick()
  {
    if(sOldUITickProc)
      sOldUITickProc();
    UIRCQProcess(NULL);
  }

  // installs tick proc
  static void UIRCQSetup() __attribute__((constructor));
  static void UIRCQSetup()
  {
    for(;;) // install our tick proc
    {
      sOldUITickProc = flascc_uiTickProc;
      if(__sync_bool_compare_and_swap(&flascc_uiTickProc, sOldUITickProc, &UIRCQTick))
        break;
    }
//inline_as3 ("trace('tick proc installed')");
  }

  // release a certain amount!
  static void UIRCRel(unsigned id, int n)
  {
    for(;;)
    {
      avm2_lock(&sUIRCQLock);
      if(sUIRCQSize > 0) // try to adjust last entry
      {
        UIRCQEntry &entry = sUIRCQ[sUIRCQSize-1];
        if(entry.id == id)
        {
          if(!(entry.rel += n)) // adjustment went to 0?
            sUIRCQSize--; // now a no-op
//inline_as3 ("trace('adjusted entry')");
          break; // done!
        }
      }
      // need to add an entry... full? process it
      if(sUIRCQSize == sizeof(sUIRCQ) / sizeof(UIRCQEntry))
      {
        avm2_unlock(&sUIRCQLock);
//inline_as3 ("trace('q full')");
        avm2_ui_thunk(UIRCQProcess, NULL);
        continue; // try again -- another thread may have added an entry w/ this id
      }
      UIRCQEntry &entry = sUIRCQ[sUIRCQSize++];
      entry.id = id;
      entry.rel = n;
//inline_as3 ("trace('new entry')");
      break; // done!
    }
    avm2_unlock(&sUIRCQLock);
  }

  void var::internal::acquire(unsigned id)
  {
    if(id > kLastSpecial)
      UIRCRel(id, -1); // acquire -> negative release
  }

  void var::internal::release(unsigned id)
  {
    if(id > kLastSpecial)
      UIRCRel(id, 1);
  }
#else
  void var::internal::acquire(unsigned id)
  {
    if(id > kLastSpecial)
      inline_as3 ("as3_id2rcv[%0].acquire()" : : "r"(id));
  }

  void var::internal::release(unsigned id)
  {
    if(id > kLastSpecial)
      inline_as3 ("as3_id2rcv[%0].release()" : : "r"(id));
  }
#endif

  var::~var()
  {
    var::internal::release(_id);
  }

  var::var(const var &val)
  { 
    _id = val._id;
    var::internal::acquire(_id);
  }

  var &var::operator=(var val)
  {
    var::internal::acquire(val._id);
    var::internal::release(_id);
    _id = val._id;
    return *this;
  }

  var internal::new_int(int n) MAYBEDELEGATEATTR;
  var internal::new_int(int n)
  {
    MAYBEDELEGATE(&new_int);
    unsigned id;
    inline_as3 ("%0 = as3_valAcquireId(%1)" : "=r"(id) : "r"(n));
    return var(id);
  }

  var internal::new_uint(unsigned n) MAYBEDELEGATEATTR;
  var internal::new_uint(unsigned n)
  {
    MAYBEDELEGATE(&new_uint);
    unsigned id;
    inline_as3 ("%0 = as3_valAcquireId(uint(%1))" : "=r"(id) : "r"(n));
    return var(id);
  }
  
  var internal::new_Number(double n) MAYBEDELEGATEATTR;
  var internal::new_Number(double n)
  {
    MAYBEDELEGATE(&new_Number);
    unsigned id;
    inline_as3 ("%0 = as3_valAcquireId(%1)" : "=r"(id) : "r"(n));
    return var(id);
  }

  var internal::new_Boolean(bool b)
  {
    return var(b ? var::internal::kTrue : var::internal::kFalse);
  }
  
  var internal::new_String(const char *s, int len) MAYBEDELEGATEATTR;
  var internal::new_String(const char *s, int len)
  {
    MAYBEDELEGATE(&new_String);
    if(len < 0)
      len = strlen(s);
    unsigned id;
    inline_as3 ("%0 = as3_valAcquireId(%2 ? CModule.readString(%1, %2) : '')" : "=r"(id) : "r"(s), "r"(len));
    return var(id);
  }

  var internal::new_Namespace(const char *s, int len) MAYBEDELEGATEATTR;
  var internal::new_Namespace(const char *s, int len)
  {
    MAYBEDELEGATE(&new_String);
    if(len < 0)
      len = strlen(s);
    unsigned id;
    inline_as3 ("%0 = as3_valAcquireId(new Namespace(%2 ? CModule.readString(%1, %2) : ''))" : "=r"(id) : "r"(s), "r"(len));
    return var(id);
  }

  var internal::new_Vector(var cc) MAYBEDELEGATEATTR;
  var internal::new_Vector(var cc)
  {
    MAYBEDELEGATE(&new_Vector);
    unsigned id;
    int except;
    inline_as3 (
      "try { "
      "var cc:Class = as3_id2rcv[%2].val; "
      "%0 = as3_valAcquireId(new Vector.<cc>()); "
      "%1 = 0; "
      "} catch(e:*) { %0 = as3_valAcquireId(e); %1 = 1; } "
      : "=r"(id), "=r"(except) : "r"(cc._id));
    if(except)
      throw var(id);
    return var(id);
  }

  var internal::new_Vector_int() MAYBEDELEGATEATTR;
  var internal::new_Vector_int()
  {
    MAYBEDELEGATE(&new_Vector_int);
    unsigned id;
    inline_as3 ("%0 = as3_valAcquireId(new Vector.<int>())" : "=r"(id));
    return var(id);
  }

  var internal::new_Vector_uint() MAYBEDELEGATEATTR;
  var internal::new_Vector_uint()
  {
    MAYBEDELEGATE(&new_Vector_uint);
    unsigned id;
    inline_as3 ("%0 = as3_valAcquireId(new Vector.<uint>())" : "=r"(id));
    return var(id);
  }

  var internal::new_Vector_Boolean() MAYBEDELEGATEATTR;
  var internal::new_Vector_Boolean()
  {
    MAYBEDELEGATE(&new_Vector_Boolean);
    unsigned id;
    inline_as3 ("%0 = as3_valAcquireId(new Vector.<Boolean>())" : "=r"(id));
    return var(id);
  }

  var internal::new_Vector_String() MAYBEDELEGATEATTR;
  var internal::new_Vector_String()
  {
    MAYBEDELEGATE(&new_Vector_String);
    unsigned id;
    inline_as3 ("%0 = as3_valAcquireId(new Vector.<String>())" : "=r"(id));
    return var(id);
  }

  var internal::new_Vector_Number() MAYBEDELEGATEATTR;
  var internal::new_Vector_Number()
  {
    MAYBEDELEGATE(&new_Vector_Number);
    unsigned id;
    inline_as3 ("%0 = as3_valAcquireId(new Vector.<Number>())" : "=r"(id));
    return var(id);
  }

  var internal::new_undefined()
  {
    return var(var::internal::kUndefined);
  }

  var internal::new_null()
  {
    return var(var::internal::kNull);
  }

  var internal::get_ram() MAYBEDELEGATEATTR;
  var internal::get_ram()
  {
    MAYBEDELEGATE(&get_ram);
    unsigned id;
    inline_as3 ("%0 = as3_valAcquireId(ram)" : "=r"(id));
  
    return var(id);
  }
  
  var internal::_typeof(var val) MAYBEDELEGATEATTR;
  var internal::_typeof(var val)
  {
    MAYBEDELEGATE(&_typeof);
    unsigned id;
    inline_as3 ("%0 = as3_valAcquireId(typeof(as3_id2rcv[%1].val))" : "=r"(id) : "r"(val._id));
    return var(id);
  }
  
  bool internal::strictequals(var a, var b)
  {
    return a._id == b._id;
  }

  bool internal::equals(var a, var b) MAYBEDELEGATEATTR;
  bool internal::equals(var a, var b)
  {
    MAYBEDELEGATE(&equals);
    int r;
    inline_as3 ("%0 = (as3_id2rcv[%1].val == as3_id2rcv[%2].val)" : "=r"(r) : "r"(a._id), "r"(b._id));
    return r != 0;
  }

  bool internal::lessthan(var a, var b) MAYBEDELEGATEATTR;
  bool internal::lessthan(var a, var b)
  {
    MAYBEDELEGATE(&lessthan);
    int r;
    inline_as3 ("%0 = (as3_id2rcv[%1].val < as3_id2rcv[%2].val)" : "=r"(r) : "r"(a._id), "r"(b._id));
    return r != 0;
  }

  bool internal::lessequals(var a, var b) MAYBEDELEGATEATTR;
  bool internal::lessequals(var a, var b)
  {
    MAYBEDELEGATE(&lessequals);
    int r;
    inline_as3 ("%0 = (as3_id2rcv[%1].val <= as3_id2rcv[%2].val)" : "=r"(r) : "r"(a._id), "r"(b._id));
    return r != 0;
  }

  bool internal::greaterthan(var a, var b) MAYBEDELEGATEATTR;
  bool internal::greaterthan(var a, var b)
  {
    MAYBEDELEGATE(&greaterthan);
    int r;
    inline_as3 ("%0 = (as3_id2rcv[%1].val > as3_id2rcv[%2].val)" : "=r"(r) : "r"(a._id), "r"(b._id));
    return r != 0;
  }

  bool internal::greaterequals(var a, var b) MAYBEDELEGATEATTR;
  bool internal::greaterequals(var a, var b)
  {
    MAYBEDELEGATE(&greaterequals);
    int r;
    inline_as3 ("%0 = (as3_id2rcv[%1].val >= as3_id2rcv[%2].val)" : "=r"(r) : "r"(a._id), "r"(b._id));
    return r != 0;
  }

  const var internal::_undefined = internal::new_undefined();
  const var internal::_null = internal::new_null();
  const var internal::_true = internal::new_Boolean(true);
  const var internal::_false = internal::new_Boolean(false);

  // TODO deprecated
  const var _undefined = internal::new_undefined();
  const var _null = internal::new_null();
  const var _true = internal::new_Boolean(true);
  const var _false = internal::new_Boolean(false);

  var internal::get_Worker(long tid) __attribute__((noinline));
  var internal::get_Worker(long tid)
  {
    MAYBEDELEGATE(&get_Worker);
    unsigned id;
    int except;
    if(tid == -1)
      thr_self(&tid);
    inline_as3 (
      "try { "
      "var match:*; "
      "for each(var w:* in workerDomainClass.current.listWorkers()) "
      "if(Number(w.getSharedProperty('flascc.threadId')) == %2) "
      "{ match = w; break; } "
      "%0 = as3_valAcquireId(match); "
      "%1 = 0; "
      "} catch(e:*) { %0 = as3_valAcquireId(e); %1 = 1; } "
      : "=r"(id), "=r"(except) : "r"(tid-8/*MIN_TID -- hack, plubm better!*/));
    if(except)
      throw var(id);
    return var(id);
  }

#if AS3_UI
  var internal::get_Stage() __attribute__((noinline));
  var internal::get_Stage()
  {
    MAYBEDELEGATE(&get_Stage);
    unsigned id;
    int except;
    inline_as3 (
      "try { "
      "%0 = as3_valAcquireId(CModule.rootSprite.stage); "
      "%1 = 0; "
      "} catch(e:*) { %0 = as3_valAcquireId(e); %1 = 1; } "
      : "=r"(id), "=r"(except));
    if(except)
      throw var(id);
    return var(id);
  }
#endif

  var internal::call_v(var fun, var rcv, var vargs, void *ramPos) __attribute__((noinline));
  var internal::call_v(var fun, var rcv, var vargs, void *ramPos)
  {
    MAYBEDELEGATE(&call_v);
    unsigned id;
    int except;
    inline_as3 (
      "try { "
      "ram.position = %5; "
      "%0 = as3_valAcquireId(as3_id2rcv[%2].val.apply(as3_id2rcv[%3].val, as3_id2rcv[%4].val)); "
      "%1 = 0; "
      "} catch(e:*) { %0 = as3_valAcquireId(e); %1 = 1; } "
      : "=r"(id), "=r"(except) : "r"(fun._id), "r"(rcv._id), "r"(vargs._id), "r"(ramPos));
    if(except)
      throw var(id);
    return var(id);
  }

  var internal::call(var fun, var rcv, int argCount, var *args, void *ramPos)
  {
    MAYBEDELEGATE(&call);
    var vargs = internal::new_Array(argCount, args);
    return internal::call_v(fun, rcv, vargs, ramPos);
  }

  var internal::getproperty(var obj, var name) __attribute__((noinline));
  var internal::getproperty(var obj, var name)
  {
    MAYBEDELEGATE((var (*)(var, var))getproperty);
    unsigned id;
    int except;
    inline_as3 (
      "try { "
      "var obj:* = as3_id2rcv[%2].val; "
      "%0 = as3_valAcquireId(obj[as3_id2rcv[%3].val]); "
      "%1 = 0; "
      "} catch(e:*) { %0 = as3_valAcquireId(e); %1 = 1; } "
      : "=r"(id), "=r"(except) : "r"(obj._id), "r"(name._id));
    if(except)
      throw var(id);
    return var(id);
  }

  var internal::getproperty(var obj, var ns, var name) __attribute__((noinline));
  var internal::getproperty(var obj, var ns, var name)
  {
    MAYBEDELEGATE((var (*)(var, var, var))getproperty);
    unsigned id;
    int except;
    inline_as3 (
      "try { "
      "var obj:* = as3_id2rcv[%2].val; "
      "var ns:Namespace = as3_id2rcv[%3].val; "
      "%0 = as3_valAcquireId(ns::obj[as3_id2rcv[%4].val]); "
      "%1 = 0; "
      "} catch(e:*) { %0 = as3_valAcquireId(e); %1 = 1; } "
      : "=r"(id), "=r"(except) : "r"(obj._id), "r"(ns._id), "r"(name._id));
    if(except)
      throw var(id);
    return var(id);
  }

  void internal::setproperty(var obj, var name, var val) __attribute__((noinline));
  void internal::setproperty(var obj, var name, var val)
  {
    MAYBEDELEGATE((void (*)(var, var, var))setproperty);
    unsigned eid;
    inline_as3 (
      "try { "
      "var obj:* = as3_id2rcv[%1].val; "
      "obj[as3_id2rcv[%2].val] = as3_id2rcv[%3].val; "
      "%0 = -1; "
      "} catch(e:*) { %0 = as3_valAcquireId(e); } "
      : "=r"(eid) : "r"(obj._id), "r"(name._id), "r"(val._id));
    if(eid != -1U)
      throw var(eid);
  }

  void internal::setproperty(var obj, var ns, var name, var val) __attribute__((noinline));
  void internal::setproperty(var obj, var ns, var name, var val)
  {
    MAYBEDELEGATE((void (*)(var, var, var, var))setproperty);
    unsigned eid;
    inline_as3 (
      "try { "
      "var obj:* = as3_id2rcv[%1].val; "
      "var ns:Namespace = as3_id2rcv[%2].val; "
      "ns::obj[as3_id2rcv[%3].val] = as3_id2rcv[%4].val; "
      "%0 = -1; "
      "} catch(e:*) { %0 = as3_valAcquireId(e); } "
      : "=r"(eid) : "r"(obj._id), "r"(ns._id), "r"(name._id), "r"(val._id));
    if(eid != -1U)
      throw var(eid);
  }

  var internal::getlex(var name) __attribute__((noinline));
  var internal::getlex(var name)
  {
    MAYBEDELEGATE((var (*)(var))getlex);
    unsigned id;
    int except;
    inline_as3 (
      "try { "
      "var ns:Namespace = new Namespace(''); "
      "%0 = as3_valAcquireId(ns::[as3_id2rcv[%2].val]); "
      "%1 = 0; "
      "} catch(e:*) { %0 = as3_valAcquireId(e); %1 = 1; } "
      : "=r"(id), "=r"(except) : "r"(name._id));
    if(except)
      throw var(id);
    return var(id);
  }

  var internal::getlex(var ns, var name) __attribute__((noinline));
  var internal::getlex(var ns, var name)
  {
    MAYBEDELEGATE((var (*)(var, var))getlex);
    unsigned id;
    int except;
    inline_as3 (
      "try { "
      "var ns:Namespace = as3_id2rcv[%2].val; "
      "%0 = as3_valAcquireId(ns::[as3_id2rcv[%3].val]); "
      "%1 = 0; "
      "} catch(e:*) { %0 = as3_valAcquireId(e); %1 = 1; } "
      : "=r"(id), "=r"(except) : "r"(ns._id), "r"(name._id));
    if(except)
      throw var(id);
    return var(id);
  }

  void internal::setlex(var name, var val) __attribute__((noinline));
  void internal::setlex(var name, var val)
  {
    MAYBEDELEGATE((void (*)(var, var))setlex);
    unsigned eid;
    inline_as3 (
      "try { "
      "var ns:Namespace = new Namespace(''); "
      "ns::[as3_id2rcv[%1].val] = as3_id2rcv[%2].val; "
      "%0 = -1; "
      "} catch(e:*) { %0 = as3_valAcquireId(e); } "
      : "=r"(eid) : "r"(name._id), "r"(val._id));
    if(eid != -1U)
      throw var(eid);
  }

  void internal::setlex(var ns, var name, var val) __attribute__((noinline));
  void internal::setlex(var ns, var name, var val)
  {
    MAYBEDELEGATE((void (*)(var, var, var))setlex);
    unsigned eid;
    inline_as3 (
      "try { "
      "var ns:Namespace = as3_id2rcv[%1].val; "
      "ns::[as3_id2rcv[%2].val] = as3_id2rcv[%3].val; "
      "%0 = -1; "
      "} catch(e:*) { %0 = as3_valAcquireId(e); } "
      : "=r"(eid) : "r"(ns._id), "r"(name._id), "r"(val._id));
    if(eid != -1U)
      throw var(eid);
  }

  var internal::construct(var cc) __attribute__((noinline));
  var internal::construct(var cc)
  {
    MAYBEDELEGATE((var (*)(var))construct);
    unsigned id;
    int except;
    inline_as3 (
      "try { "
      "%0 = as3_valAcquireId(new (as3_id2rcv[%2].val)()); "
      "%1 = 0; "
      "} catch(e:*) { %0 = as3_valAcquireId(e); %1 = 1; } "
      : "=r"(id), "=r"(except) : "r"(cc._id));
    if(except)
      throw var(id);
    return var(id);
  }

  var internal::construct(var cc, var a1) __attribute__((noinline));
  var internal::construct(var cc, var a1)
  {
    MAYBEDELEGATE((var (*)(var, var))construct);
    unsigned id;
    int except;
    inline_as3 (
      "try { "
      "%0 = as3_valAcquireId(new (as3_id2rcv[%2].val)(as3_id2rcv[%3].val)); "
      "%1 = 0; "
      "} catch(e:*) { %0 = as3_valAcquireId(e); %1 = 1; } "
      : "=r"(id), "=r"(except) : "r"(cc._id), "r"(a1._id));
    if(except)
      throw var(id);
    return var(id);
  }

  var internal::construct(var cc, var a1, var a2) __attribute__((noinline));
  var internal::construct(var cc, var a1, var a2)
  {
    MAYBEDELEGATE((var (*)(var, var, var))construct);
    unsigned id;
    int except;
    inline_as3 (
      "try { "
      "%0 = as3_valAcquireId(new (as3_id2rcv[%2].val)(as3_id2rcv[%3].val, as3_id2rcv[%4].val)); "
      "%1 = 0; "
      "} catch(e:*) { %0 = as3_valAcquireId(e); %1 = 1; } "
      : "=r"(id), "=r"(except) : "r"(cc._id), "r"(a1._id), "r"(a2._id));
    if(except)
      throw var(id);
    return var(id);
  }

  var internal::construct(var cc, var a1, var a2, var a3) __attribute__((noinline));
  var internal::construct(var cc, var a1, var a2, var a3)
  {
    MAYBEDELEGATE((var (*)(var, var, var, var))construct);
    unsigned id;
    int except;
    inline_as3 (
      "try { "
      "%0 = as3_valAcquireId(new (as3_id2rcv[%2].val)(as3_id2rcv[%3].val, as3_id2rcv[%4].val, as3_id2rcv[%5].val)); "
      "%1 = 0; "
      "} catch(e:*) { %0 = as3_valAcquireId(e); %1 = 1; } "
      : "=r"(id), "=r"(except) : "r"(cc._id), "r"(a1._id), "r"(a2._id), "r"(a3._id));
    if(except)
      throw var(id);
    return var(id);
  }

  var internal::construct(var cc, var a1, var a2, var a3, var a4) __attribute__((noinline));
  var internal::construct(var cc, var a1, var a2, var a3, var a4)
  {
    MAYBEDELEGATE((var (*)(var, var, var, var, var))construct);
    unsigned id;
    int except;
    inline_as3 (
      "try { "
      "%0 = as3_valAcquireId(new (as3_id2rcv[%2].val)(as3_id2rcv[%3].val, as3_id2rcv[%4].val, as3_id2rcv[%5].val, as3_id2rcv[%6].val)); "
      "%1 = 0; "
      "} catch(e:*) { %0 = as3_valAcquireId(e); %1 = 1; } "
      : "=r"(id), "=r"(except) : "r"(cc._id), "r"(a1._id), "r"(a2._id), "r"(a3._id), "r"(a4._id));
    if(except)
      throw var(id);
    return var(id);
  }

  var internal::construct(var cc, var a1, var a2, var a3, var a4, var a5) __attribute__((noinline));
  var internal::construct(var cc, var a1, var a2, var a3, var a4, var a5)
  {
    MAYBEDELEGATE((var (*)(var, var, var, var, var, var))construct);
    unsigned id;
    int except;
    inline_as3 (
      "try { "
      "%0 = as3_valAcquireId(new (as3_id2rcv[%2].val)(as3_id2rcv[%3].val, as3_id2rcv[%4].val, as3_id2rcv[%5].val, as3_id2rcv[%6].val, as3_id2rcv[%7].val)); "
      "%1 = 0; "
      "} catch(e:*) { %0 = as3_valAcquireId(e); %1 = 1; } "
      : "=r"(id), "=r"(except) : "r"(cc._id), "r"(a1._id), "r"(a2._id), "r"(a3._id), "r"(a4._id), "r"(a4._id));
    if(except)
      throw var(id);
    return var(id);
  }

  var internal::new_Array(int count, var *elems) __attribute__((noinline));
  var internal::new_Array(int count, var *elems)
  {
    MAYBEDELEGATE(&new_Array);
    unsigned id;
    int except;
    inline_as3 (
      "try { "
      "var a:Array = []; "
      "var count:int = %2; "
      "ram.position = %3; "
      "while(count--) a.push(as3_id2rcv[ram.readInt()].val); "
      "%0 = as3_valAcquireId(a); "
      "%1 = 0; "
      "} catch(e:*) { %0 = as3_valAcquireId(e); %1 = 1; } "
      : "=r"(id), "=r"(except) : "r"(count), "r"(elems));
    if(except)
      throw var(id);
    return var(id);
  }

  void *var::internal::uiThunk(void *data)
  {
    UIThunkReq *req = (UIThunkReq *)data;
    try
    {
      void *ret = __avm2_apply(req->proc, req->args, req->size);
      memcpy(&req->retdata, ret, sizeof(struct __avm2_retdata));
    }
    catch(var &v)
    {
      return (void *)internal::detach(&v);
    }
    return (void *)-1;
  }

  struct SelfLocker
  {
    SelfLocker() { avm2_self_lock(); }
    ~SelfLocker() { avm2_self_unlock(); }
  };

  unsigned var::internal::funThunk(var (*fun)(void *, var), void *data, unsigned id)
  {
    SelfLocker _lock;

    var val(id);
    var res = fun(data, val);
    return var::internal::detach(&res);
  }

  var internal::new_Function(var (*fun)(void *data, var args), void *data) __attribute__((noinline));
  var internal::new_Function(var (*fun)(void *data, var args), void *data)
  {
    MAYBEDELEGATE(&new_Function);
    unsigned id;
    inline_as3 ("var thunk:int = %1; var fun:int = %2; var data:int = %3; "
#if AS3_UI
          "var tTcbp:int = tcbp; var tThreadId:int = threadId; "
#endif
          "%0 = as3_valAcquireId(function(...args:Array):* { "
#if AS3_UI
          "try { var oldTcbp:int = tcbp; var oldThreadId:int = threadId; tcbp = tTcbp; threadId = tThreadId; "
#endif
          "var rcv:RCVal = as3_id2rcv[CModule.callI(thunk, new <int>[fun, data, as3_valAcquireId(args)])]; var val:* = rcv.val; rcv.release(); return val; "
#if AS3_UI
          "} finally { tcbp = oldTcbp; threadId = oldThreadId; } "
#endif
          "})"
      : "=r"(id) : "r"(var::internal::funThunk), "r"(fun), "r"(data));
    return var(id);
  }

  char *internal::utf8_toString(var val) __attribute__((noinline));
  char *internal::utf8_toString(var val)
  {
    MAYBEDELEGATE(&utf8_toString);
    char *result;
    unsigned eid;
    inline_as3 (
      "try { "
      "%0 = CModule.mallocString(String(as3_id2rcv[%2].val)); "
      "%1 = -1; "
      "} catch(e:*) { %1 = as3_valAcquireId(e); } "
      : "=r"(result), "=r"(eid) : "r"(val._id));
    if(eid != -1U)
      throw var(eid);
    return result;
  }

  bool internal::bool_valueOf(var val) __attribute__((noinline));
  bool internal::bool_valueOf(var val)
  {
    MAYBEDELEGATE(&bool_valueOf);
    int result;
    unsigned eid;
    inline_as3 (
      "try { "
      "%0 = Boolean(as3_id2rcv[%2].val); "
      "%1 = -1; "
      "} catch(e:*) { %1 = as3_valAcquireId(e); } "
      : "=r"(result), "=r"(eid) : "r"(val._id));
    if(eid != -1U)
      throw var(eid);
    return result ? true : false;
  }

  int internal::int_valueOf(var val) __attribute__((noinline));
  int internal::int_valueOf(var val)
  {
    MAYBEDELEGATE(&int_valueOf);
    int result;
    unsigned eid;
    inline_as3 (
      "try { "
      "%0 = int(as3_id2rcv[%2].val); "
      "%1 = -1; "
      "} catch(e:*) { %1 = as3_valAcquireId(e); } "
      : "=r"(result), "=r"(eid) : "r"(val._id));
    if(eid != -1U)
      throw var(eid);
    return result;
  }

  unsigned internal::uns_valueOf(var val) __attribute__((noinline));
  unsigned internal::uns_valueOf(var val)
  {
    MAYBEDELEGATE(&uns_valueOf);
    unsigned result;
    unsigned eid;
    inline_as3 (
      "try { "
      "%0 = uint(as3_id2rcv[%2].val); "
      "%1 = -1; "
      "} catch(e:*) { %1 = as3_valAcquireId(e); } "
      : "=r"(result), "=r"(eid) : "r"(val._id));
    if(eid != -1U)
      throw var(eid);
    return result;
  }

  double internal::double_valueOf(var val) __attribute__((noinline));
  double internal::double_valueOf(var val)
  {
    MAYBEDELEGATE(&double_valueOf);
    double result;
    unsigned eid;
    inline_as3 (
      "try { "
      "%0 = Number(as3_id2rcv[%2].val); "
      "%1 = -1; "
      "} catch(e:*) { %1 = as3_valAcquireId(e); } "
      : "=r"(result), "=r"(eid) : "r"(val._id));
    if(eid != -1U)
      throw var(eid);
    return result;
  }

  var internal::coerce(var cc, var v) __attribute__((noinline));
  var internal::coerce(var cc, var v)
  {
    MAYBEDELEGATE(&coerce);
    unsigned id;
    int except;
    inline_as3 (
      "try { "
      "var cc:Class = as3_id2rcv[%2].val; "
      "%0 = as3_valAcquireId((cc)(as3_id2rcv[%3].val)); "
      "%1 = 0; "
      "} catch(e:*) { %0 = as3_valAcquireId(e); %1 = 1; } "
      : "=r"(id), "=r"(except) : "r"(cc._id), "r"(v._id));
    if(except)
      throw var(id);
    return var(id);
  }

  bool internal::_delete(var val, var prop) __attribute__((noinline));
  bool internal::_delete(var val, var prop)
  {
    MAYBEDELEGATE(&_delete);
    int result;
    unsigned eid;
    inline_as3 (
      "try { "
      "%0 = delete as3_id2rcv[%2].val[as3_id2rcv[%3].val]; "
      "%1 = -1; "
      "} catch(e:*) { %1 = as3_valAcquireId(e); } "
      : "=r"(result), "=r"(eid) : "r"(val._id), "r"(prop._id));
    if(eid != -1U)
      throw var(eid);
    return result ? true : false;
  }

  void internal::_throw(var val) __attribute__((noinline));
  void internal::_throw(var val)
  {
    MAYBEDELEGATE(&_throw);
    inline_as3 ("throw(as3_id2rcv[%0].val)" : : "r"(val._id));
  }

  void internal::trace(var val) __attribute__((noinline));
  void internal::trace(var val)
  {
    MAYBEDELEGATE(&trace);
    inline_as3 ("trace(as3_id2rcv[%0].val)" : : "r"(val._id));
  }

  bool internal::is(var a, var b) MAYBEDELEGATEATTR;
  bool internal::is(var a, var b)
  {
    MAYBEDELEGATE(is);
    int result;
    unsigned eid;
    inline_as3 (
      "try { "
      "%0 = as3_id2rcv[%2].val is (as3_id2rcv[%3].val); "
      "%1 = -1; "
      "} catch(e:*) { %1 = as3_valAcquireId(e); } "
      : "=r"(result), "=r"(eid) : "r"(a._id), "r"(b._id));
    if(eid != -1U)
      throw var(eid);
    return result ? true : false;
  }

  var internal::as(var a, var b) MAYBEDELEGATEATTR;
  var internal::as(var a, var b)
  {
    MAYBEDELEGATE(as);
    unsigned id;
    int except;
    inline_as3 (
      "try { "
      "%0 = as3_valAcquireId(as3_id2rcv[%2].val as (as3_id2rcv[%3].val)); "
      "%1 = 0; "
      "} catch(e:*) { %0 = as3_valAcquireId(e); %1 = 1; } "
      : "=r"(id), "=r"(except) : "r"(a._id), "r"(b._id));
    if(except)
      throw var(id);
    return var(id);
  }

  bool internal::instanceof(var a, var b) MAYBEDELEGATEATTR;
  bool internal::instanceof(var a, var b)
  {
    MAYBEDELEGATE(instanceof);
    int result;
    unsigned eid;
    inline_as3 (
      "try { "
      "%0 = as3_id2rcv[%2].val instanceof (as3_id2rcv[%3].val); "
      "%1 = -1; "
      "} catch(e:*) { %1 = as3_valAcquireId(e); } "
      : "=r"(result), "=r"(eid) : "r"(a._id), "r"(b._id));
    if(eid != -1U)
      throw var(eid);
    return result ? true : false;
  }

  bool internal::in(var a, var b) MAYBEDELEGATEATTR;
  bool internal::in(var a, var b)
  {
    MAYBEDELEGATE(in);
    int result;
    unsigned eid;
    inline_as3 (
      "try { "
      "%0 = as3_id2rcv[%2].val in (as3_id2rcv[%3].val); "
      "%1 = -1; "
      "} catch(e:*) { %1 = as3_valAcquireId(e); } "
      : "=r"(result), "=r"(eid) : "r"(a._id), "r"(b._id));
    if(eid != -1U)
      throw var(eid);
    return result ? true : false;
  }

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

#include <stdio.h>
extern "C" {
#include <sys/thr.h>
}
#include <AS3/AVM2.h>
#include <AS3/AS3.h>
#include <AS3/AS3++.h>

__asm(
"#package public\n"
"import flash.utils.Dictionary; "
"public const as3_id2rcv:Dictionary = new Dictionary; "
"public const as3_val2id:Dictionary = new Dictionary; "
"public var as3_nextId:int = 0; "
";[GlobalMethod] "
"public function as3_val2key(val:*):Object { "
"  if(val is Namespace) " // ugh... Namespace doesn't work as a key as it's not kObject in the VM!!!
"    return 'Namespace:' + String(val); "
"  var t:String = typeof(val); "
"  if(t == 'object' || t == 'function') return val; "
"  return t + ':' + String(val); "
"} "
"public class RCVal { "
"  public const val:*; "
"  public const id:int; "
"  public var rc:int; "
"  public function RCVal(val:*, id:int) { this.val = val; this.id = id; } "
"  public function acquire(n:int=1) { rc+=n  } "
"  public function release(n:int=1) { if(!(rc-=n)) { delete as3_val2id[as3_val2key(val)]; delete as3_id2rcv[id]; } } " 
"} "
"class RCValSpecial extends RCVal { " // extending is really pointless since Dict isn't a generic...
"  public function RCValSpecial(val:*) { super(val, 0); } "
"  public override function acquire(n:int=1) { } "
"  public override function release(n:int=1) { } " 
"} "
";[GlobalMethod] "
"public function as3_valAcquireId(val:*):int { "
"  var key:Object = as3_val2key(val); "
"  var id:int = this.as3_val2id[key]; "
"  if(!id) { "
"    id = this.as3_nextId; "
"    while(this.as3_id2rcv[id]) id++; " // in theory we can wrap around... -1 is used as a special var
"    this.as3_nextId = id + 1; "
"    this.as3_id2rcv[id] = new RCVal(val, id); "
"    this.as3_val2id[key] = id; "
"  } "
"  this.as3_id2rcv[id].acquire(); "
//"try{trace(val + ' / ' + val.constructor + ' - ' + id)}catch(e:*){}; "
"  return id; "
"} "
"function as3_valRegSpecial(val:*):void { "
"  var id:int = this.as3_nextId; "
"  this.as3_nextId++; "
"  this.as3_id2rcv[id] = new RCValSpecial(val); "
"  this.as3_val2id[as3_val2key(val)] = id; "
"} "
"as3_id2rcv[-1] = -1; "
"as3_valRegSpecial(undefined); "
"as3_valRegSpecial(null); "
"as3_valRegSpecial(true); "
"as3_valRegSpecial(false); "
"as3_valRegSpecial(0); "
"as3_valRegSpecial(1); "
"as3_valRegSpecial(Number.NaN); "
);

#include <map>

namespace AS3
{
  namespace local
  {
  #define MAYBEDELEGATE(X) /**/
  #define MAYBEDELEGATEATTR /**/
  #define AS3_LOCAL
  #include "AS3++.impl.cpp"
  #undef AS3_LOCAL
  #undef MAYBEDELEGATEATTR
  #undef MAYBEDELEGATE
  }
  namespace ui 
  {
  #define ARGSIZE(X) ((sizeof(X) + 3) & ~3)
    template <typename R> struct _retArgSize { const static int size = 0; }; // most return types don't contribute to args
    template <> struct _retArgSize<var> { const static int size = 4; }; // var is not returned in a reg... it is "sret"
    template <typename R> int _argSize(R (*arg)()) { return _retArgSize<R>::size; }
    template <typename R, typename A0> int _argSize(R (*arg)(A0)) { return _retArgSize<R>::size + ARGSIZE(A0); }
    template <typename R, typename A0, typename A1> int _argSize(R (*arg)(A0, A1)) { return _retArgSize<R>::size + ARGSIZE(A0) + ARGSIZE(A1); }
    template <typename R, typename A0, typename A1, typename A2> int _argSize(R (*arg)(A0, A1, A2)) { return _retArgSize<R>::size + ARGSIZE(A0) + ARGSIZE(A1) + ARGSIZE(A2); }
    template <typename R, typename A0, typename A1, typename A2, typename A3> int _argSize(R (*arg)(A0, A1, A2, A3)) { return _retArgSize<R>::size + ARGSIZE(A0) + ARGSIZE(A1) + ARGSIZE(A2) + ARGSIZE(A3); }
    template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4> int _argSize(R (*arg)(A0, A1, A2, A3, A4)) { return _retArgSize<R>::size + ARGSIZE(A0) + ARGSIZE(A1) + ARGSIZE(A2) + ARGSIZE(A3) + ARGSIZE(A4); }
    template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5> int _argSize(R (*arg)(A0, A1, A2, A3, A4, A5)) { return _retArgSize<R>::size + ARGSIZE(A0) + ARGSIZE(A1) + ARGSIZE(A2) + ARGSIZE(A3) + ARGSIZE(A4) + ARGSIZE(A5); }
    template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> int _argSize(R (*arg)(A0, A1, A2, A3, A4, A5, A6)) { return _retArgSize<R>::size + ARGSIZE(A0) + ARGSIZE(A1) + ARGSIZE(A2) + ARGSIZE(A3) + ARGSIZE(A4) + ARGSIZE(A5) + ARGSIZE(A6); }
  #define MAYBEDELEGATE(X) \
    if(!avm2_is_ui_worker()) { \
      UIThunkReq req = { (void *)(X), __avm2_apply_args(), _argSize(X) };\
      unsigned eid = (unsigned)avm2_ui_thunk(var::internal::uiThunk, &req);\
      if(eid != -1U) throw(var(eid)); \
      __avm2_return(&req.retdata); \
    }
  #define MAYBEDELEGATEATTR __attribute__((noinline))
  #define AS3_UI 1
  #include "AS3++.impl.cpp"
  #undef AS3_UI
  #undef MAYBEDELEGATEATTR
  #undef MAYBEDELEGATE
  }
}

#ifdef AS3xx_TEST
#include "AS3/AS3.h"

void testDict()
{
  using namespace AS3::local;

  try {
  var val, valType;
  AS3_DeclareVar(val, *);
  AS3_DeclareVar(valType, String);
#define TEST(X) \
  __asm volatile("val = " #X); \
  __asm volatile("valType = typeof(val)"); \
  AS3_GetVarxxFromVar(val, val); \
  AS3_GetVarxxFromVar(valType, valType); \
  printf("%s => %s %s\n", #X, AS3::sz2stringAndFree(utf8_toString(val)).c_str(), AS3::sz2stringAndFree(utf8_toString(valType)).c_str());

  TEST(undefined);
  TEST("undefined");
  TEST(null);
  TEST("null");
  TEST(0);
  TEST("0");
  TEST(1);
  TEST("1");
  TEST(1.0);
  TEST("1.0");
  TEST(true);
  TEST("true");
  TEST("false");
  TEST(false);
#undef TEST
  } catch(var &e) {
    printf("caught!: %s\n", AS3::sz2stringAndFree(utf8_toString(e)).c_str());
    var s = internal::call(e["getStackTrace"], e, 0, NULL);
    printf("stack!: %s\n", AS3::sz2stringAndFree(utf8_toString(s)).c_str());
  }
}

using namespace AS3::ui;
using namespace AS3::ui::internal;

var testFun(void *data, var args)
{
  char buf[128];
  sprintf(buf, "#[%d] args.length: %d", (int)data, int_valueOf(args["length"]));
  printf("returning: %s\n", buf);
  return internal::new_String(buf);
}

int main()
{
  testDict();

  var val, val1, val2, val3;
  var na[] = { new_int(4), new_int(5), new_int(6) };
  val = new_Array(3, na);
  val1 = new_int(1);
  val2 = new_String("hi!");
//  trace(na[1]);
//  trace(val2);
  trace(val);
  val[val1] = val2;
  trace(val);
  val[val1] = _typeof(val3);
  trace(val);
  trace(_typeof(val3));
  trace(_typeof(_typeof(val3)));
  trace(getlex(new_String("Array")));
  try
  {
    setlex(new_String("Array"), val1);
  }
  catch(var &e)
  {
    printf("caught!: %s\n", AS3::sz2stringAndFree(utf8_toString(e)).c_str());
  }
  var array;
  try
  {
    var cc = getlex(new_String("Array"));
    trace(cc);
    array = construct(cc, new_int(5), new_Number(7.3));
    call(array["push"], array, 1, &val1);
    trace(array);
  }
  catch(var &e)
  {
    printf("caught!: %s\n", AS3::sz2stringAndFree(utf8_toString(e)).c_str());
  }
  printf("1 is Number: %s\n", is(new_int(1), getlex(new_String("Number"))) ? "true" : "false");
  printf("1 is Boolean: %s\n", is(new_int(1), getlex(new_String("Boolean"))) ? "true" : "false");
  var fun = new_Function(testFun, (void *)123);
  printf("got fun (%p)\n", testFun);
  try
  {
    printf("fun(): %s\n", AS3::sz2stringAndFree(utf8_toString(call(fun, internal::_undefined, 1, &array))).c_str());
  }
  catch(var &e)
  {
    printf("caught\n");
    printf("caught!: %s\n", AS3::sz2stringAndFree(utf8_toString(e)).c_str());
  }
  printf("hi!\n");
  var Namespace = getlex(new_String("Namespace"));
  var Sprite = getlex(construct(Namespace, new_String("flash.display")), new_String("Sprite"));
  trace(Sprite);
  var mySprite = construct(Sprite);
  var g = mySprite["graphics"];
  var color = new_uint(0xff00ff);
  call(g["beginFill"], g, 1, &color);
  var dcArgs[] = { new_Number(0), new_Number(0), new_Number(50) };
  call(g["drawCircle"], g, 3, dcArgs);
 
  var stage = get_Stage();
  call(stage["addChild"], stage, 1, &mySprite);
  
  for(int k = 0; k < 10; k++)
  {
    printf("%d\n", k);
    for(int n = 0; n < 60; n++)
    {
      mySprite["x"] = new_int(int_valueOf(mySprite["x"]) + 1);
      avm2_wait_for_ui_frame(0);
    }
  }
  _throw(new_String("blah"));
  return 0;
}
#endif

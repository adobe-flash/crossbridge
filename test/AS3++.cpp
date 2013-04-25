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

#include <AS3/AS3.h>
#include <AS3/AS3++.h>
#include <Flash++.h>

int main()
{
  AS3::local::var v;
  AS3_DeclareVar(someString, String);
  inline_nonreentrant_as3("someString = 'foo'");
  AS3_GetVarxxFromVar(v, someString);
  AS3::local::internal::trace(v);
  v = AS3::local::internal::new_String("baz");
  AS3_CopyVarxxToVar(someString, v);
  AS3_Trace(someString);

  v = AS3::local::internal::new_Number(3.4);
  AS3::local::internal::trace(v);
  AS3::local::var cc;
  AS3_DeclareVar(intCC, Class);
  inline_nonreentrant_as3("intCC = int");
  AS3_GetVarxxFromVar(cc, intCC);
  v = AS3::local::internal::coerce(cc, v);
  AS3::local::internal::trace(v);

  v = AS3::local::flash::utils::Dictionary::internal::getClosure();
  v = AS3::local::internal::new_Vector(v);
  AS3::local::internal::trace(v["constructor"]);
  v[0] = AS3::local::flash::utils::Dictionary::_new();
  AS3::local::internal::trace(v);
  try
  {
    v[0] = AS3::local::internal::new_Number(3.4); // should throw!
  }
  catch(AS3::local::var e)
  {
    AS3::local::internal::trace(e);
  }

  return 0;
}

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package StaticFunctionBody {

    class TestObjInner{
        static function noReturnNoParamsInner() { return "noReturnNoParams"; }
        static function noReturnParamsInner(s:String, b:Boolean) { return s; }
        static function noReturnCustomParamInner(c:Custom) { return new Custom(); }
        static function returnNoParamsInner():String { return "returnNoParams"; }
        static function returnParamsInner(s:String, b:Boolean):String { return s; }
        static function returnCustomNoParamsInner():Custom { return new Custom(); }
    }

    public class TestObj extends TestObjInner {
        public function noReturnNoParams() { return noReturnNoParamsInner(); }
        public function noReturnParams(s:String, b:Boolean) { return noReturnParamsInner(s,b); }
        public function noReturnCustomParam(c:Custom) { return noReturnCustomParamInner(c); }
        public function returnNoParams():String { return returnNoParamsInner(); }
        public function returnParams(s:String, b:Boolean):String { return returnParamsInner(s,b); }
        public function returnCustomNoParams():Custom { return returnCustomNoParamsInner(); }
    }

}


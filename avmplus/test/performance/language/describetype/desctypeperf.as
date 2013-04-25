/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package
{
import avmplus.*;

var iter:uint = 1000;

function test()
{
        var a = new A;
        var b = new B;
        var c = new C;
        
        var start = new Date;
        for(var i:uint = 0; i < iter; ++i)
        {
                describeType(a, FLASH10_FLAGS);
                describeType(b, FLASH10_FLAGS);
                describeType(c, FLASH10_FLAGS);
        }
        var end = new Date;
        trace("metric time " + (end-start));
}

test();



public interface myInterface {
        function interfaceMethod1 ();
        function interfaceMethod2 ();
}

public class A {
                
        public var aVar0:String = "aVar0 property";
        public var aVar1:String = "aVar1 property";
        public var aVar2:String = "aVar2 property";
        // aVar3 type is Array for ObjectInLoadedSWF
        public var aVar3:Array = new Array ("aVar3 property 1","aVar3 property 2");
        public var aVar4:String = "aVar4 property";

        private var aVar5_priv:String = "aVar5 property";
        private var aVar6_priv:String = "aVar6 property";
        private var aVar7_priv:String = "aVar7 property";
        private var aVar8_priv:String = "aVar8 property";
        // aVar9 is Array and undefined for AssignDefaultValues
        private var aVar9_priv:Array;

        /********  constructor  ****************/
        public function A () {
                                

        }

        /********  public methods that should show  ****************/
        public function aMethod0 ():String {
                return "aMethod0 called";
        }
        public function aMethod1 ():String {
                return "aMethod1 called";
        }
        public function aMethod2 (am2_1:String, am2_2:String):String {
                if (am2_1 != null && am2_2 != null) {
                        return (am2_1 + am2_2);
                }
                else {
                        return "aMethod2 called";
                }
        }
        public function aMethod3 (neededParam:Number, optionalParam:Number = 2):Number {
                if (!isNaN (optionalParam)) {
                        return neededParam;
                }
                else {
                        return neededParam + optionalParam;
                }
        }
        public function aMethod4 ():String {
                return "aMethod4 called";
        }

        /********  private methods that should not show  ****************/
        private function aMethod5 ():String {
                return "aMethod5 called";
        }
        private function aMethod6 ():String {
                return "aMethod6 called";
        }
        private function aMethod7 ():String {
                return "aMethod7 called";
        }
        private function aMethod8 ():String {
                return "aMethod8 called";
        }
        private function aMethod9 ():String {
                return "aMethod9 called";
        }

        /********  getter setters that should have accessor node  ****************/

        // 5 set to private to exclude in GetPropMethNamesCount
        private function get aVar5 ():String {
                return aVar5_priv;
        }
        private function set aVar5 (s:String) {
                aVar5_priv = s;
        }
        
        public function get aVar6 ():String {
                return aVar6_priv;
        }
        public function set aVar6 (s:String) {
                aVar6_priv = s;
        }
        public function get aVar7 ():String {
                return aVar7_priv;
        }
        public function set aVar7 (s:String) {
                aVar7_priv = s;
        }
        public function get aVar8 ():String {
                return aVar8_priv;
        }
        public function set aVar8 (s:String) {
                aVar8_priv = s;
        }
        
        // avar9 set to Array for AllowAcom
        public function get aVar9 ():Array {
                return aVar9_priv;
        }
        public function set aVar9 (arr:Array) {
                aVar9_priv = arr;
        }
}

public class B extends A implements myInterface {
                
        public var bVar0:String = "bVar0 property";
        public var bVar1:String = "bVar1 property";
        public var bVar2:String = "bVar2 property";
        public var bVar3:String = "bVar3 property";
        public var bVar4:String = "bVar4 property";

        private var bVar5_priv:String = "bVar5 property";
        private var bVar6_priv:String = "bVar6 property";
        private var bVar7_priv:String = "bVar7 property";
        private var bVar8_priv:String = "bVar8 property";
        private var bVar9_priv:String = "bVar9 property";

        /********  constructor  ****************/
        public function B () {
                                

        }

        /********  public methods that should show  ****************/
        public function bMethod0 ():String {
                return "bMethod0 called";
        }
        public function bMethod1 ():String {
                return "bMethod1 called";
        }
        public function bMethod2 (bm2_1:String, bM2_2:String):String {
                if (bm2_1 != null && bM2_2 != null) {
                        return (bm2_1 + bM2_2);
                }
                else {
                        return "bMethod2 called";
                }
        }
        public function bMethod3 (neededParam:Number, optionalParam:Number = 2):Number {
                if (!isNan(optionalParam)) {
                        return neededParam;
                }
                else {
                        return neededParam + optionalParam;
                }
        }
        public function bMethod4 ():String {
                return "bMethod4 called";
        }

        /********  private methods that should not show  ****************/
        private function bMethod5 ():String {
                return "bMethod5 called";
        }
        private function bMethod6 ():String {
                return "bMethod6 called";
        }
        private function bMethod7 ():String {
                return "bMethod7 called";
        }
        private function bMethod8 ():String {
                return "bMethod8 called";
        }
        private function bMethod9 ():String {
                return "bMethod9 called";
        }

        /********  getter setters that should hbVe accessor node  ****************/

        public function get bVar5 ():String {
                return bVar5_priv;
        }
        public function set bVar5 (s:String) {
                bVar5_priv = s;
        }
        public function get bVar6 ():String {
                return bVar6_priv;
        }
        public function set bVar6 (s:String) {
                bVar6_priv = s;
        }
        public function get bVar7 ():String {
                return bVar7_priv;
        }
        public function set bVar7 (s:String) {
                bVar7_priv = s;
        }
/*      public function get bVar8 ():String {
                return bVar8_priv;
        } */
        public function set bVar8 (s:String) {
                bVar8_priv = s;
        }
        public function get bVar9 ():String {
                return bVar9_priv;
        }
        public function set bVar9 (s:String) {
                bVar9_priv = s;
        }


        /********  definitions for methods from interface  ****************/
        public function interfaceMethod1 () {
                return "interfaceMethod1 called";
        }

        public function interfaceMethod2 () {
                return "interfaceMethod2 called";
        }

} // end class


public dynamic class C extends B {
                
        // 0 1 undefined for Ispec_TraceUndefined
        public var cVar0:String;
        public var cVar1:String;
        public var cVar2:String = "cVar2 property";
        public var cVar3:String = "cVar3 property";
        public var cVar4:String = "cVar4 property";

        // 5 6 7 undefined for test case TraceUndefined
        private var cVar5_priv:String;
        private var cVar6_priv:String;
        private var cVar7_priv:String;
        private var cVar8_priv:String = "cVar8 property";
        private var cVar9_priv:String = "cVar9 property";

        /********  constructor  ****************/
        public function C () {
                                

        }

        /********  public methods that should show  ****************/
        public function cMethod0 ():String {
                return "cMethod0 called";
        }
        public function cMethod1 ():String {
                return "cMethod1 called";
        }
        public function cMethod2 (cM2_1:String, cM2_2:String):String {
                if (cM2_1 != null && cM2_2 != null) {
                        return (cM2_1 + cM2_2);
                }
                else {
                        return "cMethod2 called";
                }
        }
        public function cMethod3 (neededParam:Number, optionalParam:Number = 2):Number {
                if (optionalParam != 0) {
                        return neededParam;
                }
                else {
                        return neededParam + optionalParam;
                }
        }
        public function cMethod4 ():String {
                return "cMethod4 called";
        }

        /********  private methods that should not show  ****************/
        private function cMethod5 ():String {
                return "cMethod5 called";
        }
        private function cMethod6 ():String {
                return "cMethod6 called";
        }
        private function cMethod7 ():String {
                return "cMethod7 called";
        }
        private function cMethod8 ():String {
                return "cMethod8 called";
        }
        private function cMethod9 ():String {
                return "cMethod9 called";
        }

        /********  getter setters that should hbVe accessor node  ****************/

        public function get cVar5 ():String {
                return cVar5_priv;
        }
        public function set cVar5 (s:String) {
                cVar5_priv = s;
        }
        public function get cVar6 ():String {
                return cVar6_priv;
        }
        public function set cVar6 (s:String) {
                cVar6_priv = s;
        }
        public function get cVar7 ():String {
                return cVar7_priv;
        }
        public function set cVar7 (s:String) {
                cVar7_priv = s;
        }
        public function get cVar8 ():String {
                return cVar8_priv;
        }
        public function set cVar8 (s:String) {
                cVar8_priv = s;
        }
        public function get cVar9 ():String {
                return cVar9_priv;
        }
        public function set cVar9 (s:String) {
                cVar9_priv = s;
        }


} // end class

}
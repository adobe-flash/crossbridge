/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package {
    public var s:String = "hello";
    public function publicFunc(i:int):String {
        return "You passed " + i.toString();
    }
    
    public namespace Kitty;
    
    public interface IClickable {
         function whoAmI():String;
    }
    
    public class A {
        public function whoAmI():String {
            return "A";
        }
    }
    
    public class B implements IClickable {
        public function whoAmI():String {
            return "B";
        }
    }
    
    use namespace Kitty;
    
    public class C {
        Kitty var b:Boolean = true;
        public function returnNSVar():Boolean {
            return Kitty::b;
        }
        
        Kitty function returnArray():Array {
            return [1,2,3];
        }
        
        public function callNSFunc() {
            var k:Array = Kitty::returnArray();
            return k;
        }
    }
    
    
    public class X{
        Kitty var num:Number = 5;
        Kitty function kittyFunc(s:String):String {
            return "You said hi";
        }
    }
}

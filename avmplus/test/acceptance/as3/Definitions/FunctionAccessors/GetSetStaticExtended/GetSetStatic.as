/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package GetSetStaticExtended {

    
    internal class GetSetStaticInternal {
    
        private static var _nt = "no type";
        private static var _x:Array = [1,2,3];
        private static var _y:int = -10;
        private static var _b:Boolean = true;
        private static var _u:uint = 1;
        private static var _s:String = "myString";

        private var _n:Number = 555;

        public static function get n():Number {
            return _n;
        }

        public static function set n(num):void {
            _n = num;
        }

        public static function get x():Array {
            return _x;
        }
        public static function set x( a:Array ) {
            _x=a;
        }
        public static function get y():int {
            return _y;
        }
        public static function set y( i:int ) {
            _y=i;
        }
        public static function get boolean():Boolean { return _b; }
        public static function set boolean(b:Boolean) { _b=b; }
        public static function get u():uint{ return _u; }
        public static function set u(ui:uint) { _u=ui; }
        public static function get string():String{ return _s; }
        public static function set string(s:String) { _s=s; }
        public static function get noType():String{ return _s; }
        public static function set noType(nt) { _nt=nt; }

            
    }
        public class GetSetStatic extends GetSetStaticInternal {
      
      
    }

}

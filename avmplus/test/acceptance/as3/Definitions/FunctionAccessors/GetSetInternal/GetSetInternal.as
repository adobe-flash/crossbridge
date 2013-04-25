/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package GetSetInternal{

    internal class GetSetInternalInternal {
    
        private var _nt = "no type";
        private var _x:Array = [1,2,3];
        private var _y:int = -10;
        private var _b:Boolean = true;
        private var _u:uint = 101;
        private var _s:String = "myString";
        private var _n:Number = 555;

        public function get x():Array {
            return _x;
        }
        public function set x( a:Array ) {
            _x=a;
        }
        public function get y():int {
            return _y;
        }
        public function set y( i:int ) {
            _y=i;
        }
        public function get boolean():Boolean { return _b; }
        public function set boolean(b:Boolean):void { _b=b; }
        public function get u():uint{ return _u; }
        public function set u(u:uint):void { _u=u; }
        public function get n():Number{ return _n; }
        public function set n(num:Number):void { _n=num; }
        public function get string():String{ return _s; }
        public function set string(s:String):void { _s=s; }
        public function get noType(){ return _nt; }
        public function set noType(nt):void { _nt=nt; }
    
    }

    public class GetSetInternal {
    
        internalC = new GetSetInternalInternal();
        public function getBoolean() {
            return internalC.boolean;
        }
        public function setBoolean(b:Boolean) {
            internalC.boolean = b;
            return internalC.boolean;
        }
        
        public function getArray() {
            return internalC.x;
        }
        public function setArray(a:Array) {
            internalC.x = a;
            return internalC.x;
        }
        public function getUint() {
            return internalC.u;
        }
        public function setUint(u:uint) {
            internalC.u = u;
            return internalC.u;
        }
        public function getString() {
            return internalC.string;
        }
        public function setString(s:String) {
            internalC.string = s;
            return internalC.string;
        }
        public function getNoType() {
            return internalC.noType;
        }
        public function setNoType(nt) {
            internalC.noType = nt;
            return internalC.noType;
        }
        
    }

    

}

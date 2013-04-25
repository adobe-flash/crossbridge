/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package GetSetProtected{

    public class GetSetProtected {
    
        private var _nt = "no type";
        private var _x:Array = [1,2,3];
        private var _y:int = -10;
        private var _b:Boolean = true;
        private var _u:uint = 101;
        private var _s:String = "myString";
        private var _n:Number = 555;

        protected function get x():Array {
            return _x;
        }
        protected function set x( a:Array ) {
            _x=a;
        }
        protected function get y():int {
            return _y;
        }
        protected function set y( i:int ) {
            _y=i;
        }
        protected function get boolean():Boolean { return _b; }
        protected function set boolean(b:Boolean):void { _b=b; }
        protected function get u():uint{ return _u; }
        protected function set u(u:uint):void { _u=u; }
        protected function get n():Number{ return _n; }
        protected function set n(num:Number):void { _n=num; }
        protected function get string():String{ return _s; }
        protected function set string(s:String):void { _s=s; }
        protected function get noType(){ return _nt; }
        protected function set noType(nt):void { _nt=nt; }
    
        
        public function getBoolean() {
            return boolean;
        }
        public function setBoolean(b:Boolean) {
            boolean = b;
            return boolean;
        }
        
        public function getArray() {
            return x;
        }
        public function setArray(a:Array) {
            x = a;
            return x;
        }
        public function getUint() {
            return u;
        }
        public function setUint(u:uint) {
            u = u;
            return u;
        }
        public function getString() {
            return string;
        }
        public function setString(s:String) {
            string = s;
            return string;
        }
        public function getNoType() {
            return noType;
        }
        public function setNoType(nt) {
            noType = nt;
            return noType;
        }
        
    }

    internal class GetSetInternalInternal {

        

    }

}

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package GetSetPublicVars {

    public class GetSetPublic {

        public var _nt = "no type";
        public var _x:Array = [1,2,3];
        public var _y:int = -10;
        public var _b:Boolean = true;
        public var _u:uint = 1;
        public var _s:String = "myString";
        
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
        public function set boolean(b:Boolean) { _b=b; }
        public function get u():uint{ return _u; }
        public function set u(ui:uint) { _u=ui; }
        public function get string():String{ return _s; }
        public function set string(s:String) { _s=s; }
        public function get noType():String{ return _nt; }
        public function set noType(nt) { _nt=nt; }

        // call setter from setter
        public var _sfs1:Number = 99;
        public var _sfs2:int = 0;
        public function get sfs1():Number{ return _sfs1; }
        public function get sfs2():int{ return _sfs2; }
        public function set sfs1(n:Number){
            _sfs1 = n;
        }
        public function set sfs2(i:int){
            this.sfs1 = i;
            _sfs2 = i;
        }

        // call setter from getter
        public var _sfg1:String = "FAILED";
        public var _sfg2:uint = 0;
        public function get sfg1():String{ return _sfg1; }
        public function get sfg2():uint{
            sfg1 = "PASSED";
            return _sfg2;
        }
        public function set sfg1(s:String){ _sfg1 = s; }
        public function set sfg2(ui:uint){ _sfg2 = ui; }
        
        // call getter from setter
        public var _gfs1:String = "FAILED";
        public var _gfs2:String = "PASSED";
        public function get gfs1():String{
            return _gfs1;
        }
        public function get gfs2():String{
            return _gfs2;
        }
        public function set gfs1(s:String){
            _gfs1=gfs2;
        }
        public function set gfs2(s:String){
            _gfs2=s;
        }

        // call getter from getter
        public var _gfg1:String = "PASSED";
        public var _gfg2:String = "FAILED";
        public function get gfg1():String{
            return _gfg1;
        }
        public function get gfg2():String{
            return gfg1;
        }

        // define a getter for a property and call the undefined setter
        public var _nosetter = "FAILED";
        public function get noSetter(){ return _nosetter; }

        // define a setter for a property and call the undefined getter
        public var _nogetter = "FAILED";
        public function set noGetter(s){ _nogetter = s; }

    }

}

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package GetSetStaticPackage {

    public class GetSetStatic {

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

        // call setter from setter
        private static var _sfs1:Number = 99;
        private static var _sfs2:int = 0;
        public static function get sfs1():Number{ return _sfs1; }
        public static function get sfs2():int{ return _sfs2; }
        public static function set sfs1(n:Number){
            _sfs1 = n;
        }
        public static function set sfs2(i:int){
            sfs1 = i;
            _sfs2 = i;
        }

        // call setter from getter
        private static var _sfg1:String = "FAILED";
        private static var _sfg2:uint = 0;
        public static function get sfg1():String{ return _sfg1; }
        public static function get sfg2():uint{
            sfg1 = "PASSED";
            return _sfg2;
        }
        public static function set sfg1(s:String){ _sfg1 = s; }
        public static function set sfg2(ui:uint){ _sfg2 = ui; }
        
        // call getter from setter
        private static var _gfs1:String = "FAILED";
        private static var _gfs2:String = "PASSED";
        public static function get gfs1():String{
            return _gfs1;
        }
        public static function get gfs2():String{
            return _gfs2;
        }
        public static function set gfs1(s:String){
            _gfs1=gfs2;
        }
        public static function set gfs2(s:String){
            _gfs2=s;
        }

        // call getter from getter
        private static var _gfg1:String = "PASSED";
        private static var _gfg2:String = "FAILED";
        public static function get gfg1():String{
            return _gfg1;
        }
        public static function get gfg2():String{
            return gfg1;
        }

        // define a getter for a property and call the undefined setter
        private static var _nosetter = "FAILED";
        public static function get noSetter(){ return _nosetter; }

        // define a setter for a property and call the undefined getter
        private static var _nogetter = "FAILED";
        public static function set noGetter(s){ _nogetter = s; }
        
    }

}

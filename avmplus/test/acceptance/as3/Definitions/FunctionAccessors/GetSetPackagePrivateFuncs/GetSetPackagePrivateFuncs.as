/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package GetSetPackagePrivateFuncs {

    public class GetSetPackagePrivateFuncs {

        var _nt = "no type";
        var _x:Array = [1,2,3];
        var _y:int = -10;
        var _b:Boolean = true;
        var _u:uint = 1;
        var _s:String = "myString";
        var _n:Number = 5;
        
        function get x():Array {
            return _x;
        }
        function set x( a:Array ) {
            _x=a;
        }
        function get y():int {
            return _y;
        }
        function set y( i:int ) {
            _y=i;
        }
        function get boolean():Boolean { return _b; }
        function set boolean(b:Boolean) { _b=b; }
        function get u():uint{ return _u; }
        function set u(ui:uint) { _u=ui; }
        function get string():String{ return _s; }
        function set string(s:String) { _s=s; }
        function get noType():String{ return _s; }
        function set noType(nt) { _nt=nt; }

        // call setter from setter
        var _sfs1:Number = 99;
        var _sfs2:int = 0;
        function get sfs1():Number{ return _sfs1; }
        function get sfs2():int{ return _sfs2; }
        function set sfs1(n:Number){
            _sfs1 = n;
        }
        function set sfs2(i:int){
            this.sfs1 = i;
            _sfs2 = i;
        }

        // call setter from getter
        var _sfg1:String = "FAILED";
        var _sfg2:uint = 0;
        function get sfg1():String{ return _sfg1; }
        function get sfg2():uint{
            sfg1 = "PASSED";
            return _sfg2;
        }
        function set sfg1(s:String){ _sfg1 = s; }
        function set sfg2(ui:uint){ _sfg2 = ui; }
        
        // call getter from setter
        var _gfs1:String = "FAILED";
        var _gfs2:String = "PASSED";
        function get gfs1():String{
            return _gfs1;
        }
        function get gfs2():String{
            return _gfs2;
        }
        function set gfs1(s:String){
            _gfs1=gfs2;
        }
        function set gfs2(s:String){
            _gfs2=s;
        }

        // call getter from getter
        var _gfg1:String = "PASSED";
        var _gfg2:String = "FAILED";
        function get gfg1():String{
            return _gfg1;
        }
        function get gfg2():String{
            return gfg1;
        }

        // define a getter for a property and call the undefined setter
        var _nosetter = "FAILED";
        function get noSetter(){ return _nosetter; }

        // define a setter for a property and call the undefined getter
        var _nogetter = "FAILED";
        function set noGetter(s){ _nogetter = s; }

    }

}

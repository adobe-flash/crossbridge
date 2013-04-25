/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package GetSetPropertyMixup {

    public class GetSetPropertyMixup {

          var _foo = "foo";
          var _poo = "poo";
          var _loo = "loo";
          var _boo = "boo";
          
        
        public function get foo():String {
            return _loo;
        }
        public function set foo( s:String ) {
            _foo=s;
        }
        
        public function get fooForReal():String {
            return _foo;
        }
        public function set fooForReal( s:String ) {
            _foo=s;
        }
        public function get poo():String {
            return _poo;
        }
        public function set poo( s:String ) {
            _boo = s;
        }
        public function set loo(s:String) {
            _loo = s;
        }
        public function get loo():String {
            return _loo;
        }
        public function set boo(s:String) {
            _boo = s;
        }
        public function get boo():String {
            return _boo;
        }
        
    }

}

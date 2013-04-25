/*
Java -jar utils/asc.jar -import generated/builtin.abc -optimize -AS3  -in test/acceptance/shell.as  inline.as
*/

/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 2.0/GPL 2.0/LGPL 2.1
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is [Open Source Virtual Machine.].
 *
 * The Initial Developer of the Original Code is
 * Adobe System Incorporated.
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Adobe AS3 Team
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

var SECTION = "Definitions";       // provide a document reference (ie, Actionscript section)
var VERSION = "AS3";        // Version of ECMAScript or ActionScript
var TITLE   = "Super Behavior Tests";       // Provide ECMA section title or a description
var BUGNUMBER = "";

startTest();                // leave this alone


///////////////////////////////////////////////////////////////
// add your tests here
  
//import SuperExprChainAccess/

//package SuperExprChainAccess {

    class base {
        function f() : String {
            return "base f()";
        }
        function g() : String {
            return "base g()";
        }
        function i() : String {
            return "base i()";
        }
    }
    
    class middle1 extends base {
        override function f() : String {
            return "middle1 f()";
        }
        function h() : String {
            return "middle1 h()";
        }
    }
    
    class middle2 extends middle1 {
        override function h() : String {
            return "middle2 h()";
        }
        override function i() : String {
            return "middle2 i()";
        }
        function callh() : String {
            return super.h();
        }
        function calli() : String {
            return super.i();
        }
    }
    
    class derived extends middle2 {
        override function f() : String {
            return "derived f()";
        }
        override function g() : String {
            return "derived g()";
        }
        
        public function callf1() : String {
            return f();
        }
        public function callf2() : String {
            return super.f();
        }
        public function callg1() : String {
            return g();
        }
        public function callg2() : String {
            return super.g();
        }
        public function callh1() : String {
            return h();
        }
        public function callh2() : String {
            return super.h();
        }
        public function callh3() : String {
            return callh();
        }
        public function calli1() : String {
            return i();
        }
        public function calli2() : String {
            return super.i();
        }
        public function calli3() : String {
            return calli();
        }
    }
    
    class SuperExprChainAccess extends derived { }
//}


var seca = new SuperExprChainAccess();

//AddTestCase( "direct call to f()", "derived f()", seca.callf1() );
//AddTestCase( "super call to f()", "middle1 f()", seca.callf2() );

//AddTestCase( "direct call to g()", "derived g()", seca.callg1() );
//AddTestCase( "super chain call to g()", "base g()", seca.callg2() );

//AddTestCase( "direct call to h()", "middle2 h()", seca.callh1() );
//AddTestCase( "super chain call to h()", "middle2 h()", seca.callh2() );
AddTestCase( "super parent call to h()", "middle1 h()", seca.callh3() );

//AddTestCase( "direct call to i()", "middle2 i()", seca.calli1() );
//AddTestCase( "super chain call to i()", "middle2 i()", seca.calli2() );
//AddTestCase( "super parent call to i()", "base i()", seca.calli3() );

//
////////////////////////////////////////////////////////////////

test();       // leave this alone.  this executes the test cases and
              // displays results.

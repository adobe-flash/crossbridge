/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
// 15.1 / 16.4.1 allowable attributes combinations
// ought to exercise explicit / public by using another
// package here but explicit doesn't work yet and it's
// not worth complicating this test any further until
// it does... we could also consider using non-default
// classes to implement each and every combination but
// the basic interfaces test will work identically and
// we're doing the exhaustive tests in Classes/Impl/
// instead of in Interfaces/
package Qualifiers {

    interface defInt {
        function defFunc();
    }
    internal interface intInt {
        function defFunc();
    }
    
    public interface pubInt {
        function defFunc();
    }
    
    class defImplDefIntByInt implements defInt {
        public function defFunc() {
            return "defFunc()";
        }
    }
    class defImplDefIntByPub implements defInt {
        public function defFunc() {
            return "defFunc()";
        }
    }
    class defImplIntIntByInt implements intInt {
        public function defFunc() {
            return "defFunc()";
        }
    }
    class defImplIntIntByPub implements intInt {
        public function defFunc() {
            return "defFunc()";
        }
    }
    
    class defImplPubIntByInt implements pubInt {
        public function defFunc() {
            return "defFunc()";
        }
    }
    class defImplPubIntByPub implements pubInt {
        public function defFunc() {
            return "defFunc()";
        }
    }

    public class QualifierTest {
        var didibi = new defImplDefIntByInt;
        var didibp = new defImplDefIntByPub;
        var diiibi = new defImplIntIntByInt;
        var diiibp = new defImplIntIntByPub;
        
        var dipibi = new defImplPubIntByInt;
        var dipibp = new defImplPubIntByPub;
        public function doDefByInt() : String {
            return didibi.defInt::defFunc();
        }
        public function doDefByPub() : String {
            return didibp.defInt::defFunc();
        }
        public function doIntByInt() : String {
            return diiibi.intInt::defFunc();
        }
        public function doIntByPub() : String {
            return diiibp.intInt::defFunc();
        }
        
        public function doPubByInt() : String {
            return dipibi.pubInt::defFunc();
        }
        public function doPubByPub() : String {
            return dipibp.pubInt::defFunc();
        }
    }
}


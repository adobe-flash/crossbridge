/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
// Access properties via super using . and []
package SuperProps {

    public dynamic class SuperBase {
        public var x : String = "base::staticX";
        var y : String = "base::staticY";
        function SuperBase() {
            this["x"] = "base::dynamicX";
        }
        public function get baseProp() : String { return y; }
        public function setBaseVal( y : String, z : String ) { this[y] = z; }
        public function getBaseVal( y : String ) : String { return this[y]; }
    }
    
    public dynamic class SuperProps extends SuperBase {
        public function get inheritedProp() : String { return y; }
        public function get superPropDot() : String { return super.y; }
        public function get superPropIndex() : String { return super["x"]; }
        
        public function set superPropDot(val) : void { super.y = val; }
        public function set superPropIndex(val) : void { super["x"] = val; }
        
        public function setDerivedVal( y : String, z : String ) { this[y] = z; }
        public function getDerivedVal( y : String ) : String { return this[y]; }
        public function getSuperVal( y : String ) : String { return super[y]; }
    }
    
}

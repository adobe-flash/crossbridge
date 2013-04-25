/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package GetSetDiffScope {
    public namespace mx_internal = "http://www.macromedia.com/2005/flex/mx/internal";

    public class GetSetDiffScope {

        private var _s1;
        private var _s2;
        private var _g1 = "original value";
        private var _g2 = 5;
        private var ind:int = 1;

        public function get index():int
        {
            return ind;
        }

        mx_internal function set index(v:int):void
        {
            ind = v;
        }

        // g1 - Setter missing
        public function get g1()
        {
            return _g1;

        }

        // g2 - Setter private
        public function get g2()
        {
            return _g2;

        }
        private function set g2(g) {
            _g2 = s;
        }

        // s1 - Getter missing
        private function set s1(s) {
            _s1 = s;
        }

        // s2 - Getter private
        private function get s2()
        {
            return _s2;

        }
        public function set s2(s) {
            _s2 = s;
        }

        private function blah() {};

    }
    public class testclass
    {
        var getset:GetSetDiffScope;
        function testclass() {
            getset = new GetSetDiffScope();
        }
        public function doGet()
        {
            return getset.index;
        }
        public function doSet()
        {   getset.index = 5;
            return getset.index;
        }
    }

}

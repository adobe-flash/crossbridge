/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package ExtractMethPackage {
    public class PubClassExtractMeth {
        var x:String = "here";
        static var xStat = "i'm here";
        
        public function pubGetX():String {
            return this.x;
        }
        
        public static function statGetX():String {
            return xStat;
        }
        

    }
}

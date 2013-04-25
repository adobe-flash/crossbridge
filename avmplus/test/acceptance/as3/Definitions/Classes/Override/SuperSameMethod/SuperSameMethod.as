/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package SuperSameMethod {
    public class SuperSameMethod extends BaseClass {
        override public function echo() : String {
            return super.echo() + " override";
        }
    }
}

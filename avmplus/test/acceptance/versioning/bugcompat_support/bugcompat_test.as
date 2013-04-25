/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package {
    import avmplus.Domain

    public class testclass
    {
        public function testclass()
        {
        }

        public function vtest(pathToLoad:String, parent_domain:Domain, bugCompat:String)
        {
            var domain = new Domain(parent_domain);
            domain.load(pathToLoad, bugCompat);
            var cl = domain.getClass("leafclass");
            var o = new cl;
            return o.vtest();
        }
    }
}

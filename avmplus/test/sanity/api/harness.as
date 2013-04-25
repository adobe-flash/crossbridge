/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
 
package {
  // t = test function
  // e = error expected
  var run_count = 0;
  public function run(t,e) {
    var r;
    var m = "";
    try {
      r = t()
      if (e) {
        m = ": error expected";
        r = false;
      }
    } catch (x) {
      r = e;
      if (!e) {
         m = ": unexpected error: " + x;
         r = false;
      }
    }
    finally {
      print("[" + run_count++ + "] " + r + m);
    }
  }
}
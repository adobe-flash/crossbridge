// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

class foo{
    // infinite recursion calling setter from setter and back again
 	// fix make sure you do handle stack overflow
    private var _inf;
    private var _inf2;
    public function get inf(){ return inf2; }
    public function get inf2(){ return inf; }

}

var OBJ = new foo();
OBJ.inf; // should cause infinite recursion
 

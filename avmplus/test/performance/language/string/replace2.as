/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

include 'timetest.as'

var str = "asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj  asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj"+
          "asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj  asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj"+
          "asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj"+
          "regularexpressions@yahoo.com asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj"+
          "asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj"+
          "asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj asdfgf lkjhj";

var regex = new RegExp(/([a-zA-Z0-9])+([\.a-zA-Z0-9_-])*@([a-zA-Z0-9])+(\.[a-zA-Z0-9_-]+)/m);
function replace(loops) {
    for (var i=0; i<loops; i++)
        str.replace(regex,"regularexpression2@msn.com");
}

timetest(replace, 500, 200);


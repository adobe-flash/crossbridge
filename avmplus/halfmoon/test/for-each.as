// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function foo(){
   var copy = new Array(5);
   copy[0] = 17;    copy[1] = 18;    copy[2] = 19;    copy[3] = 20;
   copy[4] = 21;
   var yescnt=0
   var nocnt=0

//    for (var i=0; i < 5; i++){
//        var o = copy[i]

   for each( var o in copy) {
       if ( o == 19 ){
           yescnt++
       } else {
           nocnt++
       }
   }
   trace('yescnt ' + yescnt)
   trace('nocnt ' + nocnt)
}

foo() //run it

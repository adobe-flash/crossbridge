/* -*- Mode: actionscript; -*- */

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package {

   trace( caller(42) )

   public class vec {
       var x:int;
        var y:int;

        function vec(x:uint,y:uint) {
            this.x=x;
            this.y=y;
        }

        public function sum_flow(flg:uint):uint {
			var answer:uint
			if ( flg != 0 ) {
				answer = 42
			}else{
				answer = this.x+this.y;
			}
			return answer
        }

    }//class vec


function caller( p1: uint) : uint {
   var v:vec = new vec(1,2)
   var rc:uint = v.sum_flow(1)
   return rc
   }

}
   

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package {

// To make the benchmark results predictable, we replace Math.random
// with a 100% deterministic alternative.
var seed = 49734321;
class Math2 {
 /***
 * The var seed switches between many different types. Number, int, unsigned int
* etc. This test case has some variability in it because only SOMTIMES can we have the constant values be a constant atom box. This case just checks to make sure everytime you box a constant int, its a valid constant atom box.
*/
    public static function random() {
        // Robert Jenkins' 32 bit integer hash function.
		print("Seed in Math2.random: " + seed);
        seed = ((seed + 0x7ed55d16) + (seed << 12))  & 0xffffffff;
		print("First round: " + seed);
        seed = ((seed ^ 0xc761c23c) ^ (seed >>> 19)) & 0xffffffff;
		print("second round: " + seed);
        seed = ((seed + 0x165667b1) + (seed << 5))   & 0xffffffff;
		print("third round: " + seed);
        seed = ((seed + 0xd3a2646c) ^ (seed << 9))   & 0xffffffff;
		print("fourth round: " + seed);
        seed = ((seed + 0xfd7046c5) + (seed << 3))   & 0xffffffff;
		print("fith round: " + seed);
        seed = ((seed ^ 0xb55a4f09) ^ (seed >>> 16)) & 0xffffffff;
		print("Last round: " + seed);
        return (seed & 0xfffffff) / 0x10000000;
    }
}

    public class Splayer {
        var kSplayTreeSize = 8;
        var kSplayTreeModifications = 3;
        var kSplayTreePayloadDepth = 1;
 
        var splayTree = null;



        public function InsertNewNode() {
            var key;
			var didFind;
            do {
				print("Generating key");
              	key = Math2.random();
				print(key);
				didFind = splayTree.find(key);
				print("Seed in insert new node: " + seed);
            } while (didFind != null);
            return key;
        }


        public function playerSetup() {
            splayTree = new SplayTree();
            for (var i = 0; i < kSplayTreeSize; i++) { 
                InsertNewNode(); 
            }
        }
    } // end class

    public class SplayTree {
        public function SplayTree() {
        }

        private var root_ = null;

        public function isEmpty() {
            return true;
        };


        public function find(key) {
			print("Seed is: " + seed);
            if (this.isEmpty()) {
                return null;
            } else {
				return this.root_.key == key ? this.root_ : null;
			}
        };
    }

}   // End package


    var splashPlayer = new Splayer();
    splashPlayer.playerSetup();
	

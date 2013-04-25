// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package richards {
/*
    var r = new RichardsClass();
	r.runRichards();
    
    public class RichardsClass {
        public function RichardsClass() {
        }
    
        public function runRichards() {
          var tcb = new TaskControlBlock(null);
        } // end runRichards
    } // class RichardsClass
*/
    var x:TaskControlBlock = new TaskControlBlock(null);
    
    public class TaskControlBlock {
        public var state:int;

        public function TaskControlBlock(queue) {
          if (queue == null) {
            this.state = 2;
          } else {
            this.state = 5;
          }
          print(this.state);
        }
    } // class Task Control block
} // package

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package{

    public dynamic class Animal extends Object
    {
        public var name:String;
        public var age:int;
        public var insideNum:Number;
        private var privVar:String;
        protected var protVar:String;
        internal var internalBool:Boolean;
        public static var staticStr:String = "Panda";
        public var protoVar:String;
        
        public function Animal(){
            this.privVar = "FAIL private";
            this.protVar = "FAIL protected";
            this.insideNum = 999999;
            this.protoVar = "Proto Shark";
        }
        
        private function privFunc(){
            return "privFunc called";
        }
        
    }

}

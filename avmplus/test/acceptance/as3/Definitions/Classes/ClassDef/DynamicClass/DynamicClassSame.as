/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package DynamicClassSame {

    namespace ns;

    dynamic class DynamicClass {

        var array:Array = new Array(1,2,3);                     // Default property
        internal var intNumber:Number = 100;                    // internal property
        protected var protInt:int = -1;                     // protected property
        public var pubUint:uint = 1;                            // public property
        private var privVar:Boolean = true;                     // private property
        public static var pubStatBoolean:Boolean = true;        // public static property
        ns var nsProp:String = "nsProp";                        // namespace property

        // default method
        function defaultMethod():Boolean{ return true; }
        
        // Internal method
        internal function internalMethod(n:Number):int { return n; }
    
        // protected method
        protected function protectedMethod():uint { return 1; }

        // public method
        public function publicMethod():Boolean { return true; }

        // private method
        private function privateMethod():Boolean { return true; }

        // namespace method
        ns function nsMethod():String { return "nsMethod"; }

        // public final method
        public final function publicFinalMethod():Number { return 1; }

        // public static method
        public static function publicStaticMethod():int { return 42; }

        // access private property from same class same package
        function accPrivProp():Boolean { return this.privVar; }

        // access private method from same class same package
        function accPrivMethod():Boolean { return this.privateMethod(); }
        
    }

    

    public class DynamicClassAccessor {

        var acc:DynamicClass;

        public function DynamicClassAccessor(){
            acc = new DynamicClass();
        }

        // access default property from same package
        public function accDefProp():Array{ return acc.array; }

        // access internal property from same package
        public function accIntProp():Number{ return acc.intNumber; }

        // access protected property from same package - NOT LEGAL OUTSIDE DERIVED CLASS
        //public function accProtProp():int{ return acc.protInt; }

        // access public property from same package
        public function accPubProp():uint { return acc.pubUint; }

        // access private property from same class same package
        public function accPrivProp():Boolean { return acc.accPrivProp(); }

        // access namespace property from same package
        public function accNSProp():String { return acc.ns::nsProp; }

        // access public static property from same package
        public function accPubStatProp():Boolean { return DynamicClass.pubStatBoolean; }


        // access default method from same package
        public function accDefMethod():Boolean { return acc.defaultMethod(); }

        // access internal method from same package
        public function accIntMethod():Number { return acc.internalMethod(50); }

        // access protected method from same package - NOT LEGAL OUTSIDE DERIVED CLASS
        //public function accProtMethod():uint { return acc.protectedMethod(); }

        // access public method from same package
        public function accPubMethod():Boolean { return acc.publicMethod(); }

        // access private method from same class and package
        public function accPrivMethod():Boolean { return acc.accPrivMethod(); }

        // access namespace method from same package
        public function accNSMethod():String { return acc.ns::nsMethod(); }

        // Access final public method from same package
        public function accPubFinMethod():Number { return acc.publicFinalMethod(); }

        // access static public method from same package
        public function accPubStatMethod():int { return DynamicClass.publicStaticMethod(); }

        // Error cases

        // access private property from same package not same class
        public function accPrivPropErr() { return privProp; }

        // access private method from same pakcage not same class
        public function accPrivMethErr() { return privMethod(); }

    }
    
}

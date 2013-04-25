/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Test dynamic getter setter from usage 1 of AS 1/2 help documentation
package{
    import flash.utils.*;
    import avmplus.*;

    public class Book extends Proxy
    {
        var getter:Array;
        var setter:Array;
        
        public function Book() {
            getter = new Array();
            setter = new Array();
        }
        
        public function addProperty(name:String, getFunc:Function, setFunc:Function):void {
            getter[name] = getFunc;
            setter[name] = setFunc;
        }
        
        override flash_proxy function getProperty(name:*):*
        {
            return getter[name]();
        }
    
        override flash_proxy function setProperty(name:*, value:*):void
        {
            setter[name](value);
        }
        
        override flash_proxy function callProperty(name:*, ...rest):*
        {
            return getter[name]();
        }
    }

}

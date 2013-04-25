/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// This class overrides all the basic functionality of Proxy
package{
    import flash.utils.*;
    import avmplus.*;
    
    public class ProxyAdvanced extends Proxy
    {
        private var target:*;
    
        public function ProxyAdvanced(obj:*)
        {
            target = obj;
        }
        
        override flash_proxy function getProperty(name:*):*
        {
            if (target[name])
            {
                return target[name];
            }
            else
            {
                return "property not found";
            }
        }
        
        override flash_proxy function setProperty(name:*, value:*):void
        {
            target[name] = value;
        }
        override flash_proxy function deleteProperty(name:*):Boolean
        {
            delete target[name] ;
        };
    
        override flash_proxy function hasProperty(name:*):Boolean
        {
            trace("hasProperty called");
            //return target.hasOwnProperty(name);
            return true;
        }
        
        // Handle all functions with more than 1 argument
        public function myFunction(name:*):String
        {
            return ("Method " + name + " was called");
        }
        
        override flash_proxy function callProperty(name:*, ...rest):*
        {
            // test that if a function has more than 2 arguments, just call the default function myFunction
            if(rest[2] != undefined)
            {
                //trace("more than 2 args");
                return this.myFunction(name);
            }
            // test that a call to a function with 2 arguments can be redirected properly
            else if(rest[1] != undefined)
            {
                //trace("2 args");
                return target[name](rest[0], rest[1]);
            }
            // test that a call to a function with 1 argument can be redirected properly
            else if(rest[0] != undefined)
            {
                //trace("1 arg");
                return target[name](rest[0]);
            }
            // test that a call to a function with 0 arguments can be redirected properly
            else
            {
                //trace("0 args");
                return target[name]();
            }
        }
        
        
        override flash_proxy function getDescendants (name:*):*
        {
            trace("getDescendants called for name " + name);
            return target.descendents(name);
            //return "Hello, Michael Morris!";
        }
        /* Not supported right now
        override flash_proxy function delDescendants (name:*):void {
            delete target.descendents(name)
        }
        */
        override flash_proxy function isAttribute(name:*):Boolean
        {
            trace("isAttribute called for name " + name);
            //return target.isAttribute(name)
            return true;
        }
    
    }

}

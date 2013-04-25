/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// This class overrides the basic functionality of Proxy
package{
    import flash.utils.*;
    import avmplus.*;
    
    public class PrivProxy extends Proxy
    {
        private var target:*;
    
        public function PrivProxy(obj:*):void
        {
            target = obj;
        }
        
        override flash_proxy function getProperty(name:*):*
        {
            return target[name];
        }
        
        override flash_proxy function setProperty(name:*, value:*):void
        {
            target[name] = value;
        }
        override flash_proxy function deleteProperty(name:*):Boolean
        {
            delete target[name] ;
        };
    
        // Handle all functions with more than 1 argument
        public function myFunction(name:*, arg):String
        {
            return ("Method " + name+ " was called. With args: " + arg);
        }
        
        override flash_proxy function callProperty(name:*, ...rest):*
        {
            trace("call property called");
            return this.myFunction(name, rest);
        }
        
        override flash_proxy function hasProperty(name:*):Boolean
        {
            //trace("hasProperty called with " + name);
            return target.hasOwnProperty(name);
        }
        
        protected var a:Array; // array of object's properties
            override flash_proxy function nextNameIndex (index:int):int
            {
              if (index == 0) // initial call
              {
            a = new Array();
            for (var x in target)
            {
               a.push (x);
            }
              }
              if (index < a.length)
            return index + 1;
              else
            return 0;
            }
        override flash_proxy function nextName (index:int):String
            {
              return a[index-1];
            }
        override flash_proxy function nextValue (index:int):*
            {
              return target[a[index-1]];
            }
    }
}

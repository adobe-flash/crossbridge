/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// This class overrides the basic functionality of Proxy
package {
    import flash.utils.*;
    import avmplus.*;

    public class ProxySmoke extends Proxy
    {
        private var target:*;
    
        public function ProxySmoke(obj:*):void
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
            return delete target[name] ;
        };
    
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

}//package brace


/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package{
    import flash.utils.*;
    import avmplus.*;
    
    use namespace flash_proxy;
    
    public class XMLProxy extends Proxy
    {
        protected var target:*;
        protected var myXML:XML;
    
        
        public function XMLProxy(obj:*)
        {
            target = obj;
            myXML = obj;
        }
        
        override flash_proxy function getProperty(variableName:*):*
        {
            //trace("getProperty with " + variableName);
            
            var isAttr = "";
    
            if(isAttribute(variableName))
            {
                //trace(variableName + " is an attribute");
                isAttr = "isAttribute=true";
                if(myXML..@[variableName] != undefined)
                {
                    return isAttr + " -- " + myXML..@[variableName];
                }
            }
            else
            {
                //trace(variableName + " is not an attribute");
                isAttr = "isAttribute=false";
                if(myXML.descendants(variableName) != undefined)
                {
                return isAttr + " -- " + myXML.descendants(variableName).toXMLString();
                }
            }
            
            // add 'isAttribute=true/false' in front of the property so we can make sure isAttribute worked correctly
            if(target[variableName] != undefined)
            {
                return isAttr + " -- " + target[variableName];
            }
            else
            {
                return isAttr + " -- " + variableName + " does not exist";
            }
        }
    
        
        
        override flash_proxy function setProperty(name:*, value:*):void {
            trace('setProperty('+name+', '+value+')');
            target[name] = value;
        }
        
        override flash_proxy function deleteProperty(variableName:*):Boolean {
            trace("calling delete " + variableName);
            //delete myXML.@[variableName] ;
            //delete myXML.@b;
            //delete myXML..@[variableName];
            if(isAttribute(variableName))
            {
                trace(variableName + " is an attribute -- attempting to delete");
            }
            else
            {
                trace(variableName + " is NOT an attribute -- attempting to delete");
            }
            var deleteStatus:Boolean = delete myXML[variableName];
            return deleteStatus;
            //delete myXML[variableName];
        };
        
        override flash_proxy function callProperty(name:*, ...rest):* {
            //trace("callProperty with name " + name + " and argument " + rest[0]);
    
            trace("callProperty with name " + name );//+ " and argument " + rest[0]);
    
            if(rest != undefined)
            {
                trace("and argument " + rest[0]);
            }
    
            if(name == "attributes")
            {
                // handle attributes
                return myXML..@[rest[0]];
            }
            else if (name == "descendants")
            {
                // handle descendants
    
                return myXML.descendants(rest[0]);
            }
            
            else if(name == "toString")
            {
                return myXML.toString();
            }
    
            else if(name == "toXMLString")
            {
                return myXML.toXMLString();
            }
    
            else
            {
                //return "Error in callProperty with name " + name + " and argument " + rest[0];
                return "Error in callProperty with name " + name;
            }
        };
        
        override flash_proxy function getDescendants (name:*):*
        {
            trace("getDescendants called with name " + name);
            //return target[name];
            
            if(isAttribute(name))
            {
                trace(name + " is an attribute");
            }
            else
            {
                trace(name + " is not an attribute");
            }
            
            if(myXML.descendants(name) == undefined)
            {
                trace("descendants is undefined for " + name);
                return "descendants is undefined for " + name;
            }
            else if (myXML.descendants(name) == "")
            {
                trace("empty string");
                return "empty string";
            }
            else
            {
                trace("getting descendants for " + name + " " + myXML.descendants(name));
                return myXML.descendants(name);
            }
            
        }
    }
}

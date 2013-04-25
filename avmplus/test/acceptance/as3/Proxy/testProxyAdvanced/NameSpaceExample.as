/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package NameSpacePackage{
        
    import NameSpacePackage.*;
    use namespace NameSpaceDef;
    
    public class NameSpaceExample
    {
        
        NameSpaceDef var one:String = "";
        NameSpaceDef var two:String = "";
        NameSpaceDef var three:Number = 0;
        NameSpaceDef var four:Number = 0;
        public var onePub:String;
        
        function NameSpaceExample(){
            trace('inside NameSpaceExample');
        }
    }
}

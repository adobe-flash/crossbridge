/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package ns {
    public namespace N1;
    public namespace N2;
        
    public namespace N3 = "foo";
    public namespace N4 = N3;
     
    public class foo {
        public var v1;
        public var v2;
        public var v3;
                public var o:Object;
        var holder;
        
        N1 var v3;
        N2 var n2;
        N2 var n3;
        N2 var n4;
        N3 var v1;
        N3 var v2;
                
    
        public function foo() {
            v1 = 1;
            v2 = 2;
            v3 = 5;
                        

             o = new Object();
             o.v4 = 4;
             holder = "v4";

             

             N1::v3 = v1+v2;
             N2::n2 = N1::v3;
             N2::n3 = v3;
             N2::n4 = o[holder];
             N3::v1 = v3;
                         try{
                 N3::['v2'] = v3;
                         }catch(e:Error){
                             thisError="no error";
                         }finally{
                          }
                         N4::v1 = v3;
                         N4::v2 = v3;

        }
    }
}

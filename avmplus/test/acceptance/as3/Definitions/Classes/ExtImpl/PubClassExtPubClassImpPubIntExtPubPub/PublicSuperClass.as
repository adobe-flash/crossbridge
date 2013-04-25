/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package PubClassExtPubClassImpPubIntExtPubPub{
        public interface PublicInt{
                                  function MyString():String;
                                  }
        public interface PublicInt2{
                                  function MyString():String;
                                  function MyNegInteger():int;
                                   }
        public interface PublicInt3 extends PublicInt,PublicInt2{
                                  function MyUnsignedInteger():uint;
                                  function MyNegativeInteger():int;
                                                                 }
                                              
        public class PublicSuperClass implements PublicInt3{
                                                            
                                 public function MyString():String{
                                                                    return "Hi!";
                                                                    }
                                 
                                 public function MyNegInteger():int{
                                                                    var a:int = -100;
                                                                    return a;
                                                                       }
                                  public function MyUnsignedInteger():uint{
                                                                    var postint:uint =100;
                                                                    return postint;
                                                                  }
                                  public function MyNegativeInteger():int{
                                                                    var negint:int = -100000;
                                                                    return negint;
                                                                           }

                                                            }

       public class PublicSubClass extends PublicSuperClass{}
                                                 }

                                                         

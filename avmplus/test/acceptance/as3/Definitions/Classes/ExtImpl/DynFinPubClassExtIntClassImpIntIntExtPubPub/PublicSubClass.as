/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package DynFinPubClassExtIntClassImpIntIntExtPubPub{
        public interface PublicInt{
                                  function MyString():String;
                                  }
        public interface PublicInt2{
                                  function MyString():String;
                                  function MyNegInteger():int;
                                   }
        internal interface PublicInt3 extends PublicInt,PublicInt2{
                                  function MyUnsignedInteger():uint;
                                  function MyNegativeInteger():int;
                                                                 }
        internal class InternalSuperClass{
                                   public function MySuperBoolean():Boolean{return true;}
                                   internal function MySuperNumber():Number{return 10;}
                            public static function MySuperStaticDate():Date {return new Date(0);}

                            
                                     }
                                              
        dynamic final public class PublicSubClass extends InternalSuperClass implements PublicInt3{
                                                           
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
                                  public function RetMySuperNumber():Number{return MySuperNumber();}
                                  public function RetMySuperBoolean():Boolean{return MySuperBoolean();}
                                  public function RetMySuperStaticDate():Date{return MySuperStaticDate();}
                                                            }

        public class InternalInterfaceAccessor{
                var k = new PublicSubClass();

                var PubInt3:PublicInt3 = k;
                var PubInttwo:PublicInt2=k;
                public function RetMyNegInteger():int{
                                                      return PubInt3.MyNegInteger();
                                                    }
                public function RetMyNegInteger2():int{
                                                      return PubInttwo.MyNegInteger();
                                                      }
                public function RetMyUnsignedInteger():uint{
                                                      return PubInt3.MyUnsignedInteger();
                                                            }
                public function RetMyNegativeInteger():int{
                                                 return k.PublicInt3::MyNegativeInteger()}
                                                 }
                                            }

                                                         

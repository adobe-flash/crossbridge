/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package UserDefinedErrorsPackageTryBlockOutside
{
    public class BoxDimensionException extends Error
    {
        public function BoxDimensionException(boxErrMsg:String)
        {
            super(boxErrMsg);
                                                     
        }
      
     }
     public class BoxUnderzeroException extends BoxDimensionException
     {
      
        public function BoxUnderzeroException(boxErrMsg2:String)
        {
            super(boxErrMsg2);
        }
      }

      public class BoxOverflowException extends BoxDimensionException
      {
          public function BoxOverflowException(boxErrMsg3:String)
          {
              super(boxErrMsg3);
          }
      }

      public class Box
      {
          private var width:Number;
     
          public function setWidth(w):Boolean
          {
             var k:String = decodeURI("!@#$%^&*()<>?");
             eval(m);
             var arr:Array=new Array(-10);
             var MyString:String = MyString+1;
             var errmsg:String="Illegal Box Dimension specified";
             var errmsg2:String="Box dimensions should be greater than 0";
             var errmsg3:String="Box dimensions must be less than Number.MAX_VALUE";
             if (isNaN(w)){
                  throw new BoxDimensionException(errmsg);
             }else if (w<= 0){
                  throw new BoxUnderzeroException(errmsg2);
             }else if (w>Number.MAX_VALUE){
                  throw new BoxOverflowException(errmsg3);
              }
     
             width = w;
            
           }
       }
}
package TryCatchBlockUserWithBuiltInExceptions
{
    import UserDefinedErrorsPackageTryBlockOutside.*;
import com.adobe.test.Assert;
import com.adobe.test.Utils;
    public class TryAndCatchBlockWithUserDefinedErrors
    {
        var b:Box = new Box();
        var someWidth:Number=(Number.MAX_VALUE)*10;
        var thisError = "no error";
        var thisError1 = "no error";
        var thisError2 = "no error";
        var thisError3 = "no error";
        var thisError4 = "no error";
        var thisError5 = "no error";
        var thisError6 = "no error";
        var thisError11 = "no error";
        var thisError8 = "no error";
        var thisError9 = "no error";
        var thisError10 ="no error";
        public function MyTryThrowCatchFunction():void
        {
            try {
                b.setWidth(someWidth);
                }catch(e:BoxOverflowException){
                     thisError = e.message;
                     //trace("BoxOverflowException:"+thisError);
                }catch(e1:BoxUnderzeroException){
                     thisError1=e1.message;
                     //trace("BoxUnderzeroException:"+thisError1);
                }catch(e2:BoxDimensionException){
                     thisError2 = e2.message;
                     //trace("BoxDimensionException Occurred:"+thisError2);
                }catch(e3:ReferenceError){
                     thisError3=e.toString();
                     //print(thisError3);
                }catch(e4:TypeError){
                     thisError4=e4.toString();
                     //print(thisError4)
                }catch(e5:ArgumentError){
                     thisError5=e5.toString();
                     //print(thisError5)
                }catch(e6:URIError){
                     thisError6=e6.toString();
                     //print(thisError6)
                }catch(e8:UninitializedError){
                     thisError8=e8.toString();
                     //print(thisError8)
                }catch(e9:EvalError){
                     thisError9=e9.toString();
                     //print(thisError9)
                }catch(e10:RangeError){
                     thisError10=e10.toString();
                     //print(thisError10)
                }catch(e11:Error){
                     //print(e11.toString());
                     thisError11=e11.toString();
                }finally{//print("This Error is:"+thisError);
                     Assert.expectEq( "Testing try block and multiple catch blocks with       custom error classes", "no error",thisError );
                     Assert.expectEq( "Testing catch block with type error",
                           "no error",Utils.typeError(thisError4) );
                     Assert.expectEq( "Testing catch block with Argument Error",                                        "no error" ,thisError5);
                     Assert.expectEq( "Testing catch block with URIError",
                           "URIError: Error #1052",Utils.uriError(thisError6));
                     Assert.expectEq( "Testing catch block with Eval Error",
                           "no error" ,thisError9);                                       Assert.expectEq( "Testing catch block with Range Error",
                           "no error",thisError10);
                     Assert.expectEq( "Testing catch block with Error", "no error"                                          ,thisError11);
                 }
          }
     }
}

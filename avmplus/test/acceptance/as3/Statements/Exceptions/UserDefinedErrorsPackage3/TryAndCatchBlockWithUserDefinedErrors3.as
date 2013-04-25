/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package UserDefinedErrorsPackage3
{
    import com.adobe.test.Assert;
    class BoxDimensionException extends Error
    {
        public function BoxDimensionException(boxErrMsg:String)
        {
            super(boxErrMsg);
                                                     
        }
    }
    class BoxUnderzeroException extends BoxDimensionException
    {
        public function BoxUnderzeroException(boxErrMsg2:String)
         {
             super(boxErrMsg2);
         }
    }

    class BoxOverflowException extends BoxDimensionException
    {
        public function BoxOverflowException(boxErrMsg3:String)
          {
              super(boxErrMsg3);
                                                            
          }
    }

    class Box
    {
        private var width:Number;
     
        public function setWidth(w):Boolean
           {
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

     public class TryAndCatchBlockWithUserDefinedErrors3
     {
         var b:Box = new Box();
         var someWidth:Number=NaN;
         thisError = "no error";

         public function MyTryThrowCatchFunction():void
        {
            try {
                b.setWidth(someWidth);
                }catch(e:BoxOverflowException){
                     thisError = e.message;
                    //trace("BoxOverflowException:"+thisError);
                }catch(e1:BoxUnderzeroException){
                     thisError=e1.message;
                     //trace("BoxUnderzeroException:"+thisError);
                }catch(e2:BoxDimensionException){
                     thisError = e2.message;
                    //trace("BoxDimensionException Occurred:"+thisError);
                }catch(e3:Error){
                     thisError = e3.message;
                    //trace("An error occurred:"+e3.toString());
                }finally{
                     Assert.expectEq( "Testing try block and multiple catch blocks with custom error classes", "Illegal Box Dimension specified",thisError );
                 }
        }
      }
}


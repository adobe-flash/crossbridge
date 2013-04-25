// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

package C_Run
{
  import flash.display.Sprite;
  import flash.system.System;
  import flash.utils.ByteArray;
  import flash.utils.Endian;
  import com.adobe.flascc.CModule;
  import com.adobe.flascc.GoingAsync;
  import C_Run.workerClass;

  /**
  * @private
  */
  public var workerSWFBytes:ByteArray;

  /**
  * @private
  */
  public function createRawWorker(ba:ByteArray):*
  {
    return workerDomainClass["current"].createWorker(ba);
  }

  /**
  * @private
  * we start a worker with a near clone of the current SWF...
  * the only difference is that we munge the SymbolClass entry
  * such that our Sprite becomes the root sprite
  * instead of the app's default root Sprite
  */
  function retargetSWF():ByteArray
  {
    if(!CModule.rootSprite)
      throw new Error("specify a rootSprite using \"CModule.rootSprite = ...\" before spawning threads");
    if(!CModule.rootSprite.loaderInfo)
      throw new Error("loaderinfo on rootSprite was null");
    if(!CModule.rootSprite.loaderInfo.bytes)
      throw new Error("loaderinfo.bytes on rootSprite was null");

    var swfBytes:ByteArray = CModule.rootSprite.loaderInfo.bytes;
    var headSize:int = (((swfBytes[8] >> 3) * 4 + 5 + 7) >> 3) + 12;
    var swfGuts:ByteArray = new ByteArray;

    swfGuts.endian = Endian.LITTLE_ENDIAN;

    //trace("swf head size: " + headSize);
    swfGuts.writeBytes(swfBytes, headSize);
    if(swfBytes[0] != 70/*'F'*/)
    {
      throw new Error("loaderInfo.bytes is compressed");
    }
    //else trace("uncompressed: " + swfBytes[0]);


    var newGuts:ByteArray = new ByteArray;

    newGuts.endian = Endian.LITTLE_ENDIAN;
    swfGuts.position = 0;
    // loop over records copying them
    while(swfGuts.bytesAvailable)
    {
      var tagAndLen:int = swfGuts.readUnsignedShort();
      var tag:int = tagAndLen >> 6;
      var len:int = tagAndLen & 0x3f;

      if(len == 0x3f)
      len = swfGuts.readInt();

      var payloadBA:ByteArray;
      var payloadOffs:int;

      if(tag == 76) // SymbolClass -- rewrite id 0 to our Sprite!
      {
        payloadBA = new ByteArray;
        payloadBA.endian = Endian.LITTLE_ENDIAN;
        payloadOffs = 0;

        var numSymbols:int = swfGuts.readUnsignedShort();

        payloadBA.writeShort(numSymbols);

        while(numSymbols--)
        {
          var id:int = swfGuts.readUnsignedShort();

          payloadBA.writeShort(id);
          if(id == 0) // retarget!
          {
            //trace("retarget symbol 0");
            payloadBA.writeUTFBytes("C_Run.AlcWorkerSprite");
            payloadBA.writeByte(0);
            while(swfGuts.readByte()) {} // skip symbol name
          }
          else
          {
            for(;;) // copy symbol name
            {
              var b:int = swfGuts.readByte();
              payloadBA.writeByte(b);
              if(!b)
                break;
            }
          }
        }
        len = payloadBA.length;
      }
      else
      {
        payloadBA = swfGuts;
        payloadOffs = swfGuts.position;
        swfGuts.position += len;
      }
      if(len < 0x3f) {
        newGuts.writeShort((tag << 6) | len);
      } else {
        newGuts.writeShort((tag << 6) | 0x3f);
        newGuts.writeInt(len);
      }
      //trace("wb: "+ payloadOffs + " / " + len);
      newGuts.writeBytes(payloadBA, payloadOffs, len);
    }

    var newSwf:ByteArray = new ByteArray;

    newSwf.endian = Endian.LITTLE_ENDIAN;

    // header (uncompressed)
    newSwf.writeUTFBytes("FWS");
    // ver
    //trace("swf ver: " + swfBytes[3]);
    newSwf.writeByte(swfBytes[3]);
    // new length
    newSwf.writeInt(headSize + newGuts.length);
    // rest of head
    newSwf.writeBytes(swfBytes, 8, headSize - 8);
    // guts!
    newSwf.writeBytes(newGuts);
    return newSwf;
  }

  /**
  * @private
  */
  public function createflasccWorker():*
  {
    if(!workerSWFBytes)
      workerSWFBytes = retargetSWF()

    return workerDomainClass["current"].createWorker(workerSWFBytes);
  }

  [ExcludeClass]
  /**
  * @private
  */
  public class AlcWorkerSprite extends Sprite
  {
    public function AlcWorkerSprite()
    {
      run();
    }

    public function run():void
    {
      //trace("run");
      //TODO most code shared w/ startHack.as ... unify?
      try
      {
        var entry:int = workerClass["current"].getSharedProperty("flascc.thread_entry");
        // TODO will start args support vector?
        var fargs:Vector.<int> = workerClass["current"].getSharedProperty("flascc.thread_args").readObject();

        CModule.prepForThreadedExec();
        CModule.activeConsole = this;
        if(!CModule.rootSprite)
          CModule.rootSprite = this;
        CModule.callI(entry, fargs);
      }
      catch(e:GoingAsync) {} // we're going into async mode
      catch(e:*)
      {
        if(e is Exit) {
          System.exit(e.code); // TODO doesn't appear to actually exit!
        } else {
          trace("Warning: Worker threw exception: " + threadId + " - " + e + "\n" + e.getStackTrace());
        } 
      }
      trace("Warning: Worker returned without exiting: " + threadId);
    }
  }
}

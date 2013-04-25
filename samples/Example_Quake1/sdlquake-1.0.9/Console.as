/*
** ADOBE SYSTEMS INCORPORATED
** Copyright 2012 Adobe Systems Incorporated
** All Rights Reserved.
**
** NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the
** terms of the Adobe license agreement accompanying it.  If you have received this file from a
** source other than Adobe, then your use, modification, or distribution of it requires the prior
** written permission of Adobe.
*/
package com.adobe.flascc
{
  import flash.display.Bitmap
  import flash.display.BitmapData
  import flash.display.DisplayObjectContainer;
  import flash.display.LoaderInfo;
  import flash.display.Sprite;
  import flash.display.StageScaleMode;
  import flash.events.Event;
  import flash.events.KeyboardEvent;
  import flash.events.MouseEvent;
  import flash.events.SampleDataEvent;
  import flash.external.ExternalInterface;
  import flash.geom.Rectangle
  import flash.media.Sound;
  import flash.media.SoundChannel;
  import flash.net.LocalConnection;
  import flash.net.URLRequest;
  import flash.system.Security;
  import flash.text.TextField;
  import flash.utils.ByteArray;

  import com.adobe.flascc.vfs.InMemoryBackingStore;
  import com.adobe.flascc.vfs.ISpecialFile;
  import com.adobe.flascc.vfs.RootFSBackingStore;

  /**
  * A basic implementation of a console for FlasCC apps.
  * The PlayerKernel class delegates to this for things like read/write
  * so that console output can be displayed in a TextField on the Stage.
  */
  public class Console extends Sprite implements ISpecialFile
  {
    private static const _height:int = 768 + 100
    private static const _width:int = 1024

    public var mx:int = 0, my:int = 0
    public var sndDataBuffer:ByteArray = null

    private var _tf:TextField
    private var bm:Bitmap
    private var enableConsole:Boolean = false
    private var runningInWorker:Boolean = false
    private var frameCount:int = 0
    private var enginetickptr:int, engineticksoundptr:int
    private var inputContainer
    private var bmd:BitmapData
    private var bmr:Rectangle
    private var keybytes:ByteArray = new ByteArray()
    private var last_mx:int = 0, last_my:int = 0
    private var snd:Sound = null
    private var waitForData:Boolean = false
    private var sndChan:SoundChannel = null
    private var vbuffer:int, vgl_mx:int, vgl_my:int, kp:int
    private const emptyArgs:Vector.<int> = new Vector.<int>
    
    include "../../TestingCode.as"

    /**
    * To Support the preloader case you might want to have the Console
    * act as a child of some other DisplayObjectContainer.
    */
    public function Console(container:DisplayObjectContainer = null)
    {
      CModule.rootSprite = container ? container.root : this

      if(CModule.runningAsWorker()) {
        return;
      }

      // If this SWF is being hosted in a webpage and doesn't have an embedded
      // VFS it will wait until the JS host supplies it with a PAK0.PAK file
      try {
        var paramObj:Object = CModule.rootSprite.loaderInfo.parameters.waitForData;
        if(paramObj.toString() == "true") {
          waitForData = true
          if (ExternalInterface.available) {
              ExternalInterface.addCallback("supplyData", supplyData);
              Security.allowDomain("*") // Awwww yeah!
          } else {
            trace("External interface is not available for this container.");
          }
        }
      } catch(e:*) {
      }

      if(!waitForData)
        CModule.vfs.addBackingStore(new RootFSBackingStore(), null)

      if(container) {
        container.addChild(this)
        init(null)
      } else {
        addEventListener(Event.ADDED_TO_STAGE, init)
      }
    }

    private function supplyData(value:String):void
    {
      waitForData = false
      ExternalInterface.addCallback("supplyData", null)

      var bs:InMemoryBackingStore = new InMemoryBackingStore();
      bs.addDirectory("/id1")
      bs.addFile("/id1/pak0.pak", decodeBase64(value))
      CModule.vfs.addBackingStore(bs, null)

      init(null)
    }

    /**
    * All of the real FlasCC init happens in this method
    * which is either run on startup or once the SWF has
    * been added to the stage.
    */
    protected function init(e:Event):void
    {
      if(waitForData)
        return;

      inputContainer = new Sprite()
      addChild(inputContainer)

      addEventListener(Event.ENTER_FRAME, enterFrame)
      stage.addEventListener(KeyboardEvent.KEY_DOWN, bufferKeyDown)
      stage.addEventListener(KeyboardEvent.KEY_UP, bufferKeyUp)
      stage.addEventListener(MouseEvent.MOUSE_MOVE, bufferMouseMove)
      stage.frameRate = 60
      stage.scaleMode = StageScaleMode.NO_SCALE
      graphics.lineStyle(1, 0xe0e0e0)
      graphics.drawRect(0, 0, _width, _height)
      bmd = new BitmapData(1024,768, false)
      bm = new Bitmap(bmd)
      bmr = new Rectangle(0,0,bmd.width, bmd.height)
      bmd.fillRect(bmd.rect, 0)
      inputContainer.addChild(bm)

      if(enableConsole) {
        _tf = new TextField
        _tf.multiline = true
        _tf.width = stage.stageWidth
        _tf.height = stage.stageHeight 
        inputContainer.addChild(_tf)
      }
            
      try
      {
        // This will be true if the player supports workers
        runningInWorker = CModule.canUseWorkers;

        // PlayerKernel will delegate read/write requests to the "/dev/tty"
        // file to the object specified with this API.
        CModule.vfs.console = this

        // By default we run "main" on a background worker so that
        // console updates show up in real time. Otherwise "startAsync"
        // causes main to run on the UI worker
        if(runningInWorker) {
          CModule.startBackground(this, new <String>["Quake1.swf", "-bgworker"])
        } else {
          CModule.startAsync(this)
        }
      }
      catch(e:*)
      {
        // If main gives any exceptions make sure we get a full stack trace
        // in our console
        trace(e.toString() + "\n" + e.getStackTrace().toString())
        throw e
      }
      
      vbuffer = CModule.getPublicSymbol("__avm2_vgl_argb_buffer")
      vgl_mx = CModule.getPublicSymbol("vgl_cur_mx")
      vgl_my = CModule.getPublicSymbol("vgl_cur_my")
      enginetickptr = CModule.getPublicSymbol("engineTick")
        
      initTesting()
    }

    /**
    * The callback to call when FlasCC code calls the posix exit() function. Leave null to exit silently.
    * @private
    */
    public var exitHook:Function;

    /**
    * The PlayerKernel implementation will use this function to handle
    * C process exit requests
    */
    public function exit(code:int):Boolean
    {
      // default to unhandled
      return exitHook ? exitHook(code) : false;
    }

    /**
    * The PlayerKernel implementation will use this function to handle
    * C IO write requests to the file "/dev/tty" (e.g. output from
    * printf will pass through this function). See the ISpecialFile
    * documentation for more information about the arguments and return value.
    */
    public function write(fd:int, bufPtr:int, nbyte:int, errnoPtr:int):int
    {
      var str:String = CModule.readString(bufPtr, nbyte)
      consoleWrite(str)
      return nbyte
    }

    /**
    * libVGL expects to be able to read Keyboard input from
    * file descriptor zero using normal C IO.
    */
    public function read(fd:int, bufPtr:int, nbyte:int, errnoPtr:int):int
    {
      if(fd == 0 && nbyte == 1) {
        keybytes.position = kp++
        if(keybytes.bytesAvailable) {
          CModule.write8(bufPtr, keybytes.readUnsignedByte())
          return 1
        } else {
          keybytes.length = 0
          keybytes.position = 0
          kp = 0
        }
      }
      return 0
    }

    /**
    * The PlayerKernel implementation will use this function to handle
    * C fcntl requests to the file "/dev/tty" 
    * See the ISpecialFile documentation for more information about the
    * arguments and return value.
    */
    public function fcntl(fd:int, com:int, data:int, errnoPtr:int):int
    {
      return 0
    }

    /**
    * The PlayerKernel implementation will use this function to handle
    * C ioctl requests to the file "/dev/tty" 
    * See the ISpecialFile documentation for more information about the
    * arguments and return value.
    */
    public function ioctl(fd:int, com:int, data:int, errnoPtr:int):int
    {
      return CModule.callI(CModule.getPublicSymbol("vglttyioctl"), new <int>[fd, com, data, errnoPtr]);
    }

    /**
    * Helper function that traces to the flashlog text file and also
    * displays output in the on-screen textfield console.
    */
    protected function consoleWrite(s:String):void
    {
      trace(s)
      if(enableConsole) {
        _tf.appendText(s)
        _tf.scrollV = _tf.maxScrollV
      }
    }

    public function bufferMouseMove(me:MouseEvent) {
      me.stopPropagation()
      mx = me.stageX
      my = me.stageY
    }

    public function bufferKeyDown(ke:KeyboardEvent) {
      keybytes.writeByte(int(ke.keyCode & 0x7F))
    }
    
    public function bufferKeyUp(ke:KeyboardEvent) {
      keybytes.writeByte(int(ke.keyCode | 0x80))
    }
    
    public function sndComplete(e:Event):void
    {
      sndChan.removeEventListener(Event.SOUND_COMPLETE, sndComplete)
      sndChan = snd.play()
      sndChan.addEventListener(Event.SOUND_COMPLETE, sndComplete)
    }

    public function sndData(e:SampleDataEvent):void
    {
      e.data.length = 0
      sndDataBuffer = e.data

      if(frameCount == 0)
        return

      if(engineticksoundptr == 0)
        engineticksoundptr = CModule.getPublicSymbol("engineTickSound")

      if(engineticksoundptr)
        CModule.callI(engineticksoundptr, emptyArgs)
    }

    protected function enterFrame(e:Event):void
    {
      if(runningInWorker) {
        // Background worker handles blitting
        CModule.serviceUIRequests()
        if(vbuffer == 0)
          vbuffer = CModule.getPublicSymbol("__avm2_vgl_argb_buffer")
      } else {
        CModule.write32(vgl_mx, mx)
        CModule.write32(vgl_my, my)
        CModule.callI(enginetickptr, emptyArgs)
      }

      var ram:ByteArray = CModule.ram;
      ram.position = CModule.read32(vbuffer)
      if (ram.position != 0) {
        frameCount++
        bmd.setPixels(bmr, ram)
      }

      try {
        if(!snd)
        {
          snd = new Sound()
          snd.addEventListener( SampleDataEvent.SAMPLE_DATA, sndData )
        }
        if (!sndChan)
        {
          samplePos = 0
          sndChan = snd.play()
          sndChan.addEventListener(Event.SOUND_COMPLETE, sndComplete)
        }
      } catch(e:*) { } /* in the unlikely event that we have no sound support, don't blow up */
    }

    /**
    * Provide a way to get the TextField's text.
    */
    public function get consoleText():String
    {
        var txt:String = null;

        if(_tf != null){
            txt = _tf.text;
        }
        
        return txt;
    }

    /* 
     * Copyright (C) 2012 Jean-Philippe Auclair 
     * Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php 
     * Base64 library for ActionScript 3.0. 
     * By: Jean-Philippe Auclair : http://jpauclair.net 
     * Based on article: http://jpauclair.net/2010/01/09/base64-optimized-as3-lib/
     */ 
    private static const _decodeChars:Vector.<int> = new <int>[  
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,   
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,   
            -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,   
            15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,   
            -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,   
            41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,   
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1];

    public static function decodeBase64(str:String):ByteArray  
    {  
        var c1:int;  
        var c2:int;  
        var c3:int;  
        var c4:int;  
        var i:int = 0;  
        var len:int = str.length;  
          
        var byteString:ByteArray = new ByteArray();  
        byteString.writeUTFBytes(str);  
        var outPos:int = 0;  
        while (i < len)  
        {  
            //c1  
            c1 = _decodeChars[int(byteString[i++])];  
            if (c1 == -1)  
                break;  
              
            //c2  
            c2 = _decodeChars[int(byteString[i++])];  
            if (c2 == -1)  
                break;  
              
            byteString[int(outPos++)] = (c1 << 2) | ((c2 & 0x30) >> 4);  
              
            //c3  
            c3 = byteString[int(i++)];  
            if (c3 == 61)  
            {  
                byteString.length = outPos  
                return byteString;  
            }  
              
            c3 = _decodeChars[int(c3)];  
            if (c3 == -1)  
                break;  
              
            byteString[int(outPos++)] = ((c2 & 0x0f) << 4) | ((c3 & 0x3c) >> 2);  
              
            //c4  
            c4 = byteString[int(i++)];  
            if (c4 == 61)  
            {  
                byteString.length = outPos  
                return byteString;  
            }  
              
            c4 = _decodeChars[int(c4)];  
            if (c4 == -1)  
                break;  
              
            byteString[int(outPos++)] = ((c3 & 0x03) << 6) | c4;  
        }  
        byteString.length = outPos  
        return byteString;  
    }
  }
}

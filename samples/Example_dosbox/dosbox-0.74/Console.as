package com.adobe.flascc
{
  import flash.display.Bitmap
  import flash.display.BitmapData
  import flash.display.DisplayObjectContainer;
  import flash.display.Sprite;
  import flash.display.StageScaleMode;
  import flash.events.Event;
  import flash.events.KeyboardEvent;
  import flash.events.MouseEvent;
  import flash.events.SampleDataEvent;
  import flash.geom.Rectangle
  import flash.media.Sound;
  import flash.media.SoundChannel;
  import flash.net.LocalConnection;
  import flash.net.URLRequest;
  import flash.profiler.Telemetry;
  import flash.text.TextField;
  import flash.utils.ByteArray;
  import flash.utils.getTimer;
  import flash.utils.setInterval;

  import com.adobe.flascc.vfs.InMemoryBackingStore;
  import com.adobe.flascc.vfs.ISpecialFile;
  import com.adobe.flascc.vfs.zip.*;
  
  class DSPFile implements ISpecialFile {
    var _snd:Sound;
    var _sndChan:SoundChannel;
    var _buf:ByteArray;
    const _samplePairs:int = 2048;

    public function DSPFile() {
    }
    
    public function sndData(e:SampleDataEvent):void
    {
      var samplePairs:int = _samplePairs;
      _buf.position = 0;
      while(samplePairs && _buf.bytesAvailable)
      {
        var l:Number = _buf.readShort() / 32768.0;
        var r:Number = _buf.readShort() / 32768.0;
        e.data.writeFloat(l);
        e.data.writeFloat(r);
        samplePairs--;
      }
      _buf.length = 0;
      while(samplePairs)
      {
        e.data.writeFloat(0);
        e.data.writeFloat(0);
        samplePairs--;
      }
    }
    public function fcntl(fd:int, cmd:int, data:int, errnoPtr:int):int {
      return 0;
    }
    public function ioctl(fd:int, request:int, data:int, errnoPtr:int):int {
      if(request == int(0x4004500b)) // SNDCTL_DSP_GETFMTS
      {
        //0x00000010 AFMT_S16_LE
        CModule.write32(data, 0x00000010);
        return 0;
      }
      else if(request == int(0xc0045005)) // SNDCTL_DSP_SETFMT
      {
        return (CModule.read32(data) == int(0x00000010)) ? 0 : -1;
      }
      else if(request == int(0xc0045006)) // SNDCTL_DSP_CHANNELS
      {
        CModule.write32(data, 2);
        return 0;
      }
      else if(request == int(0xc0045002)) // SNDCTL_DSP_SPEED
      {
        CModule.write32(data, 44100);
        return 0;
      }
      else if(request == int(0xc004500a)) // SNDCTL_DSP_SETFRAGMENT
      {
        CModule.write32(data, 0x2000a); // 2 1024 sized frags
        return 0;
      }
      trace("ioctl (ignored: " + request + "; returning -1)");
      return -1;
    }
    public function read(fd:int, buf:int, count:int, errnoPtr:int):int {
      trace("dsp read (ignoring; returning 0)");
      return 0;
    }
    public function write(fd:int, buf:int, count:int, errnoPtr:int):int {
      if(!_snd)
      {
        _buf = new ByteArray();
        _buf.endian = "littleEndian";
        _snd = new Sound();
        _snd.addEventListener(SampleDataEvent.SAMPLE_DATA, sndData);
        _sndChan = _snd.play();
      }
      if((count + _buf.length) > _samplePairs * 2 * 2)
      {
        throw "Sound buffer full";
      }
      if(count >= 4)
      {
        var ram:ByteArray = CModule.ram;
        _buf.writeBytes(ram, buf, count & ~3);
      }
      return count;
    }
  }

  class ZipBackingStore extends InMemoryBackingStore {
    public function ZipBackingStore(data:ByteArray)
    {
      if(data) addZip(data)
    }

    public function addZip(data:ByteArray) {
      var zip = new ZipFile(data)
      for (var i = 0; i < zip.entries.length; i++) {
        var e = zip.entries[i]
        if (e.isDirectory()) {
          addDirectory("/"+e.name.slice(0, e.name.length-1))
        } else {
          addFile("/"+e.name, zip.getInput(e))
        }
      }
    }
  }

  /**
  * A basic implementation of a console for flascc apps.
  * The PlayerKernel class delegates to this for things like read/write
  * so that console output can be displayed in a TextField on the Stage.
  */
  public class Console extends Sprite implements ISpecialFile
  {
    private static const _height:int = 768 + 100;
    private static const _width:int = 1024;

    public var mx:int = 0, my:int = 0;
    private var _tf:TextField;
    private var bm:Bitmap
    private var enableConsole:Boolean = false
    private var frameCount:int = 0;
    private var enginetickptr:int, engineticksoundptr:int
    private var inputContainer
    private var bmd:BitmapData
    private var bmr:Rectangle
    private var keybytes:ByteArray = new ByteArray()
    private var last_mx:int = 0, last_my:int = 0
    private var snd:Sound = null
    private var sndChan:SoundChannel = null
    private var vbuffer:int, vgl_mx:int, vgl_my:int, kp:int, vglkbevent:int
    private const emptyArgs:Vector.<int> = new Vector.<int>;

    /**
    * To Support the preloader case you might want to have the Console
    * act as a child of some other DisplayObjectContainer.
    */
    public function Console(container:DisplayObjectContainer = null, webfs:ByteArray = null)
    {
      CModule.rootSprite = container ? container.root : this
      
      if(CModule.runningAsWorker()) {
        return;
      }

      if(webfs)
        CModule.vfs.addBackingStore(new ZipBackingStore(webfs), null)

      CModule.vfs.addSpecialFile("/dev/dsp", new DSPFile());

      if(container) {
        container.addChild(this)
        init(null)
      } else {
        addEventListener(Event.ADDED_TO_STAGE, init)
      }
    }

    /**
    * All of the real flascc init happens in this method
    * which is either run on startup or once the SWF has
    * been added to the stage.
    */
    protected function init(e:Event):void
    {
      inputContainer = new Sprite()
      addChild(inputContainer)

      addEventListener(Event.ENTER_FRAME, enterFrame)
      setInterval(serviceUIRequests, 5);

      stage.addEventListener(KeyboardEvent.KEY_DOWN, bufferKeyDown);
      stage.addEventListener(KeyboardEvent.KEY_UP, bufferKeyUp);
      stage.addEventListener(MouseEvent.MOUSE_MOVE, bufferMouseMove);
      stage.frameRate = 60
      stage.scaleMode = StageScaleMode.NO_SCALE
      bmd = new BitmapData(1024,768, false)
      bm = new Bitmap(bmd)
      bmr = new Rectangle(0,0,bmd.width, bmd.height)
      bmd.fillRect(bmd.rect, 0);
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
        CModule.vfs.console = this

        CModule.startBackground(this,
              new <String>["dosbox", "/ROOT/RUN.BAT"],
              new <String>[])
      }
      catch(e:*)
      {
        // If main gives any exceptions make sure we get a full stack trace
        // in our console
        consoleWrite(e.toString() + "\n" + e.getStackTrace().toString())
        throw e
      }
      vbuffer = CModule.getPublicSymbol("__avm2_vgl_argb_buffer")
      vgl_mx = CModule.getPublicSymbol("vgl_cur_mx")
      vgl_my = CModule.getPublicSymbol("vgl_cur_my")
      vglkbevent = CModule.getPublicSymbol("VGLQueueKeyboardEvent")
      CModule.write32(CModule.getPublicSymbol("VGLUseUnsynchronizedIoctl"), 1)
      CModule.write32(CModule.getPublicSymbol("VGLUseKeyboardEventQueue"), 1)
    }

    /**
    * The callback to call when flascc code calls the posix exit() function. Leave null to exit silently.
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

    public function read(fd:int, bufPtr:int, nbyte:int, errnoPtr:int):int
    {
      if(fd == 0 && nbyte == 1) {
        keybytes.position = kp++
        if(keybytes.bytesAvailable) {
          CModule.write8(bufPtr, keybytes.readUnsignedByte())
        } else {
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

    public function bufferMouseMove(me:MouseEvent) {
      me.stopPropagation()
      mx = me.stageX
      my = me.stageY
    }

    var kbevent:Vector.<int> = new Vector.<int>()
    public function bufferKeyDown(ke:KeyboardEvent) {
      ke.stopPropagation()
      kbevent[0] = int(ke.keyCode & 0x7F)
      CModule.callI(vglkbevent, kbevent);
    }
    
    public function bufferKeyUp(ke:KeyboardEvent) {
      ke.stopPropagation()
      kbevent[0] = int(ke.keyCode | 0x80)
      CModule.callI(vglkbevent, kbevent);
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

    public function sndComplete(e:Event):void
    {
      sndChan.removeEventListener(Event.SOUND_COMPLETE, sndComplete);
      sndChan = snd.play();
      sndChan.addEventListener(Event.SOUND_COMPLETE, sndComplete);
    }

    protected function serviceUIRequests():void
    {
      CModule.serviceUIRequests()
    }

    /**
    * The enterFrame callback will be run once every frame. UI thunk requests should be handled
    * here by calling CModule.serviceUIRequests() (see CModule ASdocs for more information on the UI thunking functionality).
    */
    protected function enterFrame(e:Event):void
    {
      // Background worker handles blitting
      CModule.write32(vgl_mx, mx)
      CModule.write32(vgl_my, my)

      if(vbuffer == 0)
        vbuffer = CModule.getPublicSymbol("__avm2_vgl_argb_buffer")

      CModule.ram.position = CModule.read32(vbuffer)
      if (CModule.ram.position != 0) {
        frameCount++
        bmd.setPixels(bmr, CModule.ram)
      }
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
  }
}

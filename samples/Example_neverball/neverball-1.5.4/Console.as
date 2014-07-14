package com.adobe.flascc
{
  import flash.display.Bitmap;
  import flash.display.BitmapData;
  import flash.display.DisplayObjectContainer;
  import flash.display.Sprite;
  import flash.display.Stage3D;
  import flash.display.StageAlign;
  import flash.display.StageScaleMode;
  import flash.display3D.Context3D;
  import flash.display3D.Context3DProfile;
  import flash.display3D.Context3DRenderMode;
  import flash.events.AsyncErrorEvent;
  import flash.events.Event;
  import flash.events.EventDispatcher;
  import flash.events.IOErrorEvent;
  import flash.events.KeyboardEvent;
  import flash.events.MouseEvent;
  import flash.events.ProgressEvent;
  import flash.events.SampleDataEvent;
  import flash.events.SecurityErrorEvent;
  import flash.geom.Rectangle;
  import flash.media.Sound;
  import flash.media.SoundChannel;
  import flash.net.LocalConnection;
  import flash.net.URLLoader;
  import flash.net.URLLoaderDataFormat;
  import flash.net.URLRequest;
  import flash.text.TextField;
  import flash.ui.Keyboard;
  import flash.utils.ByteArray;
  import flash.utils.Endian;
  import flash.utils.getTimer;

  import com.adobe.flascc.CModule;
  import com.adobe.flascc.vfs.InMemoryBackingStore;
  import com.adobe.flascc.vfs.ISpecialFile;
  import com.adobe.flascc.vfs.zip.*;
  import GLS3D.GLAPI;

  class ZipBackingStore extends InMemoryBackingStore {
    public function ZipBackingStore()
    {
      addDirectory("/root")
      addDirectory("/root/data")
    }

    public function addZip(data:ByteArray) {
      var zip = new ZipFile(data)
      for (var i = 0; i < zip.entries.length; i++) {
        var e = zip.entries[i]
        if (e.isDirectory()) {
          addDirectory("/root/data/"+e.name)
        } else {
          addFile("/root/data/"+e.name, zip.getInput(e))
        }
      }
    }
  }

  var zfs:ZipBackingStore = new ZipBackingStore();
  public function addVFSZip(x:*) {
    if(!zfs) {
      zfs = new ZipBackingStore();
    }
    zfs.addZip(x)
  }

  /**
  * A basic implementation of a console for FlasCC apps.
  * The PlayerKernel class delegates to this for things like read/write
  * so that console output can be displayed in a TextField on the Stage.
  */
  public class Console extends Sprite implements ISpecialFile
  {
    private var vgl_mx:int, vgl_my:int, kp:int, vgl_buttons:int;
    private var mainloopTickPtr:int, soundUpdatePtr:int, audioBufferPtr:int;
    private var inputContainer
    private var keybytes:ByteArray = new ByteArray()
    private var mx:int = 0, my:int = 0, last_mx:int = 0, last_my:int = 0, button:int = 0;
    private var snd:Sound = null
    private var sndChan:SoundChannel = null
    private var s3d:Stage3D;
    private var ctx3d:Context3D;
    private var rendered:Boolean = false;
    private var inited:Boolean = false
    private const emptyVec:Vector.<int> = new Vector.<int>()

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

      if(container) {
        container.addChild(this)
        init(null)
      } else {
        addEventListener(Event.ADDED_TO_STAGE, init)
      }
    }

    /**
    * All of the real FlasCC init happens in this method
    * which is either run on startup or once the SWF has
    * been added to the stage.
    */
    protected function init(e:Event):void
    {
      inputContainer = new Sprite()
      addChild(inputContainer)

      stage.frameRate = 60;
      stage.align = StageAlign.TOP_LEFT;
      stage.scaleMode = StageScaleMode.NO_SCALE;
      
      stage.addEventListener(KeyboardEvent.KEY_DOWN, bufferKeyDown);
      stage.addEventListener(KeyboardEvent.KEY_UP, bufferKeyUp);
      stage.addEventListener(MouseEvent.MOUSE_MOVE, bufferMouseMove);
      stage.addEventListener(MouseEvent.MOUSE_DOWN, bufferMouseDown);
      stage.addEventListener(MouseEvent.MOUSE_UP, bufferMouseUp);

      try {
        stage.addEventListener(MouseEvent.RIGHT_CLICK, rightClick);
      } catch(e:*) {
        // disable the right-click menu if possible
      }
      
      s3d = stage.stage3Ds[0];
      s3d.addEventListener(Event.CONTEXT3D_CREATE, context_created);
      /*
      try {
        // If we're in FP 11.4+ this should request a constrained Stage3D context
        s3d.requestContext3D(Context3DRenderMode.AUTO, Context3DProfile.BASELINE_CONSTRAINED)
      } catch(e:*) {
        // If that failed we're in an older FP 11 player so we try for a normal Stage3D context
        s3d.requestContext3D(Context3DRenderMode.AUTO)
      }
      */
      s3d.requestContext3D(Context3DRenderMode.AUTO)
    }

    private function rightClick(e:Event):void
    {
      // no legacy right click menu! yay!
    }

    private function context_created(e:Event):void
    {
      ctx3d = s3d.context3D
      ctx3d.configureBackBuffer(stage.stageWidth, stage.stageHeight, 2, true /*enableDepthAndStencil*/ )
      ctx3d.enableErrorChecking = false;
      trace("Stage3D context: " + ctx3d.driverInfo);

      if(ctx3d.driverInfo.indexOf("Software") != -1) {
          trace("Software mode unsupported...");
          return;
      }
      
      GLAPI.init(ctx3d, null, stage);
      var gl:GLAPI = GLAPI.instance;
      gl.context.clear(0.0, 0.0, 0.0);
      gl.context.present();
      this.addEventListener(Event.ENTER_FRAME, enterFrame);
      stage.addEventListener(Event.RESIZE, stageResize);
    }
    
    private function stageResize(event:Event):void
    {
        // need to reconfigure back buffer
        ctx3d.configureBackBuffer(stage.stageWidth, stage.stageHeight, 2, true /*enableDepthAndStencil*/ )
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
      vglttyargs[0] = fd
      vglttyargs[1] = com
      vglttyargs[2] = data
      vglttyargs[3] = errnoPtr
      return CModule.callI(CModule.getPublicSymbol("vglttyioctl"), vglttyargs);
    }
    private var vglttyargs:Vector.<int> = new Vector.<int>()
    
    /**
    * Helper function that traces to the flashlog text file and also
    * displays output in the on-screen textfield console.
    */
    protected function consoleWrite(s:String):void
    {
      trace(s)
    }

    public function bufferMouseMove(me:MouseEvent) {
      me.stopPropagation()
      mx = me.stageX
      my = me.stageY
    }
    
    public function bufferMouseDown(me:MouseEvent) 
    {
      me.stopPropagation();
      mx = me.stageX;
      my = me.stageY;
      button = 1;
    }
    
    public function bufferMouseUp(me:MouseEvent) 
    {
      me.stopPropagation();
      mx = me.stageX;
      my = me.stageY;
      button = 0;
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
      CModule.callI(soundUpdatePtr, emptyVec)
      e.data.endian = Endian.LITTLE_ENDIAN
      e.data.length = 0
      var ap:int = CModule.read32(audioBufferPtr)
      //e.data.writeBytes(ram, ap, 16384);
      
      for(var i:int=0; i<16384; i+=2) {
        CModule.ram.position = ap+i;
        var s:int = CModule.ram.readShort()
        var v:Number = (s / 32768.0)
        e.data.writeFloat(v)
      }
    }

    /**
    * The enterFrame callback will be run once every frame. UI thunk requests should be handled
    * here by calling CModule.serviceUIRequests() (see CModule ASdocs for more information on the UI thunking functionality).
    */
    protected function enterFrame(e:Event):void
    {
      if(!inited) {
        inited = true
        CModule.vfs.console = this
        CModule.vfs.addBackingStore(zfs, null)

        CModule.startAsync(this, new <String>["/root/neverball.swf"])

        vgl_mx = CModule.getPublicSymbol("vgl_cur_mx")
        vgl_my = CModule.getPublicSymbol("vgl_cur_my")
        vgl_buttons = CModule.getPublicSymbol("vgl_cur_buttons")
        mainloopTickPtr = CModule.getPublicSymbol("mainLoopTick")
        soundUpdatePtr = CModule.getPublicSymbol("audio_step")
        audioBufferPtr = CModule.getPublicSymbol("audioBuffer")
      }

      CModule.serviceUIRequests()
      CModule.write32(vgl_mx, mx);
      CModule.write32(vgl_my, my);
      CModule.write32(vgl_buttons, button);

      CModule.callI(mainloopTickPtr, emptyVec);
      GLAPI.instance.context.present();

      if(!snd)
      {
        snd = new Sound();
        snd.addEventListener(SampleDataEvent.SAMPLE_DATA, sndData);
      }
      if (!sndChan)
      {
        sndChan = snd.play();
        sndChan.addEventListener(Event.SOUND_COMPLETE, sndComplete);
      }
    }
  }
}

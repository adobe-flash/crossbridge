package com.adobe.flascc
{
  import flash.display.Sprite;
  import flash.text.TextField;
  import flash.text.TextFormat;
  import flash.display.Stage;
  import flash.display.StageScaleMode;
  import flash.display.DisplayObjectContainer;
  import flash.display.Bitmap;
  import flash.display.BitmapData;
  import flash.utils.ByteArray
  import flash.net.LocalConnection;
  import flash.net.URLRequest;
  import flash.events.Event;
  import C_Run.ram;
  import com.adobe.flascc.vfs.*;
  import com.foo.bar;

  public class Console extends Sprite implements ISpecialFile
  {
    public static var current:Console;
    private var enableConsole:Boolean = true
    private var _tf:TextField;
    private var inputContainer

    private var alclogo:String = "flascc"
    private var webfs:HTTPBackingStore = null;
    private var webfs_readonly:HTTPBackingStoreReadOnly = null;

    public function Console(container:DisplayObjectContainer = null)
    {
      Console.current = this;
      if(container) {
        container.addChild(this);
        init(null);
      } else {
        addEventListener(Event.ADDED_TO_STAGE, initG);
      }
    }

    private function init(e:Event):void
    {
      inputContainer = new Sprite()
      addChild(inputContainer)
      
     if(enableConsole) {
        _tf = new TextField;
        _tf.multiline = true;
        _tf.width = stage.stageWidth;
        _tf.height = stage.stageHeight;
        _tf.setTextFormat(new TextFormat("Monaco"));
        inputContainer.addChild(_tf);
     }
  
      stage.frameRate = 30;
      stage.quality = "best";

      CModule.vfs.console = this;

      // This is the compressed read/write file system.
      CModule.vfs.addBackingStore(new RootFSBackingStore(), null);

      // This is the compressed read only file system.
      CModule.vfs.addDirectory("/compressed_readonly");
      CModule.vfs.addBackingStore(new ReadOnlyCompressedVFS(), "/compressed_readonly");

      // This is the compressed file system with a custom package name.
      CModule.vfs.addDirectory("/compressed_custompackage");
      CModule.vfs.addBackingStore(new bar(), "/compressed_custompackage");
      
      // This is the embedded file system.
      CModule.vfs.addDirectory("/embedded");
      CModule.vfs.addBackingStore(new EmbeddedVFS(), "/embedded");

      // This is the embedded read only file system.
      CModule.vfs.addDirectory("/embedded_readonly");
      CModule.vfs.addBackingStore(new EmbeddedVFSReadOnly(), "/embedded_readonly");     

      // This is the read/write LSO file system.  We aren't doing read-only LSO.
      CModule.vfs.addDirectory("/lso");
      CModule.vfs.addBackingStore(new LSOBackingStore("lso"), "/lso");
      
      // This is just a file.
      var ba:ByteArray = new ByteArray();
      ba.writeUTFBytes(alclogo);
      CModule.vfs.addFile("/flascclogo.txt", ba);

      webfs = new HTTPBackingStore();
      webfs.addEventListener(Event.COMPLETE, onHTTPComplete);

      webfs_readonly = new HTTPBackingStoreReadOnly();
      webfs_readonly.addEventListener(Event.COMPLETE, onHTTPReadOnlyComplete);
    }

    private function onHTTPComplete(e:Event):void{
      CModule.vfs.addDirectory("/webvfs");
      CModule.vfs.addBackingStore(webfs, "/webvfs");
      continueIfAllAreLoaded();
    }
    
    private function onHTTPReadOnlyComplete(e:Event):void{
      CModule.vfs.addDirectory("/webvfs_readonly");
      CModule.vfs.addBackingStore(webfs_readonly, "/webvfs_readonly");
      continueIfAllAreLoaded();     
    }

    private function continueIfAllAreLoaded():void {
      if(CModule.vfs.getDirectoryEntries( "/webvfs" ) == null){
          trace("Console onComplete(): Still waiting for webvfs.");
      }else if (CModule.vfs.getDirectoryEntries( "/webvfs_readonly") == null ){
          trace("Console onComplete(): Still waiting for webvfs_readonly.");          
      }else{
          CModule.startAsync(this);
      }
    }

    public function write(fd:int, buf:int, nbyte:int, errnoPtr:int):int
    {
      var str:String = CModule.readString(buf, nbyte);
      consoleWrite(str);
      return nbyte;
    }

    public function read(fd:int, buf:int, nbyte:int, errnoPtr:int):int
    {
      return 0
    }

    public function fcntl(fd:int, com:int, data:int, errnoPtr:int):int
    {
      return 0
    }

    public function ioctl(fd:int, com:int, data:int, errnoPtr:int):int
    {
      return 0
    }

    public function consoleWrite(s:String):void
    {
      if(enableConsole) {
        _tf.appendText(s);
        _tf.scrollV = _tf.maxScrollV
      }
    }

    public function i_exit(code:int):void
    {
      consoleWrite("\nexit code: " + code + "\n");
    }

    public function i_error(e:String):void
    {
       consoleWrite("\nexception: " + e + "\n");
    }
  }
}

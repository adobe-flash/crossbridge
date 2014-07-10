package
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
  import flash.system.System;
  import flash.system.fscommand;

  import bar.*;
  import bar.vfs.*;

  public class SwcConsole extends Sprite implements ISpecialFile
  {
    public static var current:SwcConsole;
    private var enableConsole:Boolean = true;
    private var _tf:TextField;
    private var inputContainer:Sprite;

    public function SwcConsole(container:DisplayObjectContainer = null)
    {
      SwcConsole.current = this;
      if(container) {
        container.addChild(this);
        init(null);
      } else {
        addEventListener(Event.ADDED_TO_STAGE, init);
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

      // Now we should be able to access /basic

      // If we ever do many tests, we should use the framework.
      var testName:String = "swc_vfs";
      var resultString:String = "RESULT=";

      var handle:FileHandle = CModule.vfs.getFileHandleFromPath( "/basic" );
      if( handle == null  ){
          trace( resultString + "FAIL TEST=" + testName + " MSG=Could not find file" );
      }else{
          var descriptor:int = CModule.vfs.openFile( handle );
          if( descriptor <= -1 ){
               trace( resultString + "FAIL TEST=" + testName + " MSG=Could not open file" );
          }else{
               if( handle.bytes.readUTFBytes( 5 ) != "abcde" ){
                   trace( resultString + "FAIL TEST=" + testName + " MSG=Data was not correct" );
               }else{
                   trace( resultString + "PASS TEST=" + testName );
               }
          }
      }

      trace( "vfs_test_done" );

      fscommand("quit");
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

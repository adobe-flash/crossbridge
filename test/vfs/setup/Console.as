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

  public class Console extends Sprite implements ISpecialFile
  {
    public static var current:Console;
    private var enableConsole:Boolean = true
    private var _tf:TextField;
    private var inputContainer

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
      
     stage.frameRate = 60;

     if(enableConsole) {
        _tf = new TextField;
        _tf.multiline = true;
        _tf.width = stage.stageWidth;
        _tf.height = stage.stageHeight;
        inputContainer.addChild(_tf);
     }
  
      
      //stage.quality = "best";

      CModule.vfs.console = this;
      CModule.vfs.addBackingStore(new RootFSBackingStore(), null)
      CModule.vfs.addDirectory("/lso")
      CModule.vfs.addBackingStore(new LSOBackingStore("lso"), "/lso")
      CModule.startAsync( this );
     }

    private function onComplete(e:Event):void {
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
      trace(s);
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

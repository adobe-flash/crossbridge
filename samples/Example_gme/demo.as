// An ActionScript version of demo/basics.c

package
{
  import flash.display.Sprite;
  import flash.text.TextField;
  import flash.events.Event;
  import sample.libgme.CModule;
  import sample.libgme.vfs.RootFSBackingStore;

  public class demo extends Sprite
  {
    public function demo()
    {
      CModule.vfs.addBackingStore(new RootFSBackingStore(), null);
      addEventListener(Event.ADDED_TO_STAGE, initCode);
    }

    private function doDemo():void
    {
      const sampleRate:int = 44100;
      var musicEmuPtr:int = CModule.malloc(4);
      handleError(libgme.gme_open_file("test.nsf", musicEmuPtr, sampleRate));
      var musicEmu:int = CModule.read32(musicEmuPtr);
      handleError(libgme.gme_start_track(musicEmu, 0));

      libgme.wave_open(sampleRate, "out.wav");
      libgme.wave_enable_stereo();

      const bufSize:int = 1024;
      var bufPtr:int = CModule.malloc(bufSize);
      while (libgme.gme_tell(musicEmu) < (10 * 1000)) {
        handleError(libgme.gme_play(musicEmu, bufSize, bufPtr));
        libgme.wave_write(bufPtr, bufSize);
      }

      libgme.gme_delete(musicEmu);
      libgme.wave_close();
      CModule.free(bufPtr);
      CModule.free(musicEmuPtr);

    }

    private function handleError(err:String):void
    {
      if (err) {
        throw err;
      }
    }
 
    public function initCode(e:Event):void
    {
      CModule.startAsync(this);
      
      var tf:TextField = new TextField;
      tf.multiline = true;
      tf.width = stage.stageWidth;
      tf.height = stage.stageHeight;
      addChild(tf);

      try {
        doDemo();
        tf.appendText("done!");
      } catch (e:*) {
        tf.appendText("error: " + e);
      }
    }
  }
}

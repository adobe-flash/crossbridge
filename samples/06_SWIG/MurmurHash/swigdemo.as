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
package
{
  import flash.display.Sprite;
  import flash.text.TextField;
  import flash.events.Event;
  import sample.MurmurHash.CModule;
  import MurmurHash;

  public class swigdemo extends Sprite
  {
    public function swigdemo()
    {
      addEventListener(Event.ADDED_TO_STAGE, initCode);
    }
 
    public function initCode(e:Event):void
    {
      CModule.startAsync(this)
      
      var tf:TextField = new TextField
      tf.multiline = true
      tf.width = stage.stageWidth
      tf.height = stage.stageHeight
      addChild(tf)

      var words:Array = [
        "foo",
        "bar",
        "waz"
      ]

      var seed:int = 42
      var resultptr:int = CModule.malloc(4)

      for each (var word:String in words) {
        var strptr:int = CModule.mallocString(word)
        MurmurHash.MurmurHash3_x86_32(strptr, word.length, seed, resultptr)
        var s:String = "hash of '" + word + "' is: " + CModule.read32(resultptr) + "\n"
        tf.appendText(s)
        trace(s)
        CModule.free(strptr)
      }
    }
  }
}

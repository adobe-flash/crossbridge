package
{
  import flash.display.Sprite;
  import flash.text.TextField;
  import flash.events.Event;
  import sample.MultipleSWC1.CModule;
  import sample.MultipleSWC1.multipleSWC1;
  import sample.MultipleSWC2.multipleSWC2;
  import sample.MultipleSWC3.multipleSWC3;

  public class multipleSWC extends Sprite
  {
    public function multipleSWC()
    {
      addEventListener(Event.ADDED_TO_STAGE, initCode);
    }
 
    public function initCode(e:Event):void
    {
      CModule.startAsync(this);
      
      var tf:TextField = new TextField();
      tf.multiline = true;
      tf.width = stage.stageWidth;
      tf.height = stage.stageHeight;
      addChild(tf);

      var a:uint = multipleSWC1("test");
      var b:uint = multipleSWC2("test");
      var c:uint = multipleSWC3("test");
      
      var output:String =  a + "\n" + b + "\n" + c + "\n";
      
      tf.appendText(output);
	  trace(output);
    
    }
  }
}

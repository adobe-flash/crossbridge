package c64.events
{
	import flash.events.Event;

	public class FrameRateInfoEvent extends Event
	{
		public var fps:Number;
		public var frameTime:Number;
		
		public function FrameRateInfoEvent(type:String, frameTime:Number)
		{
			super(type);
			this.fps = Math.round(10000 / frameTime) / 10;
			this.frameTime = frameTime;
		}
		
		override public function clone():Event {
			return new FrameRateInfoEvent(type, frameTime);
		}
	}
}

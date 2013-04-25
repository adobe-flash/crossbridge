package c64.events
{
	import flash.events.Event;

	public class DebuggerEvent extends Event
	{
		public var breakpointType:uint;
		
		public function DebuggerEvent(type:String, breakpointType:uint = 0)
		{
			super(type);
			this.breakpointType = breakpointType;
		}
		
		override public function clone():Event {
			return new DebuggerEvent(type, breakpointType);
		}
	}
}

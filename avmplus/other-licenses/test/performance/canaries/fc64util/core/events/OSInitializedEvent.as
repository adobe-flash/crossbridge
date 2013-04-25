package core.events
{
	import flash.events.Event;

	public class OSInitializedEvent extends Event
	{
		public var pc:uint;
		
		public function OSInitializedEvent(type:String, pc:uint)
		{
			super(type);
			this.pc = pc;
		}
		
		override public function clone():Event {
			return new OSInitializedEvent(type, pc);
		}
	}
}

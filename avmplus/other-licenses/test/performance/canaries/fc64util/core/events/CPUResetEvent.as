package core.events
{
	import flash.events.Event;

	public class CPUResetEvent extends Event
	{
		public var pcOld:uint;
		public var pcNew:uint;
		
		public function CPUResetEvent(type:String, pcOld:uint, pcNew:uint)
		{
			super(type);
			this.pcOld = pcOld;
			this.pcNew = pcNew;
		}
		
		override public function clone():Event {
			return new CPUResetEvent(type, pcOld, pcNew);
		}
	}
}

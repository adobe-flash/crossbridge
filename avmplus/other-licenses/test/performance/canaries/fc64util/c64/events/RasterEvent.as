package c64.events
{
	import flash.events.Event;

	public class RasterEvent extends Event
	{
		public var raster:uint;
		
		public function RasterEvent(type:String, raster:uint)
		{
			super(type);
			this.raster = raster;
		}
		
		override public function clone():Event {
			return new DebuggerEvent(type, raster);
		}
	}
}
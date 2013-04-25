package core.exceptions
{
	import core.misc.Convert;
	
	public class BreakpointException extends Error
	{
		public var address:uint;
		public var type:uint;
		public var cyclesConsumed:uint;
		
		public function BreakpointException(message:String, address:uint, type:uint, cyclesConsumed:uint)
		{
			super(message);
			this.address = address;
			this.type = type;
			this.cyclesConsumed = cyclesConsumed;
		}
	}
}
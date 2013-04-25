package c64.memory.io
{
	public class VICSpriteInfo
	{
		public var x:int;
		public var y:int;
		public var color:uint;
		public var enabled:Boolean;
		public var foreground:Boolean;
		public var multicolor:Boolean;
		public var expandVertical:Boolean;
		public var expandHorizontal:Boolean;
		public var collisionSpriteSprite:Boolean;
		public var collisionSpriteForeground:Boolean;
		
		public function VICSpriteInfo(color:uint) {
			x = y = 0;
			enabled = false;
			foreground = true;
			multicolor = false;
			expandVertical = false;
			expandHorizontal = false;
			collisionSpriteSprite = false;
			collisionSpriteForeground = false;
			this.color = color;
		}
	}
}
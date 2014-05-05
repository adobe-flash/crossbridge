package com.renaun.game
{
import flash.display.Sprite;

public class MovableEntity extends Sprite
{
	public function MovableEntity(size:int = 10, type:String = "enemy")
	{
		this.size = size;
		graphics.beginFill( ( (type == "enemy") ? 0xCC0000 : 0x3333AA) , 0.8);
		graphics.lineStyle(2, 0x222222, 0.8);
		graphics.drawCircle(size/2, size/2, size/2);
		graphics.endFill();
	}

	protected var size:int = 10;

	public var tileX:int = 0;
	public var tileY:int = 0;
	public var currentPath:Array;
	
	public function move(tileX:int, tileY:int):void
	{
		this.tileX = tileX;
		this.tileY = tileY;
		x = tileX * size;
		y = tileY * size;
	}
	
	public function moveByPath():void
	{
		if (!currentPath || currentPath.length == 0)
			return;
		
		var newX:int = currentPath.shift(); 
		var newY:int = currentPath.shift();
		move( newX, newY);
	}
}
}
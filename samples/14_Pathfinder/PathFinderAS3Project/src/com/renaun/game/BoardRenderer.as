package com.renaun.game
{
import flash.display.Sprite;
import flash.utils.ByteArray;
import com.renaun.PathFinder;

public class BoardRenderer extends Sprite
{
	public function BoardRenderer(pather:PathFinder, tileSize:int = 20)
	{
		this.pather = pather;
		this.tileSize = tileSize;
		render();
	}
	
	protected var pather:PathFinder;
	protected var tileSize:int;
	
	private function render():void
	{
		var mapBytes:ByteArray = pather.getMap();
		graphics.clear();
		
		for (var i:int = 0; i < pather.mapRowSize; i++) 
			for (var j:int = 0; j < pather.mapColSize; j++)
				drawTile(mapBytes.readByte(), j, i);
	}
	
	private function drawTile(type:int, x:int, y:int):void
	{
		var color:uint = (type == 1) ? 0xAAAAAA : 0x663333;
		graphics.beginFill(color);
		graphics.lineStyle(1, 0x222222, 0.5);
		graphics.drawRect(x*tileSize, y*tileSize, tileSize, tileSize);
		graphics.endFill();
	}
	
	public function drawPath(path:Array):void
	{
		graphics.lineStyle(tileSize/3, 0xff0000);
		for (var i:int = 0; i < path.length/2; i++)
		{
			if (i == 0)
				graphics.moveTo( (path[i*2]*tileSize)+(tileSize/2), (path[(i*2)+1]*tileSize)+(tileSize/2));
			graphics.lineTo( (path[i*2]*tileSize)+(tileSize/2), (path[(i*2)+1]*tileSize)+(tileSize/2));
		}
	}
}
}
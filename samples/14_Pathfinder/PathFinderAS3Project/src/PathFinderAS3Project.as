package
{
import com.renaun.PathFinder;
import com.renaun.flascc.CModule;
import com.renaun.flascc.ram;
import com.renaun.game.BoardRenderer;
import com.renaun.game.MovableEntity;

import flash.display.Sprite;
import flash.display.StageAlign;
import flash.display.StageScaleMode;
import flash.events.Event;
import flash.events.MouseEvent;
import flash.utils.ByteArray;

[SWF(frameRate="60", width="640" height="640", backgroundColor="#336699")]
public class PathFinderAS3Project extends Sprite
{
	public static const TILE_SIZE:int = 10;
	public static const TIME_SLICE_GUY:int = 6;
	public static const TIME_SLICE_ENEMY:int = 30;
	
	private var timeSlice:int = 0;
	private var validStartTiles:Array;
	private var mapCols:int;
	private var mapRows:int;
	private var theGuy:MovableEntity;
	private var enemies:Array = [];
	private var lastX:int = -1;
	private var lastY:int = -1;
	
	private var pf:PathFinder;
	
	public function PathFinderAS3Project()
	{
		addEventListener(Event.ADDED_TO_STAGE,  addedToStageHandler);
	}
	
	protected function addedToStageHandler(event:Event):void
	{
		stage.align = StageAlign.TOP_LEFT;
		stage.scaleMode = StageScaleMode.NO_SCALE;
		
		CModule.startAsync(this);
		
		// Generate random map and keep track of valid start tiles
		var map:ByteArray = new ByteArray();
		validStartTiles = [];
		mapCols = 60;
		mapRows = 60;
		var temp:int = 0;
		for (var i:int = 0; i < mapCols * mapRows; i++) 
		{
			temp = (int(Math.random()*0xffff) % 2 == 1) ? 0 : 1;
			if (temp == 1) validStartTiles.push(i);
			map.writeByte(temp);
		}
		
		pf = new PathFinder(map, mapCols, mapRows);
		
		var debugRender:BoardRenderer = new BoardRenderer(pf, PathFinderAS3Project.TILE_SIZE);
		addChildAt(debugRender,0);
		
		// Make Blue and Red Circles
		for (var k:int = 0; k < 60; k++) 		
			createEntities();
		createEntities("main");
		
		// Set up event methods for run loop and mouse down
		stage.addEventListener(MouseEvent.MOUSE_DOWN, mouseDownHandler);
		stage.addEventListener(Event.ENTER_FRAME, enterFrameHandler);
	}
	
	private function createEntities(type:String = "enemy"):void
	{
		var guy:MovableEntity = new MovableEntity(TILE_SIZE, type);
		var startIndex:int = validStartTiles[int(Math.random() * validStartTiles.length)];
		var tileY:int = startIndex / mapCols;
		guy.move(startIndex - (mapCols * tileY), tileY);
		if (type == "enemy")
			enemies.push(guy);
		else
			theGuy = guy;
		addChild(guy);
	}
	
	protected function mouseDownHandler(event:MouseEvent):void
	{
		var tileX:int = int(event.stageX / TILE_SIZE);
		var tileY:int = int(event.stageY / TILE_SIZE);
		if (tileX < mapCols && tileY < mapRows)
			theGuy.currentPath = pf.getPath(theGuy.tileX, theGuy.tileY, tileX, tileY);
	}
	
	protected function enterFrameHandler(event:Event):void
	{
		var i:int;
		if (timeSlice % TIME_SLICE_GUY == 0)
			theGuy.moveByPath();
		// Has The guy changed locations? if so re do the paths
		if (lastX != theGuy.tileX || lastY != theGuy.tileY)
		{
			lastX = theGuy.tileX;
			lastY = theGuy.tileY;
			for (i = 0; i < enemies.length; i++) 
				enemies[i].currentPath = pf.getPath(enemies[i].tileX, enemies[i].tileY, theGuy.tileX, theGuy.tileY);
		}
		if (timeSlice % TIME_SLICE_ENEMY == 0)
		{
			var hits:int = 0;
			for (i = 0; i < enemies.length; i++) 
			{
				enemies[i].moveByPath();
				if (enemies[i].tileX == theGuy.tileX && enemies[i].tileY == theGuy.tileY)
					hits++;
			}
			if (hits > 5) restartGame();
		}
		timeSlice++;
		if (timeSlice > 360)
			timeSlice = 0;
	}
	protected function restartGame():void
	{
		timeSlice = 0;
		var startIndex:int = validStartTiles[int(Math.random() * validStartTiles.length)];
		var tileY:int = startIndex / mapCols;
		theGuy.move(startIndex - (mapCols * tileY), tileY);
		for (var i:int = 0; i < enemies.length; i++) 
		{
			startIndex = validStartTiles[int(Math.random() * validStartTiles.length)];
			tileY = startIndex / mapCols;
			enemies[i].move(startIndex - (mapCols * tileY), tileY);
		}
	}
}
}
package com.renaun 
{
import com.renaun.flascc_interface.*;

import com.renaun.flascc.CModule;
import com.renaun.flascc.ram;

import flash.utils.ByteArray;

public class PathFinder
{
	public function PathFinder(map:ByteArray, colSize:int, rowSize:int):void
	{
		objectPtr = flascc_createPather();
		mapColSize = colSize;
		mapRowSize = rowSize;
		if (map.length != colSize * rowSize)
			throw new Error("Map size doesn't equal col * row size");
		// Setup bytes to be written to Flascc memory
		map.position = 0;
		mapByteArrayPtr = CModule.malloc(map.length);
		CModule.writeBytes(mapByteArrayPtr, map.length, map);
		flascc_setMap(objectPtr, mapByteArrayPtr, colSize, rowSize);
	}
	
	public var mapColSize:int;
	public var mapRowSize:int;
	private var objectPtr:int;
	public var mapByteArrayPtr:int;
	
	public function getMap():ByteArray
	{
		var mapBytes:ByteArray = new ByteArray();
		ram.position = mapByteArrayPtr;
		ram.readBytes(mapBytes, 0, mapColSize * mapRowSize);
		return mapBytes;
	}
	
	public function getPath(sx:int, sy:int, nx:int, ny:int):Array
	{
		var pathPtr:int = flascc_getPath(objectPtr, sx, sy, nx, ny);
		
		ram.position = pathPtr;
		var result:int = ram.readShort();
		var size:int = ram.readShort();
		var pathXYs:Array = [];
		if (size > 0)
		{
			ram.readShort();
			ram.readShort();
			for (var j:int = 1; j < size; j++) 
			{
				pathXYs.push(ram.readShort());
				pathXYs.push(ram.readShort());
			}
		}
		//CModule.free(pathPtr);
		return pathXYs;
	}
	
	public function destroy():void 
	{
		CModule.free(mapByteArrayPtr);
		flascc_deletePather(objectPtr);
	}
	
}
}
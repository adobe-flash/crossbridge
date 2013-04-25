/*
 * Copyright notice
 *
 * (c) 2005-2006 Darron Schall, Claus Wahlers.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

package c64.memory.io
{
	import core.cpu.CPU6502;
	import flash.display.DisplayObject;
	import flash.events.KeyboardEvent;
	import flash.ui.Keyboard;
	import flash.utils.ByteArray;
	import core.misc.Convert;
	
	public class Keyboard
	{
		/** Reference to the cpu so we can trigger NMI */
		private var cpu:CPU6502;
		
		/** 64 bit mask indicating what keys are down */
		private var keyBits:Array; // of 2 int values
		
		/** 
		 * Map of the key to it's value in the matrix: 
		 * http://sta.c64.org/cbm64kbdlay.html
		 */
		private var keyMatrixLocations:Array; // Array of 2 int values (row, col)
		 
		private var keyMaskColumns:ByteArray;

		private var joystickMask:uint;

		private var shiftDown:Boolean;

		
		/**
		 * Constructor
		 */
		public function Keyboard()
		{
			keyMaskColumns = new ByteArray();
			keyMaskColumns.length = 8;
			
			joystickMask = 0;
			
			initializeKeyMatrixLocations();
		}
		
		/**
		 * Initialization
		 * 
		 * @param cpu The CPU that an NMI can be triggered on
		 * @param stage The area to listen to for key press/release events
		 */
		public function initialize( cpu:CPU6502, listenerTarget:DisplayObject ):void 
		{
			this.cpu = cpu;
			listenerTarget.addEventListener( KeyboardEvent.KEY_DOWN, keyDown );
			listenerTarget.addEventListener( KeyboardEvent.KEY_UP, keyUp );
		}

		/**
		 * Returns a byte value representing the rows in the keyboard
		 * matrix with pressed keys
		 * 
		 * @param columns Byte value representing the selected columns
		 */
		public function getRows( columns:int ):int 
		{
			var rows:int = 0x00;
			
			// Loop over all of the columns and add in the rows that are set in that column
			// if the column is one we need to check
			rows |= ( columns & 0x01 ) == 0 ? keyMaskColumns[ 0 ] : 0x00;
			rows |= ( columns & 0x02 ) == 0 ? keyMaskColumns[ 1 ] : 0x00;
			rows |= ( columns & 0x04 ) == 0 ? keyMaskColumns[ 2 ] : 0x00;
			rows |= ( columns & 0x08 ) == 0 ? keyMaskColumns[ 3 ] : 0x00;
			rows |= ( columns & 0x10 ) == 0 ? keyMaskColumns[ 4 ] : 0x00;
			rows |= ( columns & 0x20 ) == 0 ? keyMaskColumns[ 5 ] : 0x00;
			rows |= ( columns & 0x40 ) == 0 ? keyMaskColumns[ 6 ] : 0x00;
			rows |= ( columns & 0x80 ) == 0 ? keyMaskColumns[ 7 ] : 0x00;
			
			rows |= joystickMask;
			
			return ~rows & 0xFF;
		}

		public function getJoystick2():int 
		{
			return ~joystickMask & 0xFF;
		}

		private function initializeKeyMatrixLocations():void
		{
			keyMatrixLocations = new Array();
			// The matrix needs to be flipped here.. the first value is
			// the column and the second is the row.  When they are read
			// as (row,col) it seems to work correctly.
			keyMatrixLocations[ 0x41 ] = [ 2, 1 ]; // A
			keyMatrixLocations[ 0x42 ] = [ 4, 3 ]; // B
			keyMatrixLocations[ 0x43 ] = [ 4, 2 ]; // C
			keyMatrixLocations[ 0x44 ] = [ 2, 2 ]; // D
			keyMatrixLocations[ 0x45 ] = [ 6, 1 ]; // E
			keyMatrixLocations[ 0x46 ] = [ 5, 2 ]; // F
			keyMatrixLocations[ 0x47 ] = [ 2, 3 ]; // G
			keyMatrixLocations[ 0x48 ] = [ 5, 3 ]; // H
			keyMatrixLocations[ 0x49 ] = [ 1, 4 ]; // I
			keyMatrixLocations[ 0x4A ] = [ 2, 4 ]; // J
			keyMatrixLocations[ 0x4B ] = [ 5, 4 ]; // K
			keyMatrixLocations[ 0x4C ] = [ 2, 5 ]; // L
			keyMatrixLocations[ 0x4D ] = [ 4, 4 ]; // M
			keyMatrixLocations[ 0x4E ] = [ 7, 4 ]; // N
			keyMatrixLocations[ 0x4F ] = [ 6, 4 ]; // O
			keyMatrixLocations[ 0x50 ] = [ 1, 5 ]; // P
			keyMatrixLocations[ 0x51 ] = [ 6, 7 ]; // Q
			keyMatrixLocations[ 0x52 ] = [ 1, 2 ]; // R
			keyMatrixLocations[ 0x53 ] = [ 5, 1 ]; // S
			keyMatrixLocations[ 0x54 ] = [ 6, 2 ]; // T
			keyMatrixLocations[ 0x55 ] = [ 6, 3 ]; // U
			keyMatrixLocations[ 0x56 ] = [ 7, 3 ]; // V
			keyMatrixLocations[ 0x57 ] = [ 1, 1 ]; // W
			keyMatrixLocations[ 0x58 ] = [ 7, 2 ]; // X
			keyMatrixLocations[ 0x59 ] = [ 1, 3 ]; // Y
			keyMatrixLocations[ 0x5A ] = [ 4, 1 ]; // Z
			
			keyMatrixLocations[ 0x30 ] = [ 3, 4 ]; // 0
			keyMatrixLocations[ 0x31 ] = [ 0, 7 ]; // 1
			keyMatrixLocations[ 0x32 ] = [ 3, 7 ]; // 2
			keyMatrixLocations[ 0x33 ] = [ 0, 1 ]; // 3
			keyMatrixLocations[ 0x34 ] = [ 3, 1 ]; // 4
			keyMatrixLocations[ 0x35 ] = [ 0, 2 ]; // 5
			keyMatrixLocations[ 0x36 ] = [ 3, 2 ]; // 6
			keyMatrixLocations[ 0x37 ] = [ 0, 3 ]; // 7
			keyMatrixLocations[ 0x38 ] = [ 3, 3 ]; // 8
			keyMatrixLocations[ 0x39 ] = [ 0, 4 ]; // 9
						
			keyMatrixLocations[ 0xDE ] = [ 5, 5 ]; // : (mapped on '~' key)
			keyMatrixLocations[ 0xBA ] = [ 2, 6 ]; // ;
			keyMatrixLocations[ 0xBB ] = [ 5, 6 ]; // =
			keyMatrixLocations[ 0xBC ] = [ 7, 5 ]; // ,
			keyMatrixLocations[ 0xBD ] = [ 3, 5 ]; // -
			keyMatrixLocations[ 0xBE ] = [ 4, 5 ]; // .
			keyMatrixLocations[ 0xBF ] = [ 7, 6 ]; // /
			
			//keyMatrixLocations[ 0x42 ] = [ 6, 5 ]; // @
			keyMatrixLocations[ 0xDD ] = [ 0, 5 ]; // + (mapped on '[')
			//keyMatrixLocations[ 0x5C ] = [ 0, 6 ]; // \
			keyMatrixLocations[ 0xDC ] = [ 1, 6 ]; // * (mapped on ']')
			
			keyMatrixLocations[ 0x12 ] = [ 5, 7 ]; // ALT
			keyMatrixLocations[ flash.ui.Keyboard.CONTROL ] = [ 2, 7 ];
			keyMatrixLocations[ flash.ui.Keyboard.HOME ] = [ 3, 6 ];
			keyMatrixLocations[ flash.ui.Keyboard.ENTER ] = [ 1, 0 ];
			keyMatrixLocations[ flash.ui.Keyboard.DELETE ] = [ 0, 0 ];
			keyMatrixLocations[ flash.ui.Keyboard.BACKSPACE ] = [ 0, 0 ];
			keyMatrixLocations[ flash.ui.Keyboard.ESCAPE ] = [ 7, 7 ];
			keyMatrixLocations[ flash.ui.Keyboard.SPACE ] = [ 4, 7 ];
			keyMatrixLocations[ flash.ui.Keyboard.DOWN ] = [ 7, 0 ];
			keyMatrixLocations[ flash.ui.Keyboard.RIGHT ] = [ 2, 0 ];
			keyMatrixLocations[ flash.ui.Keyboard.SHIFT ] = [ 7, 1 ]; // left shift
			//keyMatrixLocations[ flash.ui.Keyboard.SHIFT ] = [ 4, 6 ]; // right shift
			
			keyMatrixLocations[ flash.ui.Keyboard.F1 ] = [ 4, 0 ];
			keyMatrixLocations[ flash.ui.Keyboard.F3 ] = [ 5, 0 ];
			keyMatrixLocations[ flash.ui.Keyboard.F5 ] = [ 6, 0 ];
			keyMatrixLocations[ flash.ui.Keyboard.F7 ] = [ 3, 0 ];
		}
		
		private function keyDown( event:KeyboardEvent ):void 
		{
			var keyCode:int = event.keyCode;

			switch(keyCode)
			{
				case flash.ui.Keyboard.PAGE_UP:
					cpu.NMI();
					break;
				case flash.ui.Keyboard.SHIFT:
					shiftDown = true;
					break;
				case flash.ui.Keyboard.NUMPAD_8:
					joystickMask |= 0x01;
					break;
				case flash.ui.Keyboard.NUMPAD_2:
					joystickMask |= 0x02;
					break;
				case flash.ui.Keyboard.NUMPAD_4:
					joystickMask |= 0x04;
					break;
				case flash.ui.Keyboard.NUMPAD_6:
					joystickMask |= 0x08;
					break;
				case flash.ui.Keyboard.SPACE:
					joystickMask |= 0x10;
					break;
			}			
			
			if ( keyMatrixLocations[ keyCode ] != null ) 
			{
				var keyMatrixItem:Array = keyMatrixLocations[ keyCode ];
	
				// Get the row/column location of the key in the matrix
				var row:int = keyMatrixItem[ 0 ];
				var column:int = keyMatrixItem[ 1 ];
				
				// Mark the row value as being pressed for the column
				keyMaskColumns[ column ] |= 1 << row;
			}
		}
		
		private function keyUp( event:KeyboardEvent ):void 
		{
			var keyCode:int = event.keyCode;
			
			switch(keyCode)
			{
				case flash.ui.Keyboard.SHIFT:
					shiftDown = false;
					break;
				case flash.ui.Keyboard.NUMPAD_8:
					joystickMask &= ~0x01;
					break;
				case flash.ui.Keyboard.NUMPAD_2:
					joystickMask &= ~0x02;
					break;
				case flash.ui.Keyboard.NUMPAD_4:
					joystickMask &= ~0x04;
					break;
				case flash.ui.Keyboard.NUMPAD_6:
					joystickMask &= ~0x08;
					break;
				case flash.ui.Keyboard.SPACE:
					joystickMask &= ~0x10;
					break;
			}			
			
			if ( keyMatrixLocations[ keyCode ] != null ) 
			{
				// Get the row/column location of the key in the matrix
				var row:int = keyMatrixLocations[ keyCode ][ 0 ];
				var column:int = keyMatrixLocations[ keyCode ][ 1 ];
				
				// Mark the row value as being pressed for the column
				keyMaskColumns[ column ] &= ~( 1 << row );
			}
		}
	}
}

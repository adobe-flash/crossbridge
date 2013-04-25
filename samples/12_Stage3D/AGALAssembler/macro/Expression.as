/*
** ADOBE SYSTEMS INCORPORATED
** Copyright 2012 Adobe Systems Incorporated
** All Rights Reserved.
**
** NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the
** terms of the Adobe license agreement accompanying it.  If you have received this file from a
** source other than Adobe, then your use, modification, or distribution of it requires the prior
** written permission of Adobe.
*/
package com.adobe.utils.macro
{
	import com.adobe.utils.macro.VM;
	
	public class Expression
	{
		public function print( depth:int ):void { trace( "top" ); }
		public function exec( vm:VM ):void {
			trace( "WTF" );
		}
		
		protected function spaces( depth:int ):String
		{
			// Must be a clever way to do this...
			var str:String = "";
			for( var i:int=0; i<depth; ++i ) {
				str += "  ";
			}
			return str;
		}
	}
}





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
	internal class NumberExpression extends Expression
	{
		public function NumberExpression( v:Number ) {
			value = v;
		}
		private var value:Number;
		override public function print( depth:int ):void { trace( spaces( depth ) + "number=" + value ); }
		override public function exec( vm:VM ):void {
			if ( AGALPreAssembler.TRACE_VM ) {
				trace( "::NumberExpression push " + value );
			}
			if ( isNaN( value ) ) throw new Error( "Pushing NaN to stack" );
			vm.stack.push( value );
		}
	}
}
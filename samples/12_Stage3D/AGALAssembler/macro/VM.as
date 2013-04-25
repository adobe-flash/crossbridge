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
	import flash.utils.Dictionary;
	
	public class VM
	{
		public var vars:flash.utils.Dictionary = new flash.utils.Dictionary();
		public var stack:Array = new Array();
		
		public function pushIf():void	
		{ 
			m_ifIsTrue.push( false );
			m_ifWasTrue.push( false );
		}
		
		public function popEndif():void
		{ 
			m_ifIsTrue.pop();
			m_ifWasTrue.pop();
		}
		
		public function setIf( value:Number ):void
		{
			m_ifIsTrue[ m_ifIsTrue.length-1 ]  = (value != 0);
			m_ifWasTrue[ m_ifIsTrue.length-1 ] = (value != 0);
		}
		
		public function ifWasTrue():Boolean
		{
			return m_ifWasTrue[ m_ifIsTrue.length-1 ];
		}
		
		public function ifIsTrue():Boolean
		{
			if ( m_ifIsTrue.length == 0 )
				return true;
			
			// All ifs on the stack must be true for current true.
			for( var i:int=0; i<m_ifIsTrue.length; ++i ) {
				if ( !m_ifIsTrue[i] ) {
					return false;
				}
			}
			return true;
		}
		
		private var m_ifIsTrue:Vector.<Boolean> = new Vector.<Boolean>();
		private var m_ifWasTrue:Vector.<Boolean> = new Vector.<Boolean>();
	}
}
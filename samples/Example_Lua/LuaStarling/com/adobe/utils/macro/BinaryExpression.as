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
	internal class BinaryExpression extends com.adobe.utils.macro.Expression
	{
		public var op:String;
		public var left:Expression;
		public var right:Expression;
		override public function print( depth:int ):void { 
			if ( AGALPreAssembler.TRACE_VM ) {
				trace( spaces( depth ) + "binary op " + op );
			}
			left.print( depth+1 );
			right.print( depth+1 );
		}
		
		override public function exec( vm:VM ):void {
			var varLeft:Number = Number.NaN;
			var varRight:Number = Number.NaN;
			
			left.exec( vm );
			varLeft = vm.stack.pop();
			right.exec( vm );
			varRight = vm.stack.pop();
			
			if ( isNaN( varLeft ) )  throw new Error( "Left side of binary expression (" + op + ") is NaN" );
			if ( isNaN( varRight ) ) throw new Error( "Right side of binary expression (" + op + ") is NaN" );
			
			switch( op ) {
				case "*":
					vm.stack.push( varLeft * varRight );
					break;
				case "/":
					vm.stack.push( varLeft / varRight );
					break;
				case "+":
					vm.stack.push( varLeft + varRight );
					break;
				case "-":
					vm.stack.push( varLeft - varRight );
					break;
				case ">":
					vm.stack.push( (varLeft > varRight) ? 1 : 0 );
					break;
				case "<":
					vm.stack.push( (varLeft < varRight) ? 1 : 0 );
					break;
				case ">=":
					vm.stack.push( (varLeft >= varRight) ? 1 : 0 );
					break;
				case ">=":
					vm.stack.push( (varLeft <= varRight) ? 1 : 0 );
					break;			
				case "==":
					vm.stack.push( (varLeft==varRight) ? 1 : 0 );
					break;
				case "!=":
					vm.stack.push( (varLeft!=varRight) ? 1 : 0 );
					break;
				case "&&":
					vm.stack.push( (Boolean(varLeft) && Boolean(varRight)) ? 1 : 0 );
					break;
				case "||":
					vm.stack.push( (Boolean(varLeft) || Boolean(varRight)) ? 1 : 0 );
					break;
				
				default:
					throw new Error( "unimplemented BinaryExpression exec" );
					break;
			}
			if ( AGALPreAssembler.TRACE_VM ) {
				trace( "::BinaryExpression op" + op + " left=" + varLeft + " right=" +varRight + " push " + vm.stack[vm.stack.length-1] );
			}
		}
	}
}
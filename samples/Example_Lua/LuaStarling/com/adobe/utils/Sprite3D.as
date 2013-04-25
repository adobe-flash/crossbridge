/*
Copyright (c) 2011, Adobe Systems Incorporated
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are
met:

* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the 
documentation and/or other materials provided with the distribution.

* Neither the name of Adobe Systems Incorporated nor the names of its 
contributors may be used to endorse or promote products derived from 
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
package com.adobe.utils
{
	import com.adobe.utils.AGALMiniAssembler;
	
	import flash.display3D.*;
	import flash.display3D.textures.*;
	import flash.display3D.textures.Texture;
	import flash.geom.ColorTransform;
	import flash.geom.Matrix;
	import flash.geom.Point;
	import flash.geom.Rectangle;
	
	public class Sprite3D
	{		
		public function Sprite3D(texture:Texture, 
								 textureWidth:uint,
								 textureHeight:uint,
								 descriptor:XML = null, 
								 descriptorFormat:String = "cocos2d",
								 registrationPoint:String = "center")
		{
			_texture = texture;

			var sizes:Vector.<Point> = new Vector.<Point>();
			var offsets:Vector.<Point> = new Vector.<Point>();
			var frames:Vector.<Rectangle> = new Vector.<Rectangle>();
			
			if ( descriptor != null ) {
				if ( descriptorFormat == "cocos2d" ) {
					parseCocos2D(descriptor,sizes,offsets,frames);
				} else {
					throw new Error("Unknown descriptor format");
				}
			} else {
				frames.push(new Rectangle(0,0,textureWidth,textureHeight));
				offsets.push(new Point(0,0));
				sizes.push(new Point(textureWidth,textureHeight));
			}
			
			createVertices(registrationPoint,sizes,offsets,frames,textureWidth,textureHeight);
		}
		
		public static function init(context3D:Context3D, 
									backBufferWidth:uint, 
							 		backBufferHeight:uint):void
		{
			_context3D = context3D;
			
			_backBufferWidth = backBufferWidth; _backBufferHeight = backBufferHeight;	
			_backBufferWidthInv = 1.0/_backBufferWidth; _backBufferHeightInv = 1.0/_backBufferHeight;
			
			for ( var c:uint=0; c<12; c++) _matrixVector[c] = 0.0;
			_matrixVector[3] = 1.0; _matrixVector[7] = 1.0; _matrixVector[11] = 1.0;

			var vertexShader:AGALMiniAssembler = new AGALMiniAssembler();
			vertexShader.assemble( Context3DProgramType.VERTEX,
				"mov vt0.xyzw, va0.xyww\n" +
				"m33 vt0.xyz, vt0, vc0\n" +
				"mov op, vt0\n" +
				"mov v0, va1\n"
			);
			var fragmentShader:AGALMiniAssembler = new AGALMiniAssembler();
			fragmentShader.assemble( Context3DProgramType.FRAGMENT,	
				"tex oc, v0, fs0 <2d,linear,miplinear>\n"
			);
			_shaderProgram = _context3D.createProgram();
			_shaderProgram.upload( vertexShader.agalcode, fragmentShader.agalcode);
			
			var fragmentShaderAlpha:AGALMiniAssembler = new AGALMiniAssembler();
			fragmentShaderAlpha.assemble( Context3DProgramType.FRAGMENT,	
				"tex ft0, v0, fs0 <2d,linear,miplinear>\n" +
				"mul oc, ft0, fc0\n"
			);
			_shaderProgramAlpha = _context3D.createProgram();
			_shaderProgramAlpha.upload( vertexShader.agalcode, fragmentShaderAlpha.agalcode);
			
			var fragmentShaderColorTransform:AGALMiniAssembler = new AGALMiniAssembler();
			fragmentShaderColorTransform.assemble( Context3DProgramType.FRAGMENT,	
				"tex ft0, v0, fs0 <2d,linear,miplinear>\n" +
				"mul ft0, ft0, fc0\n" +
				"add oc, ft0, fc1\n"
			);
			_shaderProgramColorTransform = _context3D.createProgram();
			_shaderProgramColorTransform.upload( vertexShader.agalcode, fragmentShaderColorTransform.agalcode);
		}

		public function set rotation(angle:Number):void { _rotation = angle; }
		public function get rotation():Number { return _rotation; }
		
		public function set scaleX(scale:Number):void { _scaleX = scale; }
		public function get scaleX():Number { return _scaleX; }
		
		public function set scaleY(scale:Number):void { _scaleY = scale; }
		public function get scaleY():Number { return _scaleY; }

		public function set scale(scale:Number):void { _scaleX = _scaleY = scale; }
		public function get scale():Number { return _scaleY; }
		
		public function set x(pos:Number):void { _posX = pos; }
		public function get x():Number { return _posX; }
		
		public function set y(pos:Number):void { _posY = pos; }
		public function get y():Number { return _posY; }

		public function set frame(frame:uint):void { _frame = frame; }
		public function get frame():uint { return _frame; }

		public function set alpha(alpha:Number):void { _alpha = alpha; }
		public function get alpha():Number { return _alpha; }
		
		public function set colorTransform(colorTransform:ColorTransform):void { _colorTransform = colorTransform; }
		public function get colorTransform():ColorTransform { return _colorTransform; }
		
		public function render():void 
		{
			if ( _colorTransform == null || colorTransformIsIdentity() ) {
				if ( _alpha == 1.0 ) {
					_context3D.setProgram(_shaderProgram);
				} else {
					_context3D.setProgram(_shaderProgramAlpha);
					_transformVector[0] = 1.0; _transformVector[1] = 1.0; _transformVector[2] = 1.0; _transformVector[3] = _alpha;
					_context3D.setProgramConstantsFromVector(Context3DProgramType.FRAGMENT, 0, _transformVector, 1);
				}
			} else {
				_context3D.setProgram(_shaderProgramColorTransform);
				_transformVector[0] = _colorTransform.redMultiplier; _transformVector[1] = _colorTransform.greenMultiplier;
				_transformVector[2] = _colorTransform.blueMultiplier; _transformVector[3] = _colorTransform.alphaMultiplier;
				_transformVector[4] = _colorTransform.redOffset*(1./255.); _transformVector[5] = _colorTransform.greenOffset*(1./255.);
				_transformVector[6] = _colorTransform.blueOffset*(1./255.); _transformVector[7] = _colorTransform.alphaOffset*(1./255.);
				_context3D.setProgramConstantsFromVector(Context3DProgramType.FRAGMENT, 0, _transformVector, 2);
			}
			_matrix.createBox(_scaleX,_scaleY,
							  _rotation*_gradToRad,
							  _posX-_backBufferWidth,
							  _backBufferHeight-_posY);
			_matrix.scale(_backBufferWidthInv,_backBufferHeightInv);
			_matrixVector[0] = _matrix.a; _matrixVector[1] = _matrix.c; _matrixVector[2] = _matrix.tx;
			_matrixVector[4] = _matrix.b; _matrixVector[5] = _matrix.d; _matrixVector[6] = _matrix.ty;
			_context3D.setProgramConstantsFromVector(Context3DProgramType.VERTEX, 0, _matrixVector, 3);
			_context3D.setTextureAt(0, _texture);
			_context3D.setVertexBufferAt(0, _vertexBuffer, 0, Context3DVertexBufferFormat.FLOAT_2);
			_context3D.setVertexBufferAt(1, _vertexBuffer, 2, Context3DVertexBufferFormat.FLOAT_2);
			_context3D.drawTriangles(_indexBuffer, (_frame%_frameCount)*6, 2);
		}

		private function createVertices(registrationPoint:String,
										sizes:Vector.<Point>,
										offsets:Vector.<Point>,
										frames:Vector.<Rectangle>,
										textureWidth:uint,
										textureHeight:uint):void
		{
			var v:uint = 0;
			var i:uint = 0;

			var vd:Vector.<Number> = new Vector.<Number>(frames.length*16);
			var id:Vector.<uint> = new Vector.<uint>(frames.length*6);

			for ( var c:uint = 0; c<frames.length; c++ ) {
				
				var x:Number = frames[c].x;
				var y:Number = frames[c].y;
				var w:Number = frames[c].width;
				var h:Number = frames[c].height;
				
				var x0:Number = -w;
				var y0:Number = +h;
				var x1:Number = +w;
				var y1:Number = -h;
				
				if ( offsets.length > 0 ) {
					var ox:Number = offsets[c].x;
					var oy:Number = offsets[c].y;
					x0 += ox*2;
					y0 += oy*2;
					x1 += ox*2;
					y1 += oy*2;
				}

				if ( registrationPoint != "center" ) {
					var sx:Number;
					var sy:Number;
					if ( sizes.length > 0 ) {
						sx = sizes[c].x;
						sy = sizes[c].y;
					} else {
						sx = frames[0].width;	
						sy = frames[0].height;
					}
					switch ( registrationPoint ) {
						case	"lefttop": {
									x0 += sx;
									y0 -= sy;
									x1 += sx;
									y1 -= sy;
								} break;
						case	"righttop": {
									x0 -= sx;
									y0 -= sy;
									x1 -= sx;
									y1 -= sy;
								} break;
						case	"leftbottom": {
									x0 += sx;
									y0 += sy;
									x1 += sx;
									y1 += sy;
								} break;
						case	"rightbottom": {
									x0 -= sx;
									y0 += sy;
									x1 -= sx;
									y1 += sy;
								} break;
					}
				}
				
				var u0:Number = (x  ) / textureWidth;
				var v0:Number = (y 	) / textureHeight;
				var u1:Number = (x+w) / textureWidth;
				var v1:Number = (y+h) / textureHeight;
				
				vd[v++] = x0; vd[v++] = y0; vd[v++] = u0; vd[v++] = v0;
				vd[v++] = x1; vd[v++] = y0; vd[v++] = u1; vd[v++] = v0;
				vd[v++] = x1; vd[v++] = y1; vd[v++] = u1; vd[v++] = v1;
				vd[v++] = x0; vd[v++] = y1; vd[v++] = u0; vd[v++] = v1;
				
				id[i++] = (c*4+0); id[i++] = (c*4+1); id[i++] = (c*4+3);
				id[i++] = (c*4+1); id[i++] = (c*4+2); id[i++] = (c*4+3);
				
			}

			_frameCount = frames.length;
			_indexBuffer = _context3D.createIndexBuffer( id.length );
			_indexBuffer.uploadFromVector(id, 0, id.length );
			_vertexBuffer = _context3D.createVertexBuffer( vd.length/4, 4);
			_vertexBuffer.uploadFromVector(vd, 0, vd.length/4);
			
		}
		
		
		private function parseCocos2D(descriptor:XML,
									  sizes:Vector.<Point>,
									  offsets:Vector.<Point>,
									  frames:Vector.<Rectangle>):void {
			
			// Not very generic, only tested with TexturePacker
			
			var type:String;
			var data:String;
			var array:Array;
			
			var topKeys:XMLList = descriptor.dict.key;
			var topDicts:XMLList = descriptor.dict.dict;

			for ( var k:uint = 0; k<topKeys.length(); k++) {
				switch(topKeys[k].toString()) {
					case	"frames": {
								var frameKeys:XMLList = topDicts[k].key;
								var frameDicts:XMLList = topDicts[k].dict;
								for ( var l:uint = 0; l<frameKeys.length(); l++) {
									var propKeys:XMLList = frameDicts[l].key;
									var propAll:XMLList = frameDicts[l].*;
									for ( var m:uint = 0; m<propKeys.length(); m++) {
										type = propAll[propKeys[m].childIndex()+1].name();
										data = propAll[propKeys[m].childIndex()+1];
										switch(propKeys[m].toString()) {
											case	"frame": {
														if ( type == "string" ) {
															array = data.split(/[^0-9-]+/);
															frames.push(new Rectangle(array[1],array[2],array[3],array[4]));
														} else {
															throw new Error("Error parsing descriptor format");
														}
													} break;
											case	"offset": {
														if ( type == "string" ) {
															array = data.split(/[^0-9-]+/);
															offsets.push(new Point(array[1],array[2]));
														} else {
															throw new Error("Error parsing descriptor format");
														}
													} break;
											case	"sourceSize": {
														if ( type == "string" ) {
															array = data.split(/[^0-9-]+/);
															sizes.push(new Point(array[1],array[2]));
														} else {
															throw new Error("Error parsing descriptor format");
														}
													} break;
											case	"rotated": {
														if ( type != "false" ) {
															throw new Error("Rotated elements not supported (yet)");
														}
													} break;
										}
									}
								}
							} break;
				}
			}
			if ( frames.length == 0 ) {
				throw new Error("Error parsing descriptor format");
			}
		}

		private function colorTransformIsIdentity():Boolean {
			if ( _colorTransform.redOffset == 0 && 
				 _colorTransform.blueOffset == 0 && 
				 _colorTransform.greenOffset == 0 && 
				 _colorTransform.alphaOffset == 0 && 
				 _colorTransform.redMultiplier == 1.0 && 
				 _colorTransform.blueMultiplier == 1.0 && 
				 _colorTransform.greenMultiplier == 1.0 &&
				 _colorTransform.alphaMultiplier == 1.0
			) {
				return true;
			};
			return false;
		}
		
		private var _frame:uint = 0;
		private var _rotation:Number = 0.0;
		private var _posX:Number = 0.0;
		private var _posY:Number = 0.0;
		private var _scaleX:Number = 1.0;
		private var _scaleY:Number = 1.0;
		private var _alpha:Number = 1.0;

		private var _frameCount:uint = 0;
		private var _colorTransform:ColorTransform = new ColorTransform();

		private var _texture:Texture = null;
		private var _vertexBuffer:VertexBuffer3D = null;
		private var _indexBuffer:IndexBuffer3D = null;
		
		private static var _matrix:Matrix = new Matrix();
		private static var _matrixVector:Vector.<Number> = new Vector.<Number>();
		private static var _transformVector:Vector.<Number> = new Vector.<Number>();
		private static var _shaderProgram:Program3D = null;
		private static var _shaderProgramAlpha:Program3D = null;
		private static var _shaderProgramColorTransform:Program3D = null;
		private static var _context3D:Context3D = null;
		private static var _backBufferWidth:uint = 0;
		private static var _backBufferHeight:uint = 0;
		private static var _backBufferWidthInv:Number = 0;
		private static var _backBufferHeightInv:Number = 0;
		
		private static const _gradToRad:Number = (2*Math.PI/360);
	}
}

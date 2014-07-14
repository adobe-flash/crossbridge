/*
Copyright (c) 2012, Adobe Systems Incorporated
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

package GLS3D
{
    import flash.display.*;
    import flash.display3D.*;
    import flash.display3D.textures.*;
    import flash.events.Event;
    import flash.events.KeyboardEvent;
    import flash.events.TextEvent;
    import flash.geom.*;
    import flash.text.TextField;
    import flash.text.TextFieldType;
    import flash.trace.Trace;
    import flash.utils.*;

    import com.adobe.utils.*;
    import com.adobe.utils.macro.*;
    import com.adobe.flascc.CModule; 

    // Linker trickery
    [Csym("D", "___libgl_abc__", ".data")]

    public class GLAPI
    {
        include 'libGLconsts.as';

        private static var _instance:GLAPI

        public var disableCulling:Boolean = false
        public var disableBlending:Boolean = false
        public var log:Object = null; // new TraceLog();
        public var context:Context3D
        public var genOnBind:Boolean = false

        private const consts:Vector.<Number> = new <Number>[0.0, 0.5, 1.0, 2.0]
        private const zeroes:Vector.<Number> = new <Number>[0.0, 0.0, 0.0, 0.0]

        // bit 0 = whether textures are enabled
        private const ENABLE_TEXTURE_OFFSET:uint = 0
        // bits 1-6 = whether clip planes 0-5 are enabled
        private const ENABLE_CLIPPLANE_OFFSET:uint = 1
        // bit 7 = whether color material is enabled
        private const ENABLE_COLOR_MATERIAL_OFFSET:uint = 7
        // bit 8 = whether lighting is enabled
        private const ENABLE_LIGHTING_OFFSET:uint = 8
        // bit 9-16 = whether lights 0-7 are enabled
        private const ENABLE_LIGHT_OFFSET:uint = 9
        // bit 17 = whether specular is separate
        private const ENABLE_SEPSPEC_OFFSET:uint = 17
        // bit 18 = whether polygon offset is enabled
        private const ENABLE_POLYGON_OFFSET:uint = 18

        private var _stage:Stage
        private var overrideR:uint
        private var overrideG:uint
        private var overrideB:uint
        private var overrideA:uint = 0xFF
        private var scissorRect:Rectangle
        private var contextEnableScissor:Boolean = false
        private var bgColorOverride:Boolean = false
        private var fixed_function_programs:Dictionary = new Dictionary()
        private var contextDepthFunction:String = Context3DCompareMode.LESS
        private var reusableCommandList:CommandList = new CommandList()
        private var reusableVertexBuffers:Dictionary = new Dictionary()
        private var immediateVertexBuffers:VertexBufferPool = new VertexBufferPool()
        private var sharedIndexBuffers:Dictionary = new Dictionary()
        private var contextColor:Vector.<Number> = new <Number>[1, 1, 1, 1]
        private var frontFaceClockWise:Boolean = false // we default to CCW
        private var glCullMode:uint = GL_BACK
        private var lightingStates:Vector.<LightingState> = new Vector.<LightingState>()
        private var texID:uint = 1 // so we have 0 as non-valid id
        private var shininessVec:Vector.<Number> = new <Number>[0.0, 0.0, 0.0, 0.0]
        private var globalAmbient:Vector.<Number> = new <Number>[0.2, 0.2, 0.2, 1]
        private var polygonOffsetValue:Number = -0.0005
        private var lights:Vector.<Light> = new Vector.<Light>(8)
        private var lightsEnabled:Vector.<Boolean> = new Vector.<Boolean>(8)
        private var enableTexGenS:Boolean = false
        private var enableTexGenT:Boolean = false
        private var texGenParamS:uint = GL_SPHERE_MAP
        private var texGenParamT:uint = GL_SPHERE_MAP
        private var contextWidth:int = 0
        private var contextHeight:int = 0
        private var contextAA:int = 0
        private var contextClearR:Number
        private var contextClearG:Number
        private var contextClearB:Number
        private var contextClearA:Number
        private var contextClearDepth:Number = 1.0
        private var contextClearStencil:uint = 0
        private var contextClearMask:uint
        private var contextEnableStencil:Boolean = false
        private var contextEnableAlphaTest:Boolean = false
        private var contextStencilActionStencilFail:String = Context3DStencilAction.KEEP
        private var contextStencilActionDepthFail:String = Context3DStencilAction.KEEP
        private var contextStencilActionPass:String = Context3DStencilAction.KEEP
        private var contextStencilCompareMode:String = Context3DCompareMode.ALWAYS
        private var contextEnableDepth:Boolean = true
        private var contextDepthMask:Boolean = true
        private var contextSrcBlendFunc:String = Context3DBlendFactor.ZERO
        private var contextDstBlendFunc:String = Context3DBlendFactor.ONE
        private var contextEnableCulling:Boolean
        private var contextEnableBlending:Boolean
        private var contextDepthFunc:String = Context3DCompareMode.ALWAYS
        private var contextEnableTextures:Boolean = false
        private var contextEnableLighting:Boolean = false
        private var contextColorMaterial:Boolean = false
        private var contextSeparateSpecular:Boolean = false
        private var contextEnablePolygonOffset:Boolean = false
        private var needClear:Boolean = true
        private var vertexAttributesDirty:Boolean = true
        private var activeCommandList:CommandList = null
        private var commandLists:Vector.<CommandList> = null
        private var textures:Dictionary = new Dictionary()
        private var vertexBufferAttributes:Vector.<VertexBufferAttribute> = new Vector.<VertexBufferAttribute>(8)
        private var textureUnits:Array = new Array(32)
        private var activeProgram:ProgramInstance
        private var activeTextureUnit:uint = 0
        private var activeTexture:TextureInstance
        private var textureSamplers:Vector.<TextureInstance> = new Vector.<TextureInstance>(8)
        private var textureSamplerIDs:Vector.<uint> = new Vector.<uint>(8)
        private var vertexBufferObjects:Vector.<VertexBuffer3D> = new Vector.<VertexBuffer3D>()
        private var framestamp:uint = 1
        private var offsetFactor:Number = 0.0
        private var offsetUnits:Number = 0.0
        private var glStateFlags:uint = 0
        private var clipPlanes:Vector.<Number> = new Vector.<Number>(6 * 4)    // space for 6 clip planes
        private var clipPlaneEnabled:Vector.<Boolean> = new Vector.<Boolean>(8) // defaults to false
        private var modelViewStack:Vector.<Matrix3D> = new <Matrix3D>[ new Matrix3D() ]
        private var projectionStack:Vector.<Matrix3D> = new <Matrix3D>[ new Matrix3D() ]
        private var textureStack:Vector.<Matrix3D> = new <Matrix3D>[ new Matrix3D() ]
        private var currentMatrixStack:Vector.<Matrix3D> = modelViewStack
        private var contextMaterial:Material = new Material(true)
        private var cubeVertexData:Vector.<Number>;
        private var cubeVertexBuffer:VertexBuffer3D = null;

        public static function init(context:Context3D, log:Object, stage:Stage):void
        {
            _instance = new GLAPI(context, log, stage)
            if (log) log.send("GLAPI initialized.")
        }
        
        public static function get instance():GLAPI
        {
            if (!_instance)
            {
                trace("Instance is null, did you forget calling GLAPI.init() in AlcConsole.as?") 
            }
            return _instance
        }
        
        public function send(value:String):void
        {
            if (log)
                log.send(value)
        }

        private function matrix3DToString(m:Matrix3D):String
        {
            var data:Vector.<Number> = m.rawData
            return ("[ " + data[0].toFixed(3) + ", " + data[4].toFixed(3) + ", " + data[8].toFixed(3) + ", " + data[12].toFixed(3) + " ]\n" +
                    "[ " + data[1].toFixed(3) + ", " + data[5].toFixed(3) + ", " + data[9].toFixed(3) + ", " + data[13].toFixed(3) + " ]\n" +
                    "[ " + data[2].toFixed(3) + ", " + data[6].toFixed(3) + ", " + data[10].toFixed(3) + ", " + data[14].toFixed(3) + " ]\n" +
                    "[ " + data[3].toFixed(3) + ", " + data[7].toFixed(3) + ", " + data[11].toFixed(3) + ", " + data[15].toFixed(3) + " ]")
        }
        
        // ======================================================================
        //  Polygon Offset
        // ----------------------------------------------------------------------
 
        public function glPolygonMode(face:uint, mode:uint):void
        {
            switch(mode)
            {
                case GL_POINT:
                    if (log) log.send("glPolygonMode GL_POINT not yet implemented, mode is always GL_FILL.")
                    break
                case GL_LINE:
                    if (log) log.send("glPolygonMode GL_LINE not yet implemented, mode is always GL_FILL.")
                    break
                default:
                    // GL_FILL!
            }
        }
        
        public function glPolygonOffset(factor:Number, units:Number):void
        {
            offsetFactor = factor
            offsetUnits = units
            //if (log) log.send("glPolygonOffset() called with (" + factor + ", " + units + ")")
            if (log) log.send("glPolygonOffset() not yet implemented.")
        }

        public function glShadeModel(mode:uint):void
        {
            switch(mode)
            {
                case GL_FLAT:
                    if (log) log.send("glShadeModel GL_FLAT not yet implemented, mode is always GL_SMOOTH.")
                    break
                default:
                    // GL_SMOOTH!
            }
        }

        // ======================================================================
        //  Alpha Testing
        // ----------------------------------------------------------------------
        
        public function glAlphaFunc(func:uint, ref:Number):void
        {
            //TODO: fixme
        }
            
        private function setVector(vec:Vector.<Number>, x:Number, y:Number, z:Number, w:Number):void
        {
            vec[0] = x
            vec[1] = y
            vec[2] = z
            vec[3] = w
        }
        
        private function copyVector(dest:Vector.<Number>, src:Vector.<Number>):void
        {
            dest[0] = src[0]
            dest[1] = src[1]
            dest[2] = src[2]
            dest[3] = src[3]
        }
        
        public function glMaterial(face:uint, pname:uint, r:Number, g:Number, b:Number, a:Number):void
        {
            // if pname == GL_SPECULAR, then "r" is shininess.
            // FIXME (klin): Ignore face for now. Always GL_FRONT_AND_BACK
            var material:Material

            if (activeCommandList)
            {
                var activeState:ContextState = activeCommandList.ensureActiveState()
                material = activeCommandList.activeState.material
            }
            else
            {
                material = contextMaterial
            }
            
            
            switch (pname)
            {
                case GL_AMBIENT:
                    if (!material.ambient)
                        material.ambient = new <Number>[r, g, b, a]
                    else
                        setVector(material.ambient, r, g, b, a)
                    break
                case GL_DIFFUSE:
                    if (!material.diffuse)
                        material.diffuse = new <Number>[r, g, b, a]
                    else
                        setVector(material.diffuse, r, g, b, a)
                    break
                case GL_AMBIENT_AND_DIFFUSE:
                    if (!material.ambient)
                        material.ambient = new <Number>[r, g, b, a]
                    else
                        setVector(material.ambient, r, g, b, a)
                    
                    if (!material.diffuse)
                        material.diffuse = new <Number>[r, g, b, a]
                    else
                        setVector(material.diffuse, r, g, b, a)
                    break
                case GL_SPECULAR:
                    if (!material.specular)
                        material.specular = new <Number>[r, g, b, a]
                    else
                        setVector(material.specular, r, g, b, a)
                    break
                case GL_SHININESS:
                    material.shininess = r
                    break
                case GL_EMISSION:
                    if (!material.emission)
                        material.emission = new <Number>[r, g, b, a]
                    else
                        setVector(material.emission, r, g, b, a)
                    break
                default:
                    if (log) log.send("[NOTE] Unsupported glMaterial call with 0x" + pname.toString(16))
            }
        }

        public function glLightModeli(pname:uint, param:int):void
        {
            switch (pname)
            {
                case GL_LIGHT_MODEL_COLOR_CONTROL:
                        contextSeparateSpecular = (param == GL_SEPARATE_SPECULAR_COLOR)
                        if (contextSeparateSpecular)
                            setGLState(ENABLE_SEPSPEC_OFFSET)
                        else
                            clearGLState(ENABLE_SEPSPEC_OFFSET)
                   break
                
                // unsupported for now
                case GL_LIGHT_MODEL_TWO_SIDE: 
                case GL_LIGHT_MODEL_AMBIENT:
                case GL_LIGHT_MODEL_LOCAL_VIEWER:
                default:
                    break
            }
            
            if (log) log.send("glLightModeli() not yet implemented")
        }

        public function glLight(light:uint, pname:uint, r:Number, g:Number, b:Number, a:Number):void
        {
            var lightIndex:int = light - GL_LIGHT0
            if (lightIndex < 0 || lightIndex > 7)
            {
                if (log) log.send("glLight(): light index " + lightIndex + " out of bounds")
                return
            }
            
            var l:Light = lights[lightIndex]
            if (!l)
                l = lights[lightIndex] = new Light(true, lightIndex == 0)
            
            switch (pname)
            {
                case GL_AMBIENT:
                    l.ambient[0] = r
                    l.ambient[1] = g
                    l.ambient[2] = b
                    l.ambient[3] = a
                    break
                case GL_DIFFUSE:
                    l.diffuse[0] = r
                    l.diffuse[1] = g
                    l.diffuse[2] = b
                    l.diffuse[3] = a
                    break
                case GL_SPECULAR:
                    l.specular[0] = r
                    l.specular[1] = g
                    l.specular[2] = b
                    l.specular[3] = a
                    break
                case GL_POSITION:
                    // transform position to eye-space before storing.
                    var m:Matrix3D = modelViewStack[modelViewStack.length - 1].clone();
    				var result:Vector3D;
					if (a == 0.0)
					{	// Directional light
						m.position = new Vector3D(0, 0, 0, 1);
						result = m.transformVector(new Vector3D(r, g, b, a));
						l.position[0] = result.x;
						l.position[1] = result.y;
						l.position[2] = result.z;
						l.position[3] = 0;

						l.type = Light.LIGHT_TYPE_DIRECTIONAL;
					}
					else
					{	// Point light
						result = m.transformVector(new Vector3D(r, g, b, a));
						l.position[0] = result.x;
						l.position[1] = result.y;
						l.position[2] = result.z;
						l.position[3] = result.w;

						l.type = Light.LIGHT_TYPE_POINT;
					}
                    break
                default:
                    break
            }
        }

        public function glGetIntegerv(pname:uint, buf:ByteArray, offset:uint):void
        {
            if (log) log.send("glGetIntegerv")
            switch (pname)
            {
                case GL_MAX_TEXTURE_SIZE:
                    buf.position = offset
                    buf.writeInt(4096)
                break
                case GL_VIEWPORT:
                    buf.position = offset+0; buf.writeInt(0); // x
                    buf.position = offset+4; buf.writeInt(0); // y
                    buf.position = offset+8; buf.writeInt(contextWidth); // width
                    buf.position = offset+12; buf.writeInt(contextHeight); // height
                break

                default:
                    if (log) log.send("[NOTE] Unsupported glGetIntegerv call with 0x" + pname.toString(16))
            }
        }

        public function glGetFloatv(pname:uint, buf:ByteArray, offset:uint):void
        {
            if (log) log.send("glGetFloatv")
            switch (pname)
            {
                case GL_MODELVIEW_MATRIX:
                    var v:Vector.<Number> = new Vector.<Number>(16)
                    modelViewStack[modelViewStack.length - 1].copyRawDataTo(v)
                    buf.position = offset
                    for (var i:int; i < 16; i++)
                        buf.writeFloat(v[i])
                    break
                default:
                    if (log) log.send("[NOTE] Unsupported glGetFloatv call with 0x" + pname.toString(16))
            }
        }
       
        public function glClipPlane(plane:uint, a:Number, b:Number, c:Number, d:Number):void
        {
            if (log) log.send("[NOTE] glClipPlane called for plane 0x" + plane.toString(16) + ", with args " + a + ", " + b + ", " + c + ", " + d)
            var index:int = plane - GL_CLIP_PLANE0
            
            // Convert coordinates to eye space (modelView) before storing
            var m:Matrix3D = modelViewStack[modelViewStack.length - 1].clone()
            m.invert()
            m.transpose()
            var result:Vector3D = m.transformVector(new Vector3D(a, b, c, d))
            
            clipPlanes[ index * 4 + 0 ] = result.x
            clipPlanes[ index * 4 + 1 ] = result.y
            clipPlanes[ index * 4 + 2 ] = result.z
            clipPlanes[ index * 4 + 3 ] = a * m.rawData[3] + b * m.rawData[7] + c * m.rawData[11] + d * m.rawData[15] //result.w
        }

        private function executeCommandList(cl:CommandList):void
        {
            // FIXME (egeorgire): do this on-deamnd?
            // Pre-calculate matrix
            var m:Matrix3D = modelViewStack[modelViewStack.length - 1].clone()
            var p:Matrix3D = projectionStack[projectionStack.length - 1].clone()
            var t:Matrix3D = textureStack[textureStack.length - 1].clone()
            //m.append(p)
            
            
            p.prepend(m)
            var invM:Matrix3D = m.clone()
            invM.invert()
            var modelToClipSpace:Matrix3D = p

            if (isGLState(ENABLE_POLYGON_OFFSET))
            {
                // Adjust the projection matrix to give us z offset
                if (log)
                    log.send("Applying polygon offset")
                
                modelToClipSpace = p.clone()
                modelToClipSpace.appendTranslation(0, 0, polygonOffsetValue)
            }
            
            
            // Current active textures ??
            var ti:TextureInstance
            var i:int
            for (i = 0; i < 1; i++ )
            {
                ti = textureSamplers[i]
                if (ti && contextEnableTextures) {
                    context.setTextureAt(i, ti.boundType == GL_TEXTURE_2D ? ti.texture : ti.cubeTexture)
                    if(log) log.send("setTexture " + i + " -> " + ti.texID)
                }
                else {
                    context.setTextureAt(i, null)
                    if(log) log.send("setTexture " + i + " -> 0")
                }
            }
            
            var textureStatInvalid:Boolean = false;
            const count:int = cl.commands.length
            var command:Object
            for (var k:int = 0; k < count; k++)
            {
                command = cl.commands[k]
                var stateChange:ContextState = command as ContextState
                if (stateChange)
                {
                
                    // We execute state changes before stream changes, so
                    // we must have a state change
    
                    // Execute state changes
                    if (contextEnableTextures && stateChange.textureSamplers)
                    {
                        for (i = 0; i < 1; i++ )
                        {
                            var texID:int = stateChange.textureSamplers[i]
                            if (texID != -1)
                            {
                                if (log) log.send("Mapping texture " + texID + " to sampler " + i)
                                ti = (texID != 0) ? textures[texID] : null
                                textureSamplers[i] = ti
                                activeTexture = ti // Executing the glBind, so that after running through the list we have the side-effect correctly
                                textureStatInvalid = true
                                if (ti)
                                    context.setTextureAt(i, ti.boundType == GL_TEXTURE_2D ? ti.texture : ti.cubeTexture)
                                else
                                    context.setTextureAt(i, null)
                                if(log) log.send("setTexture " + i + " -> " + (ti ? ti.texID : 0))
                            }
                        }
                    }
                    
                    var stateMaterial:Material = stateChange.material
                    if (stateMaterial)
                    {
                        if (stateMaterial.ambient)
                            copyVector(contextMaterial.ambient, stateMaterial.ambient)
                        if (stateMaterial.diffuse)
                            copyVector(contextMaterial.diffuse, stateMaterial.diffuse)
                        if (stateMaterial.specular)
                            copyVector(contextMaterial.specular, stateMaterial.specular)
                        if (!isNaN(stateMaterial.shininess))
                            contextMaterial.shininess = stateMaterial.shininess
                        if (stateMaterial.emission)
                            copyVector(contextMaterial.emission, stateMaterial.emission)
                    }
                }

                var stream:VertexStream = command as VertexStream
                if (stream)
                {
                    
                    // Make sure we have the right program, and see if we need to updated it if some state change requires it
                    ensureProgramUpToDate(stream)
                    
                    // If the program has no textures, then disable them all:
                    if (!stream.program.hasTexture)
                    {
                        for (i = 0; i < 8; i++ )
                        {
                            context.setTextureAt(i, null)
                            if(log) log.send("setTexture " + i + " -> 0")
                        }
                    }
                    
                    context.setProgram(stream.program.program)

                    // FIXME (egeorgie): do we need to do this after setting every program, or just once after we calculate the matrix?
                    if (stream.polygonOffset)
                    {
                        // Adjust the projection matrix to give us z offset
                        if (log)
                            log.send("Applying polygon offset, recorded in the list")
                        modelToClipSpace = p.clone()
                        modelToClipSpace.appendTranslation(0, 0, polygonOffsetValue)
                    }
                    context.setProgramConstantsFromMatrix(Context3DProgramType.VERTEX, 0, modelToClipSpace, true)
                    if (stream.polygonOffset)
                    {
                        // Restore
                        modelToClipSpace = p
                    }
                    context.setProgramConstantsFromMatrix(Context3DProgramType.VERTEX, 4, m, true)
                    context.setProgramConstantsFromMatrix(Context3DProgramType.VERTEX, 8, invM, true)
                    context.setProgramConstantsFromMatrix(Context3DProgramType.VERTEX, 12, t, true)
                    context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, 16, consts)
                    context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, 17, contextColor)

                    // Upload the clip planes
                    context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, 18, clipPlanes, 6)

                    // Zero-out the ones that are not enabled
                    for (i = 0; i < 6; i++)
                    {
                        if (!clipPlaneEnabled[i])
                            context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, 18 + i, zeroes, 1)
                    }
                    
                    // Calculate origin of eye-space
                    context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, 24, new <Number>[0, 0, 0, 1], 1)
                    
                    // Upload material components
                    context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, 25, contextMaterial.ambient, 1)
                    context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, 26, contextMaterial.diffuse, 1)
                    context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, 27, contextMaterial.specular, 1)
                    shininessVec[0] = contextMaterial.shininess
                    context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, 28, shininessVec, 1)
                    context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, 29, contextMaterial.emission, 1)
                    
                    // Upload lights
                    // FIXME (klin): will be per light...for now, fake a light and assume local viewer.
                    // default global light:
                    context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, 30, globalAmbient, 1)
                    
                    // light constants
                    for (i = 0; i < 8; i++)
                    {
                        var index:int = 31 + i*4
                        if (lightsEnabled[i])
                        {
                            var l:Light = lights[i]
                            context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, index, l.position, 1)
                            context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, index+1, l.ambient, 1)
                            context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, index+2, l.diffuse, 1)
                            context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, index+3, l.specular, 1)
                        }
                        else
                        {
                            context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, index, zeroes, 1)
                            context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, index+1, zeroes, 1)
                            context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, index+2, zeroes, 1)
                            context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, index+3, zeroes, 1)
                        }

                    }

                    // Map the Vertex buffer
                    
                    // position
                    context.setVertexBufferAt(0, stream.vertexBuffer, 0 /*bufferOffset*/, Context3DVertexBufferFormat.FLOAT_3)

                    // color
                    if (0 != (stream.program.vertexStreamUsageFlags & VertexBufferBuilder.HAS_COLOR))
                        context.setVertexBufferAt(1, stream.vertexBuffer, 3 /*bufferOffset*/, Context3DVertexBufferFormat.FLOAT_4)
                    else
                        context.setVertexBufferAt(1, null) 

                    // normal
                    if (0 != (stream.program.vertexStreamUsageFlags & VertexBufferBuilder.HAS_NORMAL))
                        context.setVertexBufferAt(2, stream.vertexBuffer, 7 /*bufferOffset*/, Context3DVertexBufferFormat.FLOAT_3)
                    else
                        context.setVertexBufferAt(2, null) 

                    // texture coords
                    if (0 != (stream.program.vertexStreamUsageFlags & VertexBufferBuilder.HAS_TEXTURE2D))
                        context.setVertexBufferAt(3, stream.vertexBuffer, 10 /*bufferOffset*/, Context3DVertexBufferFormat.FLOAT_2)
                    else
                        context.setVertexBufferAt(3, null) 

                    context.drawTriangles(stream.indexBuffer)
                    
                    // If we're executing on compilation, this may be an immediate command stream, so update the pool
                    if (cl.executeOnCompile)
                        immediateVertexBuffers.markInUse(stream.vertexBuffer)
                }
            }
        }

        public function glGenLists(count:uint):uint
        {
            if (log) log.send("glGenLists " + count)
            if (!commandLists)
                commandLists = new Vector.<CommandList>()
            
            var oldLength:int = commandLists.length
            commandLists.length = oldLength + count
            return oldLength
        }

        public function glMatrixMode(mode:uint):void
        {
            if (log) log.send("glMatrixMode \nSwitch stack to " + MATRIX_MODE[mode - GL_MODELVIEW])
            
            switch (mode)
            {
                case GL_MODELVIEW: 
                    currentMatrixStack = modelViewStack
                break
                
                case GL_PROJECTION:
                    currentMatrixStack = projectionStack
                break
                
                case GL_TEXTURE:
                    currentMatrixStack = textureStack
                break

                default:
                    if (log) log.send("Unknown Matrix Mode " + mode)
            }
        }
        
        public function glPushMatrix():void
        {
            if (log) log.send("glPushMatrix")
            currentMatrixStack.push(currentMatrixStack[currentMatrixStack.length - 1].clone())
        }
        
        public function glPopMatrix():void
        {
            if (log) log.send("glPopMatrix")
            currentMatrixStack.pop()
            if(currentMatrixStack.length == 0) {
                trace("marix stack underflow!")
                currentMatrixStack.push(new Matrix3D())
            }
        }
        
        public function glLoadIdentity():void
        {
            if (log) log.send("glLoadIdentity")
            currentMatrixStack[currentMatrixStack.length - 1].identity()
        }
        
        public function glOrtho(left:Number, right:Number, bottom:Number, top:Number, zNear:Number, zFar:Number):void
        {
            if (log) log.send("glOrtho: left = " + left + ", right = " + right + ", bottom = " + bottom + ", top = " + top + ", zNear = " + zNear + ", zFar = " + zFar)
            
            var tx:Number = - (right + left) / (right - left)
            var ty:Number = - (top + bottom) / (top - bottom)
            var tz:Number = - (zFar + zNear) / (zFar - zNear)
            
            // in column-major order...
            var m:Matrix3D = new Matrix3D( new <Number>[
                                            2 / (right - left), 0, 0, 0,
                                            0, 2 / (top - bottom), 0, 0,
                                            0, 0, - 2 / ( zFar - zNear), 0,
                                            tx, ty, tz, 1]
                )
            
            // Multiply current matrix by the ortho matrix
            currentMatrixStack[currentMatrixStack.length - 1].prepend(m)
        }
        
        public function glTranslate(x:Number, y:Number, z:Number):void
        {
            if (log) log.send("glTranslate")
            currentMatrixStack[currentMatrixStack.length - 1].prependTranslation(x, y, z)
        }
        
        public function glRotate(degrees:Number, x:Number, y:Number, z:Number):void
        {
            if (log) log.send("glRotate")
            currentMatrixStack[currentMatrixStack.length - 1].prependRotation(degrees, new Vector3D(x, y, z))
        }
        
        public function glScale(x:Number, y:Number, z:Number):void
        {
            if (log) log.send("glScale")
            
            if (x != 0 && y != 0 && z != 0)
            currentMatrixStack[currentMatrixStack.length - 1].prependScale(x, y, z)
        }
        
        public function glMultMatrix(ram:ByteArray, floatArray:Boolean):void
        {
            if (log) log.send("glMultMatrix floatArray: " + floatArray.toString())

            var v:Vector.<Number> = new Vector.<Number>(16)
            for (var i:int; i < 16; i++)
                v[i] = floatArray ? ram.readFloat() : ram.readDouble()
            var m:Matrix3D = new Matrix3D(v)
            currentMatrixStack[currentMatrixStack.length - 1].prepend(m)
        }
        
        public function multMatrix(m:Matrix3D):void
        {
            currentMatrixStack[currentMatrixStack.length - 1].prepend(m)
        }

        public function glLoadMatrix(ram:ByteArray, floatArray:Boolean):void
        {
            if (log) log.send("glLoadMatrix floatArray: " + floatArray.toString())

            var v:Vector.<Number> = new Vector.<Number>(16)
            for (var i:int; i < 16; i++)
                v[i] = floatArray ? ram.readFloat() : ram.readDouble()
            var m:Matrix3D = new Matrix3D(v)
            currentMatrixStack[currentMatrixStack.length - 1] = m
        }

        public function glDepthMask(enable:Boolean):void
        {
            if (log) log.send("glDepthMask(" + enable + "), currently contextEnableDepth = " + contextEnableDepth)
            contextDepthMask = enable
            if (contextEnableDepth)
            {
                context.setDepthTest(contextDepthMask, contextDepthFunction)
            }
        }

        public function glDepthFunc(mode:uint):void
        {
            if (log) log.send("glDepthFunc( " + COMPARE_MODE[mode - GL_NEVER] + " ), currently contextEnableDepth = " + contextEnableDepth)
        
            contextDepthFunction = convertCompareMode(mode)
            if (contextEnableDepth)
                context.setDepthTest(contextDepthMask, contextDepthFunction)
        }

        private function convertCompareMode(mode:uint):String
        {
            switch (mode)
            {
                case GL_NEVER: return Context3DCompareMode.NEVER
                case GL_LESS: return Context3DCompareMode.LESS
                case GL_EQUAL: return Context3DCompareMode.EQUAL
                case GL_LEQUAL: return Context3DCompareMode.LESS_EQUAL
                case GL_GREATER: return Context3DCompareMode.GREATER
                case GL_NOTEQUAL: return Context3DCompareMode.NOT_EQUAL
                case GL_GEQUAL: return Context3DCompareMode.GREATER_EQUAL
                case GL_ALWAYS: return Context3DCompareMode.ALWAYS
            }
            return null
        }

        private function texGenParamToString(param:uint):String
        {
            if (param < GL_NORMAL_MAP)
                return GL_PARAM[param - GL_EYE_LINEAR]
            else
                return GL_PARAM[param - GL_NORMAL_MAP]
        }

        public function glTexGeni(coord:uint, pname:uint, param:uint):void
        {
            if (log) log.send("glTexGeni( " + GL_COORD_NAME[coord - GL_S] + ", " + GL_PARAM_NAME[pname - GL_TEXTURE_GEN_MODE] + ", " + texGenParamToString(param) + ")")
            
            if (GL_T < coord)
            {
                if (log) log.send("Unsupported " + GL_COORD_NAME[coord - GL_S])
                return
            }
            
            if (pname != GL_TEXTURE_GEN_MODE)
            {
                if (log) log.send("Unsupported " + GL_PARAM_NAME[pname - GL_TEXTURE_GEN_MODE])
                return
            }
            
            switch (coord)
            {
                case GL_S:
                    texGenParamS = param
                break
                
                case GL_T:
                    texGenParamT = param
                break
            }
        }

        private function setupIndexBuffer(stream:VertexStream, mode:uint, count:int):void
        {
            var key:uint = ((mode << 20) | count)
            var indexBuffer:IndexBuffer3D = sharedIndexBuffers[key]

            if (!indexBuffer)
            {
                var indexData:Vector.<uint> = new Vector.<uint>()
                generateDLIndexData(mode, count, indexData)
                indexBuffer = context.createIndexBuffer(indexData.length)
                indexBuffer.uploadFromVector(indexData, 0, indexData.length)
                
                // Cache
                sharedIndexBuffers[key] = indexBuffer
            }
            stream.indexBuffer = indexBuffer
        }
        
        private function generateDLIndexData(mode:uint, count:int, indexData:Vector.<uint>):void
        {
            var i:int
            var p0:int
            var p1:int
            var p2:int
            var p3:int
            
            switch (mode)
            {
                case GL_QUADS:
                    // Assert count == n * 4, n >= 1
                    // for each group of 4 vertices 0, 1, 2, 3 draw two triangles 0, 1, 2 and 0, 2, 3
                    
                    for (i = 0; i < count; i += 4)
                    {
                        indexData.push(i)
                        indexData.push(i + 1)
                        indexData.push(i + 2)

                        indexData.push(i)
                        indexData.push(i + 2)
                        indexData.push(i + 3)
                    }
                    
                break
                
                case GL_QUAD_STRIP:
                    // Assert count == n * 2, n >= 2
                    // Draws a connected group of quadrilaterals. One quadrilateral is defined for each pair of vertices presented after the first pair.
                    // Vertices 2n - 2, 2n - 1, 2n + 1, 2n  define a quadrilateral.
                    
                    for (i = 0; i < count - 2; i += 2)
                    {
                        // The four corners of the quadrilateral are

                        p0 = i
                        p1 = i + 1
                        p2 = i + 2
                        p3 = i + 3
                        
                        // Draw as two triangles 0, 1, 2 and 2, 1, 3
                        indexData.push(p0)
                        indexData.push(p1)
                        indexData.push(p2)
                        
                        indexData.push(p2)
                        indexData.push(p1)
                        indexData.push(p3)
                    }
                    
                break

                case GL_TRIANGLES:
                    for (i = 0; i <count; i++)
                    {
                        indexData.push(i)
                    }
                break

               case GL_TRIANGLE_STRIP:
                    for (i = 0; i < count - 2; i++)
                    {
                        p0 = i
                        p1 = i + 1
                        p2 = i + 2

                        indexData.push(p0)
                        if(i % 2 == 0) {
                            indexData.push(p1)
                            indexData.push(p2)
                        } else {
                            indexData.push(p2)
                            indexData.push(p1)
                        }
                    }
                break

                case GL_POLYGON:
                case GL_TRIANGLE_FAN:
                    for (i = 0; i < count-2; i++)
                    {
                        p0 = i + 1
                        p1 = i + 2
                        
                        indexData.push(0)
                        indexData.push(p0)
                        indexData.push(p1)
                    }
                break

                default:
                    if (log) log.send("Not yet implemented mode for glBegin " + BEGIN_MODE[mode])
                    for (i = 0; i <count; i++)
                    {
                        indexData.push(i)
                    }
            }
        }

        public function glGenBuffers(count:uint, dataPtr:uint):void
        {
            if(count != 1)
                throw "unimplemented"

            vertexBufferObjects.push(null); // will be created for real by glBufferData
            CModule.write32(dataPtr, vertexBufferObjects.length - 1);
        }

        public function glBufferData(target:uint, size:uint, dataPtr:uint, usage:uint):void
        {
            if(target != GL_ARRAY_BUFFER)
                throw "unimplemented"
        }

        var debugCubeStream:VertexStream

        public function glDebugCube():void
        {
            if(!cubeVertexBuffer) {
                cubeVertexBuffer = context.createVertexBuffer(36, 12);
                cubeVertexBuffer.uploadFromVector(cubeVertexData, 0, 36);
            }

            if(!debugCubeStream) {
                debugCubeStream = new VertexStream()
                debugCubeStream.vertexBuffer = cubeVertexBuffer
                debugCubeStream.vertexFlags = VertexBufferBuilder.HAS_NORMAL
                debugCubeStream.polygonOffset = isGLState(ENABLE_POLYGON_OFFSET)
                setupIndexBuffer(debugCubeStream, GL_TRIANGLES, 36)
            }

            var cl:CommandList  = reusableCommandList
            cl.executeOnCompile = true
            cl.commands.length = 0
            cl.activeState = null

            if (cl.activeState)
            {
                cl.commands.push(cl.activeState)
                cl.activeState = null
            }

            cl.commands.push(debugCubeStream)
            
            //if (log) log.send("========== DEBUG CUBE >>")
            executeCommandList(cl)
            //if (log) log.send("========== DEBUG CUBE <<")
        }

        public function glEndVertexData(count:uint, mode:uint, data:ByteArray, dataPtr:uint, dataHash:uint, flags:uint):void
        {
            // FIXME: build an actual VertexBuffer3D
            //var buffer:DataBuffer = acquireBufferFromPool(numElements, data32PerVertext, target)
            if (log) log.send("glEnd()")

            // FIXME (egeorgie): refactor into the VertexBufferbuilder
            const data32PerVertex:int = 12 // x, y, z,  r, g, b, a,  nx, ny, nz, tx, ty 
            
            // Number of Vertexes
            if (count == 0)
            {
                if (log) log.send("0 vertices, no-op")
                return
            }
            
            var b:VertexBuffer3D
            if (activeCommandList)
            {
                b = this.context.createVertexBuffer(count, data32PerVertex)
                b.uploadFromByteArray(data, dataPtr, 0, count)
            }
            else 
            {
                b = immediateVertexBuffers.acquire(dataHash, count, data, dataPtr)
                if (!b)
                {
                    b = immediateVertexBuffers.allocateOrReuse(dataHash, count, data, dataPtr, context)
                }
            }       
            
            var cl:CommandList = activeCommandList
            
            // If we don't have a list, create a temporary one and execute it on glEndList()
            if (!cl)
            {
                cl = reusableCommandList
                cl.executeOnCompile = true
                cl.commands.length = 0
                cl.activeState = null
            }
            
            var stream:VertexStream = new VertexStream()
            stream.vertexBuffer = b
            //stream.indexBuffer = indexBuffer
            stream.vertexFlags = flags
            stream.polygonOffset = isGLState(ENABLE_POLYGON_OFFSET)
            
            setupIndexBuffer(stream, mode, count)
            
            // Remember whether we need to generate texture coordiantes on the fly,
            // we'll use that value later on to pick the right shader when we render the list
            
            if (enableTexGenS)
            {
                if (texGenParamS == GL_SPHERE_MAP)
                    stream.vertexFlags |= VertexBufferBuilder.TEX_GEN_S_SPHERE
                else
                    if (log) log.send("[Warning] Unsupported glTexGen mode for GL_S: 0x" + texGenParamS.toString(16))
            }
            
            if (enableTexGenT)
            {
                if (texGenParamT == GL_SPHERE_MAP)
                    stream.vertexFlags |= VertexBufferBuilder.TEX_GEN_T_SPHERE
                else
                    if (log) log.send("[Warning] Unsupported glTexGen mode for GL_S: 0x" + texGenParamT.toString(16))
            }

            // Make sure that if we have any active state changes, we push them in front of the stream commands
            if (cl.activeState)
            {
                cl.commands.push(cl.activeState)
                cl.activeState = null
            }
            
            cl.commands.push(stream)
            
            if (!activeCommandList)
            {
                if (log) log.send("Rendering Immediate Vertex Stream ")
                executeCommandList(cl)
            }
        }
        
        private function setGLState(bit:uint):void
        {
            glStateFlags |= (1 << bit)
        }
        
        private function clearGLState(bit:uint):void
        {
            glStateFlags &= ~(1 << bit)
        }
        
        private function isGLState(bit:uint):Boolean
        {
            return 0 != (glStateFlags & (1 << bit))
        }

        private function getFixedFunctionPipelineKey(flags:uint):String
        {
            var key:String = flags.toString() + glStateFlags.toString();

            if (0 != (flags & VertexBufferBuilder.HAS_TEXTURE2D))
            {
                for(var i:int=0; i<8; i++)
                {
                    key = key.concat("ti", i,",")
                     var ti:TextureInstance = textureSamplers[i]
                     if (ti) {
                         var textureParams:TextureParams = ti.params
                        key = key.concat((textureParams ? textureParams.GL_TEXTURE_WRAP_S : 0), ",",
                                        (textureParams ? textureParams.GL_TEXTURE_WRAP_T : 0), ",",
                                        (textureParams ? textureParams.GL_TEXTURE_MIN_FILTER : 0), ",",
                                        (ti.mipLevels > 1 ? 1 : 0))
                    }
                }
                
            }
            return key
        }
    
        private function ensureProgramUpToDate(stream:VertexStream):void
        {
            var flags:uint = stream.vertexFlags
            var key:String = getFixedFunctionPipelineKey(flags)
            if (log) log.send("program key is:" + key)
            
            if (!stream.program || stream.program.key != key)
                stream.program = getFixedFunctionPipelineProgram(key, flags)
        }
        
        private function getFixedFunctionPipelineProgram(key:String, flags:uint):FixedFunctionProgramInstance
        {
            var p:FixedFunctionProgramInstance = fixed_function_programs[ key ]
            
            if (!p)
            {
                p = new FixedFunctionProgramInstance()
                p.key = key
                fixed_function_programs[key] = p
                
                p.program = context.createProgram()
                p.hasTexture = contextEnableTextures &&
                               ((0 != (flags & VertexBufferBuilder.HAS_TEXTURE2D)) ||
                                (0 != (flags & VertexBufferBuilder.TEX_GEN_S_SPHERE) && 0 != (flags & VertexBufferBuilder.TEX_GEN_T_SPHERE)))

                var textureParams:TextureParams = null
                var ti:TextureInstance
                if (p.hasTexture)
                {
                    // FIXME (egeorgie): Assume sampler 0
                    ti = textureSamplers[0]
                    if (ti)
                        textureParams = ti.params
                }

                // For all Vertex shaders:
                //
                // va0 - position
                // va1 - color
                // va2 - normal
                // va3 - texture coords
                //
                // vc0,1,2,3 - modelViewProjection
                // vc4,5,6,7 - modelView
                // vc8,9,10,11 - inverse modelView
                // vc12, 13, 14, 15 - texture matrix
                // vc16 - (0, 0.5, 1.0, 2.0)
                // vc17 - current color state (to be used when vertex color is not specified)
                // vc18 - clipPlane0 
                // vc19 - clipPlane1 
                // vc20 - clipPlane2 
                // vc21 - clipPlane3 
                // vc22 - clipPlane4 
                // vc23 - clipPlane5
                //
                // v6, v7 - reserved for clipping
                
                // For all Fragment shaders 
                // v4 - reserved for specular color
                // v5 - reserved for incoming color (either per-vertex color or the current color state) 
                // v6 - dot(clipPlane0, pos), dot(clipPlane1, pos), dot(clipPlane2, pos), dot(clipPlane3, pos) 
                // v7 - dot(clipPlane4, pos), dot(clipPlane5, pos) 
                //

                const _vertexShader_Color_Flags:uint = 0//VertexBufferBuilder.HAS_COLOR
                const _vertexShader_Color:String = [
                    "m44 op, va0, vc0",     // multiply vertex by modelViewProjection
                ].join("\n")

                const _debugShader_Color:String = [
                    "m44 op, va0, vc0",     // multiply vertex by modelViewProjection
                    "mov v0, va1",          // copy the vertex color to be interpolated per fragment
                    "mov v0, vc16", // solid blue for debugging
                ].join("\n")

                const _fragmentShader_Color:String = [
                    "mov ft0, v5",
                    "add ft0.xyz, ft0.xyz, v4.xyz",                 // add specular color
                    "mov oc, ft0",           // output the interpolated color
                ].join("\n")
                

                const _vertexShader_Texture_Flags:uint = VertexBufferBuilder.HAS_TEXTURE2D
                const _vertexShader_Texture:String = [
                    "m44 op, va0, vc0",     // multiply vertex by modelViewProjection
                    "m44 v1, va3, vc12",    // multiply texture coords by texture matrix
                ].join("\n")

                const _fragmentShader_Texture:String = [
                    "tex ft0, v1, fs0 <2d, wrapMode, minFilter> ",     // sample the texture
                    "mul ft0, ft0, v5",                             // modulate with the interpolated color (hardcoding GL_TEXTURE_ENV_MODE to GL_MODULATE)
                    "add ft0.xyz, ft0.xyz, v4.xyz",                 // add specular color
                    "mov oc, ft0",                                  // output interpolated color.                             
                ].join("\n")

//                for(i=0 i<total i++)
//                {
//                    myEyeVertex = MatrixTimesVector(ModelviewMatrix, myVertex[i])
//                    myEyeVertex = Normalize(myEyeVertex)
//                    myEyeNormal = VectorTimesMatrix(myNormal[i], InverseModelviewMatrix)
//                    reflectionVector = myEyeVertex - myEyeNormal * 2.0 * dot3D(myEyeVertex, myEyeNormal)
//                    reflectionVector.z += 1.0
//                    m = 1.0 / (2.0 * sqrt(dot3D(reflectionVector, reflectionVector)))
//                    //I am emphasizing that we write to s and t. Used to sample a 2D texture.
//                    myTexCoord[i].s = reflectionVector.x * m + 0.5
//                    myTexCoord[i].t = reflectionVector.y * m + 0.5
//                }
                
                
                // For all Vertex shaders:
                //
                // va0 - position
                // va1 - color
                // va2 - normal
                // va3 - texture coords
                //
                // vc0,1,2,3 - modelViewProjection
                // vc4,5,6,7 - modelView
                // vc8,9,10,11 - inverse modelView
                // vc12, 13, 14, 15 - texture matrix
                // vc16 - (0, 0.5, 1.0, 2.0)
                //
                
                const _vertexShader_GenTexSphereST_Flags:uint = VertexBufferBuilder.HAS_NORMAL
                const _vertexShader_GenTexSphereST:String = [
                    "m44 op, va0, vc0",     // multiply vertex by modelViewProjection
                    
                    "m44 vt0, va0, vc4",        // eyeVertex = vt0 = pos * modelView 
                    "nrm vt0.xyz, vt0",         // normalize vt0
                    "m44 vt1, va2, vc8",        // eyeNormal = vt1 = normal * inverse modelView
                    "nrm vt1.xyz, vt1",
                    
                    // vt2 = vt0 - vt1 * 2 * dot(vt0, vt1):
                    "dp3 vt4.x, vt0, vt1",          // vt4.x = dot(vt0, vt1)     
                    "mul vt4.x, vt4.x, vc16.w",     // vt4.x *= 2.0
                    "mul vt4, vt1, vt4.x",   // vt4 = vt1 * 2.0 * dot (vt0, vt1)
                    "sub vt2, vt0, vt4",    // 
                    "add vt2.z, vt2.z, vc16.z", // vt2.z += 1.0
                    // vt2 is the reflectionVector now

                    // m = vt4.x = 1 / (2.0 * sqrt(dot3D(reflectionVector, reflectionVector))
                    "dp3 vt4.x, vt2, vt2",
                    "sqt vt4.x, vt4.x",
                    "mul vt4.x, vt4.x, vc16.w",
                    "rcp vt4.x, vt4.x",
                    // vt4.x is m now 

                    // myTexCoord[i].s = reflectionVector.x * m + 0.5
                    // myTexCoord[i].t = reflectionVector.y * m + 0.5
                    "mul vt3.x, vt2.x, vt4.x",
                    "add vt3.x, vt3.x, vc16.y",  // += 0.5 
                    "mul vt3.y, vt2.y, vt4.x",
                    "add vt3.y, vt3.y, vc16.y",  // += 0.5

                    // zero-out the rest z & w
                    "mov vt3.z, vc16.x",
                    "mov vt3.w, vc16.x",

                    // copy the texture coordiantes to be interpolated per fragment
                    "mov v1, vt3",          
                   // "mov v1, va2",          // copy the vertex color to be interpolated per fragment
                ].join("\n")
                
                const _fragmentShader_GenTexSphereST:String = [
                    "tex ft0, v1, fs0 <2d, wrapMode, minFilter> ",     // sample the texture 
                    "mul ft0, ft0, v5",                             // modulate with the interpolated color (hardcoding GL_TEXTURE_ENV_MODE to GL_MODULATE)
                    "add ft0.xyz, ft0.xyz, v4.xyz",                 // add specular color
                    "mov oc, ft0",
                ].join("\n")   

                var vertexShader:String
                var fragmentShader:String

                if (p.hasTexture)
                {
                    if (0 != (flags & VertexBufferBuilder.TEX_GEN_S_SPHERE) &&
                        0 != (flags & VertexBufferBuilder.TEX_GEN_T_SPHERE))
                    {
                        if (log) log.send("using reflection shaders...")
                        vertexShader = _vertexShader_GenTexSphereST
                        p.vertexStreamUsageFlags = _vertexShader_GenTexSphereST_Flags
                        fragmentShader = _fragmentShader_GenTexSphereST
                    }
                    else if (0 != (flags & VertexBufferBuilder.HAS_TEXTURE2D))
                    {
                        if (log) log.send("using texture shaders...")
                        vertexShader = _vertexShader_Texture
                        p.vertexStreamUsageFlags = _vertexShader_Texture_Flags
    
                        if (textureParams.GL_TEXTURE_WRAP_S != textureParams.GL_TEXTURE_WRAP_T)
                        {
                            if (log) log.send("[Warning] Unsupported different texture addressing modes for S and T: 0x" + 
                                textureParams.GL_TEXTURE_WRAP_S.toString(16) + ", 0x" +
                                textureParams.GL_TEXTURE_WRAP_T.toString(16))
                        }
    
                        if (textureParams.GL_TEXTURE_WRAP_S != GL_REPEAT && textureParams.GL_TEXTURE_WRAP_S != GL_CLAMP)
                        {
                            if (log) log.send("[Warning] Unsupported texture wrap mode: 0x" + textureParams.GL_TEXTURE_WRAP_S.toString(16))
                        }
    
                        var wrapModeS:String = (textureParams.GL_TEXTURE_WRAP_S == GL_REPEAT) ? "repeat" : "clamp"
                        fragmentShader = _fragmentShader_Texture.replace("wrapMode", wrapModeS)

                        if(log) log.send("mipmapping levels " + ti.mipLevels)

                        if (ti.mipLevels > 1) {
                            fragmentShader = fragmentShader.replace("minFilter", "linear, miplinear, -2.0")
                        } else if(textureParams.GL_TEXTURE_MIN_FILTER == GL_NEAREST) {
                            fragmentShader = fragmentShader.replace("minFilter", "nearest")
                        } else {
                            fragmentShader = fragmentShader.replace("minFilter", "linear")
                        }
                    }
                }
                else
                {
                    if (log) log.send("using color shaders...")
                    vertexShader = _vertexShader_Color
                    p.vertexStreamUsageFlags = _vertexShader_Color_Flags
                    fragmentShader = _fragmentShader_Color
                }
                
                // CALCULATE VERTEX COLOR
                var useVertexColor:Boolean = (0 != (flags & VertexBufferBuilder.HAS_COLOR))
                if (useVertexColor)
                    p.vertexStreamUsageFlags |= VertexBufferBuilder.HAS_COLOR
                
                if (contextEnableLighting)
                {
                    
                    // va0 - position
                    // va1 - color
                    // va2 - normal
                    // va3 - texture coords
                    //
                    // vc0,1,2,3 - modelViewProjection
                    // vc4,5,6,7 - modelView
                    // vc8,9,10,11 - inverse modelView
                    // vc12, 13, 14, 15 - texture matrix
                    // vc16 - (0, 0.5, 1.0, 2.0)
                    // vc17 - current color state (to be used when vertex color is not specified)
                    // vc18-vc23 - clipPlanes
                    // vc24 - viewpoint (origin of eyespace)
                    // vc25 - mat_ambient
                    // vc26 - mat_diffuse
                    // vc27 - mat_specular
                    // vc28 - mat_shininess (in the form [shininess, 0, 0, 0])
                    // vc29 - mat_emission
                    // vc30 - global ambient lighting
                    // vc31 - light 0 position (in eye-space)
                    // vc32 - light 0 ambient
                    // vc33 - light 0 diffuse
                    // vc34 - light 0 specular
                    // vc35-38 - light 1
                    // vc39-42 - light 2
                    // vc43-46 - light 3
                    // vc47-50 - light 4
                    // vc51-54 - light 5
                    // vc55-58 - light 6
                    // vc59-62 - light 7
                    //
                    // v6, v7 - reserved for clipping
                    
                    // vertex color = 
                    //    emissionmaterial + 
                    //    ambientlight model * ambientmaterial +
                    //    [ambientlight *ambientmaterial +
                    //     (max { L · n , 0} ) * diffuselight * diffusematerial +
                    //     (max { s · n , 0} )shininess * specularlight * specularmaterial ] per light. 
                    // vertex alpha = diffuse material alpha
                    
                    p.vertexStreamUsageFlags |= VertexBufferBuilder.HAS_NORMAL
                    
                    // matColorReg == ambient and diffuse material color to use
                    var matAmbReg:String = (contextColorMaterial) ?
                                                ((useVertexColor) ? "va1" : "vc17") : "vc25"
                    var matDifReg:String = (contextColorMaterial) ? 
                                                ((useVertexColor) ? "va1" : "vc17") : "vc26"
                    
                    // FIXME (klin): Need to refactor to take into account multiple lights...
                    /*var lightingShader:String = [
                        "mov vt0, vc29",                   // start with emission material
                        "add vt0, vt0, " + matAmbReg,      // add ambient material color
                        "add vt0, vt0, " + matDifReg,      // add diffuse material color
                        "mov vt0.w, " + matDifReg + ".w",  // alpha = diffuse material alpha
                        "sat vt0, vt0",                    // clamp to 0 or 1
                        "mov v5, vt0",
                    ].join("\n")*/
                    
                    // v5 = vt3 will be used to calculate the final color.
                    // v4 = vt7 is the specular color if contextSeparateSpecular == true
                    //      otherwise, specular is included in v5.
                    var lightingShader:String = [
                        // init v4 to 0
                        "mov v4.xyzw, vc16.xxxx",
                        
                        // calculate some useful constants
                        // vt0 = vertex in eye space
                        // vt1 = normalized normal vector in eye space
                        // vt2 = |V| = normalized vector from origin of eye space to vertex
                        "m44 vt0, va0, vc4",               // vt0 = vertex in eye space
                        "mov vt1, va2",                    // vt1 = normal vector
                        "m33 vt1.xyz, vt1, vc4",           // vt1 = normal vector in eye space
                        "nrm vt1.xyz, vt1",                // vt1 = n = norm(normal vector)
                        "neg vt2, vt0",                    // vt2 = V = origin - vertex in eye space  
                        "nrm vt2.xyz, vt2",                // vt2 = norm(V)
                        
                        // general lighting
                        "mov vt3, vc29",                   // start with emission material
                        "mov vt4, vc30",                   // vt4 = global ambient light
                        "mul vt4, vt4, " + matAmbReg,      // global ambientlight model * ambient material
                        "add vt3, vt3, vt4",               // add ambient color from global light
                        
                        // Light specific calculations
                        
                        // Initialize temp for specular
                        "mov vt7, vc16.xxxx",              // vt7 is specular, will end in v4
                        
                        //   ambient color
//                        "mov vt4, vc32",
//                        "mul vt4, vt4, " + matAmbReg,      // ambientlight0 * ambientmaterial
//                        "add vt3, vt3, vt4",               // add ambient color from light0
//                        
//                        //   diffuse color
//                        "sub vt4, vc31, vt0",              // vt4 = L = light0 pos - vertex pos
//                        "nrm vt4.xyz, vt4",                // vt4 = norm(L)
//                        "mov vt5, vt1",
//                        "dp3 vt5.x, vt4, vt5",             // vt5.x = L · n
//                        "max vt5.x, vt5.x, vc16.x",        // vt5.x = max { L · n , 0}
//                        "neg vt6.x, vt5.x",                // check if L · n is <= 0 
//                        "slt vt6.x, vt6.x, vc16.x",
//                        "mul vt5.xyz, vt5.xxx, vc33.xyz",  // vt5 = vt5.x * diffuselight0
//                        "mul vt5, vt5, " + matDifReg,      // vt0 = vt0 * diffusematerial
//                        "add vt3, vt3, vt5",               // add diffuse color from light0
//                        
//                        //   specular color
//                        "add vt5, vt4, vt2",               // vt5 = s = L + V
//                        "nrm vt5.xyz, vt5",                // vt5 = norm(s)
//                        "dp3 vt5.x, vt5, vt1",             // vt5.x = s · n
//                        "max vt5.x, vt5.x, vc16.x",        // vt5.x = max { s · n , 0}
//                        "pow vt5.x, vt5.x, vc28.x",        // vt5.x = max { s · n , 0}^shininess
//                        "max vt5.x, vt5.x, vc16.x",        // make sure vt5 is not negative.
//                        "mul vt5.xyz, vt5.xxx, vc34.xyz",  // vt5 = vt5.x * specularlight0
//                        "mul vt5, vt5, vc27",              // vt5 = vt5 * specularmaterial
//                        "mul vt5, vt5.xyz, vt6.xxx",       // specular = 0 if L · n is <= 0.
                        
//                        "sat vt5, vt5",
//                        "mov v4, vt5",                     // specular is separate and added later.
//
//                        //"add vt3, vt3, vt5",               // add specular color from light0
//                        
//                        // alpha determined by diffuse material
//                        "mov vt3.w, " + matDifReg + ".w",  // alpha = diffuse material alpha
//                        
//                        "sat vt3, vt3",                    // clamp to 0 or 1
//                        "mov v5, vt3",                     // v5 = final color
                    ].join("\n")
                    
                    if (!lightsEnabled[0] && !lightsEnabled[1])
                        if (log) log.send("GL_LIGHTING enabled, but no lights are enabled...")
                    
                    // concatenate shader for each light
                    for (var i:int = 0; i < 8; i++)
                    {
                        if (!lightsEnabled[i])
                            continue
                        
                        var l:Light = lights[i]
                        var starti:int = 31 + i*4
                        var lpos:String = "vc" + starti.toString()
                        var lamb:String = "vc" + (starti+1).toString()
                        var ldif:String = "vc" + (starti+2).toString()
                        var lspe:String = "vc" + (starti+3).toString()
                        
    					var lightVectorAgalInstr:String;
						if (l.type == Light.LIGHT_TYPE_DIRECTIONAL)
						{
							lightVectorAgalInstr = "mov vt4, " + lpos;
						}
						else	// Assume point light
						{
							lightVectorAgalInstr = "sub vt4, " + lpos + ", vt0";
						}

                        var lightpiece:String = [
                            //   ambient color
                            "mov vt4, " + lamb,
                            "mul vt4, vt4, " + matAmbReg,      // ambientlight0 * ambientmaterial
                            "add vt3, vt3, vt4",               // add ambient color from light0
                            
                            //   diffuse color
                            lightVectorAgalInstr,    		   // vt4 = L = light0 pos - vertex pos
                            "nrm vt4.xyz, vt4",                // vt4 = norm(L)
                            "mov vt5, vt1",
                            "dp3 vt5.x, vt4, vt5",             // vt5.x = L · n
                            "max vt5.x, vt5.x, vc16.x",        // vt5.x = max { L · n , 0}
                            "neg vt6.x, vt5.x",                // check if L · n is <= 0 
                            "slt vt6.x, vt6.x, vc16.x",
                            "mul vt5.xyz, vt5.xxx, " + ldif + ".xyz",  // vt5 = vt5.x * diffuselight0
                            "mul vt5, vt5, " + matDifReg,      // vt0 = vt0 * diffusematerial
                            "add vt3, vt3, vt5",               // add diffuse color from light0
                            
                            //   specular color
                            "add vt5, vt4, vt2",               // vt5 = s = L + V
                            "nrm vt5.xyz, vt5",                // vt5 = norm(s)
                            "dp3 vt5.x, vt5, vt1",             // vt5.x = s · n
                            "max vt5.x, vt5.x, vc16.x",        // vt5.x = max { s · n , 0}
                            "pow vt5.x, vt5.x, vc28.x",        // vt5.x = max { s · n , 0}^shininess
                            "max vt5.x, vt5.x, vc16.x",        // make sure vt5 is not negative.
                            "mul vt5.xyz, vt5.xxx, " + lspe + ".xyz",  // vt5 = vt5.x * specularlight0
                            "mul vt5, vt5, vc27",              // vt5 = vt5 * specularmaterial
                            "mul vt5, vt5.xyz, vt6.xxx",       // specular = 0 if L · n is <= 0.
                            "add vt7, vt7, vt5",               // add specular to output (will be in v4)
                        ].join("\n")
                        
                        lightingShader = lightingShader + "\n" + lightpiece
                    }
                    
                    lightingShader = lightingShader + "\n" + [
                        "sat vt7, vt7",
                        "mov v4, vt7",                     // specular is separate and added later.

                        // alpha determined by diffuse material
                        "mov vt3.w, " + matDifReg + ".w",  // alpha = diffuse material alpha
                        
                        "sat vt3, vt3",                    // clamp to 0 or 1
                        "mov v5, vt3",                     // v5 = final color
                    ].join("\n")
                    
                    if (useVertexColor)
                        lightingShader = "mov vt0, va1\n" + lightingShader //HACK
                    vertexShader = lightingShader + "\n" + vertexShader
                }
                else if (useVertexColor)
                {
                    // Color should come from the vertex buffer
                    // also init v4 to 0.
                    vertexShader = "mov v4.xyzw, vc16.xxxx\n" + "mov v5, va1" + "\n" + vertexShader
                }
                else
                {
                    // Color should come form the current color
                    // also init v4 to 0.
                    vertexShader = "mov v4.xyzw, vc16.xxxx\n" + "mov v5, vc17" + "\n" + vertexShader
                }

                // CLIPPING
                var clippingOn:Boolean = clipPlaneEnabled[0] || clipPlaneEnabled[1] || clipPlaneEnabled[2] || clipPlaneEnabled[3] || clipPlaneEnabled[4] || clipPlaneEnabled[5]
                if (clippingOn)
                {
                    // va0 - position
                    // va1 - color
                    // va2 - normal
                    // va3 - texture coords
                    //
                    // vc0,1,2,3 - modelViewProjection
                    // vc4,5,6,7 - modelView
                    // vc8,9,10,11 - inverse modelView
                    // vc12, 13, 14, 15 - texture matrix
                    // vc16 - (0, 0.5, 1.0, 2.0)
                    // vc17 - current color state (to be used when vertex color is not specified)
                    // vc18 - clipPlane0 
                    // vc19 - clipPlane1 
                    // vc20 - clipPlane2 
                    // vc21 - clipPlane3 
                    // vc22 - clipPlane4 
                    // vc23 - clipPlane5
                    //
                    // v6, v7 - reserved for clipping
                    
                    // For all Fragment shaders 
                    //
                    // v6 - dot(clipPlane0, pos), dot(clipPlane1, pos), dot(clipPlane2, pos), dot(clipPlane3, pos) 
                    // v7 - dot(clipPlane4, pos), dot(clipPlane5, pos) 
                    //
                    const clipVertex:String = [
                        "m44 vt0, va0, vc4",        // position in eye (modelVeiw) space
                        "dp4 v6.x, vt0, vc18",       // calculate clipPlane0 
                        "dp4 v6.y, vt0, vc19",       // calculate clipPlane1 
                        "dp4 v6.z, vt0, vc20",       // calculate clipPlane2 
                        "dp4 v6.w, vt0, vc21",       // calculate clipPlane3 
                        "dp4 v7.x, vt0, vc22",       // calculate clipPlane4 
                        "dp4 v7.yzw, vt0, vc23",       // calculate clipPlane5 
                    ].join("\n")

                    const clipFragment:String = [
                        "min ft0.x, v6.x, v6.y",
                        "min ft0.y, v6.z, v6.w",
                        "min ft0.z, v7.x, v7.y",
                        "min ft0.w, ft0.x, ft0.y",
                        "min ft0.w, ft0.w, ft0.z",
                        "kil ft0.w",
                    ].join("\n")

                    vertexShader = clipVertex + "\n" + vertexShader
                    fragmentShader = clipFragment + "\n" + fragmentShader
                }

                if(log) {
                log.send("vshader:\n" + vertexShader)
                log.send("fshader:\n" + fragmentShader)
                }

                // FIXME (egeorgie): cache the agalcode?
                var vsAssembler:AGALMiniAssembler = new AGALMiniAssembler
                vsAssembler.assemble(Context3DProgramType.VERTEX, vertexShader)
                var fsAssembler:AGALMiniAssembler = new AGALMiniAssembler
                fsAssembler.assemble(Context3DProgramType.FRAGMENT, fragmentShader)
                p.program.upload(vsAssembler.agalcode, fsAssembler.agalcode)
            }
            return p
        }

        public function glColor(r:Number, g:Number, b:Number, alpha:Number):void
        {
            // Change current color if we're not recording a command
            if (!activeCommandList)
            {
                contextColor[0] = r                
                contextColor[1] = g                
                contextColor[2] = b                
                contextColor[3] = alpha                
            }
        }

        public function glNewList(id:uint, mode:uint):void
        {
            // Allocate and active a new CommandList
            if (log) log.send("glNewList : " + id + ", compileAndExecute = " + (mode == GL_COMPILE_AND_EXECUTE).toString())
            activeCommandList = new CommandList()
            activeCommandList.executeOnCompile = (mode == GL_COMPILE_AND_EXECUTE)
            commandLists[id] = activeCommandList
        }
    
        public function glEndList():void
        {
            // Make sure if we have any pending state changes, we push them as a command at the end of the list
            if (activeCommandList.activeState)
            {
                activeCommandList.commands.push(activeCommandList.activeState)
                activeCommandList.activeState = null
            }
            
            if (activeCommandList.executeOnCompile)
                executeCommandList(activeCommandList)
            
            // We're done with this list, it's no longer active
            activeCommandList = null
        }

        public function glCallList(id:uint):void
        {
            if (log) log.send("glCallList")
            if (activeCommandList)
                if (log) log.send("Warning: Calling a command list while building a command list not yet implemented.")
            
            if (log) log.send("Rendering List " + id)
            executeCommandList(commandLists[id])
        }

        public function GLAPI(context:Context3D, log:Object, stage:Stage):void
        {
            // For the debug console
            _stage = stage

            //this.log = new TraceLog()
            this.context = context

            // id zero is null
            vertexBufferObjects.push(null);

            const indices:Array = [
                        0,1,2,
                        3,2,1,
                        4,0,6,
                        6,0,2,
                        5,1,4,
                        4,1,0,
                        7,3,1,
                        7,1,5,
                        5,4,7,
                        7,4,6,
                        7,2,3,
                        7,6,2];

            const vertices:Array = [    
                        [1.0,1.0,1.0],
                        [-1.0,1.0,1.0],
                        [1.0,-1.0,1.0], 
                        [-1.0,-1.0,1.0],    
                        [1.0,1.0,-1.0],
                        [-1.0,1.0,-1.0],    
                        [1.0,-1.0,-1.0],    
                        [-1.0,-1.0,-1.0]];

            cubeVertexData = new Vector.<Number>();
            var si:int=36;
            for (var i:int=0;i<si;i+=3)
            {
                const v1:Array = vertices[indices[i]];
                const v2:Array = vertices[indices[i+1]];
                const v3:Array = vertices[indices[i+2]];
                cubeVertexData.push(v1[0], v1[1], v1[2], 1, 1, 1, 1, 0, 0, 0, 0, 0);
                cubeVertexData.push(v2[0], v2[1], v2[2], 1, 1, 1, 1, 0, 0, 0, 0, 0);
                cubeVertexData.push(v3[0], v3[1], v3[2], 1, 1, 1, 1, 0, 0, 0, 0, 0);        
            }
        }

        public function glClear(mask:uint):void
        {
            if (log) log.send( "glClear called with " + mask)

            contextClearMask = 0
            if (mask & GL_COLOR_BUFFER_BIT) contextClearMask |= Context3DClearMask.COLOR
            if (mask & GL_STENCIL_BUFFER_BIT) contextClearMask |= Context3DClearMask.STENCIL
            if (mask & GL_DEPTH_BUFFER_BIT) contextClearMask |= Context3DClearMask.DEPTH
            
            if (bgColorOverride)
            {
                context.clear(overrideR / 255.0, overrideG / 255.0, overrideB / 255.0, overrideA / 255.0,
                    contextClearDepth, contextClearStencil, contextClearMask)
            }
            else
            {
                context.clear(contextClearR, contextClearG, contextClearB, contextClearA,
                    contextClearDepth, contextClearStencil, contextClearMask)
            }

            // Make sure the vertex buffer pool knows it's next frame already to enable recycling
            immediateVertexBuffers.nextFrame()
        }
        
        public function glClearColor(red:Number, green:Number, blue:Number, alpha:Number):void
        {
            if (log) log.send("[IMPLEMENTED] glClearColor " + red + " " + green + " " + blue + " " + alpha + "\n")
    
            contextClearR = red
            contextClearG = green
            contextClearB = blue
            contextClearA = alpha
        }
        
        public function glActiveTexture(index:uint):void
        {
            var unitIndex:uint = index - GL_TEXTURE0
            if(unitIndex <= 31) {
                activeTextureUnit = unitIndex
                // log.send( "[IMPLEMENTED] glActiveTexture " + activeTextureUnit + "\n")
            } else {
                // log.send( "[NOTE] Invalid texture unit requested " + uint)
            }
        }
        
        public function glBindTexture(type:uint, texture:uint):void
        {
            textureSamplerIDs[activeTextureUnit] = texture

            if(genOnBind){
                if(textures[texture] == null) {
                    textures[texture] = new TextureInstance()
                    textures[texture].texID = texture
                }
            } else if (texture == 0) {
                // FIXME (egeorgie): just set the sampler to null and clear the active texture params?
                if (log) log.send("Trying bind the non-existent texture 0!")
                return
            }
            
            if (log) log.send( "[IMPLEMENTED] glBindTexture " + type + " " + texture + ", tu: " + activeTextureUnit + "\n")

            if (activeCommandList)
            {
                if (log) log.send("Recording texture " + texture + " for the active list.")
                
                var activeState:ContextState = activeCommandList.ensureActiveState()                
                activeState.textureSamplers[activeTextureUnit] = texture
                
                // FIXME (egeorgie): we should not execute here, but only while executing the lsit
                // return
            }
            
            activeTexture = textures[texture]
            activeTexture.boundType = type
            textureSamplers[activeTextureUnit] = activeTexture
            
            if (type != GL_TEXTURE_2D && type != GL_TEXTURE_CUBE_MAP)
            {
                if (log) log.send( "[NOTE] Unsupported texture type " + type + " for glBindTexture")
            }
        }

        private function glCullModeToContext3DTriangleFace(mode:uint, frontFaceClockWise:Boolean):String
        {
            switch (mode)
            {
                case GL_FRONT: //log.send("culling=GL_FRONT") 
                    return frontFaceClockWise ? Context3DTriangleFace.FRONT : Context3DTriangleFace.BACK
                case GL_BACK: //log.send("culling=GL_BACK")
                    return frontFaceClockWise ? Context3DTriangleFace.BACK : Context3DTriangleFace.FRONT
                case GL_FRONT_AND_BACK: //log.send("culling=GL_FRONT_AND_BACK")
                    return Context3DTriangleFace.FRONT_AND_BACK
                default:
                    if (log) log.send("Unsupported glCullFace mode: 0x" + mode.toString(16))
                    return Context3DTriangleFace.NONE
            }
        }

        public function glCullFace(mode:uint):void
        {
            if (log) log.send("glCullFace")

            if (activeCommandList)
                if (log) log.send("[Warning] Recording glCullMode as part of command list not yet implememnted")
            
            this.glCullMode = mode

            // culling affects the context3D stencil 
            commitStencilState()
            
            if (contextEnableCulling)
                context.setCulling(disableCulling ? Context3DTriangleFace.NONE: glCullModeToContext3DTriangleFace(glCullMode, frontFaceClockWise))         
        }

        public function glFrontFace(mode:uint):void
        {
            if (log) log.send("glFrontFace")

            if (activeCommandList)
                if (log) log.send("[Warning] Recording glFrontFace as part of command list not yet implememnted")
            
            frontFaceClockWise = (mode == GL_CW)
            
            // culling affects the context3D stencil 
            commitStencilState()
            
            if (contextEnableCulling)
                context.setCulling(disableCulling ? Context3DTriangleFace.NONE : glCullModeToContext3DTriangleFace(glCullMode, frontFaceClockWise))         
        }

        public function glEnable(cap:uint):void
        {
            if (log) log.send( "[IMPLEMENTED] glEnable 0x" + cap.toString(16) + "\n")
            switch (cap)
            {
                case GL_DEPTH_TEST:
                    contextEnableDepth = true
                    context.setDepthTest(contextDepthMask, contextDepthFunction)
                    break
                case GL_CULL_FACE:
                    if (!contextEnableCulling)
                    {
                        contextEnableCulling = true
                        context.setCulling(disableCulling ? Context3DTriangleFace.NONE : glCullModeToContext3DTriangleFace(glCullMode, frontFaceClockWise))
                        
                        // Stencil depends on culling
                        commitStencilState()
                    }
                    break
                case GL_STENCIL_TEST:
                    if (!contextEnableStencil)
                    {
                        contextEnableStencil = true
                        commitStencilState()
                    }
                    break
                case GL_SCISSOR_TEST:
                    if (!contextEnableScissor)
                    {
                        contextEnableScissor = true
                        if(!scissorRect)
                            scissorRect = new Rectangle(0,0,contextWidth,contextHeight)

                        context.setScissorRectangle(scissorRect)
                    }
                    break
                case GL_ALPHA_TEST:
                    if (!contextEnableAlphaTest)
                    {
                        contextEnableAlphaTest = true
                    }
                    break
                case GL_BLEND:
                    contextEnableBlending = true
                    if(!disableBlending)
                        context.setBlendFactors(contextSrcBlendFunc, contextDstBlendFunc)
                    break
                
                case GL_TEXTURE_GEN_S:
                    enableTexGenS = true
                break
                
                case GL_TEXTURE_GEN_T:
                    enableTexGenT = true
                break
                
                case GL_CLIP_PLANE0:
                case GL_CLIP_PLANE1:
                case GL_CLIP_PLANE2:
                case GL_CLIP_PLANE3:
                case GL_CLIP_PLANE4:
                case GL_CLIP_PLANE5:
                    var clipPlaneIndex:int = cap - GL_CLIP_PLANE0
                    clipPlaneEnabled[clipPlaneIndex] = true
                    setGLState(ENABLE_LIGHT_OFFSET + clipPlaneIndex)
                break
                
                case GL_TEXTURE_2D:
                    contextEnableTextures = true
                    setGLState(ENABLE_TEXTURE_OFFSET)
                    break
                
                case GL_LIGHTING:
                    contextEnableLighting = true
                    setGLState(ENABLE_LIGHTING_OFFSET)
                    break
                
                case GL_COLOR_MATERIAL:
                    contextColorMaterial = true // default is GL_FRONT_AND_BACK and GL_AMBIENT_AND_DIFFUSE
                    setGLState(ENABLE_COLOR_MATERIAL_OFFSET)
                    break
                
                case GL_LIGHT0:
                case GL_LIGHT1:
                case GL_LIGHT2:
                case GL_LIGHT3:
                case GL_LIGHT4:
                case GL_LIGHT5:
                case GL_LIGHT6:
                case GL_LIGHT7:
                    var lightIndex:int = cap - GL_LIGHT0
                    if (lights[lightIndex] == null)
                    {
                        lights[lightIndex] = new Light(true, lightIndex == 0)
                    }
                    lightsEnabled[lightIndex] = true
                    setGLState(ENABLE_LIGHT_OFFSET + lightIndex)
                    break
                
                case GL_POLYGON_OFFSET_FILL:
                    contextEnablePolygonOffset = true
                    setGLState(ENABLE_POLYGON_OFFSET)
                    break
                
                default:
                    if (log) log.send( "[NOTE] Unsupported cap for glEnable: 0x" + cap.toString(16) )
            }
        }
        
        public function glDisable(cap:uint):void
        {
            if (log) log.send( "[IMPLEMENTED] glDisable 0x" + cap.toString(16) + "\n")
            switch (cap)
            {
                case GL_DEPTH_TEST:
                    contextEnableDepth = false
                    context.setDepthTest(false, Context3DCompareMode.ALWAYS)
                    break
                case GL_CULL_FACE:
                    if (contextEnableCulling)
                    {
                        contextEnableCulling = false
                        context.setCulling(Context3DTriangleFace.NONE)

                        // Stencil depends on culling
                        commitStencilState()
                    }
                    break
                case GL_STENCIL_TEST:
                    if (contextEnableStencil)
                    {
                        contextEnableStencil = false
                        commitStencilState()
                    }
                    break
                case GL_SCISSOR_TEST:
                    if (contextEnableScissor)
                    {
                        contextEnableScissor = false
                        context.setScissorRectangle(new Rectangle(0,0,contextWidth,contextHeight))
                    }
                    break
                case GL_ALPHA_TEST:
                    if (!contextEnableAlphaTest)
                    {
                        contextEnableAlphaTest = false
                    }
                    break
                case GL_BLEND:
                    contextEnableBlending = false
                    if(!disableBlending)
                        context.setBlendFactors(Context3DBlendFactor.ONE, Context3DBlendFactor.ZERO)
                    break

                case GL_TEXTURE_GEN_S:
                    enableTexGenS = false
                    break
                
                case GL_TEXTURE_GEN_T:
                    enableTexGenT = false
                    break

                case GL_CLIP_PLANE0:
                case GL_CLIP_PLANE1:
                case GL_CLIP_PLANE2:
                case GL_CLIP_PLANE3:
                case GL_CLIP_PLANE4:
                case GL_CLIP_PLANE5:
                    var clipPlaneIndex:int = cap - GL_CLIP_PLANE0
                    clipPlaneEnabled[clipPlaneIndex] = false
                    clearGLState(ENABLE_LIGHT_OFFSET + clipPlaneIndex)
                    break
                
               case GL_TEXTURE_2D:
                    contextEnableTextures = false
                    clearGLState(ENABLE_TEXTURE_OFFSET)
                    break
               
               case GL_LIGHTING:
                   contextEnableLighting = false
                   clearGLState(ENABLE_LIGHTING_OFFSET)
                   break
               
               case GL_COLOR_MATERIAL:
                   contextColorMaterial = false // default is GL_FRONT_AND_BACK and GL_AMBIENT_AND_DIFFUSE
                   clearGLState(ENABLE_COLOR_MATERIAL_OFFSET)
                   break
               
               case GL_LIGHT0:
               case GL_LIGHT1:
               case GL_LIGHT2:
               case GL_LIGHT3:
               case GL_LIGHT4:
               case GL_LIGHT5:
               case GL_LIGHT6:
               case GL_LIGHT7:
                   var lightIndex:int = cap - GL_LIGHT0
                   lightsEnabled[lightIndex] = false
                   clearGLState(ENABLE_LIGHT_OFFSET + lightIndex)
                   break
               
               case GL_POLYGON_OFFSET_FILL:
                   contextEnablePolygonOffset = false
                   clearGLState(ENABLE_POLYGON_OFFSET)
                   break
                
                default:
                    if (log) log.send( "[NOTE] Unsupported cap for glDisable: 0x" + cap.toString(16) )
            }
        }

        public function glPushAttrib(mask:uint):void
        {
            if (log) log.send("glPushAttrib + 0x" + mask.toString(16))
            var bits:String = null
            
            for (var i:int = 0; i < GL_ATTRIB_BIT.length; i++)
            {
                if (mask & (1 << i))
                    bits = bits + ", " + GL_ATTRIB_BIT[i]
            }
            
            if (mask & GL_LIGHTING_BIT)
            {
                pushCurrentLightingState()
            }
            
            if (log) log.send( "[NOTE] Unsupported attrib bits " + bits + " for glPushAttrib" )
        }
        
        public function glPopAttrib():void
        {
            // only lighting state for now.
            popCurrentLightingState()         
        }

        private function pushCurrentLightingState():void
        {
            var lState:LightingState = new LightingState()
            lState.enableColorMaterial = this.contextColorMaterial
            lState.enableLighting = this.contextEnableLighting
            lState.lightsEnabled = this.lightsEnabled.concat()
            
            var newLights:Vector.<Light> = new Vector.<Light>(8)
            var lightsLength:int = this.lights.length
            for (var i:int = 0; i < lightsLength; i++)
            {
                var l:Light = this.lights[i]
                newLights[i] = (l) ? l.createClone() : null
            }
            
            lState.lights = newLights
            lState.contextMaterial = this.contextMaterial.createClone()
            lightingStates.push(lState)
        }
        
        private function popCurrentLightingState():void
        {
            var lState:LightingState = lightingStates.pop()
            if(lState == null) {
              if (log) log.send("[WARNING] Calling popCurrentLightingState with lighting state")
              return   
            }
            this.contextColorMaterial = lState.enableColorMaterial
            this.contextEnableLighting = lState.enableLighting
            this.lightsEnabled = lState.lightsEnabled
            this.lights = lState.lights
            this.contextMaterial = lState.contextMaterial
        }

        public function glTexEnvf(target:uint, pname:uint, param:Number):void
        {
            if (!activeTexture)
            {
                if (log) log.send("[WARNING] Calling glTexEnvf with no active texture")
                return
            }

            var textureParams:TextureParams = activeTexture.params
            if (log) log.send("[WARNING] Calling glTexEnvf with unsupported pname " + pname + ", " + param)
            switch(pname)
            {
                case GL_TEXTURE_ENV_MODE:
                    textureParams.GL_TEXTURE_ENV_MODE = param
                    break
                default:
                    if (log) log.send("[WARNING] Calling glTexEnvf with unsupported pname " + pname + ", " + param)
            }
        }

        public function glTexParameterf(target:uint, pname:uint, param:Number):void
        {
            if (log) log.send( "[IMPLEMENTED] glTexParameteri " + target + " 0x" + pname.toString(16) + " 0x" + param.toString(16) + "\n")
            
            if (!activeTexture)
            {
                if (log) log.send("[WARNING] Calling glTexParameteri with no active texture")
                return
            }

            var textureParams:TextureParams = activeTexture.params

            switch (pname)
            {
                case GL_TEXTURE_MIN_LOD:
                    textureParams.GL_TEXTURE_MIN_LOD = param
                break
                case GL_TEXTURE_MAX_LOD:
                    textureParams.GL_TEXTURE_MAX_LOD = param
                break
                case GL_TEXTURE_MIN_FILTER:
                    textureParams.GL_TEXTURE_MIN_FILTER = param
                break
                case GL_TEXTURE_MAG_FILTER:
                    textureParams.GL_TEXTURE_MAG_FILTER = param
                break

                default:
                    if (log) log.send( "[NOTE] Unsupported pname 0x" + pname.toString(16) + " for glTexParameterf" + (target == GL_TEXTURE_2D ? "(2D)" : "(Cube)"))
            }
        }

        public function glTexParameteri(target:uint, pname:uint, param:int):void
        {
            if (log) log.send( "[IMPLEMENTED] glTexParameteri " + target + " 0x" + pname.toString(16) + " 0x" + param.toString(16) + "\n")
            
            if (!activeTexture)
            {
                if (log) log.send("[WARNING] Calling glTexParameteri with no active texture")
                return
            }

            var textureParams:TextureParams = activeTexture.params

            switch (pname)
            {
                case GL_TEXTURE_MAX_ANISOTROPY_EXT:
                    textureParams.GL_TEXTURE_MAX_ANISOTROPY_EXT = param
                break

                case GL_TEXTURE_MAG_FILTER:
                    textureParams.GL_TEXTURE_MAG_FILTER = param
                break
                
                case GL_TEXTURE_MIN_FILTER:
                    textureParams.GL_TEXTURE_MIN_FILTER = param
                break

                case GL_TEXTURE_WRAP_S:
                    textureParams.GL_TEXTURE_WRAP_S = param
                    if (log) log.send("Setting GL_TEXTURE_WRAP_S to: 0x" + param.toString(16)) 
                break
                
                case GL_TEXTURE_WRAP_T:
                    textureParams.GL_TEXTURE_WRAP_T = param
                    if (log) log.send("Setting GL_TEXTURE_WRAP_S to: 0x" + param.toString(16)) 
                break
                
                default:
                    if (log) log.send( "[NOTE] Unsupported pname 0x" + pname.toString(16) + " for glTexParameteri" + (target == GL_TEXTURE_2D ? "(2D)" : "(Cube)"))
            }
        }
        
        private function pixelTypeToString(type:uint):String
        {
            if (type == GL_BITMAP)
                return PIXEL_TYPE[type - GL_BITMAP]
            else if (type <= GL_FLOAT)
                return PIXEL_TYPE[type - GL_BYTE]
            else if (type <= GL_BGRA)
                return PIXEL_TYPE[type - GL_BGR]
            else
                return PIXEL_TYPE[type - GL_UNSIGNED_BYTE_3_3_2]
        }

        private function convertPixelDataToBGRA(width:int, height:int, srcFormat:uint, src:ByteArray, srcOffset:uint):ByteArray
        {
            //var srcBytesPerPixel:int
            var pixelCount:int = width * height
            var dst:ByteArray = new ByteArray()
            dst.length = pixelCount * 4 // BGRA is 4 bytes
            
            var originalPosition:uint = src.position
            src.position = srcOffset
            
            var b:int = 0
            var g:int = 0
            var r:int = 0
            var a:int = 0xFF // fully opaque by default (for conversions from formats that don't have alpha)
            //var a:int = 100 // transparent for debugging

            for (var i:int = 0; i < pixelCount; i++)
            {
                switch (srcFormat)
                {
                    case GL_RGBA:
                        r = src.readByte()
                        g = src.readByte()
                        b = src.readByte()
                        a = src.readByte()
                    break
                    
                    case GL_RGB:
                        r = src.readByte()
                        g = src.readByte()
                        b = src.readByte()
                    break

                    default:
                        if (log) log.send("[Warning] Unsupported texture format: " + PIXEL_FORMAT[srcFormat - GL_COLOR_INDEX])
                        return dst
                }
                
                // BGRA
                dst.writeByte(b)
                dst.writeByte(g)
                dst.writeByte(r)
                dst.writeByte(a)
            }

            // restore the position so the function doesn't have side-effects
            src.position = originalPosition
            return dst
        }

        public function glTexSubImage2D(target:uint, level:int, xoff:int, yoff:int, width:int, height:int, format:uint, imgType:uint, ptr:uint, ram:ByteArray):void
        {
            if (log) log.send( "glTexSubImage2D " + target + " l:" + level + " " + xoff + " " + yoff + " " + width + "x" + height +  
                      PIXEL_FORMAT[format - GL_COLOR_INDEX] + " " + pixelTypeToString(imgType) + " " + ptr.toString(16) + "\n")

            if(activeTexture && activeTexture.texture) {
                activeTexture.texture.dispose()
                textures[textureSamplerIDs[activeTextureUnit]] = null
                glBindTexture(target, textureSamplerIDs[activeTextureUnit])
            }

            glTexImage2D(target, level, format, width, height, 0, format, imgType, ptr, ram)
        }

        public function glTexImage2D(target:uint, level:int, intFormat:int, width:int, height:int, border:int, format:uint, imgType:uint, ptr:uint, ram:ByteArray):void
        {
            if (log) log.send( "[IMPLEMENTED] glTexImage2D " + target + " texid: " + textureSamplerIDs[activeTextureUnit] + " l:" + level + " " + intFormat + " " + width + "x" + height + " b:" + border + " " + 
                      PIXEL_FORMAT[format - GL_COLOR_INDEX] + " " + pixelTypeToString(imgType) + " " + imgType.toString(16) + "\n")

            if (intFormat == GL_LUMINANCE)
            {
                // Unsupported. TODO - Squelch all PF_G8 textures.
                width = width/2
                height = height/2
            }
            
            if (width == 0 || height == 0) 
                return
            
            // Context3D only supports BGRA and COMPRESSED formats
            var data:ByteArray
            var dataOffset:uint
            if (format != GL_BGRA)
            {
                // Convert the texture format
                data = convertPixelDataToBGRA(width, height, format, ram, ptr)
                dataOffset = 0
            }
            else
            {
                data = ram
                dataOffset = ptr
            }

            // Create appropriate texture type and upload data.
            if (target == GL_TEXTURE_2D)
            {
                create2DTexture(width, height, level, data, dataOffset)
            }
            else if (target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
            {
                createCubeTexture(width, target, level, data, dataOffset)
            }
            else 
            {
                if (log) log.send( "[NOTE] Unsupported texture type " + target + " for glCompressedTexImage2D")
            }
        }
        
        public function glCompressedTexImage2D(target:uint, level:int, intFormat:uint, width:int, height:int, border:int, imageSize:int, ptr:uint, ram:ByteArray):void
        {
            // Create appropriate texture type and upload data.
            if (target == GL_TEXTURE_2D)
                create2DTexture(width, height, level, ram, ptr, (intFormat == 2 || intFormat == 3), true)
            else if (target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
                createCubeTexture(width, target, level, ram, ptr, (intFormat == 2 || intFormat == 3), true)
            else {
                if (log) log.send( "[NOTE] Unsupported texture type " + target + " for glCompressedTexImage2D")
            }
        }

        // Returns index of first texture, guaranteed to be contiguous
        public function glGenTextures(length:uint):uint
        {
            var result:uint = texID
            if (log) log.send( "[IMPLEMENTED] glGenTextures " + length + ", returning ID = [ " + result + ", " + (result + length - 1) + " ]\n")
            for (var i:int = 0; i < length; i++) {
                textures[texID] = new TextureInstance()
                textures[texID].texID = texID
                texID++
            }
            return result
        }

        public function glDeleteTexture(texid:uint)
        {
            if(textures[texid] == null) {
                if (log) log.send( "[WARNING] glDeleteTexture called on non-existant texture " + texid + "\n")
                return
            }

            if (log) log.send( "glDeleteTexture called for " + texid + "\n")

            if(textures[texid].texture)
                textures[texid].texture.dispose()

            if(textures[texid].cubeTexture)
                textures[texid].cubeTexture.dispose()

            textures[texid] = null // TODO: fix things so we can eventually reuse textureIDs
        }

        public function glColorMask(red:Boolean, green:Boolean, blue:Boolean, alpha:Boolean):void
        {
            if (log) log.send( "[IMPLEMENTED] glColorMask " + red + " " + green + " " + blue + " " + alpha + "\n")
            context.setColorMask(red, green, blue, alpha)  
        }
        
        private function stencilOpToContext3DStencilAction(op:uint):String
        {
            switch (op)
            {
                case GL_ZERO: return Context3DStencilAction.ZERO
                case GL_KEEP: return Context3DStencilAction.KEEP
                case GL_REPLACE: return Context3DStencilAction.SET
                case GL_INCR: return Context3DStencilAction.INCREMENT_SATURATE
                case GL_DECR: return Context3DStencilAction.DECREMENT_SATURATE
                case GL_INVERT: return Context3DStencilAction.INVERT
                case GL_INCR_WRAP: return Context3DStencilAction.INCREMENT_WRAP
                case GL_DECR_WRAP: return Context3DStencilAction.DECREMENT_WRAP
                default:
                    if (log) log.send("[Warning] Unknown stencil op: 0x" + op.toString(16))
                    return null
            }
        }
        
        private function commitStencilState():void
        {
            if (contextEnableStencil)
            {
                var triangleFace:String = contextEnableCulling ? glCullModeToContext3DTriangleFace(glCullMode, !frontFaceClockWise) : Context3DTriangleFace.FRONT_AND_BACK
                context.setStencilActions(triangleFace, 
                    contextStencilCompareMode, 
                    contextStencilActionPass, 
                    contextStencilActionDepthFail, 
                    contextStencilActionStencilFail)
            }
            else
            {
                // Reset to default
                context.setStencilActions(Context3DTriangleFace.FRONT_AND_BACK, 
                    Context3DCompareMode.ALWAYS, 
                    Context3DStencilAction.KEEP, 
                    Context3DStencilAction.KEEP, 
                    Context3DStencilAction.KEEP)
            }
        }
        
        public function glStencilOp(fail:uint, zfail:uint, zpass:uint):void
        {
            if (log) log.send("glStencilOp")
            contextStencilActionStencilFail = stencilOpToContext3DStencilAction(fail)
            contextStencilActionDepthFail = stencilOpToContext3DStencilAction(zfail)
            contextStencilActionPass = stencilOpToContext3DStencilAction(zpass)
            commitStencilState()
        }
        
        //extern void glStencilFunc (GLenum func, GLint ref, GLuint mask):void
        public function glStencilFunc(func:uint, ref:int, mask:uint):void
        {
            if (log) log.send("glStencilFunc")
            contextStencilCompareMode = convertCompareMode(func)
            context.setStencilReferenceValue(ref, mask, mask)
            commitStencilState()
        }

        public function glScissor(x:int, y:int, width:int, height:int):void
        {
            if(log) log.send("glScissor " + x + ", " + y + ", " + width + ", " + height)
            scissorRect = new Rectangle(x, y, x + width, y + height)
            if(contextEnableScissor)
                context.setScissorRectangle(scissorRect)
        }

        public function glViewport(x:int, y:int, width:int, height:int):void
        {
            // Not natively supported on this platform. Emulate with a scissor and VS scale/bias.
        }

        public function glDepthRangef(near:Number, far:Number):void
        {
            // if (log) log.send( "[STUBBED] glDepthRangef " + near + " " + far + "\n")         
        }

        public function glClearDepth(depth:Number):void
        {
            // if (log) log.send( "[IMPLEMENTED] glClearDepthf " + depth + "\n")   
            contextClearDepth = depth          
        }

        public function glClearStencil(s:int):void
        {
            // if (log) log.send( "[IMPLEMENTED] glClearStencil " + s + "\n")
            contextClearStencil = s                
        }

        private function translateBlendFactor( openGLBlendFactor:uint ): String
        {
            if ( openGLBlendFactor == GL_ONE )
            {
                return Context3DBlendFactor.ONE
            }
            else if ( openGLBlendFactor == GL_ZERO )
            {
                return Context3DBlendFactor.ZERO
            }
            else if ( openGLBlendFactor == GL_SRC_ALPHA )
            {
                return Context3DBlendFactor.SOURCE_ALPHA
            }
            else if ( openGLBlendFactor == GL_ONE_MINUS_SRC_ALPHA )
            {
                return Context3DBlendFactor.ONE_MINUS_SOURCE_ALPHA
            }
            else if ( openGLBlendFactor == GL_DST_ALPHA )
            {
                return Context3DBlendFactor.DESTINATION_ALPHA
            }
            else if ( openGLBlendFactor == GL_ONE_MINUS_DST_ALPHA )
            {
                return Context3DBlendFactor.ONE_MINUS_DESTINATION_ALPHA
            }
            else if ( openGLBlendFactor == GL_SRC_COLOR )
            {
                return Context3DBlendFactor.SOURCE_COLOR
            }
            else if ( openGLBlendFactor == GL_ONE_MINUS_SRC_COLOR )
            {
                return Context3DBlendFactor.ONE_MINUS_SOURCE_COLOR
            }
            else if ( openGLBlendFactor == GL_DST_COLOR )
            {
                return Context3DBlendFactor.DESTINATION_COLOR
            }
            else if ( openGLBlendFactor == GL_ONE_MINUS_DST_COLOR )
            {
                return Context3DBlendFactor.ONE_MINUS_DESTINATION_COLOR
            }
            return Context3DBlendFactor.ONE
        }
        
        public function glBlendFunc(sourceFactor:uint, destinationFactor:uint):void
        {
            contextSrcBlendFunc = translateBlendFactor(sourceFactor)
            contextDstBlendFunc = translateBlendFactor(destinationFactor)

            if (log) log.send("glBlendFunc " + contextSrcBlendFunc + ", " + contextDstBlendFunc)
            
            if (contextEnableBlending && !disableBlending)
                context.setBlendFactors(contextSrcBlendFunc, contextDstBlendFunc)
        }

        public function glBlendFuncSeparate(srcRGB:uint, dstRGB:uint,srcAlpha:uint, dstAlpha:uint):void
        {
            contextSrcBlendFunc = translateBlendFactor( srcRGB )
            contextDstBlendFunc = translateBlendFactor( dstRGB )

            if(srcRGB == GL_ONE && dstRGB == GL_ONE && srcAlpha == GL_ZERO && dstAlpha == GL_ONE) 
            {
            }
            else if(srcRGB == GL_SRC_ALPHA && dstRGB == GL_ONE && srcAlpha == GL_ZERO && dstAlpha == GL_ONE) 
            {
            }
            else if(srcRGB == GL_SRC_ALPHA && dstRGB == GL_ONE_MINUS_SRC_ALPHA && srcAlpha == GL_ZERO && dstAlpha == GL_ONE) 
            {
            }
            else if (srcRGB == GL_DST_COLOR && dstRGB == GL_ZERO && srcAlpha == GL_ZERO && dstAlpha == GL_ONE)
            {
            }
            else if (srcRGB == GL_DST_COLOR && dstRGB == GL_ZERO && srcAlpha == GL_ONE && dstAlpha == GL_ZERO)
            {
            }
            else if ( srcRGB != srcAlpha || dstRGB != dstAlpha )
            {
                if (log) log.send("glBlendFuncSeparate missing blend func: srcRGB = " + srcRGB + ", drtRGB = " + dstRGB + ", srcA = " + srcAlpha + ", dstA = " + dstAlpha)
            }
            
            if (contextEnableBlending && !disableBlending)
            {
                context.setBlendFactors(contextSrcBlendFunc, contextDstBlendFunc)
            }
        }
        
        // ======================================================================
        //  Functions
        // ----------------------------------------------------------------------

        protected function create2DTexture(width:int, height:int, level:int, data:ByteArray, dataOff:uint, compressed:Boolean=false, compressedUpload:Boolean=false):void
        {
            var instance:TextureInstance = activeTexture
            if (!instance)
            {
                if (log) log.send( "[NOTE] No previously bound texture for glTexImage2D (2D)")
                return
            }

            if (!instance.texture)
            {
                //trace("Compressed is " + compressed ? Context3DTextureFormat.COMPRESSED : Context3DTextureFormat.BGRA)
                instance.texture = 
                    context.createTexture(width, height, compressed ? Context3DTextureFormat.COMPRESSED : Context3DTextureFormat.BGRA, dataOff == 0 ? true : false)
                
                textureSamplers[activeTextureUnit] = instance
            }

            if(level >= instance.mipLevels) {
                instance.mipLevels++
            } else {
                if (log) log.send( "[NOTE] glTexImage2D replacing mip...")
            }
            
            // FIXME (egeorgie) - we need a boolean param instead?
            //if (dataOff != 0)
            {
                if (compressedUpload)
                    instance.texture.uploadCompressedTextureFromByteArray(data, dataOff)
                else
                {
                    instance.texture.uploadFromByteArray(data, dataOff, level)
                }
            }
        }
        
        protected function createCubeTexture(width:int, target:uint, level:int, data:ByteArray, dataOff:uint, compressed:Boolean=false, compressedUpload:Boolean=false):void
        {
            var instance:TextureInstance = activeTexture
            if (instance)
            {
                if (!instance.cubeTexture)
                {
                    instance.cubeTexture = 
                        context.createCubeTexture(width, compressed ? Context3DTextureFormat.COMPRESSED : Context3DTextureFormat.BGRA, false)
                    
                    textureSamplers[activeTextureUnit] = instance
                }
                    
                var side:int = target - GL_TEXTURE_CUBE_MAP_POSITIVE_X
                
                if (compressedUpload)
                    instance.cubeTexture.uploadCompressedTextureFromByteArray(data, dataOff)
                else
                    instance.cubeTexture.uploadFromByteArray(data, dataOff, side, level)
            }
            else 
                if (log) log.send( "[NOTE] No previously bound texture for glCompressedTexImage2D (2D)")
        }
    }
}

import GLS3D.GLAPI;
import flash.display3D.*;
import flash.display3D.textures.*;
import flash.geom.*;
import flash.utils.*;

class BufferPool    
{
    public var framestamp:uint = 0
    public var idx:uint = 0
    public var buffers:Vector.<DataBuffer> = new Vector.<DataBuffer>()

    public function getBuffer(fs:uint):DataBuffer
    {
        if(framestamp != fs) {
            framestamp = fs
            idx = 0
        }

        if(idx >= buffers.length)
            buffers.push(new DataBuffer())

        return buffers[idx++]            
    }
}

internal class DataBuffer 
{
    public var id:uint
    public var target:uint
    public var usage:uint
    public var data:ByteArray
    public var size:uint
    public var indicesSize:uint
    public var stride:uint
    public var uploaded:Boolean
    public var indexBuffer:IndexBuffer3D
    public var vertexBuffer:VertexBuffer3D
    public var inUse:Boolean

    public function DataBuffer() {
        data = new ByteArray()
        data.endian = "littleEndian"
    }
}

class TextureInstance
{
    public var texture:Texture
    public var cubeTexture:CubeTexture
    public var mipLevels:uint
    public var params:TextureParams = new TextureParams()
    public var boundType:uint
    public var texID:uint
}

class TextureParams
{
    public var GL_TEXTURE_MAX_ANISOTROPY_EXT:Number = -1
    public var GL_TEXTURE_MAG_FILTER:Number = GLAPI.GL_LINEAR
    public var GL_TEXTURE_MIN_FILTER:Number = GLAPI.GL_NEAREST_MIPMAP_LINEAR
    public var GL_TEXTURE_MIN_LOD:Number = -1000.0
    public var GL_TEXTURE_MAX_LOD:Number = 1000.0
    public var GL_TEXTURE_WRAP_S:uint = GLAPI.GL_REPEAT
    public var GL_TEXTURE_WRAP_T:uint = GLAPI.GL_REPEAT
    public var GL_TEXTURE_ENV_MODE:uint = GLAPI.GL_MODULATE
}

class VertexBufferAttribute
{
    public var offset:uint
    public var buffer:DataBuffer
    public var stride:uint
    public var size:uint
    public var type:uint
    public var normalize:Boolean
    public var enabled:Boolean = false
    public var isGeneric:Boolean = true
}

class ProgramInstance
{
    public var program:Program3D
    public var id:uint
    public var activeSamplers:Vector.<Boolean> = new Vector.<Boolean>(8)
    public var vertexConstantsDirty:Boolean = true
    public var vertexConstantsData:Vector.<Number> = new Vector.<Number>()
    public var fragmentConstantsDirty:Boolean = true
    public var fragmentConstantsData:Vector.<Number> = new Vector.<Number>(4)
    public var vertexStreamIndicies:Vector.<Boolean> = new Vector.<Boolean>(128)

    public function updateConstants(context:Context3D):void
    {
        if (vertexConstantsDirty && vertexConstantsData.length) {
                    context.setProgramConstantsFromVector( Context3DProgramType.VERTEX, 0, vertexConstantsData)
            vertexConstantsDirty = false
        }
        if (fragmentConstantsDirty && fragmentConstantsData.length) {
                    context.setProgramConstantsFromVector( Context3DProgramType.FRAGMENT, 0, fragmentConstantsData)
            fragmentConstantsDirty = false
        }
    }
}

class VertexBufferBuilder
{
    public static const HAS_COLOR:uint      = 0x00000001
    public static const HAS_TEXTURE2D:uint  = 0x00000002
    public static const HAS_NORMAL:uint     = 0x00000004
    public static const TEX_GEN_S_SPHERE:uint   = 0x00000008
    public static const TEX_GEN_T_SPHERE:uint   = 0x00000010
}

class FixedFunctionProgramInstance
{
    public var program:Program3D
    public var vertexStreamUsageFlags:uint = 0
    public var hasTexture:Boolean = false
    public var key:String
}

class TraceLog
{
    public function send(value:String):void
    {
        trace(value)
    }
}

/**
 *  Represents the vertices as defined between calls of glBeing() and glEnd().
 *  Holds and instance to the associated shader program.
 */
class VertexStream
{
    public var vertexBuffer:VertexBuffer3D
    public var indexBuffer:IndexBuffer3D
    public var vertexFlags:uint
    public var program:FixedFunctionProgramInstance
    public var polygonOffset:Boolean = false
}

/**
 *  Represents consequtive context state changes as defined between calls of glNewList() and glEndList().
 *  A single CommandList can have multiple context state changes.  
 */
class ContextState
{
    public var textureSamplers:Vector.<int>// = new Vector.<uint>(8)
    public var material:Material
}

/**
 *  Records of the OpenGL commands between calls of glNewList() and glEndList().  
 */
class CommandList
{
    // Used during building, move out?
    public var executeOnCompile:Boolean = false
    public var activeState:ContextState = null

    // Storage
    public var commands:Vector.<Object> = new Vector.<Object>()
    
    public function ensureActiveState():ContextState
    {
        if (!activeState)
        {
            activeState = new ContextState()
            activeState.textureSamplers = new Vector.<int>(8)
            for (var i:int = 0; i < 8; i++)
            {
                activeState.textureSamplers[i] = -1 // Set to 'undefined'
            }
            
            activeState.material = new Material() // don't initialize, so we know what has changed.
        }
        return activeState
    }
}

class Light
{
    public static const LIGHT_TYPE_POINT:uint		= 0;
    public static const LIGHT_TYPE_DIRECTIONAL:uint	= 1;
    public static const LIGHT_TYPE_SPOT:uint		= 2;		// Not supported

    public var position:Vector.<Number>
    public var ambient:Vector.<Number>
    public var diffuse:Vector.<Number>
    public var specular:Vector.<Number>
    public var type:uint

    // FIXME (klin): No spotlight for now...neverball doesn't use it
    
    public function Light(init:Boolean = false, isLight0:Boolean = false)
    {
        if (init)
        {
            position = new <Number>[0, 0, 1, 0]
            ambient = new <Number>[0, 0, 0, 1]
            diffuse = (isLight0) ? new <Number>[1, 1, 1, 1] :
                                   new <Number>[0, 0, 0, 1]
            specular = (isLight0) ? new <Number>[1, 1, 1, 1] :
                                    new <Number>[0, 0, 0, 1]
            type = LIGHT_TYPE_POINT;
        }
    }
    
    public function createClone():Light
    {
        var clone:Light = new Light(false)
        clone.position = (position) ? position.concat() : null
        clone.ambient = (ambient) ? ambient.concat() : null
        clone.diffuse = (diffuse) ? diffuse.concat() : null
        clone.specular = (specular) ? specular.concat() : null
        clone.type = type;
        return clone
    }
}

class Material
{
    public var ambient:Vector.<Number>
    public var diffuse:Vector.<Number>
    public var specular:Vector.<Number>
    public var shininess:Number
    public var emission:Vector.<Number>

    public function Material(init:Boolean = false)
    {
        // If init is true, we initialize to default values.
        if (init)
        {
            ambient = new <Number>[0.2, 0.2, 0.2, 1.0]
            diffuse = new <Number>[0.8, 0.8, 0.8, 1.0]
            specular = new <Number>[0.0, 0.0, 0.0, 1.0]
            shininess = 0.0
            emission = new <Number>[0.0, 0.0, 0.0, 1.0]
        }
    }
    
    public function createClone():Material
    {
        var clone:Material = new Material(false)
        clone.ambient = (ambient) ? ambient.concat() : null
        clone.diffuse = (diffuse) ? diffuse.concat() : null
        clone.specular = (specular) ? specular.concat() : null
        clone.shininess = shininess
        clone.emission = (emission) ? emission.concat() : null
        return clone
    }
}

class LightingState
{
    public var enableColorMaterial:Boolean // GL_COLOR_MATERIAL enable bit
    public var enableLighting:Boolean // GL_LIGHTING enable bit
    public var lightsEnabled:Vector.<Boolean>
    public var lights:Vector.<Light>
    public var contextMaterial:Material
}

class BufferNode
{
    public var buffer:VertexBuffer3D
    public var prev:int
    public var next:int
    public var count:uint
    public var hash:uint
}

class VertexBufferPool
{
    private var hashToIndex:Dictionary = new Dictionary()
    private var bufferToIndex:Dictionary = new Dictionary(true)
    private var buffers:Vector.<BufferNode> = new Vector.<BufferNode>()
    private var tail:int = -1
    private var head:int = -1
    private var prevFrame:int = -1 // index of MRU node previous frame
    private var prevPrevFrame:int = -1 // index of MRU node two frames ago
    
    public function acquire(hash:uint, count:uint, data:ByteArray, dataPtr:uint):VertexBuffer3D
    {
//        // Debug:
//        var h:uint = calcHash(count, data, dataPtr)
//        if (h != hash)
//            trace("Hashes don't match: " + hash + " " + h)
//        else
//            trace("Hashes match: " + hash)
        
        if (!(hash in hashToIndex))
            return null
        
        var index:int = hashToIndex[hash]
        var node:BufferNode = buffers[index]
        if (node.count != count)
            throw("Collision in count " + node.count + " != " + count)

//        // Debug:
//        var src:ByteArray = node.src
//        src.position = 0
//        data.position = dataPtr
//        for (var i:int = 0; i < src.length / 4; i++)
//            if (src.readUnsignedInt() != data.readUnsignedInt())
//            {
//                trace("Collision in data at vertex " + (i / 12) + ", offset " + (i % 12))
//                // print out the source & dst data
//                {
//                    src.position = 0
//                    data.position = dataPtr
//                    for (i = 0; i < src.length / 4; i++)
//                    {
//                        var value:Number = src.readFloat()
//                        var value1:Number = data.readFloat()
//                        if (value != value1)
//                            trace("Difference: at position " + i + ": " + value + " != " + value1) 
//                    }
//                    
//                    // Calculate improved hash function:
//                    src.position = 0
//                    data.position = dataPtr
//                    var hash1:uint = calcHash(count, src, 0)
//                    var hash2:uint = calcHash(count, data, dataPtr)
//                    trace("Computed Hashes are " + hash1 + " (stored data), " + hash2 + " (new data), stored hash is " + node.hash)
//                    return node.buffer
//                }
//            }

        return node.buffer  
    }

    // Debug:
    static public function calcHash(count:uint, data:ByteArray, dataPtr:uint):uint
    {
        const offset_basis:uint = 2166136261
        // 32 bit FNV_prime = 224 + 28 + 0x93 = 16777619

        const prime:uint = 16777619
        var hash:uint = offset_basis

        data.position = dataPtr
        for (var i:int = 0; i < count * 12 * 4; i++)
        {
            var v:uint = data.readUnsignedByte()
            
            hash = hash ^ v
            hash = hash * prime
        }
        return hash
    }

    public function allocateOrReuse(hash:uint, count:uint, data:ByteArray, dataPtr:uint, context:Context3D):VertexBuffer3D
    {
        var index:int = reuseBufferNode(count)
        var node:BufferNode
        if (index != -1)
        {
            node = buffers[index]
            // Remove the old entry
            delete hashToIndex[node.hash]
        }
        else
        {
            node = new BufferNode()
            node.count = count
            node.buffer = context.createVertexBuffer(count, 12)
            index = insertNode(node)
        }

//        // Debug:
//        node.src = new ByteArray()
//        node.src.endian = data.endian
//        data.position = dataPtr
//        var length:int = count * 12 * 4
//        node.src.length = length
//        node.src.position = 0
//        data.readBytes(node.src, 0, length)
//        trace("Allocating: passed on hash " + hash + ", computed hash " + calcHash(count, data, dataPtr) + ", computed on copy " + calcHash(count, node.src, 0)) 

        node.buffer.uploadFromByteArray(data, dataPtr, 0, count)
        bufferToIndex[node.buffer] = index
        hashToIndex[hash] = index
        node.hash = hash
        return node.buffer
    }
    
    private function reuseBufferNode(count:uint):int
    {
        if (prevPrevFrame == -1)
            return -1

        // Iterate backwards, starting from the tail
        var current:int = tail
        var node:BufferNode = null
        while (current != -1)
        {
            node = buffers[ current ]
            
            // Make sure we don't reuse a buffer that's been used this or last frame
            if (node.next == prevPrevFrame)
                return -1

            // Found a node with correct count
            if (node.count == count)
                return current

            current = node.prev
        }
        return -1
    }
    
    private function insertNode(node:BufferNode):int
    {
        var index:int = buffers.length
        buffers.push(node)
        if (head == -1)
        {
            tail = index
        }
        else
        {
            var headNode:BufferNode = buffers[head]
            headNode.prev = index
        }
        node.next = head
        node.prev = -1
        head = index
        return index
    }
    
    public function markInUse(buffer:VertexBuffer3D):void
    {
        if (!(buffer in bufferToIndex))
            return
        
        var index:int = bufferToIndex[buffer]
        
        // Already at the head?
        if (head == index)
            return

        var node:BufferNode = buffers[index]
        
        // Make sure we adjust the pointers for MRU last Frame and the frame before
        if (prevPrevFrame == index)
            prevPrevFrame = node.next
        if (prevFrame == index)
            prevFrame = node.next

        // Update the neighboring nodes
        var prevNode:BufferNode = node.prev != -1 ? buffers[node.prev] : null
        var nextNode:BufferNode = node.next != -1 ? buffers[node.next] : null
        if (prevNode)
            prevNode.next = node.next
        if (nextNode)
            nextNode.prev = node.prev
        
        // Update the tail
        if (tail == index)
            tail = node.prev

        // Update the head
        var headNode:BufferNode = buffers[head]
        headNode.prev = index

        // Make the node the head of the list
        node.next = head
        node.prev = -1
        head = index
    }
    
    public function nextFrame():void
    {
        prevPrevFrame = prevFrame    
        prevFrame = head
        
        // FIXME (egeorgie): cleanup for nodes at the tail if we're exceeding limit?

        //trace(print)
    }
    
    // For debugging:
    private function get print():String
    {
        var output:String = ""
        var current:int = head
        while (current != -1)
        {
            var n:String = current.toString()
            if (prevFrame == current || prevPrevFrame == current)
                output += " | " + current.toString()
            else
                output += " " + current.toString()

            var node:BufferNode = buffers[current]
            current = node.next
        }
        if (prevFrame == -1)
            output += " |"
        if (prevPrevFrame == -1)
            output += " |"
        return output
    }
}

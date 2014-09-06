
/* CullFaceMode */
public static const GL_FRONT:uint =             0x0404
public static const GL_BACK:uint =              0x0405
public static const GL_FRONT_AND_BACK:uint =    0x0408

public static const GL_TEXTURE_GEN_S:uint = 0x0C60
public static const GL_TEXTURE_GEN_T:uint = 0x0C61
public static const GL_TEXTURE_GEN_R:uint = 0x0C62
public static const GL_TEXTURE_GEN_Q:uint = 0x0C63

public static const GL_DEPTH_BUFFER_BIT:uint    = 0x00000100
public static const GL_STENCIL_BUFFER_BIT:uint = 0x00000400
public static const GL_COLOR_BUFFER_BIT:uint    = 0x00004000

public static const GL_ALPHA_TEST:uint      = 0x0BC0
public static const GL_DITHER:uint          = 0x0BD0
public static const GL_BLEND:uint           = 0x0BE2
public static const GL_STENCIL_TEST:uint    = 0x0B90
public static const GL_SCISSOR_TEST:uint    = 0x0C11
public static const GL_DEPTH_TEST:uint      = 0x0B71
public static const GL_CULL_FACE:uint       = 0x0B44
public static const GL_NORMALIZE:uint       = 0x0BA1

public static const GL_CW:uint  = 0x0900
public static const GL_CCW:uint = 0x0901

public static const GL_NEVER:uint       = 0x0200
public static const GL_LESS:uint        = 0x0201
public static const GL_EQUAL:uint       = 0x0202
public static const GL_LEQUAL:uint      = 0x0203
public static const GL_GREATER:uint     = 0x0204
public static const GL_NOTEQUAL:uint    = 0x0205
public static const GL_GEQUAL:uint      = 0x0206
public static const GL_ALWAYS:uint      = 0x0207

public static const GL_ZERO:uint                    = 0x0
public static const GL_ONE:uint                     = 0x1
public static const GL_SRC_COLOR:uint               = 0x0300
public static const GL_ONE_MINUS_SRC_COLOR:uint     = 0x0301
public static const GL_SRC_ALPHA:uint               = 0x0302
public static const GL_ONE_MINUS_SRC_ALPHA:uint     = 0x0303
public static const GL_DST_ALPHA:uint               = 0x0304
public static const GL_ONE_MINUS_DST_ALPHA:uint     = 0x0305
public static const GL_DST_COLOR:uint                = 0x0306
public static const GL_ONE_MINUS_DST_COLOR:uint        = 0x0307
public static const GL_FUNC_ADD:uint                = 0x8006

public static const GL_TEXTURE0:uint                    = 0x84C0
public static const GL_ACTIVE_TEXTURE:uint              = 0x84E0
public static const GL_TEXTURE_2D:uint                  = 0x0DE1
public static const GL_TEXTURE_CUBE_MAP:uint            = 0x8513
public static const GL_TEXTURE_MAX_ANISOTROPY_EXT:uint  = 0x84FE
public static const GL_TEXTURE_MAG_FILTER:uint          = 0x2800
public static const GL_TEXTURE_MIN_FILTER:uint          = 0x2801

public static const GL_TEXTURE_MIN_LOD:uint          = 0x813A
public static const GL_TEXTURE_MAX_LOD:uint          = 0x813B

public static const GL_TEXTURE_CUBE_MAP_POSITIVE_X:uint    = 0x8515
public static const GL_TEXTURE_CUBE_MAP_NEGATIVE_X:uint    = 0x8516
public static const GL_TEXTURE_CUBE_MAP_POSITIVE_Y:uint    = 0x8517
public static const GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:uint    = 0x8518
public static const GL_TEXTURE_CUBE_MAP_POSITIVE_Z:uint    = 0x8519
public static const GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:uint    = 0x851A

public static const GL_ARRAY_BUFFER:uint            = 0x8892
public static const GL_ELEMENT_ARRAY_BUFFER:uint    = 0x8893

public static const GL_STREAM_DRAW:uint   = 0x88E0
public static const GL_STATIC_DRAW:uint   = 0x88E4
public static const GL_DYNAMIC_DRAW:uint  = 0x88E8

/* Shaders */
public static const GL_FRAGMENT_SHADER:uint  					= 0x8B30
public static const GL_VERTEX_SHADER:uint						= 0x8B31
public static const GL_MAX_VERTEX_ATTRIBS:uint					= 0x8869
public static const GL_MAX_VERTEX_UNIFORM_VECTORS:uint			= 0x8DFB
public static const GL_MAX_VARYING_VECTORS:uint					= 0x8DFC
public static const GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS:uint    = 0x8B4D
public static const GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS:uint		= 0x8B4C
public static const GL_MAX_TEXTURE_IMAGE_UNITS:uint				= 0x8872
public static const GL_MAX_FRAGMENT_UNIFORM_VECTORS:uint		= 0x8DFD
public static const GL_SHADER_TYPE:uint							= 0x8B4F
public static const GL_DELETE_STATUS:uint						= 0x8B80
public static const GL_LINK_STATUS:uint							= 0x8B82
public static const GL_VALIDATE_STATUS:uint						= 0x8B83
public static const GL_ATTACHED_SHADERS:uint					= 0x8B85
public static const GL_ACTIVE_UNIFORMS:uint						= 0x8B86
public static const GL_ACTIVE_UNIFORM_MAX_LENGTH:uint			= 0x8B87
public static const GL_ACTIVE_ATTRIBUTES:uint					= 0x8B89
public static const GL_ACTIVE_ATTRIBUTE_MAX_LENGTH:uint			= 0x8B8A
public static const GL_SHADING_LANGUAGE_VERSION:uint			= 0x8B8C
public static const GL_CURRENT_PROGRAM:uint						= 0x8B8D

/* Shader Source */
public static const GL_COMPILE_STATUS:uint						= 0x8B81
public static const GL_INFO_LOG_LENGTH:uint						= 0x8B84
public static const GL_SHADER_SOURCE_LENGTH:uint				= 0x8B88
public static const GL_SHADER_COMPILER:uint						= 0x8DFA


public static const CDATA_FLOAT1:uint        = 1
public static const CDATA_FLOAT2:uint        = 2
public static const CDATA_FLOAT3:uint        = 3
public static const CDATA_FLOAT4:uint        = 4
public static const CDATA_MATRIX4x4:uint     = 16

public static const GL_LIGHTING_BIT:uint =                   0x00000040

public static const GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:uint = 0x83F0


/* StencilOp */
public static const GL_KEEP:uint =        0x1E00
public static const GL_REPLACE:uint =     0x1E01
public static const GL_INCR:uint =        0x1E02
public static const GL_DECR:uint =        0x1E03
public static const GL_INVERT:uint =      0x150A
public static const GL_INCR_WRAP:uint =   0x8507
public static const GL_DECR_WRAP:uint =   0x8508

// ======================================================================
//  Lighting and Materials
// ----------------------------------------------------------------------  

public static const GL_LIGHTING                   :uint = 0x0B50
public static const GL_LIGHT_MODEL_LOCAL_VIEWER   :uint = 0x0B51
public static const GL_LIGHT_MODEL_TWO_SIDE       :uint = 0x0B52
public static const GL_LIGHT_MODEL_AMBIENT        :uint = 0x0B53
public static const GL_SHADE_MODEL                :uint = 0x0B54
public static const GL_COLOR_MATERIAL_FACE        :uint = 0x0B55
public static const GL_COLOR_MATERIAL_PARAMETER   :uint = 0x0B56
public static const GL_COLOR_MATERIAL             :uint = 0x0B57
public static const GL_MODELVIEW_MATRIX           :uint = 0x0BA6

/* LightName */
public static const GL_LIGHT0:uint = 0x4000
public static const GL_LIGHT1:uint = 0x4001
public static const GL_LIGHT2:uint = 0x4002
public static const GL_LIGHT3:uint = 0x4003
public static const GL_LIGHT4:uint = 0x4004
public static const GL_LIGHT5:uint = 0x4005
public static const GL_LIGHT6:uint = 0x4006
public static const GL_LIGHT7:uint = 0x4007

/* LightParameter */
public static const GL_AMBIENT               :uint =  0x1200
public static const GL_DIFFUSE               :uint =  0x1201
public static const GL_SPECULAR              :uint =  0x1202
public static const GL_POSITION              :uint =  0x1203
public static const GL_SPOT_DIRECTION        :uint =  0x1204
public static const GL_SPOT_EXPONENT         :uint =  0x1205
public static const GL_SPOT_CUTOFF           :uint =  0x1206
public static const GL_CONSTANT_ATTENUATION  :uint =  0x1207
public static const GL_LINEAR_ATTENUATION    :uint =  0x1208
public static const GL_QUADRATIC_ATTENUATION :uint =  0x1209

/* MaterialParameter */
public static const GL_EMISSION                :uint = 0x1600
public static const GL_SHININESS               :uint = 0x1601
public static const GL_AMBIENT_AND_DIFFUSE     :uint = 0x1602
public static const GL_COLOR_INDEXES           :uint = 0x1603

/* Polygon Modes */
public static const GL_POINT                   :uint = 0x1B00
public static const GL_LINE                    :uint = 0x1B01
public static const GL_PFILL                   :uint = 0x1B02

/* Shading model */
public static const GL_FLAT                     :uint = 0x1D00
public static const GL_SMOOTH                   :uint = 0x1D01 

/* separate_specular_color */
public static const GL_LIGHT_MODEL_COLOR_CONTROL :uint = 0x81F8
public static const GL_SINGLE_COLOR              :uint = 0x81F9
public static const GL_SEPARATE_SPECULAR_COLOR   :uint = 0x81FA

/* Texture Env Modes */
public static const GL_MODULATE:uint = 0x2100
public static const GL_NEAREST:uint = 0x2600
public static const GL_LINEAR:uint = 0x2601
public static const GL_NEAREST_MIPMAP_LINEAR:uint = 0x2702
public static const GL_TEXTURE_ENV_MODE:uint = 0x2200

/* polygon_offset */
public static const GL_POLYGON_OFFSET_FACTOR:uint = 0x8038
public static const GL_POLYGON_OFFSET_UNITS :uint = 0x2A00
public static const GL_POLYGON_OFFSET_POINT :uint = 0x2A01
public static const GL_POLYGON_OFFSET_LINE  :uint = 0x2A02
public static const GL_POLYGON_OFFSET_FILL  :uint = 0x8037

/* TextureCoordName */
public static const GL_S:uint =         0x2000
public static const GL_T:uint =         0x2001
public static const GL_R:uint =         0x2002
public static const GL_Q:uint =         0x2003

public static const GL_COORD_NAME:Array = [
"GL_S",
"GL_T",
"GL_R",
"GL_Q",
]

/* TextureGenParameter */
public static const GL_TEXTURE_GEN_MODE:uint =  0x2500
public static const GL_OBJECT_PLANE:uint =      0x2501
public static const GL_EYE_PLANE:uint =         0x2502

public static const GL_PARAM_NAME:Array = [
"GL_TEXTURE_GEN_MODE",
"GL_OBJECT_PLANE",
"GL_EYE_PLANE",
]

/* param */
public static const GL_EYE_LINEAR:uint =        0x2400
public static const GL_OBJECT_LINEAR:uint =     0x2401
public static const GL_SPHERE_MAP:uint =        0x2402
public static const GL_NORMAL_MAP:uint =        0x8511
public static const GL_REFLECTION_MAP:uint =    0x8512

public static const GL_PARAM:Array = [
"GL_EYE_LINEAR",
"GL_OBJECT_LINEAR",
"GL_SPHERE_MAP",
"GL_NORMAL_MAP",
"GL_REFLECTION_MAP",
]

// For Debug purposes
public static const BEGIN_MODE:Array = [
"GL_POINTS",
"GL_LINES",
"GL_LINE_LOOP",
"GL_LINE_STRIP",
"GL_TRIANGLES",
"GL_TRIANGLE_STRIP",
"GL_TRIANGLE_FAN",
"GL_QUADS",
"GL_QUAD_STRIP",
"GL_POLYGON",
]

/* MatrixMode */
public static const GL_MODELVIEW:uint = 0x1700
public static const GL_PROJECTION:uint = 0x1701
public static const GL_TEXTURE:uint = 0x1702

// For Debug purposes
public static const MATRIX_MODE:Array = [
"GL_MODELVIEW",
"GL_PROJECTION",
"GL_TEXTURE",
]

/* BeginMode */
public static const GL_POINTS:uint           = 0x0000
public static const GL_LINES:uint            = 0x0001
public static const GL_LINE_LOOP:uint        = 0x0002
public static const GL_LINE_STRIP:uint       = 0x0003
public static const GL_TRIANGLES:uint        = 0x0004
public static const GL_TRIANGLE_STRIP:uint   = 0x0005
public static const GL_TRIANGLE_FAN:uint     = 0x0006
public static const GL_QUADS:uint            = 0x0007
public static const GL_QUAD_STRIP:uint       = 0x0008
public static const GL_POLYGON:uint          = 0x0009

/* ClipPlaneName */
public static const GL_CLIP_PLANE0:uint = 0x3000
public static const GL_CLIP_PLANE1:uint = 0x3001
public static const GL_CLIP_PLANE2:uint = 0x3002
public static const GL_CLIP_PLANE3:uint = 0x3003
public static const GL_CLIP_PLANE4:uint = 0x3004
public static const GL_CLIP_PLANE5:uint = 0x3005

public static const GL_MAX_TEXTURE_SIZE:uint =          0x0D33
public static const GL_VIEWPORT:uint =                  0x0BA2

public static const COMPARE_MODE:Array = [
"GL_NEVER",
"GL_LESS",
"GL_EQUAL",
"GL_LEQUAL",
"GL_GREATER",
"GL_NOTEQUAL",
"GL_GEQUAL",
"GL_ALWAYS",
]


/* ListMode */
public static const GL_COMPILE:uint = 0x1300
public static const GL_COMPILE_AND_EXECUTE:uint = 0x1301

private static const GL_ATTRIB_BIT:Vector.<String> = new <String>[
"GL_CURRENT_BIT",
"GL_POINT_BIT",
"GL_LINE_BIT",
"GL_POLYGON_BIT",
"GL_POLYGON_STIPPLE_BIT",
"GL_PIXEL_MODE_BIT",
"GL_LIGHTING_BIT",
"GL_FOG_BIT",
"GL_DEPTH_BUFFER_BIT",
"GL_ACCUM_BUFFER_BIT",
"GL_STENCIL_BUFFER_BIT",
"GL_VIEWPORT_BIT",
"GL_TRANSFORM_BIT",
"GL_ENABLE_BIT",
"GL_COLOR_BUFFER_BIT",
"GL_HINT_BIT",
"GL_EVAL_BIT",
"GL_LIST_BIT",
"GL_TEXTURE_BIT",
"GL_SCISSOR_BIT",
//"GL_ALL_ATTRIB_BITS",
]

// pname
public static const GL_TEXTURE_WRAP_S:uint = 0x2802
public static const GL_TEXTURE_WRAP_T:uint = 0x2803

// param
public static const GL_CLAMP:uint               = 0x2900
public static const GL_REPEAT:uint              = 0x2901
public static const GL_CLAMP_TO_EDGE:uint       = 0x812F
public static const GL_CLAMP_TO_BORDER:uint     = 0x812D
public static const GL_MIRRORED_REPEAT:uint     = 0x8370


/* PixelFormat */
public static const GL_COLOR_INDEX:uint =                    0x1900
public static const GL_STENCIL_INDEX:uint =                  0x1901
public static const GL_DEPTH_COMPONENT:uint =                0x1902
public static const GL_RED:uint =                            0x1903
public static const GL_GREEN:uint =                          0x1904
public static const GL_BLUE:uint =                           0x1905
public static const GL_ALPHA:uint =                          0x1906
public static const GL_RGB:uint =                            0x1907
public static const GL_RGBA:uint =                           0x1908
public static const GL_LUMINANCE:uint =                      0x1909
public static const GL_LUMINANCE_ALPHA:uint =                0x190A

private static const PIXEL_FORMAT:Array = [
"GL_COLOR_INDEX",
"GL_STENCIL_INDEX",
"GL_DEPTH_COMPONENT",
"GL_RED",
"GL_GREEN",
"GL_BLUE",
"GL_ALPHA",
"GL_RGB",
"GL_RGBA",
]


/* PixelType */
public static const GL_BITMAP:uint =                         0x1A00
public static const GL_BYTE:uint =                           0x1400
public static const GL_UNSIGNED_BYTE:uint =                  0x1401
public static const GL_SHORT:uint =                          0x1402
public static const GL_UNSIGNED_SHORT:uint =                 0x1403
public static const GL_INT:uint =                            0x1404
public static const GL_UNSIGNED_INT:uint =                   0x1405
public static const GL_FLOAT:uint =                          0x1406
public static const GL_BGR:uint =                            0x80E0
public static const GL_BGRA:uint =                           0x80E1
public static const GL_UNSIGNED_BYTE_3_3_2:uint =            0x8032
public static const GL_UNSIGNED_SHORT_4_4_4_4:uint =         0x8033
public static const GL_UNSIGNED_SHORT_5_5_5_1:uint =         0x8034
public static const GL_UNSIGNED_INT_8_8_8_8:uint =           0x8035
public static const GL_UNSIGNED_INT_10_10_10_2:uint =        0x8036
public static const GL_UNSIGNED_BYTE_2_3_3_REV:uint =        0x8362
public static const GL_UNSIGNED_SHORT_5_6_5:uint =           0x8363
public static const GL_UNSIGNED_SHORT_5_6_5_REV:uint =       0x8364
public static const GL_UNSIGNED_SHORT_4_4_4_4_REV:uint =     0x8365
public static const GL_UNSIGNED_SHORT_1_5_5_5_REV:uint =     0x8366
public static const GL_UNSIGNED_INT_8_8_8_8_REV:uint =       0x8367
public static const GL_UNSIGNED_INT_2_10_10_10_REV:uint =    0x8368

private static const PIXEL_TYPE:Array = [
"GL_BITMAP",
"GL_BYTE",
"GL_UNSIGNED_BYTE",
"GL_SHORT",
"GL_UNSIGNED_SHORT",
"GL_INT",
"GL_UNSIGNED_INT",
"GL_FLOAT",
"GL_BGR",
"GL_BGRA",
"GL_UNSIGNED_BYTE_3_3_2",
"GL_UNSIGNED_SHORT_4_4_4_4",
"GL_UNSIGNED_SHORT_5_5_5_1",
"GL_UNSIGNED_INT_8_8_8_8",
"GL_UNSIGNED_INT_10_10_10_2",
"GL_UNSIGNED_BYTE_2_3_3_REV",
"GL_UNSIGNED_SHORT_5_6_5",
"GL_UNSIGNED_SHORT_5_6_5_REV",
"GL_UNSIGNED_SHORT_4_4_4_4_REV",
"GL_UNSIGNED_SHORT_1_5_5_5_REV",
"GL_UNSIGNED_INT_8_8_8_8_REV",
"GL_UNSIGNED_INT_2_10_10_10_REV",
]

/* Uniform Types */
public static const GL_FLOAT_VEC2:uint =                     0x8B50
public static const GL_FLOAT_VEC3:uint =                     0x8B51
public static const GL_FLOAT_VEC4:uint =                     0x8B52
public static const GL_INT_VEC2:uint =                       0x8B53
public static const GL_INT_VEC3:uint =                       0x8B54
public static const GL_INT_VEC4:uint =                       0x8B55
public static const GL_BOOL:uint =                           0x8B56
public static const GL_BOOL_VEC2:uint =                      0x8B57
public static const GL_BOOL_VEC3:uint =                      0x8B58
public static const GL_BOOL_VEC4:uint =                      0x8B59
public static const GL_FLOAT_MAT2:uint =                     0x8B5A
public static const GL_FLOAT_MAT3:uint =                     0x8B5B
public static const GL_FLOAT_MAT4:uint =                     0x8B5C
public static const GL_SAMPLER_2D:uint =                     0x8B5E
public static const GL_SAMPLER_CUBE:uint =                   0x8B60

public static const UNIFORM_TYPES:Array = [
	"GL_FLOAT_VEC2",
	"GL_FLOAT_VEC3",
	"GL_FLOAT_VEC4",
	"GL_INT_VEC2",
	"GL_INT_VEC3",
	"GL_INT_VEC4",
	"GL_BOOL",
	"GL_BOOL_VEC2",
	"GL_BOOL_VEC3",
	"GL_BOOL_VEC4",
	"GL_FLOAT_MAT2",
	"GL_FLOAT_MAT3",
	"GL_FLOAT_MAT4",
	"Unknown",
	"GL_SAMPLER_2D",
	"Unknown",
	"GL_SAMPLER_CUBE",
]

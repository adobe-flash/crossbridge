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

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <GL/gl.h>
#include <AS3/AS3.h>
#include <stdlib.h>



//=======================================================================
//		CPP HELPER FUNCTIONS
//=======================================================================
template <typename T, bool INDEXED>
class PrimIterator
{
	int			max;
	int			idx;
	const T*	indices;
public:
	PrimIterator(int first, int count, const void* indicesPtr) : idx(first), max(first+count), indices((const T*)indicesPtr) {}
	bool	hasNext()const	{return (idx<max);}
	int		getIndex()const	{return (INDEXED) ? indices[idx] : idx;}	// Compiler should eliminate the if
	void	next()			{++idx;}
};

typedef void (*glDrawFnPtr)(GLint first, GLsizei count, const GLvoid* indices);

template <bool INDEXED>
void _glDrawPrimitives(GLint first, GLsizei count, const GLvoid *indices, GLenum mode, GLenum type);

template <typename T, bool INDEXED>
void glDraw_Triangles(GLint first, GLsizei count, const GLvoid* indices);

template <typename T, bool INDEXED>
void glDraw_TriStrips(GLint first, GLsizei count, const GLvoid* indices);


extern "C" {

// Force libGL.abc to get linked in
extern int __libgl_abc__;
void __libgl_abc__linker_hack() { __libgl_abc__ = 0; }


//=======================================================================
//		CONSTANTS & MACROS
//=======================================================================

#define null									0

#define GLS3D_COLOR_WRITE_MASK_R				0
#define GLS3D_COLOR_WRITE_MASK_G				1
#define GLS3D_COLOR_WRITE_MASK_B				2
#define GLS3D_COLOR_WRITE_MASK_A				3

#define GL_DRAW_PRIMITIVES_DO_PARAM_CHECK		1			// 1=Input validation (Should be on for debug lib)



//=======================================================================
//		HELPER CLASSES/STRUCTS
//=======================================================================

class VertexBufferBuilder
{
private:

    unsigned hashSum;
    unsigned mode;
    unsigned flags;
    static const unsigned HAS_COLOR          = 0x00000001;
    static const unsigned HAS_TEXTURE2D      = 0x00000002;
    static const unsigned HAS_NORMAL         = 0x00000004;
    static const unsigned TEX_GEN_S_SPHERE   = 0x00000008;
    static const unsigned TEX_GEN_T_SPHERE   = 0x00000010;

    //32 bit offset_basis = 2166136261
    static const unsigned offset_basis = 2166136261U;
    // 32 bit FNV_prime = 224 + 28 + 0x93 = 16777619
    static const unsigned prime = 16777619U;

    float x;
    float y;
    float z;
    float r;
    float g;
    float b;
    float a;
    float nx;
    float ny;
    float nz;
    float tx;
    float ty;

public:

    VertexBufferBuilder() {
        pBuffer = null;
        totalBufferSize = 0;
        count = 0;

        // Initialize
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;

        r = 1.0f;
        g = 1.0f;
        b = 1.0f;
        a = 1.0f;

        nx = 0.0f;
        ny = 0.0f;
        nz = 0.0f;

        tx = 0.0f;
        ty = 0.0f;

        flags = 0;
    }

    void reset(unsigned mode) {
        flags = 0;
        count = 0;
        this->mode = mode;
        hashSum = offset_basis;
    }

    void glColor(float r, float g, float b, float a) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
        this->flags |= HAS_COLOR;
    }

    void glTexCoord(float tx, float ty) {
        this->tx = tx;
        this->ty = ty;
        this->flags |= HAS_TEXTURE2D;
    }

    void glNormal(float nx, float ny, float nz) {
        this->nx = nx;
        this->ny = ny;
        this->nz = nz;
        this->flags |= HAS_NORMAL;
    }

    void glVertex(float x, float y, float z) {
        // Grow buffer if we need to
        if (count * 12 == totalBufferSize) {
            realloc((count + 1024) * 12);
        }

        pBuffer[count * 12 + 0]  = x;
        pBuffer[count * 12 + 1]  = y;
        pBuffer[count * 12 + 2]  = z;

        pBuffer[count * 12 + 3]  = r;
        pBuffer[count * 12 + 4]  = g;
        pBuffer[count * 12 + 5]  = b;
        pBuffer[count * 12 + 6]  = a;

        pBuffer[count * 12 + 7]  = nx;
        pBuffer[count * 12 + 8]  = ny;
        pBuffer[count * 12 + 9]  = nz;

        pBuffer[count * 12 + 10] = tx;
        pBuffer[count * 12 + 11] = ty;

        ++count;
        }

    void glEnd() {
        hashSum = offset_basis;
        int *data = (int*)pBuffer;
        for (int i = 0; i < count * 12; i++) {
            hashSum ^= data[i];
            hashSum *= prime;
        }

        inline_as3("import GLS3D.GLAPI;\n"\
                   "GLAPI.instance.glEndVertexData(%0, %1, ram, %2, %3, %4);\n" :: "r"(count), "r"(mode), "r"(pBuffer), "r"(hashSum), "r"(flags));
    }

private:

    float *pBuffer;
    int totalBufferSize;
    int count;

    void realloc(int newSize) {
        float* newBuf = (float *)malloc(newSize * sizeof(float));
        int copyCount = count * 12;
        for (int i = 0; i < copyCount; i++) {
            newBuf[i] = pBuffer[i];
        }

        free(pBuffer);
        pBuffer = newBuf;
        totalBufferSize = newSize;
    }
};


static VertexBufferBuilder vbb;


// OpenGL state
//**NOTE**: Incomplete, should add as we go
struct GLState
{
	GLboolean	depthWriteMask;
	GLboolean	colorWriteMask[4];	// RGBA
};
static GLState glState;


extern void glVertex2i (GLint x, GLint y)
{
    float fx = (float)x;
    float fy = (float)y;
    float fz = (float)0.0f;
    vbb.glVertex(fx, fy, fz);
}

extern void glTexCoord2f (GLfloat s, GLfloat t)
{
    float fs = (float)s;
    float ft = (float)t;
    vbb.glTexCoord(fs, ft);
}

extern void glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glRotate(%0, %1, %2, %3);\n" : : "r"(angle), "r"(x), "r"(y), "r"(z));
}

extern void glRotated (GLdouble angle, GLdouble x, GLdouble y, GLdouble z)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glRotate(%0, %1, %2, %3);\n" : : "r"(angle), "r"(x), "r"(y), "r"(z));
}

extern void glEnd (void)
{
    vbb.glEnd();
}

extern void glBegin (GLenum mode)
{
    vbb.reset(mode);
}

extern void glClear (GLbitfield mask)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glClear(%0);\n" : : "r"(mask));
}

extern void glDebugCube() {
  inline_as3("import GLS3D.GLAPI;\n"\
               "GLAPI.instance.glDebugCube();\n");
}

extern void glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "GLAPI.instance.glClearColor(%0, %1, %2, %3);\n"
           : : "r"(red), "r"(green), "r"(blue), "r"(alpha) );
}

extern void glClearDepth (GLclampd depth)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "GLAPI.instance.glClearDepth(%0);\n"
           : : "r"(depth) );
}

extern void glClearDepthf (GLclampf depth)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "GLAPI.instance.glClearDepth(%0);\n"
           : : "r"(depth) );
}

extern void glClearStencil (GLint s)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "GLAPI.instance.glClearStencil(%0);\n"
           : : "r"(s) );
}

extern void glVertex3fv (const GLfloat *v)
{
    float fx = (float)v[0];
    float fy = (float)v[1];
    float fz = (float)v[2];
    vbb.glVertex(fx, fy, fz);
}

extern void glVertex3dv (const GLdouble *v)
{
    float fx = (float)v[0];
    float fy = (float)v[1];
    float fz = (float)v[2];
    vbb.glVertex(fx, fy, fz);
}

extern void glColor3fv (const GLfloat *v)
{
    float fr = (float)v[0];
    float fg = (float)v[1];
    float fb = (float)v[2];
    float fa = (float)1.0f;

    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glColor(%0, %1, %2, %3);\n" : : "r"(fr), "r"(fg), "r"(fb), "r"(fa));
    vbb.glColor(fr, fg, fb, fa);
}

extern void glColor4ubv (const GLubyte *v)
{
    float fr = (float)v[0] / 255.0f;
    float fg = (float)v[1] / 255.0f;
    float fb = (float)v[2] / 255.0f;
    float fa = (float)v[3] / 255.0f;

    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glColor(%0, %1, %2, %3);\n" : : "r"(fr), "r"(fg), "r"(fb), "r"(fa));
    vbb.glColor(fr, fg, fb, fa);
}

extern void glColor4ub (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
    float fr = (float)red / 255.0f;
    float fg = (float)green / 255.0f;
    float fb = (float)blue / 255.0f;
    float fa = (float)alpha / 255.0f;

    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glColor(%0, %1, %2, %3);\n" : : "r"(fr), "r"(fg), "r"(fb), "r"(fa));
    vbb.glColor(fr, fg, fb, fa);
}

/* ===================== From MESA ============================== */
static void
frustum(GLdouble left, GLdouble right,
        GLdouble bottom, GLdouble top, 
        GLdouble nearval, GLdouble farval)
{
   GLdouble x, y, a, b, c, d;
   GLdouble m[16];

   x = (2.0 * nearval) / (right - left);
   y = (2.0 * nearval) / (top - bottom);
   a = (right + left) / (right - left);
   b = (top + bottom) / (top - bottom);
   c = -(farval + nearval) / ( farval - nearval);
   d = -(2.0 * farval * nearval) / (farval - nearval);

#define M(row,col)  m[col*4+row]
   M(0,0) = x;     M(0,1) = 0.0F;  M(0,2) = a;      M(0,3) = 0.0F;
   M(1,0) = 0.0F;  M(1,1) = y;     M(1,2) = b;      M(1,3) = 0.0F;
   M(2,0) = 0.0F;  M(2,1) = 0.0F;  M(2,2) = c;      M(2,3) = d;
   M(3,0) = 0.0F;  M(3,1) = 0.0F;  M(3,2) = -1.0F;  M(3,3) = 0.0F;
#undef M

   glMultMatrixd(m);
}

extern void gluOrtho2D(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top)
{
    glOrtho(left, right, bottom, top, -1, 1);
}

extern void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
   GLdouble xmin, xmax, ymin, ymax;

   ymax = zNear * tan(fovy * M_PI / 360.0);
   ymin = -ymax;
   xmin = ymin * aspect;
   xmax = ymax * aspect;

   /* don't call glFrustum() because of error semantics (covglu) */
   frustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

extern void gluLookAt( GLdouble eyex, GLdouble eyey, GLdouble eyez,
                         GLdouble centerx, GLdouble centery, GLdouble centerz,
                         GLdouble upx, GLdouble upy, GLdouble upz )
{
   GLdouble m[16];
   GLdouble x[3], y[3], z[3];
   GLdouble mag;

   /* Make rotation matrix */

   /* Z vector */
   z[0] = eyex - centerx;
   z[1] = eyey - centery;
   z[2] = eyez - centerz;
   mag = sqrt( z[0]*z[0] + z[1]*z[1] + z[2]*z[2] );
   if (mag) {  /* mpichler, 19950515 */
      z[0] /= mag;
      z[1] /= mag;
      z[2] /= mag;
   }

   /* Y vector */
   y[0] = upx;
   y[1] = upy;
   y[2] = upz;

   /* X vector = Y cross Z */
   x[0] =  y[1]*z[2] - y[2]*z[1];
   x[1] = -y[0]*z[2] + y[2]*z[0];
   x[2] =  y[0]*z[1] - y[1]*z[0];

   /* Recompute Y = Z cross X */
   y[0] =  z[1]*x[2] - z[2]*x[1];
   y[1] = -z[0]*x[2] + z[2]*x[0];
   y[2] =  z[0]*x[1] - z[1]*x[0];

   /* mpichler, 19950515 */
   /* cross product gives area of parallelogram, which is < 1.0 for
    * non-perpendicular unit-length vectors; so normalize x, y here
    */

   mag = sqrt( x[0]*x[0] + x[1]*x[1] + x[2]*x[2] );
   if (mag) {
      x[0] /= mag;
      x[1] /= mag;
      x[2] /= mag;
   }

   mag = sqrt( y[0]*y[0] + y[1]*y[1] + y[2]*y[2] );
   if (mag) {
      y[0] /= mag;
      y[1] /= mag;
      y[2] /= mag;
   }

#define M(row,col)  m[col*4+row]
   M(0,0) = x[0];  M(0,1) = x[1];  M(0,2) = x[2];  M(0,3) = 0.0;
   M(1,0) = y[0];  M(1,1) = y[1];  M(1,2) = y[2];  M(1,3) = 0.0;
   M(2,0) = z[0];  M(2,1) = z[1];  M(2,2) = z[2];  M(2,3) = 0.0;
   M(3,0) = 0.0;   M(3,1) = 0.0;   M(3,2) = 0.0;   M(3,3) = 1.0;
#undef M
   glMultMatrixd( m );

   /* Translate Eye to Origin */
   glTranslated( -eyex, -eyey, -eyez );

}


/* ========================================================================= */

extern void glShadeModel (GLenum mode)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glShadeModel(%0);" : : "r"(mode));
}

extern GLenum glGetError (void)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "GLAPI.instance.send('glGetError not yet implemented.');");
    return GL_NO_ERROR;
}

extern void glTexEnvf (GLenum target, GLenum pname, GLfloat param)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glTexEnvf(%0, %1, %2);\n" : : "r"(target), "r"(pname), "r"(param));
}

extern void glOrtho (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glOrtho(%0, %1, %2, %3, %4, %5)\n" : : "r"(left),"r"(right),"r"(bottom),"r"(top),"r"(zNear),"r"(zFar));
}

extern void glOrthof (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
	inline_as3("import GLS3D.GLAPI;\n"\
		"GLAPI.instance.glOrtho(%0, %1, %2, %3, %4, %5)\n" : : "r"(left),"r"(right),"r"(bottom),"r"(top),"r"(zNear),"r"(zFar));
}

extern void glLoadIdentity (void)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "GLAPI.instance.glLoadIdentity()\n");
}

extern void glPushMatrix (void)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "GLAPI.instance.glPushMatrix()\n");
}

extern void glViewport (GLint x, GLint y, GLsizei width, GLsizei height)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "GLAPI.instance.send('glViewport not yet implemented.');");
}

extern void glBlendFunc (GLenum sfactor, GLenum dfactor)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "GLAPI.instance.glBlendFunc(%0, %1)\n" :: "r"(sfactor), "r"(dfactor));
}

extern void glEnable (GLenum cap)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "GLAPI.instance.glEnable(%0)\n" :: "r"(cap));
}

extern void glDisable (GLenum cap)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "GLAPI.instance.glDisable(%0)\n" :: "r"(cap));
}

extern void glPushAttrib (GLbitfield mask)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "GLAPI.instance.glPushAttrib(%0)\n" :: "r"(mask));
}

extern void glPopAttrib (void)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "GLAPI.instance.glPopAttrib()\n");
}

extern void glPopMatrix (void)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "GLAPI.instance.glPopMatrix()\n");
}

extern void glMatrixMode (GLenum mode)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glMatrixMode(%0)\n" : : "r"(mode));
}

extern void glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glTexImage2D(%0, %1, %2, %3, %4, %5, %6, %7, %8, ram);\n" : :
               "r"(target), "r"(level), "r"(internalformat), "r"(width), "r"(height), "r"(border), "r"(format), "r"(type), "r"(pixels));
}

extern void glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glTexSubImage2D(%0, %1, %2, %3, %4, %5, %6, %7, %8, ram);\n" : :
               "r"(target), "r"(level), "r"(xoffset), "r"(yoffset), "r"(width), "r"(height), "r"(format), "r"(type), "r"(pixels));
}

extern void glTexParameteri (GLenum target, GLenum pname, GLint param)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glTexParameteri(%0, %1, %2);\n" : : "r"(target), "r"(pname), "r"(param));
}

extern void glTexParameterf (GLenum target, GLenum pname, GLfloat param)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glTexParameterf(%0, %1, %2);\n" : : "r"(target), "r"(pname), "r"(param));
}

extern void glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params)
{
    switch(pname) {
    case GL_TEXTURE_BORDER_COLOR:
        // do we care?
        break;
    default:
        inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.send('glTexParameterfv: ' + %0 + ','+ %1 + ','+ %2);\n" : : "r"(target), "r"(pname), "r"(params[0]));
        break;
    }
}

extern void glBindTexture (GLenum target, GLuint texture)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glBindTexture(%0, %1);\n" : : "r"(target), "r"(texture));
}

extern void glActiveTexture (GLenum texture)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glActiveTexture(%0);\n" : : "r"(texture));
}

extern void glGenTextures (GLsizei n, GLuint *textures)
{
    int i;
    unsigned firstIndex;

    inline_as3("import GLS3D.GLAPI;\n"\
               "var result:uint = GLAPI.instance.glGenTextures(%0);\n" :: "r"(n));
    AS3_GetScalarFromVar(firstIndex, result);

    for (i = 0; i < n; i++) {
        textures[i] = firstIndex + i;
    }
}


struct StateInfo {
    GLboolean enabled;
    GLint size;
    GLenum type;
    GLsizei stride;
    const void *ptr;
};

int activeTextureUnit = GL_TEXTURE0;

struct ArrayEXTState {
    StateInfo verts;
	StateInfo colors;
	StateInfo normals;
	StateInfo texcoords[GL_MAX_TEXTURE_COORDS];
} AState;


extern void glClientActiveTexture (GLenum texture)
{
    ///inline_as3("import GLS3D.GLAPI;\n"\
    //         "GLAPI.instance.send('stubbed glClientActiveTexture '  +%0);\n" : : "r"(texture));
}

extern void glEnableClientState (GLenum array)
{
    switch(array) {
    case GL_VERTEX_ARRAY:
        AState.verts.enabled = GL_TRUE;
        break;
    case GL_COLOR_ARRAY:
        AState.colors.enabled = GL_TRUE;
        break;
    case GL_TEXTURE_COORD_ARRAY:
        AState.texcoords[activeTextureUnit - GL_TEXTURE0].enabled = GL_TRUE;
        break;
	case GL_NORMAL_ARRAY:
		AState.normals.enabled = GL_TRUE;
		break;
    default:
        inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.send('stubbed glEnableClientState '  +%0);\n" : : "r"(array));
        break;
    }
}

extern void glDisableClientState (GLenum array)
{
    switch(array) {
    case GL_VERTEX_ARRAY:
        AState.verts.enabled = GL_FALSE;
        break;
    case GL_COLOR_ARRAY:
        AState.colors.enabled = GL_FALSE;
        break;
    case GL_TEXTURE_COORD_ARRAY:
        AState.texcoords[activeTextureUnit - GL_TEXTURE0].enabled = GL_FALSE;
        break;
	case GL_NORMAL_ARRAY:
		AState.normals.enabled = GL_FALSE;
		break;
    default:
        inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.send('stubbed glDisableClientState '  +%0);\n" : : "r"(array));
        break;
    }
}

extern void glUnlockArraysEXT ()
{
}

extern void glLockArraysEXT (GLint i, GLsizei j)
{
}

static GLfloat* getFloatPtr(const void *ptr, int stride, int i)
{
    return (GLfloat*)&ptr[i*stride];
}
static GLubyte* getUBytePtr(const void *ptr, int stride, int i)
{
    return (GLubyte*)&ptr[i*stride];
}

static int max(int a, int b)
{
    return a > b ? a : b;
}

static int verboseDebug = 0;

extern void glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{
	// Draw Indexed Primitive
	_glDrawPrimitives<true>(0, count, indices, mode, type);
}

extern void glDrawArrays (GLenum mode, GLint first, GLsizei count)
{
	// Draw NonIndexed Primitive
	_glDrawPrimitives<false>(first, count, NULL, mode, GL_UNSIGNED_INT);
}

extern void glGenBuffers (GLsizei n, GLuint *buffers)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "GLAPI.instance.glGenBuffers(%0, %1);" :  : "r"(n), "r"(buffers));
}

extern void glBufferData (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "GLAPI.instance.glBufferData(%0, %1, %2, %3);" :  : "r"(target), "r"(size), "r"(data), "r"(usage));
}

extern void glColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    AState.colors.size = size;
    AState.colors.type = type;
    AState.colors.stride = stride;
    AState.colors.ptr = pointer;
}

extern void glVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    AState.verts.size = size;
    AState.verts.type = type;
    AState.verts.stride = stride;
    AState.verts.ptr = pointer;
}

extern void glTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    AState.texcoords[activeTextureUnit - GL_TEXTURE0].size = size;
    AState.texcoords[activeTextureUnit - GL_TEXTURE0].type = type;
    AState.texcoords[activeTextureUnit - GL_TEXTURE0].stride = stride;
    AState.texcoords[activeTextureUnit - GL_TEXTURE0].ptr = pointer;
}

extern void glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "GLAPI.instance.send('glReadPixels not yet implemented.');");
}

extern void glGetIntegerv (GLenum pname, GLint *params)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glGetIntegerv(%0, ram, %1);\n" : : "r"(pname), "r"(params));
}

extern void glGetFloatv (GLenum pname, GLfloat *params)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glGetFloatv(%0, ram, %1);\n" : : "r"(pname), "r"(params));
}

extern void glVertex2f (GLfloat x, GLfloat y)
{
    float fx = (float)x;
    float fy = (float)y;
    float fz = (float)0.0f;
    vbb.glVertex(fx, fy, fz);
}

extern void glMultMatrixf (const GLfloat *m)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "ram.position = %0;" \
               "GLAPI.instance.glMultMatrix(ram, true /*floatArray*/);\n":: "r"(m));
}

extern void glDepthMask (GLboolean flag)
{
	// Cache glState
	glState.depthWriteMask = flag;

    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glDepthMask(%0);\n" : : "r"(flag));
}

extern void glDepthFunc (GLenum func)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glDepthFunc(%0);\n" : : "r"(func));
}

extern void glCallList (GLuint list)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glCallList(%0)\n" : : "r"(list));
}

extern void glTranslatef (GLfloat x, GLfloat y, GLfloat z)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glTranslate(%0, %1, %2);\n" : : "r"(x), "r"(y), "r"(z));
}

extern void glTranslated (GLdouble x, GLdouble y, GLdouble z)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glTranslate(%0, %1, %2);\n" : : "r"(x), "r"(y), "r"(z));
}

extern void glEndList (void)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glEndList()\n" : :);
}

extern void glNewList (GLuint list, GLenum mode)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glNewList(%0, %1)\n" : : "r"(list), "r"(mode));
}

extern GLuint glGenLists (GLsizei range)
{
    GLuint retVal;

    inline_as3("import GLS3D.GLAPI;\n"\
           "var result:uint = GLAPI.instance.glGenLists(%0);\n" : : "r"(range));
    AS3_GetScalarFromVar(retVal, result);

    return retVal;
}

extern void glPolygonOffset (GLfloat factor, GLfloat units)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glPolygonOffset(%0, %1);\n" : : "r"(factor), "r"(units));
}

extern void glLightModeli (GLenum pname, GLint param)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glLightModeli(%0, %1);\n" : : "r"(pname), "r"(param));
}

extern void glTexGeni (GLenum coord, GLenum pname, GLint param)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glTexGeni(%0, %1, %2);\n" : : "r"(coord), "r"(pname), "r"(param));
}

extern void glMaterialfv (GLenum face, GLenum pname, const GLfloat *params)
{
    float fr, fg, fb, fa;

    if (pname == GL_SHININESS) {
        fr = (float)params[0];
        fg = 0.0f;
        fb = 0.0f;
        fa = 0.0f;
    } else {
        fr = (float)params[0];
        fg = (float)params[1];
        fb = (float)params[2];
        fa = (float)params[3];
    }

    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glMaterial(%0, %1, %2, %3, %4, %5);\n" : : "r"(face), "r"(pname), "r"(fr), "r"(fg), "r"(fb), "r"(fa));
}

extern void glNormal3fv (const GLfloat *v)
{
    float nx = (float)v[0];
    float ny = (float)v[1];
    float nz = (float)v[2];
    vbb.glNormal(nx, ny, nz);
}

extern void glNormal3dv (const GLdouble *v)
{
    float nx = (float)v[0];
    float ny = (float)v[1];
    float nz = (float)v[2];
    vbb.glNormal(nx, ny, nz);
}

extern void glTexCoord2fv (const GLfloat *v)
{
    float fs = (float)v[0];
    float ft = (float)v[1];
    vbb.glTexCoord(fs, ft);
}

extern void glScissor (GLint x, GLint y, GLsizei width, GLsizei height)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glScissor(%0, %1, %2, %3);\n" : : "r"(x), "r"(y), "r"(width), "r"(height));
}

extern void glDeleteLists (GLuint list, GLsizei range)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "GLAPI.instance.send('glDeleteLists not yet implemented.');");
}

extern GLboolean glIsList (GLuint list)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "GLAPI.instance.send('glIsList not yet implemented.');");
}

extern void glDeleteTextures (GLsizei n, const GLuint *textures)
{
    for(int i=0; i<n; i++)
        inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glDeleteTexture(%0);\n" :  : "r"(textures[i]));
}

extern GLboolean glIsTexture (GLuint texture)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "GLAPI.instance.send('glIsTexture not yet implemented.');");
}

extern void glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    float fr = (float)red;
    float fg = (float)green;
    float fb = (float)blue;
    float fa = (float)alpha;

    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glColor(%0, %1, %2, %3);\n" : : "r"(fr), "r"(fg), "r"(fb), "r"(fa));
    vbb.glColor(fr, fg, fb, fa);
}

extern void glScalef (GLfloat x, GLfloat y, GLfloat z)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glScale(%0, %1, %2);\n" : : "r"(x), "r"(y), "r"(z));
}

extern void glScaled (GLdouble x, GLdouble y, GLdouble z)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glScale(%0, %1, %2);\n" : : "r"(x), "r"(y), "r"(z));
}

extern void glVertex3f (GLfloat x, GLfloat y, GLfloat z)
{
    float fx = (float)x;
    float fy = (float)y;
    float fz = (float)z;
    vbb.glVertex(fx, fy, fz);
}

extern void glVertex3d (GLdouble x, GLdouble y, GLdouble z)
{
    float fx = (float)x;
    float fy = (float)y;
    float fz = (float)z;
    vbb.glVertex(fx, fy, fz);
}

extern void glColor3f (GLfloat red, GLfloat green, GLfloat blue)
{
    float fr = (float)red;
    float fg = (float)green;
    float fb = (float)blue;
    float fa = (float)1.0f;

    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glColor(%0, %1, %2, %3);\n" : : "r"(fr), "r"(fg), "r"(fb), "r"(fa));
    vbb.glColor(fr, fg, fb, fa);
}

extern void glColor3d (GLdouble red, GLdouble green, GLdouble blue)
{
    float fr = (float)red;
    float fg = (float)green;
    float fb = (float)blue;
    float fa = (float)1.0f;

    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glColor(%0, %1, %2, %3);\n" : : "r"(fr), "r"(fg), "r"(fb), "r"(fa));
    vbb.glColor(fr, fg, fb, fa);
}

extern void glNormal3f (GLfloat nx, GLfloat ny, GLfloat nz)
{
    vbb.glNormal(nx, ny, nz);
}

extern void glNormal3d (GLdouble nx, GLdouble ny, GLdouble nz)
{
    vbb.glNormal(nx, ny, nz);
}

extern void glColor4fv (const GLfloat *v)
{
    float fr = (float)v[0];
    float fg = (float)v[1];
    float fb = (float)v[2];
    float fa = (float)v[3];

    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glColor(%0, %1, %2, %3);\n" : : "r"(fr), "r"(fg), "r"(fb), "r"(fa));
    vbb.glColor(fr, fg, fb, fa);
}

extern void glReadBuffer (GLenum mode)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "GLAPI.instance.send('glReadBuffer not yet implemented.');");
}

static const char *GL_VENDOR_str="Adobe";
static const char *GL_RENDERER_str="Stage3D";
static const char *GL_VERSION_str="2.1";
static const char *GL_EXTENSIONS_str="GL_EXT_compiled_vertex_array";
static const char *GL_default_str="";


extern const GLubyte * glGetString (GLenum name)
{
    switch(name) {
    case GL_VENDOR:
        return (const GLubyte*)GL_VENDOR_str;
    case GL_RENDERER:
        return (const GLubyte*)GL_RENDERER_str;
    case GL_VERSION:
        return (const GLubyte*)GL_VERSION_str;
    case GL_EXTENSIONS:
        return (const GLubyte*)GL_EXTENSIONS_str;
    default:
        inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.send('glGetString not yet implemented. for ' + %0);" :  : "r"(name));
    }
    return (const GLubyte*)GL_default_str;
}

extern void glMultMatrixd (const GLdouble *m)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "ram.position = %0;"\
               "GLAPI.instance.glMultMatrix(ram, false /*floatArray*/);\n" :: "r"(m));
}

extern void glLoadMatrixf (const GLfloat *m)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "ram.position = %0;"\
               "GLAPI.instance.glLoadMatrix(ram, true /*floatArray*/);\n" :: "r"(m));
}

extern void glFrustum (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble nearval, GLdouble farval)
{
    // http://cgit.freedesktop.org/mesa/mesa/diff/src?id=eeb861df0b1843ffd765b969ed80394f663a7dc9
    GLdouble x, y, a, b, c, d;
    GLdouble m[16];

    x = (2.0 * nearval) / (right - left);
    y = (2.0 * nearval) / (top - bottom);
    a = (right + left) / (right - left);
    b = (top + bottom) / (top - bottom);
    c = -(farval + nearval) / ( farval - nearval);
    d = -(2.0 * farval * nearval) / (farval - nearval);

#define M(row,col)  m[col*4+row]
    M(0,0) = x;     M(0,1) = 0.0F;  M(0,2) = a;      M(0,3) = 0.0F;
    M(1,0) = 0.0F;  M(1,1) = y;     M(1,2) = b;      M(1,3) = 0.0F;
    M(2,0) = 0.0F;  M(2,1) = 0.0F;  M(2,2) = c;      M(2,3) = d;
    M(3,0) = 0.0F;  M(3,1) = 0.0F;  M(3,2) = -1.0F;  M(3,3) = 0.0F;
#undef M

    glMultMatrixd(m);
}

extern void glDrawBuffers (GLsizei n, const GLenum *bufs)
{
    inline_as3("import GLS3D.GLAPI;\n"\
               "GLAPI.instance.send('glDrawBuffers not yet implemented.');");
}

extern void glFrontFace (GLenum mode)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glFrontFace(%0);\n" : : "r"(mode));
}

extern void glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
	// Cache glState
	glState.colorWriteMask[GLS3D_COLOR_WRITE_MASK_R] = red;
	glState.colorWriteMask[GLS3D_COLOR_WRITE_MASK_G] = green;
	glState.colorWriteMask[GLS3D_COLOR_WRITE_MASK_B] = blue;
	glState.colorWriteMask[GLS3D_COLOR_WRITE_MASK_A] = alpha;

    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glColorMask(%0, %1, %2, %3);\n" : : "r"(red), "r"(green), "r"(blue), "r"(alpha));
}

extern void glStencilOp (GLenum fail, GLenum zfail, GLenum zpass)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glStencilOp(%0, %1, %2);\n" : : "r"(fail), "r"(zfail), "r"(zpass));
}

extern void glStencilFunc (GLenum func, GLint ref, GLuint mask)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glStencilFunc(%0, %1, %2);\n" : : "r"(func), "r"(ref), "r"(mask));
}

extern void glClipPlane (GLenum plane, const GLdouble *equation)
{
    double a = equation[0];
    double b = equation[1];
    double c = equation[2];
    double d = equation[3];
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glClipPlane(%0, %1, %2, %3, %4);\n" : : "r"(plane), "r"(a), "r"(b), "r"(c), "r"(d));
}

extern void glLightfv (GLenum light, GLenum pname, const GLfloat *params)
{
    float fr, fg, fb, fa;

    if (pname == GL_SPOT_EXPONENT || 
		pname == GL_SPOT_CUTOFF ||
        pname == GL_CONSTANT_ATTENUATION || 
		pname == GL_LINEAR_ATTENUATION ||
        pname == GL_QUADRATIC_ATTENUATION) {
        fr = (float)params[0];
        fg = 0.0f;
        fb = 0.0f;
        fa = 0.0f;
    } else {
        fr = (float)params[0];
        fg = (float)params[1];
        fb = (float)params[2];
        fa = (float)params[3];
    }

    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glLight(%0, %1, %2, %3, %4, %5);\n" : : "r"(light), "r"(pname), "r"(fr), "r"(fg), "r"(fb), "r"(fa));
}

extern void glPolygonMode (GLenum face, GLenum mode)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glPolygonMode(%0, %1);\n" : : "r"(face), "r"(mode));
}

extern void glDrawBuffer (GLenum mode)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.send('glDrawBuffer not yet implemented: ' + %0);" :  : "r"(mode));
}

extern void glCullFace (GLenum mode)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glCullFace(%0);\n" : : "r"(mode));
}

extern void glAlphaFunc (GLenum func, GLclampf ref)
{
    inline_as3("import GLS3D.GLAPI;\n"\
           "GLAPI.instance.glAlphaFunc(%0, %1);\n" : : "r"(func), "r"(ref));
}

// cat /p4/sb/alcmain/mainline/gl2stage3d/include/GL/gl.h | grep "void" | sed 's/;//g' | sed 's/$/ { if(stubMsg) fprintf(stderr, \"stubbed...\\\n"); }/' >> gl2stage3d/stubs.c

#include <stdio.h>
#include <GL/gl.h>

static int stubMsg = 0;
extern void glAccum (GLenum op, GLfloat value)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glAccum...\n");
    }
}

extern void glArrayElement (GLint i)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glArrayElement...\n");
    }
}

extern void glBitmap (GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glBitmap...\n");
    }
}

extern void glBlendColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glBlendColor...\n");
    }
}

extern void glBlendEquation (GLenum mode)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glBlendEquation...\n");
    }
}

extern void glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glBlendEquationSeparate(GLenum...\n");
    }
}

extern void glCallLists (GLsizei n, GLenum type, const GLvoid *lists)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glCallLists...\n");
    }
}

extern void glClearAccum (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glClearAccum...\n");
    }
}

extern void glClearIndex (GLfloat c)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glClearIndex...\n");
    }
}

extern void glColor3b (GLbyte red, GLbyte green, GLbyte blue)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor3b...\n");
    }
}

extern void glColor3bv (const GLbyte *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor3bv...\n");
    }
}

extern void glColor3dv (const GLdouble *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor3dv...\n");
    }
}

extern void glColor3i (GLint red, GLint green, GLint blue)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor3i...\n");
    }
}

extern void glColor3iv (const GLint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor3iv...\n");
    }
}

extern void glColor3s (GLshort red, GLshort green, GLshort blue)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor3s...\n");
    }
}

extern void glColor3sv (const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor3sv...\n");
    }
}

extern void glColor3ub (GLubyte red, GLubyte green, GLubyte blue)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor3ub...\n");
    }
}

extern void glColor3ubv (const GLubyte *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor3ubv...\n");
    }
}

extern void glColor3ui (GLuint red, GLuint green, GLuint blue)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor3ui...\n");
    }
}

extern void glColor3uiv (const GLuint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor3uiv...\n");
    }
}

extern void glColor3us (GLushort red, GLushort green, GLushort blue)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor3us...\n");
    }
}

extern void glColor3usv (const GLushort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor3usv...\n");
    }
}

extern void glColor4b (GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor4b...\n");
    }
}

extern void glColor4bv (const GLbyte *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor4bv...\n");
    }
}

extern void glColor4d (GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor4d...\n");
    }
}

extern void glColor4dv (const GLdouble *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor4dv...\n");
    }
}

extern void glColor4i (GLint red, GLint green, GLint blue, GLint alpha)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor4i...\n");
    }
}

extern void glColor4iv (const GLint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor4iv...\n");
    }
}

extern void glColor4s (GLshort red, GLshort green, GLshort blue, GLshort alpha)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor4s...\n");
    }
}

extern void glColor4sv (const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor4sv...\n");
    }
}

extern void glColor4ui (GLuint red, GLuint green, GLuint blue, GLuint alpha)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor4ui...\n");
    }
}

extern void glColor4uiv (const GLuint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor4uiv...\n");
    }
}

extern void glColor4us (GLushort red, GLushort green, GLushort blue, GLushort alpha)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor4us...\n");
    }
}

extern void glColor4usv (const GLushort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColor4usv...\n");
    }
}

extern void glColorMaterial (GLenum face, GLenum mode)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColorMaterial...\n");
    }
}

extern void glColorSubTable (GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColorSubTable...\n");
    }
}

extern void glColorTable (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColorTable...\n");
    }
}

extern void glColorTableParameterfv (GLenum target, GLenum pname, const GLfloat *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColorTableParameterfv...\n");
    }
}

extern void glColorTableParameteriv (GLenum target, GLenum pname, const GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glColorTableParameteriv...\n");
    }
}

extern void glConvolutionFilter1D (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glConvolutionFilter1D...\n");
    }
}

extern void glConvolutionFilter2D (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *image)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glConvolutionFilter2D...\n");
    }
}

extern void glConvolutionParameterf (GLenum target, GLenum pname, GLfloat params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glConvolutionParameterf...\n");
    }
}

extern void glConvolutionParameterfv (GLenum target, GLenum pname, const GLfloat *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glConvolutionParameterfv...\n");
    }
}

extern void glConvolutionParameteri (GLenum target, GLenum pname, GLint params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glConvolutionParameteri...\n");
    }
}

extern void glConvolutionParameteriv (GLenum target, GLenum pname, const GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glConvolutionParameteriv...\n");
    }
}

extern void glCopyColorSubTable (GLenum target, GLsizei start, GLint x, GLint y, GLsizei width)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glCopyColorSubTable...\n");
    }
}

extern void glCopyColorTable (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glCopyColorTable...\n");
    }
}

extern void glCopyConvolutionFilter1D (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glCopyConvolutionFilter1D...\n");
    }
}

extern void glCopyConvolutionFilter2D (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glCopyConvolutionFilter2D...\n");
    }
}

extern void glCopyPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum type)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glCopyPixels...\n");
    }
}

extern void glCopyTexImage1D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glCopyTexImage1D...\n");
    }
}

extern void glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glCopyTexImage2D...\n");
    }
}

extern void glCopyTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glCopyTexSubImage1D...\n");
    }
}

extern void glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glCopyTexSubImage2D...\n");
    }
}

extern void glCopyTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glCopyTexSubImage3D...\n");
    }
}

extern void glDepthRange (GLclampd zNear, GLclampd zFar)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glDepthRange...\n");
    }
}

extern void glDrawPixels (GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glDrawPixels...\n");
    }
}

extern void glDrawRangeElements (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glDrawRangeElements...\n");
    }
}

extern void glEdgeFlag (GLboolean flag)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glEdgeFlag...\n");
    }
}

extern void glEdgeFlagPointer (GLsizei stride, const GLvoid *pointer)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glEdgeFlagPointer...\n");
    }
}

extern void glEdgeFlagv (const GLboolean *flag)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glEdgeFlagv...\n");
    }
}


extern void glEvalCoord1d (GLdouble u)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glEvalCoord1d...\n");
    }
}

extern void glEvalCoord1dv (const GLdouble *u)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glEvalCoord1dv...\n");
    }
}

extern void glEvalCoord1f (GLfloat u)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glEvalCoord1f...\n");
    }
}

extern void glEvalCoord1fv (const GLfloat *u)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glEvalCoord1fv...\n");
    }
}

extern void glEvalCoord2d (GLdouble u, GLdouble v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glEvalCoord2d...\n");
    }
}

extern void glEvalCoord2dv (const GLdouble *u)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glEvalCoord2dv...\n");
    }
}

extern void glEvalCoord2f (GLfloat u, GLfloat v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glEvalCoord2f...\n");
    }
}

extern void glEvalCoord2fv (const GLfloat *u)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glEvalCoord2fv...\n");
    }
}

extern void glEvalMesh1 (GLenum mode, GLint i1, GLint i2)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glEvalMesh1...\n");
    }
}

extern void glEvalMesh2 (GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glEvalMesh2...\n");
    }
}

extern void glEvalPoint1 (GLint i)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glEvalPoint1...\n");
    }
}

extern void glEvalPoint2 (GLint i, GLint j)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glEvalPoint2...\n");
    }
}

extern void glFeedbackBuffer (GLsizei size, GLenum type, GLfloat *buffer)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glFeedbackBuffer...\n");
    }
}

extern void glFinish (void)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glFinish...\n");
    }
}

extern void glFlush (void)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glFlush...\n");
    }
}

extern void glFogf (GLenum pname, GLfloat param)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glFogf...\n");
    }
}

extern void glFogfv (GLenum pname, const GLfloat *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glFogfv...\n");
    }
}

extern void glFogi (GLenum pname, GLint param)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glFogi...\n");
    }
}

extern void glFogiv (GLenum pname, const GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glFogiv...\n");
    }
}

extern void glGetBooleanv (GLenum pname, GLboolean *params)
{
	switch (pname)
	{
	case GL_DEPTH_WRITEMASK:
		params[0] = glState.depthWriteMask;
		break;

	case GL_COLOR_WRITEMASK:
		params[0] = glState.colorWriteMask[GLS3D_COLOR_WRITE_MASK_R];
		params[1] = glState.colorWriteMask[GLS3D_COLOR_WRITE_MASK_G];
		params[2] = glState.colorWriteMask[GLS3D_COLOR_WRITE_MASK_B];
		params[3] = glState.colorWriteMask[GLS3D_COLOR_WRITE_MASK_A];
		break;

	default:
		// Not implemented
		if(stubMsg) {
			fprintf(stderr, "stubbed glGetBooleanv...\n");
		}
	}

}

extern void glGetClipPlane (GLenum plane, GLdouble *equation)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetClipPlane...\n");
    }
}

extern void glGetColorTable (GLenum target, GLenum format, GLenum type, GLvoid *table)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetColorTable...\n");
    }
}

extern void glGetColorTableParameterfv (GLenum target, GLenum pname, GLfloat *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetColorTableParameterfv...\n");
    }
}

extern void glGetColorTableParameteriv (GLenum target, GLenum pname, GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetColorTableParameteriv...\n");
    }
}

extern void glGetConvolutionFilter (GLenum target, GLenum format, GLenum type, GLvoid *image)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetConvolutionFilter...\n");
    }
}

extern void glGetConvolutionParameterfv (GLenum target, GLenum pname, GLfloat *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetConvolutionParameterfv...\n");
    }
}

extern void glGetConvolutionParameteriv (GLenum target, GLenum pname, GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetConvolutionParameteriv...\n");
    }
}

extern void glGetDoublev (GLenum pname, GLdouble *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetDoublev...\n");
    }
}

extern void glGetHistogram (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetHistogram...\n");
    }
}

extern void glGetHistogramParameterfv (GLenum target, GLenum pname, GLfloat *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetHistogramParameterfv...\n");
    }
}

extern void glGetHistogramParameteriv (GLenum target, GLenum pname, GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetHistogramParameteriv...\n");
    }
}

extern void glGetLightfv (GLenum light, GLenum pname, GLfloat *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetLightfv...\n");
    }
}

extern void glGetLightiv (GLenum light, GLenum pname, GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetLightiv...\n");
    }
}

extern void glGetMapdv (GLenum target, GLenum query, GLdouble *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetMapdv...\n");
    }
}

extern void glGetMapfv (GLenum target, GLenum query, GLfloat *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetMapfv...\n");
    }
}

extern void glGetMapiv (GLenum target, GLenum query, GLint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetMapiv...\n");
    }
}

extern void glGetMaterialfv (GLenum face, GLenum pname, GLfloat *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetMaterialfv...\n");
    }
}

extern void glGetMaterialiv (GLenum face, GLenum pname, GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetMaterialiv...\n");
    }
}

extern void glGetMinmax (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetMinmax...\n");
    }
}

extern void glGetMinmaxParameterfv (GLenum target, GLenum pname, GLfloat *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetMinmaxParameterfv...\n");
    }
}

extern void glGetMinmaxParameteriv (GLenum target, GLenum pname, GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetMinmaxParameteriv...\n");
    }
}

extern void glGetPixelMapfv (GLenum map, GLfloat *values)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetPixelMapfv...\n");
    }
}

extern void glGetPixelMapuiv (GLenum map, GLuint *values)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetPixelMapuiv...\n");
    }
}

extern void glGetPixelMapusv (GLenum map, GLushort *values)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetPixelMapusv...\n");
    }
}

extern void glGetPointerv (GLenum pname, GLvoid* *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetPointerv...\n");
    }
}

extern void glGetPolygonStipple (GLubyte *mask)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetPolygonStipple...\n");
    }
}

extern void glGetSeparableFilter (GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetSeparableFilter...\n");
    }
}

extern void glGetTexEnvfv (GLenum target, GLenum pname, GLfloat *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetTexEnvfv...\n");
    }
}

extern void glGetTexEnviv (GLenum target, GLenum pname, GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetTexEnviv...\n");
    }
}

extern void glGetTexGendv (GLenum coord, GLenum pname, GLdouble *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetTexGendv...\n");
    }
}

extern void glGetTexGenfv (GLenum coord, GLenum pname, GLfloat *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetTexGenfv...\n");
    }
}

extern void glGetTexGeniv (GLenum coord, GLenum pname, GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetTexGeniv...\n");
    }
}

extern void glGetTexImage (GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetTexImage...\n");
    }
}

extern void glGetTexLevelParameterfv (GLenum target, GLint level, GLenum pname, GLfloat *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetTexLevelParameterfv...\n");
    }
}

extern void glGetTexLevelParameteriv (GLenum target, GLint level, GLenum pname, GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetTexLevelParameteriv...\n");
    }
}

extern void glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetTexParameterfv...\n");
    }
}

extern void glGetTexParameteriv (GLenum target, GLenum pname, GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetTexParameteriv...\n");
    }
}

extern void glHint (GLenum target, GLenum mode)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glHint...\n");
    }
}

extern void glHistogram (GLenum target, GLsizei width, GLenum internalformat, GLboolean sink)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glHistogram...\n");
    }
}

extern void glIndexMask (GLuint mask)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glIndexMask...\n");
    }
}

extern void glIndexPointer (GLenum type, GLsizei stride, const GLvoid *pointer)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glIndexPointer...\n");
    }
}

extern void glIndexd (GLdouble c)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glIndexd...\n");
    }
}

extern void glIndexdv (const GLdouble *c)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glIndexdv...\n");
    }
}

extern void glIndexf (GLfloat c)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glIndexf...\n");
    }
}

extern void glIndexfv (const GLfloat *c)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glIndexfv...\n");
    }
}

extern void glIndexi (GLint c)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glIndexi...\n");
    }
}

extern void glIndexiv (const GLint *c)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glIndexiv...\n");
    }
}

extern void glIndexs (GLshort c)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glIndexs...\n");
    }
}

extern void glIndexsv (const GLshort *c)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glIndexsv...\n");
    }
}

extern void glIndexub (GLubyte c)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glIndexub...\n");
    }
}

extern void glIndexubv (const GLubyte *c)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glIndexubv...\n");
    }
}

extern void glInitNames (void)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glInitNames...\n");
    }
}

extern void glInterleavedArrays (GLenum format, GLsizei stride, const GLvoid *pointer)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glInterleavedArrays...\n");
    }
}

extern void glLightModelf (GLenum pname, GLfloat param)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glLightModelf...\n");
    }
}

extern void glLightModelfv (GLenum pname, const GLfloat *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glLightModelfv...\n");
    }
}

extern void glLightModeliv (GLenum pname, const GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glLightModeliv...\n");
    }
}

extern void glLightf (GLenum light, GLenum pname, GLfloat param)
{
    if (pname == GL_SPOT_EXPONENT || 
		pname == GL_SPOT_CUTOFF ||
        pname == GL_CONSTANT_ATTENUATION || 
		pname == GL_LINEAR_ATTENUATION ||
        pname == GL_QUADRATIC_ATTENUATION) {
		glLightfv(light, pname, &param);
    } else if(verboseDebug) {
		inline_as3("import GLS3D.GLAPI; GLAPI.instance.send('FAIL glLightf only accepts GL_SPOT_EXPONENT, GL_SPOT_CUTOFF, GL_CONSTANT_ATTENUATION, GL_LINEAR_ATTENUATION, GL_QUADRATIC_ATTENUATION');\n");
    }
}

extern void glLighti (GLenum light, GLenum pname, GLint param)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glLighti...\n");
    }
}

extern void glLightiv (GLenum light, GLenum pname, const GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glLightiv...\n");
    }
}

extern void glLineStipple (GLint factor, GLushort pattern)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glLineStipple...\n");
    }
}

extern void glLineWidth (GLfloat width)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glLineWidth...\n");
    }
}

extern void glListBase (GLuint base)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glListBase...\n");
    }
}

extern void glLoadMatrixd (const GLdouble *m)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glLoadMatrixd...\n");
    }
}

extern void glLoadName (GLuint name)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glLoadName...\n");
    }
}

extern void glLogicOp (GLenum opcode)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glLogicOp...\n");
    }
}

extern void glMap1d (GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMap1d...\n");
    }
}

extern void glMap1f (GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMap1f...\n");
    }
}

extern void glMap2d (GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMap2d...\n");
    }
}

extern void glMap2f (GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMap2f...\n");
    }
}

extern void glMapGrid1d (GLint un, GLdouble u1, GLdouble u2)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMapGrid1d...\n");
    }
}

extern void glMapGrid1f (GLint un, GLfloat u1, GLfloat u2)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMapGrid1f...\n");
    }
}

extern void glMapGrid2d (GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMapGrid2d...\n");
    }
}

extern void glMapGrid2f (GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMapGrid2f...\n");
    }
}

extern void glMaterialf (GLenum face, GLenum pname, GLfloat param)
{
    if (pname == GL_SHININESS) {
		glMaterialfv(face, pname, &param);
    } else if(verboseDebug) {
		inline_as3("import GLS3D.GLAPI; GLAPI.instance.send('FAIL glMaterialf only accepts GL_SPOT_EXPONENT, GL_SPOT_CUTOFF, GL_CONSTANT_ATTENUATION, GL_LINEAR_ATTENUATION, GL_QUADRATIC_ATTENUATION');\n");
    }
}

extern void glMateriali (GLenum face, GLenum pname, GLint param)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMateriali...\n");
    }
}

extern void glMaterialiv (GLenum face, GLenum pname, const GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMaterialiv...\n");
    }
}

extern void glMinmax (GLenum target, GLenum internalformat, GLboolean sink)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMinmax...\n");
    }
}

extern void glNormal3b (GLbyte nx, GLbyte ny, GLbyte nz)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glNormal3b...\n");
    }
}

extern void glNormal3bv (const GLbyte *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glNormal3bv...\n");
    }
}

extern void glNormal3i (GLint nx, GLint ny, GLint nz)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glNormal3i...\n");
    }
}

extern void glNormal3iv (const GLint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glNormal3iv...\n");
    }
}

extern void glNormal3s (GLshort nx, GLshort ny, GLshort nz)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glNormal3s...\n");
    }
}

extern void glNormal3sv (const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glNormal3sv...\n");
    }
}

extern void glNormalPointer (GLenum type, GLsizei stride, const GLvoid *pointer)
{
	AState.normals.size		= 3;
	AState.normals.type		= type;
	AState.normals.stride	= stride;
	AState.normals.ptr		= pointer;
}

extern void glPassThrough (GLfloat token)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glPassThrough...\n");
    }
}

extern void glPixelMapfv (GLenum map, GLint mapsize, const GLfloat *values)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glPixelMapfv...\n");
    }
}

extern void glPixelMapuiv (GLenum map, GLint mapsize, const GLuint *values)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glPixelMapuiv...\n");
    }
}

extern void glPixelMapusv (GLenum map, GLint mapsize, const GLushort *values)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glPixelMapusv...\n");
    }
}

extern void glPixelStoref (GLenum pname, GLfloat param)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glPixelStoref...\n");
    }
}

extern void glPixelStorei (GLenum pname, GLint param)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glPixelStorei...\n");
    }
}

extern void glPixelTransferf (GLenum pname, GLfloat param)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glPixelTransferf...\n");
    }
}

extern void glPixelTransferi (GLenum pname, GLint param)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glPixelTransferi...\n");
    }
}

extern void glPixelZoom (GLfloat xfactor, GLfloat yfactor)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glPixelZoom...\n");
    }
}

extern void glPointSize (GLfloat size)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glPointSize...\n");
    }
}

extern void glPolygonStipple (const GLubyte *mask)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glPolygonStipple...\n");
    }
}

extern void glPopClientAttrib (void)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glPopClientAttrib...\n");
    }
}

extern void glPopName (void)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glPopName...\n");
    }
}

extern void glPrioritizeTextures (GLsizei n, const GLuint *textures, const GLclampf *priorities)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glPrioritizeTextures...\n");
    }
}

extern void glPushClientAttrib (GLbitfield mask)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glPushClientAttrib...\n");
    }
}

extern void glPushName (GLuint name)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glPushName...\n");
    }
}

extern void glRasterPos2d (GLdouble x, GLdouble y)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos2d...\n");
    }
}

extern void glRasterPos2dv (const GLdouble *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos2dv...\n");
    }
}

extern void glRasterPos2f (GLfloat x, GLfloat y)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos2f...\n");
    }
}

extern void glRasterPos2fv (const GLfloat *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos2fv...\n");
    }
}

extern void glRasterPos2i (GLint x, GLint y)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos2i...\n");
    }
}

extern void glRasterPos2iv (const GLint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos2iv...\n");
    }
}

extern void glRasterPos2s (GLshort x, GLshort y)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos2s...\n");
    }
}

extern void glRasterPos2sv (const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos2sv...\n");
    }
}

extern void glRasterPos3d (GLdouble x, GLdouble y, GLdouble z)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos3d...\n");
    }
}

extern void glRasterPos3dv (const GLdouble *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos3dv...\n");
    }
}

extern void glRasterPos3f (GLfloat x, GLfloat y, GLfloat z)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos3f...\n");
    }
}

extern void glRasterPos3fv (const GLfloat *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos3fv...\n");
    }
}

extern void glRasterPos3i (GLint x, GLint y, GLint z)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos3i...\n");
    }
}

extern void glRasterPos3iv (const GLint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos3iv...\n");
    }
}

extern void glRasterPos3s (GLshort x, GLshort y, GLshort z)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos3s...\n");
    }
}

extern void glRasterPos3sv (const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos3sv...\n");
    }
}

extern void glRasterPos4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos4d...\n");
    }
}

extern void glRasterPos4dv (const GLdouble *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos4dv...\n");
    }
}

extern void glRasterPos4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos4f...\n");
    }
}

extern void glRasterPos4fv (const GLfloat *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos4fv...\n");
    }
}

extern void glRasterPos4i (GLint x, GLint y, GLint z, GLint w)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos4i...\n");
    }
}

extern void glRasterPos4iv (const GLint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos4iv...\n");
    }
}

extern void glRasterPos4s (GLshort x, GLshort y, GLshort z, GLshort w)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos4s...\n");
    }
}

extern void glRasterPos4sv (const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRasterPos4sv...\n");
    }
}

extern void glRectd (GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRectd...\n");
    }
}

extern void glRectdv (const GLdouble *v1, const GLdouble *v2)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRectdv...\n");
    }
}

extern void glRectf (GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRectf...\n");
    }
}

extern void glRectfv (const GLfloat *v1, const GLfloat *v2)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRectfv...\n");
    }
}

extern void glRecti (GLint x1, GLint y1, GLint x2, GLint y2)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRecti...\n");
    }
}

extern void glRectiv (const GLint *v1, const GLint *v2)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRectiv...\n");
    }
}

extern void glRects (GLshort x1, GLshort y1, GLshort x2, GLshort y2)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRects...\n");
    }
}

extern void glRectsv (const GLshort *v1, const GLshort *v2)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRectsv...\n");
    }
}

extern void glResetHistogram (GLenum target)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glResetHistogram...\n");
    }
}

extern void glResetMinmax (GLenum target)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glResetMinmax...\n");
    }
}

extern void glSelectBuffer (GLsizei size, GLuint *buffer)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glSelectBuffer...\n");
    }
}

extern void glSeparableFilter2D (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *row, const GLvoid *column)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glSeparableFilter2D...\n");
    }
}

extern void glStencilMask (GLuint mask)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glStencilMask...\n");
    }
}

extern void glTexCoord1d (GLdouble s)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord1d...\n");
    }
}

extern void glTexCoord1dv (const GLdouble *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord1dv...\n");
    }
}

extern void glTexCoord1f (GLfloat s)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord1f...\n");
    }
}

extern void glTexCoord1fv (const GLfloat *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord1fv...\n");
    }
}

extern void glTexCoord1i (GLint s)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord1i...\n");
    }
}

extern void glTexCoord1iv (const GLint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord1iv...\n");
    }
}

extern void glTexCoord1s (GLshort s)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord1s...\n");
    }
}

extern void glTexCoord1sv (const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord1sv...\n");
    }
}

extern void glTexCoord2d (GLdouble s, GLdouble t)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord2d...\n");
    }
}

extern void glTexCoord2dv (const GLdouble *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord2dv...\n");
    }
}

extern void glTexCoord2i (GLint s, GLint t)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord2i...\n");
    }
}

extern void glTexCoord2iv (const GLint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord2iv...\n");
    }
}

extern void glTexCoord2s (GLshort s, GLshort t)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord2s...\n");
    }
}

extern void glTexCoord2sv (const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord2sv...\n");
    }
}

extern void glTexCoord3d (GLdouble s, GLdouble t, GLdouble r)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord3d...\n");
    }
}

extern void glTexCoord3dv (const GLdouble *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord3dv...\n");
    }
}

extern void glTexCoord3f (GLfloat s, GLfloat t, GLfloat r)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord3f...\n");
    }
}

extern void glTexCoord3fv (const GLfloat *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord3fv...\n");
    }
}

extern void glTexCoord3i (GLint s, GLint t, GLint r)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord3i...\n");
    }
}

extern void glTexCoord3iv (const GLint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord3iv...\n");
    }
}

extern void glTexCoord3s (GLshort s, GLshort t, GLshort r)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord3s...\n");
    }
}

extern void glTexCoord3sv (const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord3sv...\n");
    }
}

extern void glTexCoord4d (GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord4d...\n");
    }
}

extern void glTexCoord4dv (const GLdouble *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord4dv...\n");
    }
}

extern void glTexCoord4f (GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord4f...\n");
    }
}

extern void glTexCoord4fv (const GLfloat *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord4fv...\n");
    }
}

extern void glTexCoord4i (GLint s, GLint t, GLint r, GLint q)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord4i...\n");
    }
}

extern void glTexCoord4iv (const GLint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord4iv...\n");
    }
}

extern void glTexCoord4s (GLshort s, GLshort t, GLshort r, GLshort q)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord4s...\n");
    }
}

extern void glTexCoord4sv (const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexCoord4sv...\n");
    }
}

extern GLboolean glIsEnabled (GLenum cap)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glIsEnabled...\n");
    }
}

extern GLboolean glAreTexturesResident (GLsizei x, const GLuint *y, GLboolean *z)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glAreTexturesResident...\n");
    }
}

extern GLint glRenderMode (GLenum mode)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glRenderMode...\n");
    }
}

extern void glTexEnvfv (GLenum target, GLenum pname, const GLfloat *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexEnvfv...\n");
    }
}

extern void glTexEnvi (GLenum target, GLenum pname, GLint param)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexEnvi...\n");
    }
}

extern void glTexEnviv (GLenum target, GLenum pname, const GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexEnviv...\n");
    }
}

extern void glTexGend (GLenum coord, GLenum pname, GLdouble param)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexGend...\n");
    }
}

extern void glTexGendv (GLenum coord, GLenum pname, const GLdouble *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexGendv...\n");
    }
}

extern void glTexGenf (GLenum coord, GLenum pname, GLfloat param)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexGenf...\n");
    }
}

extern void glTexGenfv (GLenum coord, GLenum pname, const GLfloat *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexGenfv...\n");
    }
}

extern void glTexGeniv (GLenum coord, GLenum pname, const GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexGeniv...\n");
    }
}

extern void glTexImage1D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexImage1D...\n");
    }
}

extern void glTexImage3D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexImage3D...\n");
    }
}

extern void glTexParameteriv (GLenum target, GLenum pname, const GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexParameteriv...\n");
    }
}

extern void glTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexSubImage1D...\n");
    }
}

extern void glTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glTexSubImage3D...\n");
    }
}

extern void glVertex2d (GLdouble x, GLdouble y)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertex2d...\n");
    }
}

extern void glVertex2dv (const GLdouble *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertex2dv...\n");
    }
}

extern void glVertex2fv (const GLfloat *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertex2fv...\n");
    }
}

extern void glVertex2iv (const GLint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertex2iv...\n");
    }
}

extern void glVertex2s (GLshort x, GLshort y)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertex2s...\n");
    }
}

extern void glVertex2sv (const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertex2sv...\n");
    }
}

extern void glVertex3i (GLint x, GLint y, GLint z)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertex3i...\n");
    }
}

extern void glVertex3iv (const GLint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertex3iv...\n");
    }
}

extern void glVertex3s (GLshort x, GLshort y, GLshort z)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertex3s...\n");
    }
}

extern void glVertex3sv (const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertex3sv...\n");
    }
}

extern void glVertex4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertex4d...\n");
    }
}

extern void glVertex4dv (const GLdouble *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertex4dv...\n");
    }
}

extern void glVertex4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertex4f...\n");
    }
}

extern void glVertex4fv (const GLfloat *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertex4fv...\n");
    }
}

extern void glVertex4i (GLint x, GLint y, GLint z, GLint w)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertex4i...\n");
    }
}

extern void glVertex4iv (const GLint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertex4iv...\n");
    }
}

extern void glVertex4s (GLshort x, GLshort y, GLshort z, GLshort w)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertex4s...\n");
    }
}

extern void glVertex4sv (const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertex4sv...\n");
    }
}

extern void glSampleCoverage (GLclampf value, GLboolean invert)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glSampleCoverage...\n");
    }
}

extern void glSamplePass (GLenum pass)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glSamplePass...\n");
    }
}

extern void glLoadTransposeMatrixf (const GLfloat *m)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glLoadTransposeMatrixf...\n");
    }
}

extern void glLoadTransposeMatrixd (const GLdouble *m)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glLoadTransposeMatrixd...\n");
    }
}

extern void glMultTransposeMatrixf (const GLfloat *m)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultTransposeMatrixf...\n");
    }
}

extern void glMultTransposeMatrixd (const GLdouble *m)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultTransposeMatrixd...\n");
    }
}

extern void glCompressedTexImage3D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glCompressedTexImage3D...\n");
    }
}

extern void glCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glCompressedTexImage2D...\n");
    }
}

extern void glCompressedTexImage1D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glCompressedTexImage1D...\n");
    }
}

extern void glCompressedTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glCompressedTexSubImage3D...\n");
    }
}

extern void glCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glCompressedTexSubImage2D...\n");
    }
}

extern void glCompressedTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glCompressedTexSubImage1D...\n");
    }
}

extern void glGetCompressedTexImage (GLenum target, GLint lod, GLvoid *img)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetCompressedTexImage...\n");
    }
}

extern void glMultiTexCoord1d (GLenum target, GLdouble s)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord1d...\n");
    }
}

extern void glMultiTexCoord1dv (GLenum target, const GLdouble *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord1dv...\n");
    }
}

extern void glMultiTexCoord1f (GLenum target, GLfloat s)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord1f...\n");
    }
}

extern void glMultiTexCoord1fv (GLenum target, const GLfloat *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord1fv...\n");
    }
}

extern void glMultiTexCoord1i (GLenum target, GLint s)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord1i...\n");
    }
}

extern void glMultiTexCoord1iv (GLenum target, const GLint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord1iv...\n");
    }
}

extern void glMultiTexCoord1s (GLenum target, GLshort s)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord1s...\n");
    }
}

extern void glMultiTexCoord1sv (GLenum target, const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord1sv...\n");
    }
}

extern void glMultiTexCoord2d (GLenum target, GLdouble s, GLdouble t)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord2d...\n");
    }
}

extern void glMultiTexCoord2dv (GLenum target, const GLdouble *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord2dv...\n");
    }
}

extern void glMultiTexCoord2f (GLenum target, GLfloat s, GLfloat t)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord2f...\n");
    }
}

extern void glMultiTexCoord2fv (GLenum target, const GLfloat *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord2fv...\n");
    }
}

extern void glMultiTexCoord2i (GLenum target, GLint s, GLint t)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord2i...\n");
    }
}

extern void glMultiTexCoord2iv (GLenum target, const GLint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord2iv...\n");
    }
}

extern void glMultiTexCoord2s (GLenum target, GLshort s, GLshort t)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord2s...\n");
    }
}

extern void glMultiTexCoord2sv (GLenum target, const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord2sv...\n");
    }
}

extern void glMultiTexCoord3d (GLenum target, GLdouble s, GLdouble t, GLdouble r)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord3d...\n");
    }
}

extern void glMultiTexCoord3dv (GLenum target, const GLdouble *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord3dv...\n");
    }
}

extern void glMultiTexCoord3f (GLenum target, GLfloat s, GLfloat t, GLfloat r)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord3f...\n");
    }
}

extern void glMultiTexCoord3fv (GLenum target, const GLfloat *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord3fv...\n");
    }
}

extern void glMultiTexCoord3i (GLenum target, GLint s, GLint t, GLint r)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord3i...\n");
    }
}

extern void glMultiTexCoord3iv (GLenum target, const GLint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord3iv...\n");
    }
}

extern void glMultiTexCoord3s (GLenum target, GLshort s, GLshort t, GLshort r)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord3s...\n");
    }
}

extern void glMultiTexCoord3sv (GLenum target, const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord3sv...\n");
    }
}

extern void glMultiTexCoord4d (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord4d...\n");
    }
}

extern void glMultiTexCoord4dv (GLenum target, const GLdouble *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord4dv...\n");
    }
}

extern void glMultiTexCoord4f (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord4f...\n");
    }
}

extern void glMultiTexCoord4fv (GLenum target, const GLfloat *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord4fv...\n");
    }
}

extern void glMultiTexCoord4i (GLenum target, GLint x, GLint s, GLint t, GLint r)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord4i...\n");
    }
}

extern void glMultiTexCoord4iv (GLenum target, const GLint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord4iv...\n");
    }
}

extern void glMultiTexCoord4s (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord4s...\n");
    }
}

extern void glMultiTexCoord4sv (GLenum target, const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiTexCoord4sv...\n");
    }
}

extern void glFogCoordf (GLfloat coord)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glFogCoordf...\n");
    }
}

extern void glFogCoordfv (const GLfloat *coord)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glFogCoordfv...\n");
    }
}

extern void glFogCoordd (GLdouble coord)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glFogCoordd...\n");
    }
}

extern void glFogCoorddv (const GLdouble * coord)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glFogCoorddv...\n");
    }
}

extern void glFogCoordPointer (GLenum type, GLsizei stride, const GLvoid *pointer)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glFogCoordPointer...\n");
    }
}

extern void glSecondaryColor3b (GLbyte red, GLbyte green, GLbyte blue)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glSecondaryColor3b...\n");
    }
}

extern void glSecondaryColor3bv (const GLbyte *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glSecondaryColor3bv...\n");
    }
}

extern void glSecondaryColor3d (GLdouble red, GLdouble green, GLdouble blue)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glSecondaryColor3d...\n");
    }
}

extern void glSecondaryColor3dv (const GLdouble *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glSecondaryColor3dv...\n");
    }
}

extern void glSecondaryColor3f (GLfloat red, GLfloat green, GLfloat blue)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glSecondaryColor3f...\n");
    }
}

extern void glSecondaryColor3fv (const GLfloat *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glSecondaryColor3fv...\n");
    }
}

extern void glSecondaryColor3i (GLint red, GLint green, GLint blue)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glSecondaryColor3i...\n");
    }
}

extern void glSecondaryColor3iv (const GLint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glSecondaryColor3iv...\n");
    }
}

extern void glSecondaryColor3s (GLshort red, GLshort green, GLshort blue)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glSecondaryColor3s...\n");
    }
}

extern void glSecondaryColor3sv (const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glSecondaryColor3sv...\n");
    }
}

extern void glSecondaryColor3ub (GLubyte red, GLubyte green, GLubyte blue)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glSecondaryColor3ub...\n");
    }
}

extern void glSecondaryColor3ubv (const GLubyte *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glSecondaryColor3ubv...\n");
    }
}

extern void glSecondaryColor3ui (GLuint red, GLuint green, GLuint blue)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glSecondaryColor3ui...\n");
    }
}

extern void glSecondaryColor3uiv (const GLuint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glSecondaryColor3uiv...\n");
    }
}

extern void glSecondaryColor3us (GLushort red, GLushort green, GLushort blue)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glSecondaryColor3us...\n");
    }
}

extern void glSecondaryColor3usv (const GLushort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glSecondaryColor3usv...\n");
    }
}

extern void glSecondaryColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glSecondaryColorPointer...\n");
    }
}

extern void glPointParameterf (GLenum pname, GLfloat param)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glPointParameterf...\n");
    }
}

extern void glPointParameterfv (GLenum pname, const GLfloat *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glPointParameterfv...\n");
    }
}

extern void glPointParameteri (GLenum pname, GLint param)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glPointParameteri...\n");
    }
}

extern void glPointParameteriv (GLenum pname, const GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glPointParameteriv...\n");
    }
}

extern void glBlendFuncSeparate (GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glBlendFuncSeparate...\n");
    }
}

extern void glMultiDrawArrays (GLenum mode, const GLint *first, const GLsizei *count, GLsizei primcount)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiDrawArrays...\n");
    }
}

extern void glMultiDrawElements (GLenum mode, const GLsizei *count, GLenum type, const GLvoid* *indices, GLsizei primcount)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glMultiDrawElements...\n");
    }
}

extern void glWindowPos2d (GLdouble x, GLdouble y)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glWindowPos2d...\n");
    }
}

extern void glWindowPos2dv (const GLdouble *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glWindowPos2dv...\n");
    }
}

extern void glWindowPos2f (GLfloat x, GLfloat y)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glWindowPos2f...\n");
    }
}

extern void glWindowPos2fv (const GLfloat *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glWindowPos2fv...\n");
    }
}

extern void glWindowPos2i (GLint x, GLint y)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glWindowPos2i...\n");
    }
}

extern void glWindowPos2iv (const GLint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glWindowPos2iv...\n");
    }
}

extern void glWindowPos2s (GLshort x, GLshort y)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glWindowPos2s...\n");
    }
}

extern void glWindowPos2sv (const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glWindowPos2sv...\n");
    }
}

extern void glWindowPos3d (GLdouble x, GLdouble y, GLdouble z)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glWindowPos3d...\n");
    }
}

extern void glWindowPos3dv (const GLdouble *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glWindowPos3dv...\n");
    }
}

extern void glWindowPos3f (GLfloat x, GLfloat y, GLfloat z)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glWindowPos3f...\n");
    }
}

extern void glWindowPos3fv (const GLfloat *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glWindowPos3fv...\n");
    }
}

extern void glWindowPos3i (GLint x, GLint y, GLint z)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glWindowPos3i...\n");
    }
}

extern void glWindowPos3iv (const GLint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glWindowPos3iv...\n");
    }
}

extern void glWindowPos3s (GLshort x, GLshort y, GLshort z)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glWindowPos3s...\n");
    }
}

extern void glWindowPos3sv (const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glWindowPos3sv...\n");
    }
}

extern void glGenQueries(GLsizei n, GLuint *ids)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGenQueries(GLsizei...\n");
    }
}

extern void glDeleteQueries(GLsizei n, const GLuint *ids)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glDeleteQueries(GLsizei...\n");
    }
}

extern void glBeginQuery(GLenum target, GLuint id)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glBeginQuery(GLenum...\n");
    }
}

extern void glEndQuery(GLenum target)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glEndQuery(GLenum...\n");
    }
}

extern void glGetQueryiv(GLenum target, GLenum pname, GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetQueryiv(GLenum...\n");
    }
}

extern void glGetQueryObjectiv(GLuint id, GLenum pname, GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetQueryObjectiv(GLuint...\n");
    }
}

extern void glGetQueryObjectuiv(GLuint id, GLenum pname, GLuint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetQueryObjectuiv(GLuint...\n");
    }
}

extern void glBindBuffer (GLenum target, GLuint buffer)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glBindBuffer...\n");
    }
}

extern void glDeleteBuffers (GLsizei n, const GLuint *buffers)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glDeleteBuffers...\n");
    }
}

extern void glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glBufferSubData...\n");
    }
}

extern void glGetBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, GLvoid *data)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetBufferSubData...\n");
    }
}

extern GLvoid * glMapBuffer (GLenum target, GLenum access)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed *...\n");
    }
}

extern void glGetBufferParameteriv (GLenum target, GLenum pname, GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetBufferParameteriv...\n");
    }
}

extern void glGetBufferPointerv (GLenum target, GLenum pname, GLvoid **params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetBufferPointerv...\n");
    }
}

extern void glVertexAttrib1d (GLuint index, GLdouble x)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib1d...\n");
    }
}

extern void glVertexAttrib1dv (GLuint index, const GLdouble *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib1dv...\n");
    }
}

extern void glVertexAttrib1f (GLuint index, GLfloat x)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib1f...\n");
    }
}

extern void glVertexAttrib1fv (GLuint index, const GLfloat *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib1fv...\n");
    }
}

extern void glVertexAttrib1s (GLuint index, GLshort x)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib1s...\n");
    }
}

extern void glVertexAttrib1sv (GLuint index, const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib1sv...\n");
    }
}

extern void glVertexAttrib2d (GLuint index, GLdouble x, GLdouble y)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib2d...\n");
    }
}

extern void glVertexAttrib2dv (GLuint index, const GLdouble *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib2dv...\n");
    }
}

extern void glVertexAttrib2f (GLuint index, GLfloat x, GLfloat y)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib2f...\n");
    }
}

extern void glVertexAttrib2fv (GLuint index, const GLfloat *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib2fv...\n");
    }
}

extern void glVertexAttrib2s (GLuint index, GLshort x, GLshort y)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib2s...\n");
    }
}

extern void glVertexAttrib2sv (GLuint index, const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib2sv...\n");
    }
}

extern void glVertexAttrib3d (GLuint index, GLdouble x, GLdouble y, GLdouble z)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib3d...\n");
    }
}

extern void glVertexAttrib3dv (GLuint index, const GLdouble *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib3dv...\n");
    }
}

extern void glVertexAttrib3f (GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib3f...\n");
    }
}

extern void glVertexAttrib3fv (GLuint index, const GLfloat *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib3fv...\n");
    }
}

extern void glVertexAttrib3s (GLuint index, GLshort x, GLshort y, GLshort z)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib3s...\n");
    }
}

extern void glVertexAttrib3sv (GLuint index, const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib3sv...\n");
    }
}

extern void glVertexAttrib4Nbv (GLuint index, const GLbyte *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib4Nbv...\n");
    }
}

extern void glVertexAttrib4Niv (GLuint index, const GLint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib4Niv...\n");
    }
}

extern void glVertexAttrib4Nsv (GLuint index, const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib4Nsv...\n");
    }
}

extern void glVertexAttrib4Nub (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib4Nub...\n");
    }
}

extern void glVertexAttrib4Nubv (GLuint index, const GLubyte *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib4Nubv...\n");
    }
}

extern void glVertexAttrib4Nuiv (GLuint index, const GLuint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib4Nuiv...\n");
    }
}

extern void glVertexAttrib4Nusv (GLuint index, const GLushort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib4Nusv...\n");
    }
}

extern void glVertexAttrib4bv (GLuint index, const GLbyte *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib4bv...\n");
    }
}

extern void glVertexAttrib4d (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib4d...\n");
    }
}

extern void glVertexAttrib4dv (GLuint index, const GLdouble *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib4dv...\n");
    }
}

extern void glVertexAttrib4f (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib4f...\n");
    }
}

extern void glVertexAttrib4fv (GLuint index, const GLfloat *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib4fv...\n");
    }
}

extern void glVertexAttrib4iv (GLuint index, const GLint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib4iv...\n");
    }
}

extern void glVertexAttrib4s (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib4s...\n");
    }
}

extern void glVertexAttrib4sv (GLuint index, const GLshort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib4sv...\n");
    }
}

extern void glVertexAttrib4ubv (GLuint index, const GLubyte *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib4ubv...\n");
    }
}

extern void glVertexAttrib4uiv (GLuint index, const GLuint *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib4uiv...\n");
    }
}

extern void glVertexAttrib4usv (GLuint index, const GLushort *v)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttrib4usv...\n");
    }
}

extern void glVertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glVertexAttribPointer...\n");
    }
}

extern void glEnableVertexAttribArray (GLuint index)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glEnableVertexAttribArray...\n");
    }
}

extern void glDisableVertexAttribArray (GLuint index)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glDisableVertexAttribArray...\n");
    }
}

extern void glGetVertexAttribdv (GLuint index, GLenum pname, GLdouble *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetVertexAttribdv...\n");
    }
}

extern void glGetVertexAttribfv (GLuint index, GLenum pname, GLfloat *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetVertexAttribfv...\n");
    }
}

extern void glGetVertexAttribiv (GLuint index, GLenum pname, GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetVertexAttribiv...\n");
    }
}

extern void glGetVertexAttribPointerv (GLuint index, GLenum pname, GLvoid* *pointer)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetVertexAttribPointerv...\n");
    }
}

extern GLuint glCreateShader (GLenum)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glCreateShader...\n");
    }
}
    
extern void glDeleteShader (GLuint shader)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glDeleteShader...\n");
    }
}

extern void glDetachShader (GLuint program, GLuint shader)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glDetachShader...\n");
    }
}

extern void glShaderSource (GLuint shader, GLsizei count, const GLchar* *string, const GLint *length)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glShaderSource...\n");
    }
}

extern void glCompileShader (GLuint shader)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glCompileShader...\n");
    }
}

extern GLuint glCreateProgram (void)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glCreateProgram...\n");
    }
}

extern void glAttachShader (GLuint program, GLuint shader)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glAttachShader...\n");
    }
}

extern void glLinkProgram (GLuint program)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glLinkProgram...\n");
    }
}

extern void glUseProgram (GLuint program)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUseProgram...\n");
    }
}

extern void glDeleteProgram (GLuint program)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glDeleteProgram...\n");
    }
}

extern void glValidateProgram (GLuint program)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glValidateProgram...\n");
    }
}

extern void glUniform1f (GLint location, GLfloat v0)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniform1f...\n");
    }
}

extern void glUniform2f (GLint location, GLfloat v0, GLfloat v1)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniform2f...\n");
    }
}

extern void glUniform3f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniform3f...\n");
    }
}

extern void glUniform4f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniform4f...\n");
    }
}

extern void glUniform1i (GLint location, GLint v0)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniform1i...\n");
    }
}

extern void glUniform2i (GLint location, GLint v0, GLint v1)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniform2i...\n");
    }
}

extern void glUniform3i (GLint location, GLint v0, GLint v1, GLint v2)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniform3i...\n");
    }
}

extern void glUniform4i (GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniform4i...\n");
    }
}

extern void glUniform1fv (GLint location, GLsizei count, const GLfloat *value)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniform1fv...\n");
    }
}

extern void glUniform2fv (GLint location, GLsizei count, const GLfloat *value)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniform2fv...\n");
    }
}

extern void glUniform3fv (GLint location, GLsizei count, const GLfloat *value)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniform3fv...\n");
    }
}

extern void glUniform4fv (GLint location, GLsizei count, const GLfloat *value)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniform4fv...\n");
    }
}

extern void glUniform1iv (GLint location, GLsizei count, const GLint *value)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniform1iv...\n");
    }
}

extern void glUniform2iv (GLint location, GLsizei count, const GLint *value)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniform2iv...\n");
    }
}

extern void glUniform3iv (GLint location, GLsizei count, const GLint *value)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniform3iv...\n");
    }
}

extern void glUniform4iv (GLint location, GLsizei count, const GLint *value)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniform4iv...\n");
    }
}

extern void glUniformMatrix2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniformMatrix2fv...\n");
    }
}

extern void glUniformMatrix3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniformMatrix3fv...\n");
    }
}

extern void glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniformMatrix4fv...\n");
    }
}

extern void glGetShaderiv (GLuint shader, GLenum pname, GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetShaderiv...\n");
    }
}

extern void glGetProgramiv (GLuint program, GLenum pname, GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetProgramiv...\n");
    }
}

extern void glGetAttachedShaders (GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetAttachedShaders...\n");
    }
}

extern void glGetShaderInfoLog (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetShaderInfoLog...\n");
    }
}

extern void glGetProgramInfoLog (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetProgramInfoLog...\n");
    }
}

extern void glGetActiveUniform (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetActiveUniform...\n");
    }
}

extern void glGetUniformfv (GLuint program, GLint location, GLfloat *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetUniformfv...\n");
    }
}

extern void glGetUniformiv (GLuint program, GLint location, GLint *params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetUniformiv...\n");
    }
}

extern int glGetUniformLocation (GLuint, const GLchar*)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetUniformLocation...\n");
    }
}

extern void glGetShaderSource (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetShaderSource...\n");
    }
}
    
extern int glGetAttribLocation (GLuint program, const GLchar* name)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetAttribLocation...\n");
    }
}
    
extern void glBindAttribLocation (GLuint program, GLuint index, const GLchar *name)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glBindAttribLocation...\n");
    }
}

extern void glGetActiveAttrib (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetActiveAttrib...\n");
    }
}

extern void glStencilFuncSeparate (GLenum face, GLenum func, GLint ref, GLuint mask)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glStencilFuncSeparate...\n");
    }
}

extern void glStencilOpSeparate (GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glStencilOpSeparate...\n");
    }
}

extern void glStencilMaskSeparate (GLenum face, GLuint mask)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glStencilMaskSeparate...\n");
    }
}

extern void glUniformMatrix2x3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniformMatrix2x3fv...\n");
    }
}

extern void glUniformMatrix3x2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniformMatrix3x2fv...\n");
    }
}

extern void glUniformMatrix2x4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniformMatrix2x4fv...\n");
    }
}

extern void glUniformMatrix4x2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniformMatrix4x2fv...\n");
    }
}

extern void glUniformMatrix3x4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniformMatrix3x4fv...\n");
    }
}

extern void glUniformMatrix4x3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glUniformMatrix4x3fv...\n");
    }
}

extern void glGenFramebuffers (GLsizei, GLuint*)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGenFramebuffers...\n");
    }
}

extern void glGetRenderbufferParameteriv (GLenum target, GLenum pname, GLint* params)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glGetRenderbufferParameteriv...\n");
    }
}

extern void glBindFramebuffer (GLenum target, GLuint framebuffer)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glBindFramebuffer...\n");
    }
}

extern GLenum glCheckFramebufferStatus (GLenum target)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glCheckFramebufferStatus...\n");
    }
}

extern void glDeleteFramebuffers (GLsizei n, const GLuint* framebuffers)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glDeleteFramebuffers...\n");
    }
}

extern void glFramebufferTexture2D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    if(stubMsg) {
        fprintf(stderr, "stubbed glFramebufferTexture2D...\n");
    }
}

}// End of extern "C" 

//**************************************************************************************
// Name: glDraw_XXX
// Note: can further be optimized by using policy based design
// TODO: Refactor this to another file
//**************************************************************************************

#define CALL_GL_DRAW_PRIMITIVES(fn, type, INDEXED, first, count, indices)			\
	switch (type)																	\
	{																				\
	case GL_UNSIGNED_INT:	fn<GLuint,   INDEXED>(first, count, indices); break;	\
	case GL_UNSIGNED_SHORT:	fn<GLushort, INDEXED>(first, count, indices); break;	\
	default:																		\
		if(verboseDebug) {															\
			inline_as3("import GLS3D.GLAPI; GLAPI.instance.send('FAIL glDrawElements must have ubyte/ushort indicies');\n");	\
		}																			\
	}


// Helper struct
struct DrawContext
{
	const StateInfo*	pColor;
	const StateInfo*	pTex0;
	const StateInfo*	pNormal;
	const StateInfo*	pVerts;
	int			colorStride;
	int			tex0Stride;
	int			normalStride;
	int			vertsStride;
	
	void	SetDrawContext	(const ArrayEXTState& states)
	{
		pColor	= &states.colors;
		pTex0	= &states.texcoords[activeTextureUnit - GL_TEXTURE0];
		pNormal	= &states.normals;
		pVerts	= &states.verts;
	
		colorStride = max(pColor->stride, pColor->size);
		tex0Stride	= max(pTex0->stride,  pTex0->size*sizeof(GLfloat));
		normalStride= max(pNormal->stride, pNormal->size*sizeof(GLfloat));
		vertsStride	= max(pVerts->stride, pVerts->size*sizeof(GLfloat));
	}
	void	glSetVertex(int idx)
	{
		// Color
		if (pColor->enabled) {
			GLubyte *cptr = getUBytePtr(pColor->ptr, colorStride, idx);
			glColor4ub(cptr[0], cptr[1], cptr[2], cptr[3]);
		}

		// Texcoord
		if (pTex0->enabled) {
			GLfloat *tptr = getFloatPtr(pTex0->ptr, tex0Stride, idx);
			glTexCoord2f(tptr[0], tptr[1]);
		}

		// Normal
		if (pNormal->enabled) {
			GLfloat *nptr = getFloatPtr(pNormal->ptr, normalStride, idx);
			glNormal3f(nptr[0], nptr[1], nptr[2]);
		}

		// Verts
		if (pVerts->enabled) {
			GLfloat *vptr = getFloatPtr(pVerts->ptr, vertsStride, idx);
			switch (pVerts->size) {
			case 2:
				glVertex2f(vptr[0], vptr[1]);
				break;
			case 3:
				glVertex3f(vptr[0], vptr[1], vptr[2]);
				break;
			default:
				if(verboseDebug) {
					inline_as3("import GLS3D.GLAPI; GLAPI.instance.send('FAIL glDrawElements unknown vert num' + %0);\n" :  : "r"(AState.verts.size));
				}
				break;
			}
		}
	}
};
static DrawContext glDrawContext;

// glDrawPrimitives
template <bool INDEXED>
void _glDrawPrimitives(GLint first, GLsizei count, const GLvoid *indices, GLenum mode, GLenum type)
{
#if (GL_DRAW_PRIMITIVES_DO_PARAM_CHECK)
	// glColor state check 
	if (AState.colors.enabled) {
		// Supported type
		bool glColorSupported  = (AState.colors.type == GL_UNSIGNED_BYTE);	// **NOTE: Can be expanded as we expand more types
			 glColorSupported &= (AState.colors.size == 4);					// Only RGBA colors are supported

		if (!glColorSupported && verboseDebug) {
			inline_as3("import GLS3D.GLAPI; GLAPI.instance.send('FAIL glDrawElements must have ubyte colors');\n");
		}
	}

	// glTexCoord state check
	const StateInfo& tex0 = AState.texcoords[activeTextureUnit - GL_TEXTURE0];
	if (tex0.enabled) {
		// Supported type
		bool glTexCoordSupported  = (tex0.type == GL_FLOAT);				// **NOTE: Can be expanded as we support more types
			 glTexCoordSupported &= (tex0.size == 2);						// Only support 2-component texcoord

		if (!glTexCoordSupported && verboseDebug) {
			inline_as3("import GLS3D.GLAPI; GLAPI.instance.send('FAIL glDrawElements must have float texcoords');\n");
		}
	}

	// glNormal state check
	if (AState.normals.enabled) {
		// Supported type
		bool glNormalSupported	= (AState.normals.type == GL_FLOAT);		//**NOTE: Can be expanded as we support more types
			 glNormalSupported &= (AState.normals.size == 3);

		if (!glNormalSupported && verboseDebug) {
			inline_as3("import GLS3D.GLAPI; GLAPI.instance.send('FAIL glDrawElements must have float normals');\n");
		}
	}

	// glVertex state check
	if (AState.verts.enabled) {
		// Supported type
		bool glVertexSupported	= (AState.verts.type == GL_FLOAT);			//**NOTE: Can be expanded as we support more types
			 glVertexSupported &= (AState.verts.size == 2 || AState.verts.size == 3);

		if (!glVertexSupported && verboseDebug) {
			inline_as3("import GLS3D.GLAPI; GLAPI.instance.send('FAIL glDrawElements must have float verts');\n");
			inline_as3("import GLS3D.GLAPI; GLAPI.instance.send('FAIL glDrawElements unknown vert num' + %0);\n" :  : "r"(AState.verts.size));
		}
	}	
#endif	// GL_DRAW_ELEMENTS_DO_PARAM_CHECK

	// We are going to convert everything to triangles
    glBegin(GL_TRIANGLES);

    switch(mode) 
	{
    case GL_TRIANGLES:
		CALL_GL_DRAW_PRIMITIVES(glDraw_Triangles, type, INDEXED, first, count, indices);
		break;
	case GL_TRIANGLE_STRIP:
		CALL_GL_DRAW_PRIMITIVES(glDraw_TriStrips, type, INDEXED, first, count, indices);
		break;
    default:
        if(verboseDebug) {
            inline_as3("import GLS3D.GLAPI; GLAPI.instance.send('FAIL glDrawElements cant only handle triangles/triangle strips');\n");
			inline_as3("import GLS3D.GLAPI;\n GLAPI.instance.send('stubbed glDrawElements '  +%0 + ',' + %1 + ',' + %2 + ',' + %3);\n" : : "r"(mode), "r"(count), "r"(type), "r"(indices));
        }
        break;
    }

    glEnd();	
}

// GL_TRIANGLES
template < typename T, bool INDEXED >
void glDraw_Triangles(GLint first, GLsizei count, const GLvoid* indices)
{	
	glDrawContext.SetDrawContext(AState);
	PrimIterator<T, INDEXED> iter(first, count, indices); 
	while (iter.hasNext())
	{
		int idx = iter.getIndex();
		glDrawContext.glSetVertex(idx);
		iter.next();
	}
}


// GL_TRIANGLE_STRIP
template <typename T, bool INDEXED>
void glDraw_TriStrips(GLint first, GLsizei count, const GLvoid* indices)
{
#if (GL_DRAW_PRIMITIVES_DO_PARAM_CHECK)
	if (count < 3)
	{
		if(verboseDebug) {
			inline_as3("import GLS3D.GLAPI; GLAPI.instance.send('GL_TRIANGLE_STRIP requires at least 3 verts');\n");
		}
		return;
	}
#endif
	glDrawContext.SetDrawContext(AState);

	PrimIterator<T, INDEXED> iter(first, count, indices);

	int numTriangles	= count-2;

	int	cachedIdx[2];
	cachedIdx[0] = iter.getIndex(); iter.next();
	cachedIdx[1] = iter.getIndex(); iter.next();

	int numLoop = numTriangles / 2;
	while (numLoop-- > 0)
	{
		// Grab 4 indices
		int idx0 = cachedIdx[0];
		int idx1 = cachedIdx[1];
		int idx2 = iter.getIndex(); iter.next();
		int idx3 = iter.getIndex(); iter.next();

		// First triangle - 0,1,2
		glDrawContext.glSetVertex(idx0);
		glDrawContext.glSetVertex(idx1);
		glDrawContext.glSetVertex(idx2);

		// Second triangle - 1,3,2
		glDrawContext.glSetVertex(idx1);
		glDrawContext.glSetVertex(idx3);
		glDrawContext.glSetVertex(idx2);
		
		// Update cached verts
		cachedIdx[0] = idx2;
		cachedIdx[1] = idx3;
	}

	// Finish off the final vert if any
	if ((numTriangles & 1) != 0)
	{
		int idx0 = cachedIdx[0];
		int idx1 = cachedIdx[1];
		int idx2 = iter.getIndex();

		glDrawContext.glSetVertex(idx0);
		glDrawContext.glSetVertex(idx1);
		glDrawContext.glSetVertex(idx2);
	}
}

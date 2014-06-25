/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redestributed without written permission.*/
//Version: 013

#ifndef LTEXTURE_H
#define LTEXTURE_H

#include "LOpenGL.h"
#include <stdio.h>
#include <string>
#include "LFRect.h"

class LTexture
{
    public:
        LTexture();
        /*
        Pre Condition:
         -None
        Post Condition:
         -Initializes member variables
        Side Effects:
         -None
        */

        virtual ~LTexture();
        /*
        Pre Condition:
         -None
        Post Condition:
         -Frees texture
        Side Effects:
         -None
        */

        bool loadTextureFromFile32( std::string path );
        /*
        Pre Condition:
         -A valid OpenGL context
         -Initialized DevIL
        Post Condition:
         -Creates RGBA texture from the given file
         -Pads image to have power-of-two dimensions
         -Reports error to console if texture could not be created
        Side Effects:
         -Binds a NULL texture
        */

        bool loadPixelsFromFile32( std::string path );
        /*
        Pre Condition:
         -Initialized DevIL
        Post Condition:
         -Loads member 32bit pixels from the given file
         -Pads image to have power-of-two dimensions
         -Reports error to console if pixels could not be loaded
        Side Effects:
         -None
        */

        bool loadTextureFromFileWithColorKey32( std::string path, GLubyte r, GLubyte g, GLubyte b, GLubyte a = 000 );
        /*
        Pre Condition:
         -A valid OpenGL context
         -Initialized DevIL
        Post Condition:
         -Creates RGBA texture from the given file
         -Pads image to have power-of-two dimensions
         -Sets given RGBA value to RFFGFFBFFA00 in pixel data
         -If A = 0, only RGB components are compared
         -Reports error to console if texture could not be created
        Side Effects:
         -Binds a NULL texture
        */

        bool loadTextureFromPixels32();
        /*
        Pre Condition:
         -A valid OpenGL context
         -Valid member pixels
        Post Condition:
         -Creates RGBA texture from the 32bit member pixels
         -Deletes member pixels on success
         -Reports error to console if texture could not be created
        Side Effects:
         -Binds a NULL texture
        */

        bool loadTextureFromPixels32( GLuint* pixels, GLuint imgWidth, GLuint imgHeight, GLuint texWidth, GLuint texHeight );
        /*
        Pre Condition:
         -A valid OpenGL context
        Post Condition:
         -Creates RGBA texture from the given pixels
         -Reports error to console if texture could not be created
        Side Effects:
         -Binds a NULL texture
        */

        void createPixels32( GLuint imgWidth, GLuint imgHeight );
        /*
        Pre Condition:
         -A valid OpenGL context
        Post Condition:
         -Frees existing texture data
         -Allocates 32bit pixel data for member pixels
        Side Effects:
         -None
        */

        void copyPixels32( GLuint* pixels, GLuint imgWidth, GLuint imgHeight );
        /*
        Pre Condition:
         -A valid OpenGL context
        Post Condition:
         -Copies given pixel data into member pixels
        Side Effects:
         -None
        */

        void padPixels32();
        /*
        Pre Condition:
         -Valid 32bit member pixels
        Post Condition:
         -Takes current member pixel data and gives it power of two dimensions
        Side Effects:
         -None
        */

        bool loadPixelsFromFile8( std::string path );
        /*
        Pre Condition:
         -Initialized DevIL
        Post Condition:
         -Loads member 8bit pixels from the given file
         -Pads image to have power-of-two dimensions
         -Reports error to console if pixels could not be loaded
        Side Effects:
         -None
        */

        bool loadTextureFromPixels8();
        /*
        Pre Condition:
         -A valid OpenGL context
         -Valid member pixels
        Post Condition:
         -Creates alpha texture from the 8bit member pixels
         -Deletes member pixels on success
         -Reports error to console if texture could not be created
        Side Effects:
         -Binds a NULL texture
        */

        void createPixels8( GLuint imgWidth, GLuint imgHeight );
        /*
        Pre Condition:
         -A valid OpenGL context
        Post Condition:
         -Frees existing texture data
         -Allocates 8bit pixel data for member pixels
        Side Effects:
         -None
        */

        void copyPixels8( GLubyte* pixels, GLuint imgWidth, GLuint imgHeight );
        /*
        Pre Condition:
         -A valid OpenGL context
        Post Condition:
         -Copies given pixel data into member pixels
        Side Effects:
         -None
        */

        void padPixels8();
        /*
        Pre Condition:
         -Valid 8bit member pixels
        Post Condition:
         -Takes current member pixel data and gives it power of two dimensions
        Side Effects:
         -None
        */

        virtual void freeTexture();
        /*
        Pre Condition:
         -A valid OpenGL context
        Post Condition:
         -Deletes texture if it exists
         -Deletes member pixels if they exist
         -Sets texture ID to 0
        Side Effects:
         -None
        */

        bool lock();
        /*
        Pre Condition:
         -An existing unlocked texture
        Post Condition:
         -Gets member pixels from texture data
         -Returns true if texture pixels were retrieved
        Side Effects:
         -Binds a NULL texture
        */

        bool unlock();
        /*
        Pre Condition:
         -A locked texture
        Post Condition:
         -Updates texture with member pixels
         -Returns true if texture pixels were updated
        Side Effects:
         -Binds a NULL texture
        */

        GLuint* getPixelData32();
        /*
        Pre Condition:
         -Available 32bit member pixels
        Post Condition:
         -Returns 32bit member pixels
        Side Effects:
         -None
        */

        GLubyte* getPixelData8();
        /*
        Pre Condition:
         -Available 8bit member pixels
        Post Condition:
         -Returns 8bit member pixels
        Side Effects:
         -None
        */

        GLuint getPixel32( GLuint x, GLuint y );
        /*
        Pre Condition:
         -Available 32bit member pixels
        Post Condition:
         -Returns pixel at given position
         -Function will segfault if the texture is not locked.
        Side Effects:
         -None
        */

        void setPixel32( GLuint x, GLuint y, GLuint pixel );
        /*
        Pre Condition:
         -Available 32bit member pixels
        Post Condition:
         -Sets pixel at given position
         -Function will segfault if the texture is not locked.
        Side Effects:
         -None
        */

        GLubyte getPixel8( GLuint x, GLuint y );
        /*
        Pre Condition:
         -Available 8bit member pixels
        Post Condition:
         -Returns pixel at given position
         -Function will segfault if the texture is not locked.
        Side Effects:
         -None
        */

        void setPixel8( GLuint x, GLuint y, GLubyte pixel );
        /*
        Pre Condition:
         -Available 8bit member pixels
        Post Condition:
         -Sets pixel at given position
         -Function will segfault if the texture is not locked.
        Side Effects:
         -None
        */

        void render( GLfloat x, GLfloat y, LFRect* clip = NULL );
        /*
        Pre Condition:
         -A valid OpenGL context
         -Active modelview matrix
        Post Condition:
         -Translates to given position and renders the texture area mapped to a quad
         -If given texture clip is NULL, the full image is rendered
        Side Effects:
         -Binds member texture ID
         -Binds member VBO and IBO
        */

        void blitPixels32( GLuint x, GLuint y, LTexture& destination );
        /*
        Pre Condition:
         -Available 32bit member and destination pixels
         -Valid blitting coordinates
        Post Condition:
         -Copies member pixels to destination member pixels at given location
         -Function will misbehave if invalid blitting coordinate or image dimensions
         are used
        Side Effects:
         -None
        */

        void blitPixels8( GLuint x, GLuint y, LTexture& destination );
        /*
        Pre Condition:
         -Available 8bit member and destination pixels
         -Valid blitting coordinates
        Post Condition:
         -Copies member pixels to destination member pixels at given location
         -Function will misbehave if invalid blitting coordinate or image dimensions
         are used
        Side Effects:
         -None
        */

        GLuint getTextureID();
        /*
        Pre Condition:
         -None
        Post Condition:
         -Returns texture name/ID
        Side Effects:
         -None
        */

        GLuint textureWidth();
        /*
        Pre Condition:
         -None
        Post Condition:
         -Returns texture width
        Side Effects:
         -None
        */

        GLuint textureHeight();
        /*
        Pre Condition:
         -None
        Post Condition:
         -Returns texture height
        Side Effects:
         -None
        */

        GLuint imageWidth();
        /*
        Pre Condition:
         -None
        Post Condition:
         -Returns unpadded image width
        Side Effects:
         -None
        */

        GLuint imageHeight();
        /*
        Pre Condition:
         -None
        Post Condition:
         -Returns unpadded image height
        Side Effects:
         -None
        */

    protected:
        GLuint powerOfTwo( GLuint num );
        /*
        Pre Condition:
         -None
        Post Condition:
         -Returns nearest power of two integer that is greater
        Side Effects:
         -None
        */

        void initVBO();
        /*
        Pre Condition:
         -A valid OpenGL context
         -A loaded member texture
        Post Condition:
         -Generates VBO and IBO to use for rendering
        Side Effects:
         -Binds NULL VBO and IBO
        */

        void freeVBO();
        /*
        Pre Condition:
         -A generated VBO
        Post Condition:
         -Frees VBO and IBO
        Side Effects:
         -None
        */

        //Texture name
        GLuint mTextureID;

        //Current pixels
        GLuint* mPixels32;
        GLubyte* mPixels8;

        //Pixel format
        GLuint mPixelFormat;

        //Texture dimensions
        GLuint mTextureWidth;
        GLuint mTextureHeight;

        //Unpadded image dimensions
        GLuint mImageWidth;
        GLuint mImageHeight;

        //VBO IDs
        GLuint mVBOID;
        GLuint mIBOID;
};

#endif

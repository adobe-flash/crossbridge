/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 009

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

        ~LTexture();
        /*
        Pre Condition:
         -None
        Post Condition:
         -Frees texture
        Side Effects:
         -None
        */

        bool loadTextureFromFile( std::string path );
        /*
        Pre Condition:
         -A valid OpenGL context
         -Initialized DevIL
        Post Condition:
         -Creates a texture from the given file
         -Pads image to have power-of-two dimensions
         -Reports error to console if texture could not be created
        Side Effects:
         -Binds a NULL texture
        */

        bool loadPixelsFromFile( std::string path );
        /*
        Pre Condition:
         -Initialized DevIL
        Post Condition:
         -Loads member pixels from the given file
         -Pads image to have power-of-two dimensions
         -Reports error to console if pixels could not be loaded
        Side Effects:
         -None
        */

        bool loadTextureFromFileWithColorKey( std::string path, GLubyte r, GLubyte g, GLubyte b, GLubyte a = 000 );
        /*
        Pre Condition:
         -A valid OpenGL context
         -Initialized DevIL
        Post Condition:
         -Creates a texture from the given file
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
         -Creates a texture from the member pixels
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
         -Creates a texture from the given pixels
         -Reports error to console if texture could not be created
        Side Effects:
         -Binds a NULL texture
        */

        void freeTexture();
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
         -Available member pixels
        Post Condition:
         -Returns member pixels
        Side Effects:
         -None
        */

        GLuint getPixel32( GLuint x, GLuint y );
        /*
        Pre Condition:
         -Available member pixels
        Post Condition:
         -Returns pixel at given position
         -Function will segfault if the texture is not locked.
        Side Effects:
         -None
        */

        void setPixel32( GLuint x, GLuint y, GLuint pixel );
        /*
        Pre Condition:
         -Available member pixels
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

    private:
        GLuint powerOfTwo( GLuint num );
        /*
        Pre Condition:
         -None
        Post Condition:
         -Returns nearest power of two integer that is greater
        Side Effects:
         -None
        */

        //Texture name
        GLuint mTextureID;

        //Current pixels
        GLuint* mPixels;

        //Texture dimensions
        GLuint mTextureWidth;
        GLuint mTextureHeight;

        //Unpadded image dimensions
        GLuint mImageWidth;
        GLuint mImageHeight;
};

#endif

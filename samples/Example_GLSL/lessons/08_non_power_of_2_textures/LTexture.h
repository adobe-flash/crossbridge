/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 004

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
         -Sets texture ID to 0
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

        //Texture dimensions
        GLuint mTextureWidth;
        GLuint mTextureHeight;

        //Unpadded image dimensions
        GLuint mImageWidth;
        GLuint mImageHeight;
};

#endif

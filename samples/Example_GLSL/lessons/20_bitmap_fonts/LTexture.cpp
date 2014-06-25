/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 011

#include "LTexture.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include "LVertexData2D.h"

GLenum DEFAULT_TEXTURE_WRAP = GL_REPEAT;

LTexture::LTexture()
{
    //Initialize texture ID and pixels
    mTextureID = 0;
    mPixels = NULL;

    //Initialize image dimensions
    mImageWidth = 0;
    mImageHeight = 0;

    //Initialize texture dimensions
    mTextureWidth = 0;
    mTextureHeight = 0;

    //Initialize VBO
    mVBOID = 0;
    mIBOID = 0;
}

LTexture::~LTexture()
{
    //Free texture data if needed
    freeTexture();

    //Free VBO and IBO if needed
    freeVBO();
}

bool LTexture::loadTextureFromFile( std::string path )
{
    //Texture loading success
    bool textureLoaded = false;

    //Generate and set current image ID
    ILuint imgID = 0;
    ilGenImages( 1, &imgID );
    ilBindImage( imgID );

    //Load image
    ILboolean success = ilLoadImage( path.c_str() );

    //Image loaded successfully
    if( success == IL_TRUE )
    {
        //Convert image to RGBA
        success = ilConvertImage( IL_RGBA, IL_UNSIGNED_BYTE );
        if( success == IL_TRUE )
        {
            //Initialize dimensions
            GLuint imgWidth = (GLuint)ilGetInteger( IL_IMAGE_WIDTH );
            GLuint imgHeight = (GLuint)ilGetInteger( IL_IMAGE_HEIGHT );

            //Calculate required texture dimensions
            GLuint texWidth = powerOfTwo( imgWidth );
            GLuint texHeight = powerOfTwo( imgHeight );

            //Texture is the wrong size
            if( imgWidth != texWidth || imgHeight != texHeight )
            {
                //Place image at upper left
                iluImageParameter( ILU_PLACEMENT, ILU_UPPER_LEFT );

                //Resize image
                iluEnlargeCanvas( (int)texWidth, (int)texHeight, 1 );
            }

            //Create texture from file pixels
            textureLoaded = loadTextureFromPixels32( (GLuint*)ilGetData(), imgWidth, imgHeight, texWidth, texHeight );
        }

        //Delete file from memory
        ilDeleteImages( 1, &imgID );
    }

    //Report error
    if( !textureLoaded )
    {
        printf( "Unable to load %s\n", path.c_str() );
    }

    return textureLoaded;
}

bool LTexture::loadPixelsFromFile( std::string path )
{
    //Deallocate texture data
    freeTexture();

    //Texture loading success
    bool pixelsLoaded = false;

    //Generate and set current image ID
    ILuint imgID = 0;
    ilGenImages( 1, &imgID );
    ilBindImage( imgID );

    //Load image
    ILboolean success = ilLoadImage( path.c_str() );

    //Image loaded successfully
    if( success == IL_TRUE )
    {
        //Convert image to RGBA
        success = ilConvertImage( IL_RGBA, IL_UNSIGNED_BYTE );
        if( success == IL_TRUE )
        {
            //Initialize dimensions
            GLuint imgWidth = (GLuint)ilGetInteger( IL_IMAGE_WIDTH );
            GLuint imgHeight = (GLuint)ilGetInteger( IL_IMAGE_HEIGHT );

            //Calculate required texture dimensions
            GLuint texWidth = powerOfTwo( imgWidth );
            GLuint texHeight = powerOfTwo( imgHeight );

            //Texture is the wrong size
            if( imgWidth != texWidth || imgHeight != texHeight )
            {
                //Place image at upper left
                iluImageParameter( ILU_PLACEMENT, ILU_UPPER_LEFT );

                //Resize image
                iluEnlargeCanvas( (int)texWidth, (int)texHeight, 1 );
            }

            //Allocate memory for texture data
            GLuint size = texWidth * texHeight;
            mPixels = new GLuint[ size ];

            //Get image dimensions
            mImageWidth = imgWidth;
            mImageHeight = imgHeight;
            mTextureWidth = texWidth;
            mTextureHeight = texHeight;

            //Copy pixels
            memcpy( mPixels, ilGetData(), size * 4 );
            pixelsLoaded = true;
        }

        //Delete file from memory
        ilDeleteImages( 1, &imgID );
    }

    //Report error
    if( !pixelsLoaded )
    {
        printf( "Unable to load %s\n", path.c_str() );
    }

    return pixelsLoaded;
}

bool LTexture::loadTextureFromFileWithColorKey( std::string path, GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    //Load pixels
    if( !loadPixelsFromFile( path ) )
    {
        return false;
    }

    //Go through pixels
    GLuint size = mTextureWidth * mTextureHeight;
    for( int i = 0; i < size; ++i )
    {
        //Get pixel colors
        GLubyte* colors = (GLubyte*)&mPixels[ i ];

        //Color matches
        if( colors[ 0 ] == r && colors[ 1 ] == g && colors[ 2 ] == b && ( 0 == a || colors[ 3 ] == a ) )
        {
            //Make transparent
            colors[ 0 ] = 255;
            colors[ 1 ] = 255;
            colors[ 2 ] = 255;
            colors[ 3 ] = 000;
        }
    }

    //Create texture
    return loadTextureFromPixels32();
}

bool LTexture::loadTextureFromPixels32()
{
    //Loading flag
    bool success = true;

    //There is loaded pixels
    if( mTextureID == 0 && mPixels != NULL )
    {
        //Generate texture ID
        glGenTextures( 1, &mTextureID );

        //Bind texture ID
        glBindTexture( GL_TEXTURE_2D, mTextureID );

        //Generate texture
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, mTextureWidth, mTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, mPixels );

        //Set texture parameters
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, DEFAULT_TEXTURE_WRAP );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, DEFAULT_TEXTURE_WRAP );

        //Unbind texture
        glBindTexture( GL_TEXTURE_2D, NULL );

        //Check for error
        GLenum error = glGetError();
        if( error != GL_NO_ERROR )
        {
            printf( "Error loading texture from %p pixels! %s\n", mPixels, gluErrorString( error ) );
            success = false;
        }
        else
        {
            //Release pixels
            delete[] mPixels;
            mPixels = NULL;

            //Generate VBO
            initVBO();
        }
    }
    //Error
    else
    {
        printf( "Cannot load texture from current pixels! " );

        //Texture already exists
        if( mTextureID != 0 )
        {
            printf( "A texture is already loaded!\n" );
        }
        //No pixel loaded
        else if( mPixels == NULL )
        {
            printf( "No pixels to create texture from!\n" );
        }

        success = false;
    }

    return success;
}

bool LTexture::loadTextureFromPixels32( GLuint* pixels, GLuint imgWidth, GLuint imgHeight, GLuint texWidth, GLuint texHeight )
{
    //Free texture if it exists
    freeTexture();

    //Get image dimensions
    mImageWidth = imgWidth;
    mImageHeight = imgHeight;
    mTextureWidth = texWidth;
    mTextureHeight = texHeight;

    //Generate texture ID
    glGenTextures( 1, &mTextureID );

    //Bind texture ID
    glBindTexture( GL_TEXTURE_2D, mTextureID );

    //Generate texture
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, mTextureWidth, mTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

    //Set texture parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, DEFAULT_TEXTURE_WRAP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, DEFAULT_TEXTURE_WRAP );

    //Unbind texture
    glBindTexture( GL_TEXTURE_2D, NULL );

    //Check for error
    GLenum error = glGetError();
    if( error != GL_NO_ERROR )
    {
        printf( "Error loading texture from %p pixels! %s\n", pixels, gluErrorString( error ) );
        return false;
    }

    //Generate VBO
    initVBO();

    return true;
}

void LTexture::freeTexture()
{
    //Delete texture
    if( mTextureID != 0 )
    {
        glDeleteTextures( 1, &mTextureID );
        mTextureID = 0;
    }

    //Delete pixels
    if( mPixels != NULL )
    {
        delete[] mPixels;
        mPixels = NULL;
    }

    mImageWidth = 0;
    mImageHeight = 0;
    mTextureWidth = 0;
    mTextureHeight = 0;
}

bool LTexture::lock()
{
    //If texture is not locked and a texture exists
    if( mPixels == NULL && mTextureID != 0 )
    {
        //Allocate memory for texture data
        GLuint size = mTextureWidth * mTextureHeight;
        mPixels = new GLuint[ size ];

        //Set current texture
        glBindTexture( GL_TEXTURE_2D, mTextureID );

        //Get pixels
        glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, mPixels );

        //Unbind texture
        glBindTexture( GL_TEXTURE_2D, NULL );

        return true;
    }

    return false;
}

bool LTexture::unlock()
{
    //If texture is locked and a texture exists
    if( mPixels != NULL && mTextureID != 0 )
    {
        //Set current texture
        glBindTexture( GL_TEXTURE_2D, mTextureID );

        //Update texture
        glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, mTextureWidth, mTextureHeight, GL_RGBA, GL_UNSIGNED_BYTE, mPixels );

        //Delete pixels
        delete[] mPixels;
        mPixels = NULL;

        //Unbind texture
        glBindTexture( GL_TEXTURE_2D, NULL );

        return true;
    }

    return false;
}

GLuint* LTexture::getPixelData32()
{
    return mPixels;
}

GLuint LTexture::getPixel32( GLuint x, GLuint y )
{
    return mPixels[ y * mTextureWidth + x ];
}

void LTexture::setPixel32( GLuint x, GLuint y, GLuint pixel )
{
    mPixels[ y * mTextureWidth + x ] = pixel;
}

void LTexture::render( GLfloat x, GLfloat y, LFRect* clip )
{
    //If the texture exists
    if( mTextureID != 0 )
    {
        //Texture coordinates
        GLfloat texTop = 0.f;
        GLfloat texBottom = (GLfloat)mImageHeight / (GLfloat)mTextureHeight;
        GLfloat texLeft = 0.f;
        GLfloat texRight = (GLfloat)mImageWidth / (GLfloat)mTextureWidth;

        //Vertex coordinates
        GLfloat quadWidth = mImageWidth;
        GLfloat quadHeight = mImageHeight;

        //Handle clipping
        if( clip != NULL )
        {
            //Texture coordinates
            texLeft = clip->x / mTextureWidth;
            texRight = ( clip->x + clip->w ) / mTextureWidth;
            texTop = clip->y / mTextureHeight;
            texBottom = ( clip->y + clip->h ) / mTextureHeight;

            //Vertex coordinates
            quadWidth = clip->w;
            quadHeight = clip->h;
        }

        //Move to rendering point
        glTranslatef( x, y, 0.f );

        //Set vertex data
        LVertexData2D vData[ 4 ];

        //Texture coordinates
        vData[ 0 ].texCoord.s =  texLeft; vData[ 0 ].texCoord.t =    texTop;
        vData[ 1 ].texCoord.s = texRight; vData[ 1 ].texCoord.t =    texTop;
        vData[ 2 ].texCoord.s = texRight; vData[ 2 ].texCoord.t = texBottom;
        vData[ 3 ].texCoord.s =  texLeft; vData[ 3 ].texCoord.t = texBottom;

        //Vertex positions
        vData[ 0 ].position.x =       0.f; vData[ 0 ].position.y =        0.f;
        vData[ 1 ].position.x = quadWidth; vData[ 1 ].position.y =        0.f;
        vData[ 2 ].position.x = quadWidth; vData[ 2 ].position.y = quadHeight;
        vData[ 3 ].position.x =       0.f; vData[ 3 ].position.y = quadHeight;

        //Set texture ID
        glBindTexture( GL_TEXTURE_2D, mTextureID );

        //Enable vertex and texture coordinate arrays
        glEnableClientState( GL_VERTEX_ARRAY );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );

            //Bind vertex buffer
            glBindBuffer( GL_ARRAY_BUFFER, mVBOID );

            //Update vertex buffer data
            glBufferSubData( GL_ARRAY_BUFFER, 0, 4 * sizeof(LVertexData2D), vData );

            //Set texture coordinate data
            glTexCoordPointer( 2, GL_FLOAT, sizeof(LVertexData2D), (GLvoid*)offsetof( LVertexData2D, texCoord ) );

            //Set vertex data
            glVertexPointer( 2, GL_FLOAT, sizeof(LVertexData2D), (GLvoid*)offsetof( LVertexData2D, position ) );

            //Draw quad using vertex data and index data
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIBOID );
            glDrawElements( GL_QUADS, 4, GL_UNSIGNED_INT, NULL );

        //Disable vertex and texture coordinate arrays
        glDisableClientState( GL_TEXTURE_COORD_ARRAY );
        glDisableClientState( GL_VERTEX_ARRAY );
    }
}

GLuint LTexture::getTextureID()
{
    return mTextureID;
}

GLuint LTexture::textureWidth()
{
    return mTextureWidth;
}

GLuint LTexture::textureHeight()
{
    return mTextureHeight;
}

GLuint LTexture::imageWidth()
{
    return mImageWidth;
}

GLuint LTexture::imageHeight()
{
    return mImageHeight;
}

GLuint LTexture::powerOfTwo( GLuint num )
{
    if( num != 0 )
    {
        num--;
        num |= (num >> 1); //Or first 2 bits
        num |= (num >> 2); //Or next 2 bits
        num |= (num >> 4); //Or next 4 bits
        num |= (num >> 8); //Or next 8 bits
        num |= (num >> 16); //Or next 16 bits
        num++;
    }

    return num;
}

void LTexture::initVBO()
{
    //If texture is loaded and VBO does not already exist
    if( mTextureID != 0 && mVBOID == 0 )
    {
        //Vertex data
        LVertexData2D vData[ 4 ];
        GLuint iData[ 4 ];

        //Set rendering indices
        iData[ 0 ] = 0;
        iData[ 1 ] = 1;
        iData[ 2 ] = 2;
        iData[ 3 ] = 3;

        //Create VBO
        glGenBuffers( 1, &mVBOID );
        glBindBuffer( GL_ARRAY_BUFFER, mVBOID );
        glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(LVertexData2D), vData, GL_DYNAMIC_DRAW );

        //Create IBO
        glGenBuffers( 1, &mIBOID );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIBOID );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), iData, GL_DYNAMIC_DRAW );

        //Unbind buffers
        glBindBuffer( GL_ARRAY_BUFFER, NULL );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, NULL );
    }
}

void LTexture::freeVBO()
{
    //Free VBO and IBO
    if( mVBOID != 0 )
    {
        glDeleteBuffers( 1, &mVBOID );
        glDeleteBuffers( 1, &mIBOID );
    }
}

/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redestributed without written permission.*/
//Version: 022

#include "LUtil.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include "LTexture.h"

//Loaded textures
LTexture gLeft;
LTexture gRight;

//Generated combined texture
LTexture gCombined;

bool initGL()
{
    //Initialize GLEW
    GLenum glewError = glewInit();
    if( glewError != GLEW_OK )
    {
        printf( "Error initializing GLEW! %s\n", glewGetErrorString( glewError ) );
        return false;
    }

    //Make sure OpenGL 2.1 is supported
    if( !GLEW_VERSION_2_1 )
    {
        printf( "OpenGL 2.1 not supported!\n" );
        return false;
    }

    //Set the viewport
    glViewport( 0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT );

    //Initialize Projection Matrix
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 1.0, -1.0 );

    //Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    //Initialize clear color
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    //Enable texturing
    glEnable( GL_TEXTURE_2D );

    //Set blending
    glEnable( GL_BLEND );
    glDisable( GL_DEPTH_TEST );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    //Check for error
    GLenum error = glGetError();
    if( error != GL_NO_ERROR )
    {
        printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
        return false;
    }

    //Initialize DevIL and DevILU
    ilInit();
    iluInit();
    ilClearColour( 255, 255, 255, 000 );

    //Check for error
    ILenum ilError = ilGetError();
    if( ilError != IL_NO_ERROR )
    {
        printf( "Error initializing DevIL! %s\n", iluErrorString( ilError ) );
        return false;
    }

    return true;
}

bool loadMedia()
{
    //Load left texture
    if( !gLeft.loadPixelsFromFile32( "22_texture_blitting_and_texture_padding/left.png" ) )
    {
        printf( "Unable to load left texture!\n" );
        return false;
    }

    //Load right texture
    if( !gRight.loadPixelsFromFile32( "22_texture_blitting_and_texture_padding/right.png" ) )
    {
        printf( "Unable to load right texture!\n" );
        return false;
    }

    //Create blank pixels
    gCombined.createPixels32( gLeft.imageWidth() + gRight.imageWidth(), gLeft.imageHeight() );

    //Blit images
    gLeft.blitPixels32( 0, 0, gCombined );
    gRight.blitPixels32( gLeft.imageWidth(), 0, gCombined );

    //Pad and create texture
    gCombined.padPixels32();
    gCombined.loadTextureFromPixels32();

    //Get rid of old textures
    gLeft.freeTexture();
    gRight.freeTexture();

    return true;
}

void update()
{

}

void render()
{
    //Clear color buffer
    glClear( GL_COLOR_BUFFER_BIT );
    glLoadIdentity();

    //Render combined texture
    gCombined.render( ( SCREEN_WIDTH - gCombined.imageWidth() ) / 2.f, ( SCREEN_HEIGHT - gCombined.imageHeight() ) / 2.f );

    //Update screen
    glutSwapBuffers();
}

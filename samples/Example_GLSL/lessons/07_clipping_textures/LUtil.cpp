/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 007

#include "LUtil.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include "LTexture.h"

//Sprite texture
LTexture gArrowTexture;

//Sprite area
LFRect gArrowClips[ 4 ];

bool initGL()
{
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

    //Check for error
    GLenum error = glGetError();
    if( error != GL_NO_ERROR )
    {
        printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
        return false;
    }

    //Initialize DevIL
    ilInit();
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
    //Set clip rectangles
    gArrowClips[ 0 ].x = 0.f;
    gArrowClips[ 0 ].y = 0.f;
    gArrowClips[ 0 ].w = 128.f;
    gArrowClips[ 0 ].h = 128.f;

    gArrowClips[ 1 ].x = 128.f;
    gArrowClips[ 1 ].y = 0.f;
    gArrowClips[ 1 ].w = 128.f;
    gArrowClips[ 1 ].h = 128.f;

    gArrowClips[ 2 ].x = 0.f;
    gArrowClips[ 2 ].y = 128.f;
    gArrowClips[ 2 ].w = 128.f;
    gArrowClips[ 2 ].h = 128.f;

    gArrowClips[ 3 ].x = 128.f;
    gArrowClips[ 3 ].y = 128.f;
    gArrowClips[ 3 ].w = 128.f;
    gArrowClips[ 3 ].h = 128.f;

    //Load texture
    if( !gArrowTexture.loadTextureFromFile( "07_clipping_textures/arrows.png" ) )
    {
        printf( "Unable to load arrow texture!\n" );
        return false;
    }

    return true;
}

void update()
{

}

void render()
{
    //Clear color buffer
    glClear( GL_COLOR_BUFFER_BIT );

    //Render arrows
    gArrowTexture.render( 0.f, 0.f, &gArrowClips[ 0 ] );
    gArrowTexture.render( SCREEN_WIDTH - gArrowClips[ 1 ].w, 0.f, &gArrowClips[ 1 ] );
    gArrowTexture.render( 0.f, SCREEN_HEIGHT - gArrowClips[ 2 ].h, &gArrowClips[ 2 ] );
    gArrowTexture.render( SCREEN_WIDTH - gArrowClips[ 3 ].w, SCREEN_HEIGHT - gArrowClips[ 3 ].h, &gArrowClips[ 3 ] );

    //Update screen
    glutSwapBuffers();
}

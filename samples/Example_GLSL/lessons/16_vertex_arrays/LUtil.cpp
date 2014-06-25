/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 016

#include "LUtil.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include "LVertexPos2D.h"

//Quad vertices
LVertexPos2D gQuadVertices[ 4 ];

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
    //Set quad verticies
    gQuadVertices[ 0 ].x = SCREEN_WIDTH * 1.f / 4.f;
    gQuadVertices[ 0 ].y = SCREEN_HEIGHT * 1.f / 4.f;

    gQuadVertices[ 1 ].x = SCREEN_WIDTH * 3.f / 4.f;
    gQuadVertices[ 1 ].y = SCREEN_HEIGHT * 1.f / 4.f;

    gQuadVertices[ 2 ].x = SCREEN_WIDTH * 3.f / 4.f;
    gQuadVertices[ 2 ].y = SCREEN_HEIGHT * 3.f / 4.f;

    gQuadVertices[ 3 ].x = SCREEN_WIDTH * 1.f / 4.f;
    gQuadVertices[ 3 ].y = SCREEN_HEIGHT * 3.f / 4.f;

    return true;
}

void update()
{

}

void render()
{
    //Clear color buffer
    glClear( GL_COLOR_BUFFER_BIT );

    //Enable vertex arrays
    glEnableClientState( GL_VERTEX_ARRAY );

        //Set vertex data
        glVertexPointer( 2, GL_FLOAT, 0, gQuadVertices );

        //Draw quad using vertex data
        glDrawArrays( GL_QUADS, 0, 4 );

    //Disable vertex arrays
    glDisableClientState( GL_VERTEX_ARRAY );

    //Update screen
    glutSwapBuffers();
}

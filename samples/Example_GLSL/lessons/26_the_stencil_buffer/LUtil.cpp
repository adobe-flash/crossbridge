/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 026

#include "LUtil.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include "LTexture.h"

//OpenGL texture
LTexture gTexture;

//Polygon attributes
GLfloat gPolygonAngle = 0.f;
GLfloat gPolygonX = SCREEN_WIDTH / 2.f, gPolygonY = SCREEN_HEIGHT / 2.f;

//Stencil operation
GLuint gStencilRenderOp = GL_NOTEQUAL;

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

    //Initialize stencil clear value
    glClearStencil( 0 );

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
    //Load texture
	if( !gTexture.loadTextureFromFile32( "26_the_stencil_buffer/opengl.png" ) )
	{
	    printf( "Unable to load texture!\n" );
		return false;
	}

    return true;
}

void update()
{
    //Rotate triangle
    gPolygonAngle += 6.f;
}

void render()
{
    //Clear color and stencil buffer
    glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    glLoadIdentity();

    //Disable rendering to the color buffer
    glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );

    //Start using the stencil
    glEnable( GL_STENCIL_TEST );

    //Place a 1 where rendered
    glStencilFunc( GL_ALWAYS, 1, 1 );

    //Replace where rendered
    glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );

    //Render stencil triangle
    glTranslatef( gPolygonX, gPolygonY, 0.f );
    glRotatef( gPolygonAngle, 0.f, 0.f, 1.f );
    glBegin( GL_TRIANGLES );
        glVertex2f(           -0.f / 4.f, -SCREEN_HEIGHT / 4.f );
        glVertex2f(   SCREEN_WIDTH / 4.f,  SCREEN_HEIGHT / 4.f );
        glVertex2f(  -SCREEN_WIDTH / 4.f,  SCREEN_HEIGHT / 4.f );
    glEnd();

    //Reenable color
    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

    //Where a 1 was not rendered
    glStencilFunc( gStencilRenderOp, 1, 1 );

    //Keep the pixel
    glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );

    //Render stenciled texture
    glLoadIdentity();
    gTexture.render( ( SCREEN_WIDTH - gTexture.imageWidth() ) / 2.f, ( SCREEN_HEIGHT - gTexture.imageHeight() ) / 2.f );

    //Finished using stencil
    glDisable( GL_STENCIL_TEST );

    //Update screen
    glutSwapBuffers();
}

void handleKeys( unsigned char key, int x, int y )
{
    //If the user presses q
    if( key == 'q' )
    {
        //Cycle through stencil operations
        if( gStencilRenderOp == GL_NOTEQUAL )
        {
            //Render where stencil polygon was rendered
            gStencilRenderOp = GL_EQUAL;
        }
        else if( gStencilRenderOp == GL_EQUAL )
        {
            //Render everything
            gStencilRenderOp = GL_ALWAYS;
        }
        else if( gStencilRenderOp == GL_ALWAYS )
        {
            //Render where stencil polygon was not rendered
            gStencilRenderOp = GL_NOTEQUAL;
        }
    }
}

void handleMouseMotion( int x, int y )
{
    //Set polygon position
    gPolygonX = x;
    gPolygonY = y;
}

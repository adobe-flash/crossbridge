/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 028

#include "LUtil.h"
#include <IL/il.h>
#include <IL/ilu.h>

//Aliasing
AliasMode gMode = ALIAS_MODE_ALIASED;

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

    //Set antialiasing/multisampling
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
    glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
    glDisable( GL_LINE_SMOOTH );
    glDisable( GL_POLYGON_SMOOTH );
    glDisable( GL_MULTISAMPLE );

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
    return true;
}

void update()
{

}

void render()
{
    //Clear color
    glClear( GL_COLOR_BUFFER_BIT );

	//Start alias mode
	switch( gMode )
	{
	    case ALIAS_MODE_ALIASED:
            glDisable( GL_LINE_SMOOTH );
			glDisable( GL_POLYGON_SMOOTH );
			glDisable( GL_MULTISAMPLE );
            break;

		case ALIAS_MODE_ANTIALIASED:
			glEnable( GL_LINE_SMOOTH );
			glEnable( GL_POLYGON_SMOOTH );
			glDisable( GL_MULTISAMPLE );
			break;

		case ALIAS_MODE_MULTISAMPLE:
            glDisable( GL_LINE_SMOOTH );
			glDisable( GL_POLYGON_SMOOTH );
			glEnable( GL_MULTISAMPLE );
			break;
	}

	//Render Triangle
	glColor3f( 1.f, 1.f, 1.f );
	glBegin( GL_TRIANGLES );
	 glVertex2f( SCREEN_WIDTH, 0.f );
	 glVertex2f( SCREEN_WIDTH, SCREEN_HEIGHT );
	 glVertex2f( 0.f, SCREEN_HEIGHT );
	glEnd();

	//End alias mode
	switch( gMode )
	{
		case ALIAS_MODE_ANTIALIASED:
			glDisable( GL_LINE_SMOOTH );
			glDisable( GL_POLYGON_SMOOTH );
			break;

		case ALIAS_MODE_MULTISAMPLE:
			glDisable( GL_MULTISAMPLE );
			break;
	}

    //Update screen
    glutSwapBuffers();
}

void handleKeys( unsigned char key, int x, int y )
{
    //If the user presses q
    if( key == 'q' )
    {
		//Cycle alias mode
		switch( gMode )
		{
			case ALIAS_MODE_ALIASED:
				printf( "Antialiased\n" );
				gMode = ALIAS_MODE_ANTIALIASED;
				break;

			case ALIAS_MODE_ANTIALIASED:
				printf( "Multisampled\n" );
				gMode = ALIAS_MODE_MULTISAMPLE;
				break;

			case ALIAS_MODE_MULTISAMPLE:
				printf( "Aliased\n" );
				gMode = ALIAS_MODE_ALIASED;
				break;
		}
    }
}

/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 031

#include "LUtil.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include "LPlainPolygonProgram2D.h"
#include "LVertexPos2D.h"

//Basic shader
LPlainPolygonProgram2D gPlainPolygonProgram2D;

//VBO names
GLuint gVBO = NULL;
GLuint gIBO = NULL;

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

bool loadGP()
{
	//Load basic shader program
	if( !gPlainPolygonProgram2D.loadProgram() )
	{
		printf( "Unable to load basic shader!\n" );
		return false;
	}

	//Bind basic shader program
	gPlainPolygonProgram2D.bind();

    return true;
}

bool loadMedia()
{
	//VBO data
	LVertexPos2D quadVertices[ 4 ];
	GLuint indices[ 4 ];

    //Set quad vertices
    quadVertices[ 0 ].x = -50.f;
    quadVertices[ 0 ].y = -50.f;

    quadVertices[ 1 ].x =  50.f;
    quadVertices[ 1 ].y = -50.f;

    quadVertices[ 2 ].x =  50.f;
    quadVertices[ 2 ].y =  50.f;

    quadVertices[ 3 ].x = -50.f;
    quadVertices[ 3 ].y =  50.f;

    //Set rendering indices
    indices[ 0 ] = 0;
    indices[ 1 ] = 1;
    indices[ 2 ] = 2;
    indices[ 3 ] = 3;

    //Create VBO
    glGenBuffers( 1, &gVBO );
    glBindBuffer( GL_ARRAY_BUFFER, gVBO );
    glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(LVertexPos2D), quadVertices, GL_STATIC_DRAW );

    //Create IBO
    glGenBuffers( 1, &gIBO );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gIBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indices, GL_STATIC_DRAW );

    return true;
}

void update()
{

}

void render()
{
    //Clear color buffer
    glClear( GL_COLOR_BUFFER_BIT );

	//Reset transformations
	glLoadIdentity();

    //Solid cyan quad in the center
	glTranslatef( SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f, 0.f );
	gPlainPolygonProgram2D.setColor( 0.f, 1.f, 1.f );

    //Enable vertex arrays
    glEnableClientState( GL_VERTEX_ARRAY );

		//Set vertex data
		glBindBuffer( GL_ARRAY_BUFFER, gVBO );
		glVertexPointer( 2, GL_FLOAT, 0, NULL );

	    //Draw quad using vertex data and index data
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gIBO );
        glDrawElements( GL_QUADS, 4, GL_UNSIGNED_INT, NULL );

    //Disable vertex arrays
    glDisableClientState( GL_VERTEX_ARRAY );

    //Update screen
    glutSwapBuffers();
}

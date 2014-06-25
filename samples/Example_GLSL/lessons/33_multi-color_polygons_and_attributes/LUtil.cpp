/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redestributed without written permission.*/
//Version: 033

#include "LUtil.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include "LMultiColorPolygonProgram2D.h"
#include "LMultiColorVertex2D.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

//Multicolor shader
LMultiColorPolygonProgram2D gMultiColorPolygonProgram2D;

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
	//Load multicolor shader program
	if( !gMultiColorPolygonProgram2D.loadProgram() )
	{
		printf( "Unable to load multicolor shader!\n" );
		return false;
	}

	//Bind multicolor shader program
	gMultiColorPolygonProgram2D.bind();

	//Initialize projection
	gMultiColorPolygonProgram2D.setProjection( glm::ortho<GLfloat>( 0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 1.0, -1.0 ) );
	gMultiColorPolygonProgram2D.updateProjection();

	//Initialize modelview
	gMultiColorPolygonProgram2D.setModelView( glm::mat4() );
	gMultiColorPolygonProgram2D.updateModelView();

	return true;
}

bool loadMedia()
{
	//VBO data
	LMultiColorVertex2D quadVertices[ 4 ];
	GLuint indices[ 4 ];

    //Set quad verticies
    quadVertices[ 0 ].pos.x = -50.f;
    quadVertices[ 0 ].pos.y = -50.f;
	quadVertices[ 0 ].rgba.r = 1.f;
    quadVertices[ 0 ].rgba.g = 0.f;
    quadVertices[ 0 ].rgba.b = 0.f;
    quadVertices[ 0 ].rgba.a = 1.f;

    quadVertices[ 1 ].pos.x =  50.f;
    quadVertices[ 1 ].pos.y = -50.f;
	quadVertices[ 1 ].rgba.r = 1.f;
    quadVertices[ 1 ].rgba.g = 1.f;
    quadVertices[ 1 ].rgba.b = 0.f;
    quadVertices[ 1 ].rgba.a = 1.f;

    quadVertices[ 2 ].pos.x =  50.f;
    quadVertices[ 2 ].pos.y =  50.f;
	quadVertices[ 2 ].rgba.r = 0.f;
    quadVertices[ 2 ].rgba.g = 1.f;
    quadVertices[ 2 ].rgba.b = 0.f;
    quadVertices[ 2 ].rgba.a = 1.f;

    quadVertices[ 3 ].pos.x = -50.f;
    quadVertices[ 3 ].pos.y =  50.f;
	quadVertices[ 3 ].rgba.r = 0.f;
    quadVertices[ 3 ].rgba.g = 0.f;
    quadVertices[ 3 ].rgba.b = 1.f;
    quadVertices[ 3 ].rgba.a = 1.f;

    //Set rendering indices
    indices[ 0 ] = 0;
    indices[ 1 ] = 1;
    indices[ 2 ] = 2;
    indices[ 3 ] = 3;

    //Create VBO
    glGenBuffers( 1, &gVBO );
    glBindBuffer( GL_ARRAY_BUFFER, gVBO );
    glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(LMultiColorVertex2D), quadVertices, GL_STATIC_DRAW );

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

    //Multicolor quad in the center
	gMultiColorPolygonProgram2D.setModelView( glm::translate<GLfloat>( SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f, 0.f ) );
	gMultiColorPolygonProgram2D.updateModelView();

    //Enable vertex attributes
	gMultiColorPolygonProgram2D.enableVertexPointer();
	gMultiColorPolygonProgram2D.enableColorPointer();

		//Set vertex data
		glBindBuffer( GL_ARRAY_BUFFER, gVBO );
		gMultiColorPolygonProgram2D.setVertexPointer( sizeof(LMultiColorVertex2D), (GLvoid*)offsetof( LMultiColorVertex2D, pos ) );
		gMultiColorPolygonProgram2D.setColorPointer( sizeof(LMultiColorVertex2D), (GLvoid*)offsetof( LMultiColorVertex2D, rgba ) );

	    //Draw quad using vertex data and index data
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gIBO );
        glDrawElements( GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL );

    //Disable vertex attributes
	gMultiColorPolygonProgram2D.disableVertexPointer();
	gMultiColorPolygonProgram2D.disableColorPointer();

    //Update screen
    glutSwapBuffers();
}

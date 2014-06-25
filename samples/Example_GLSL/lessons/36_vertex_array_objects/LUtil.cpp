/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 036

#include "LUtil.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include "LDoubleMultiColorPolygonProgram2D.h"
#include "LVertexPos2D.h"
#include "LColorRGBA.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

//Double Multicolor shader
LDoubleMultiColorPolygonProgram2D gDoubleMultiColorPolygonProgram2D;

//VBO names
GLuint gVertexVBO = NULL;
GLuint gRGBYVBO = NULL;
GLuint gCYMWVBO = NULL;
GLuint gGrayVBO = NULL;
GLuint gIBO = NULL;

//VAO Names
GLuint gLeftVAO = NULL;
GLuint gRightVAO = NULL;

bool initGL()
{
    //Initialize GLEW
    GLenum glewError = glewInit();
    if( glewError != GLEW_OK )
    {
        printf( "Error initializing GLEW! %s\n", glewGetErrorString( glewError ) );
        return false;
    }

    //Make sure OpenGL 3.0 is supported
    if( !GLEW_VERSION_3_0 )
    {
        printf( "OpenGL 3.0 not supported!\n" );
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
	//Load double multicolor shader program
	if( !gDoubleMultiColorPolygonProgram2D.loadProgram() )
	{
		printf( "Unable to load double multicolor shader!\n" );
		return false;
	}

	//Bind double multicolor shader program
	gDoubleMultiColorPolygonProgram2D.bind();

	//Initialize projection
	gDoubleMultiColorPolygonProgram2D.setProjection( glm::ortho<GLfloat>( 0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 1.0, -1.0 ) );
	gDoubleMultiColorPolygonProgram2D.updateProjection();

	//Initialize modelview
	gDoubleMultiColorPolygonProgram2D.setModelView( glm::mat4() );
	gDoubleMultiColorPolygonProgram2D.updateModelView();

	return true;
}

bool loadMedia()
{
	//VBO data
	LVertexPos2D quadPos[ 4 ];
	LColorRGBA quadColorRGBY[ 4 ];
	LColorRGBA quadColorCYMW[ 4 ];
	LColorRGBA quadColorGray[ 4 ];
	GLuint indices[ 4 ];

    //Set quad verticies
    quadPos[ 0 ].x = -50.f;
    quadPos[ 0 ].y = -50.f;

	quadPos[ 1 ].x =  50.f;
    quadPos[ 1 ].y = -50.f;

	quadPos[ 2 ].x =  50.f;
	quadPos[ 2 ].y =  50.f;

	quadPos[ 3 ].x = -50.f;
	quadPos[ 3 ].y =  50.f;

	quadColorRGBY[ 0 ].r = 1.f;
    quadColorRGBY[ 0 ].g = 0.f;
    quadColorRGBY[ 0 ].b = 0.f;
    quadColorRGBY[ 0 ].a = 1.f;

	quadColorRGBY[ 1 ].r = 1.f;
    quadColorRGBY[ 1 ].g = 1.f;
    quadColorRGBY[ 1 ].b = 0.f;
    quadColorRGBY[ 1 ].a = 1.f;

	quadColorRGBY[ 2 ].r = 0.f;
    quadColorRGBY[ 2 ].g = 1.f;
    quadColorRGBY[ 2 ].b = 0.f;
    quadColorRGBY[ 2 ].a = 1.f;

	quadColorRGBY[ 3 ].r = 0.f;
    quadColorRGBY[ 3 ].g = 0.f;
    quadColorRGBY[ 3 ].b = 1.f;
    quadColorRGBY[ 3 ].a = 1.f;

	quadColorCYMW[ 0 ].r = 0.f;
    quadColorCYMW[ 0 ].g = 1.f;
    quadColorCYMW[ 0 ].b = 1.f;
    quadColorCYMW[ 0 ].a = 1.f;

	quadColorCYMW[ 1 ].r = 1.f;
    quadColorCYMW[ 1 ].g = 1.f;
    quadColorCYMW[ 1 ].b = 0.f;
    quadColorCYMW[ 1 ].a = 1.f;

	quadColorCYMW[ 2 ].r = 1.f;
    quadColorCYMW[ 2 ].g = 0.f;
    quadColorCYMW[ 2 ].b = 1.f;
    quadColorCYMW[ 2 ].a = 1.f;

	quadColorCYMW[ 3 ].r = 1.f;
    quadColorCYMW[ 3 ].g = 1.f;
    quadColorCYMW[ 3 ].b = 1.f;
    quadColorCYMW[ 3 ].a = 1.f;

	quadColorGray[ 0 ].r = 0.75f;
    quadColorGray[ 0 ].g = 0.75f;
    quadColorGray[ 0 ].b = 0.75f;
    quadColorGray[ 0 ].a = 1.f;

	quadColorGray[ 1 ].r = 0.50f;
    quadColorGray[ 1 ].g = 0.50f;
    quadColorGray[ 1 ].b = 0.50f;
    quadColorGray[ 1 ].a = 0.50f;

	quadColorGray[ 2 ].r = 0.75f;
    quadColorGray[ 2 ].g = 0.75f;
    quadColorGray[ 2 ].b = 0.75f;
    quadColorGray[ 2 ].a = 1.f;

	quadColorGray[ 3 ].r = 0.50f;
    quadColorGray[ 3 ].g = 0.50f;
    quadColorGray[ 3 ].b = 0.50f;
    quadColorGray[ 3 ].a = 1.f;

    //Set rendering indices
    indices[ 0 ] = 0;
    indices[ 1 ] = 1;
    indices[ 2 ] = 2;
    indices[ 3 ] = 3;

    //Create VBOs
    glGenBuffers( 1, &gVertexVBO );
    glBindBuffer( GL_ARRAY_BUFFER, gVertexVBO );
    glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(LVertexPos2D), quadPos, GL_STATIC_DRAW );

	glGenBuffers( 1, &gRGBYVBO );
    glBindBuffer( GL_ARRAY_BUFFER, gRGBYVBO );
    glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(LColorRGBA), quadColorRGBY, GL_STATIC_DRAW );
	
	glGenBuffers( 1, &gCYMWVBO );
    glBindBuffer( GL_ARRAY_BUFFER, gCYMWVBO );
    glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(LColorRGBA), quadColorCYMW, GL_STATIC_DRAW );
	
	glGenBuffers( 1, &gGrayVBO );
    glBindBuffer( GL_ARRAY_BUFFER, gGrayVBO );
    glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(LColorRGBA), quadColorGray, GL_STATIC_DRAW );

    //Create IBO
    glGenBuffers( 1, &gIBO );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gIBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indices, GL_STATIC_DRAW );

	//Generate left quad VAO
	glGenVertexArrays( 1, &gLeftVAO );

	//Bind vertex array
    glBindVertexArray( gLeftVAO );

	//Enable vertex attributes
	gDoubleMultiColorPolygonProgram2D.enableDataPointers();

	//Set vertex data
	glBindBuffer( GL_ARRAY_BUFFER, gVertexVBO );
	gDoubleMultiColorPolygonProgram2D.setVertexPointer( 0, NULL );

	glBindBuffer( GL_ARRAY_BUFFER, gRGBYVBO );
	gDoubleMultiColorPolygonProgram2D.setColor1Pointer( 0, NULL );

	glBindBuffer( GL_ARRAY_BUFFER, gGrayVBO );
	gDoubleMultiColorPolygonProgram2D.setColor2Pointer( 0, NULL );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gIBO );

	//Unbind VAO
	glBindVertexArray( NULL );


	//Generate right quad VAO
	glGenVertexArrays( 1, &gRightVAO );

	//Bind vertex array
    glBindVertexArray( gRightVAO );

	//Enable vertex attributes
	gDoubleMultiColorPolygonProgram2D.enableDataPointers();

	//Set vertex data
	glBindBuffer( GL_ARRAY_BUFFER, gVertexVBO );
	gDoubleMultiColorPolygonProgram2D.setVertexPointer( 0, NULL );

	glBindBuffer( GL_ARRAY_BUFFER, gCYMWVBO );
	gDoubleMultiColorPolygonProgram2D.setColor1Pointer( 0, NULL );

	glBindBuffer( GL_ARRAY_BUFFER, gGrayVBO );
	gDoubleMultiColorPolygonProgram2D.setColor2Pointer( 0, NULL );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gIBO );

	//Unbind VAO
	glBindVertexArray( NULL );

    return true;
}

void update()
{

}

void render()
{
    //Clear color buffer
    glClear( GL_COLOR_BUFFER_BIT );

	//Multicolor quad on the left
	gDoubleMultiColorPolygonProgram2D.setModelView( glm::translate<GLfloat>( SCREEN_WIDTH * 1.f / 4.f, SCREEN_HEIGHT / 2.f, 0.f ) );
	gDoubleMultiColorPolygonProgram2D.updateModelView();

	//Set left vertex array object
	glBindVertexArray( gLeftVAO );
    glDrawElements( GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL );

	//Multicolor quad on the right
	gDoubleMultiColorPolygonProgram2D.setModelView( glm::translate<GLfloat>( SCREEN_WIDTH * 3.f / 4.f, SCREEN_HEIGHT / 2.f, 0.f ) );
	gDoubleMultiColorPolygonProgram2D.updateModelView();

	//Set right vertex array object
	glBindVertexArray( gRightVAO );
    glDrawElements( GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL );

    //Update screen
    glutSwapBuffers();
}

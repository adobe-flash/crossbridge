/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redestributed without written permission.*/
//Version: 001

#include "LDoubleMultiColorPolygonProgram2D.h"
#include <glm/gtc/type_ptr.hpp>

LDoubleMultiColorPolygonProgram2D::LDoubleMultiColorPolygonProgram2D()
{
	mVertexPos2DLocation = 0;
	mMultiColor1Location = 0;
	mMultiColor2Location = 0;

	mProjectionMatrixLocation = 0;
	mModelViewMatrixLocation = 0;
}

bool LDoubleMultiColorPolygonProgram2D::loadProgram()
{
	//Generate program
	mProgramID = glCreateProgram();

	//Load vertex shader
	GLuint vertexShader = loadShaderFromFile( "36_vertex_array_objects/LDoubleMultiColorPolygonProgram2D.glvs", GL_VERTEX_SHADER );

    //Check for errors
    if( vertexShader == 0 )
    {
        glDeleteProgram( mProgramID );
        mProgramID = 0;
        return false;
    }

	//Attach vertex shader to program
	glAttachShader( mProgramID, vertexShader );


	//Create fragment shader
	GLuint fragmentShader = loadShaderFromFile( "36_vertex_array_objects/LDoubleMultiColorPolygonProgram2D.glfs", GL_FRAGMENT_SHADER );

    //Check for errors
    if( fragmentShader == 0 )
    {
        glDeleteProgram( mProgramID );
        mProgramID = 0;
        return false;
    }

	//Attach fragment shader to program
	glAttachShader( mProgramID, fragmentShader );

	//Link program
    glLinkProgram( mProgramID );

	//Check for errors
	GLint programSuccess = GL_TRUE;
	glGetProgramiv( mProgramID, GL_LINK_STATUS, &programSuccess );
	if( programSuccess != GL_TRUE )
    {
		printf( "Error linking program %d!\n", mProgramID );
		printProgramLog( mProgramID );
        glDeleteProgram( mProgramID );
        mProgramID = 0;
        return false;
    }

	//Get variable locations
	mVertexPos2DLocation = glGetAttribLocation( mProgramID, "LVertexPos2D" );
	if( mVertexPos2DLocation == -1 )
	{
		printf( "%s is not a valid glsl program variable!\n", "LVertexPos2D" );
	}

	mMultiColor1Location = glGetAttribLocation( mProgramID, "LMultiColor1" );
	if( mMultiColor1Location == -1 )
	{
		printf( "%s is not a valid glsl program variable!\n", "LMultiColor1" );
	}

	mMultiColor2Location = glGetAttribLocation( mProgramID, "LMultiColor2" );
	if( mMultiColor2Location == -1 )
	{
		printf( "%s is not a valid glsl program variable!\n", "LMultiColor2" );
	}

	mProjectionMatrixLocation = glGetUniformLocation( mProgramID, "LProjectionMatrix" );
	if( mProjectionMatrixLocation == -1 )
	{
		printf( "%s is not a valid glsl program variable!\n", "LProjectionMatrix" );
	}

	mModelViewMatrixLocation = glGetUniformLocation( mProgramID, "LModelViewMatrix" );
	if( mModelViewMatrixLocation == -1 )
	{
		printf( "%s is not a valid glsl program variable!\n", "LModelViewMatrix" );
	}

	return true;
}

void LDoubleMultiColorPolygonProgram2D::setVertexPointer( GLsizei stride, const GLvoid* data )
{
	glVertexAttribPointer( mVertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, stride, data );
}

void LDoubleMultiColorPolygonProgram2D::setColor1Pointer( GLsizei stride, const GLvoid* data )
{
	glVertexAttribPointer( mMultiColor1Location, 4, GL_FLOAT, GL_FALSE, stride, data );
}

void LDoubleMultiColorPolygonProgram2D::setColor2Pointer( GLsizei stride, const GLvoid* data )
{
	glVertexAttribPointer( mMultiColor2Location, 4, GL_FLOAT, GL_FALSE, stride, data );
}

void LDoubleMultiColorPolygonProgram2D::enableDataPointers()
{
	glEnableVertexAttribArray( mVertexPos2DLocation );
	glEnableVertexAttribArray( mMultiColor1Location );
	glEnableVertexAttribArray( mMultiColor2Location );
}

void LDoubleMultiColorPolygonProgram2D::disableDataPointers()
{
	glDisableVertexAttribArray( mMultiColor2Location );
	glDisableVertexAttribArray( mMultiColor1Location );
	glDisableVertexAttribArray( mVertexPos2DLocation );
}

void LDoubleMultiColorPolygonProgram2D::setProjection( glm::mat4 matrix )
{
	mProjectionMatrix = matrix;
}

void LDoubleMultiColorPolygonProgram2D::setModelView( glm::mat4 matrix )
{
	mModelViewMatrix = matrix;
}

void LDoubleMultiColorPolygonProgram2D::leftMultProjection( glm::mat4 matrix )
{
	mProjectionMatrix = matrix * mProjectionMatrix;
}

void LDoubleMultiColorPolygonProgram2D::leftMultModelView( glm::mat4 matrix )
{
	mModelViewMatrix = matrix * mModelViewMatrix;
}

void LDoubleMultiColorPolygonProgram2D::updateProjection()
{
	glUniformMatrix4fv(	mProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr( mProjectionMatrix ) );
}

void LDoubleMultiColorPolygonProgram2D::updateModelView()
{
	glUniformMatrix4fv(	mModelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr( mModelViewMatrix ) );
}

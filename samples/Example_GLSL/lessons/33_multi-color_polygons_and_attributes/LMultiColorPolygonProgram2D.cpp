/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redestributed without written permission.*/
//Version: 001

#include "LMultiColorPolygonProgram2D.h"
#include <glm/gtc/type_ptr.hpp>

LMultiColorPolygonProgram2D::LMultiColorPolygonProgram2D()
{
	mVertexPos2DLocation = 0;
	mMultiColorLocation = 0;

	mProjectionMatrixLocation = 0;
	mModelViewMatrixLocation = 0;
}

bool LMultiColorPolygonProgram2D::loadProgram()
{
	//Generate program
	mProgramID = glCreateProgram();

	//Load vertex shader
	GLuint vertexShader = loadShaderFromFile( "33_multi-color_polygons_and_attributes/LMultiColorPolygonProgram2D.glvs", GL_VERTEX_SHADER );

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
	GLuint fragmentShader = loadShaderFromFile( "33_multi-color_polygons_and_attributes/LMultiColorPolygonProgram2D.glfs", GL_FRAGMENT_SHADER );

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

	mMultiColorLocation = glGetAttribLocation( mProgramID, "LMultiColor" );
	if( mMultiColorLocation == -1 )
	{
		printf( "%s is not a valid glsl program variable!\n", "LMultiColor" );
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

void LMultiColorPolygonProgram2D::setVertexPointer( GLsizei stride, const GLvoid* data )
{
	glVertexAttribPointer( mVertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, stride, data );
}

void LMultiColorPolygonProgram2D::setColorPointer( GLsizei stride, const GLvoid* data )
{
	glVertexAttribPointer( mMultiColorLocation, 4, GL_FLOAT, GL_FALSE, stride, data );
}

void LMultiColorPolygonProgram2D::enableVertexPointer()
{
	glEnableVertexAttribArray( mVertexPos2DLocation );
}

void LMultiColorPolygonProgram2D::disableVertexPointer()
{
	glDisableVertexAttribArray( mVertexPos2DLocation );
}

void LMultiColorPolygonProgram2D::enableColorPointer()
{
	glEnableVertexAttribArray( mMultiColorLocation );
}

void LMultiColorPolygonProgram2D::disableColorPointer()
{
	glDisableVertexAttribArray( mMultiColorLocation );
}

void LMultiColorPolygonProgram2D::setProjection( glm::mat4 matrix )
{
	mProjectionMatrix = matrix;
}

void LMultiColorPolygonProgram2D::setModelView( glm::mat4 matrix )
{
	mModelViewMatrix = matrix;
}

void LMultiColorPolygonProgram2D::leftMultProjection( glm::mat4 matrix )
{
	mProjectionMatrix = matrix * mProjectionMatrix;
}

void LMultiColorPolygonProgram2D::leftMultModelView( glm::mat4 matrix )
{
	mModelViewMatrix = matrix * mModelViewMatrix;
}

void LMultiColorPolygonProgram2D::updateProjection()
{
	glUniformMatrix4fv(	mProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr( mProjectionMatrix ) );
}

void LMultiColorPolygonProgram2D::updateModelView()
{
	glUniformMatrix4fv(	mModelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr( mModelViewMatrix ) );
}

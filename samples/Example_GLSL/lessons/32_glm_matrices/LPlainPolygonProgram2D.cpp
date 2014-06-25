/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 003

#include "LPlainPolygonProgram2D.h"
#include <glm/gtc/type_ptr.hpp>

LPlainPolygonProgram2D::LPlainPolygonProgram2D()
{
	mPolygonColorLocation = 0;
	mProjectionMatrixLocation = 0;
	mModelViewMatrixLocation = 0;
}

bool LPlainPolygonProgram2D::loadProgram()
{
	//Generate program
	mProgramID = glCreateProgram();

	//Load vertex shader
	GLuint vertexShader = loadShaderFromFile( "32_glm_matrices/LPlainPolygonProgram2D.glvs", GL_VERTEX_SHADER );

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
	GLuint fragmentShader = loadShaderFromFile( "32_glm_matrices/LPlainPolygonProgram2D.glfs", GL_FRAGMENT_SHADER );

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
	mPolygonColorLocation = glGetUniformLocation( mProgramID, "LPolygonColor" );
	if( mPolygonColorLocation == -1 )
	{
		printf( "%s is not a valid glsl program variable!\n", "LPolygonColor" );
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

void LPlainPolygonProgram2D::setColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a )
{
	glUniform4f( mPolygonColorLocation, r, g, b, a );
}

void LPlainPolygonProgram2D::setProjection( glm::mat4 matrix )
{
	mProjectionMatrix = matrix;
}

void LPlainPolygonProgram2D::setModelView( glm::mat4 matrix )
{
	mModelViewMatrix = matrix;
}

void LPlainPolygonProgram2D::leftMultProjection( glm::mat4 matrix )
{
	mProjectionMatrix = matrix * mProjectionMatrix;
}

void LPlainPolygonProgram2D::leftMultModelView( glm::mat4 matrix )
{
	mModelViewMatrix = matrix * mModelViewMatrix;
}

void LPlainPolygonProgram2D::updateProjection()
{
	glUniformMatrix4fv(	mProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr( mProjectionMatrix ) );
}

void LPlainPolygonProgram2D::updateModelView()
{
	glUniformMatrix4fv(	mModelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr( mModelViewMatrix ) );
}

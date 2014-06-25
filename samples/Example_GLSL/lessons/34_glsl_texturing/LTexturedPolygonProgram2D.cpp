/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redestributed without written permission.*/
//Version: 001

#include "LTexturedPolygonProgram2D.h"
#include <glm/gtc/type_ptr.hpp>

LTexturedPolygonProgram2D::LTexturedPolygonProgram2D()
{
	mVertexPos2DLocation = 0;
	mTexCoordLocation = 0;

	mProjectionMatrixLocation = 0;
	mModelViewMatrixLocation = 0;
    mTextureColorLocation = 0;
    mTextureUnitLocation = 0;
}

bool LTexturedPolygonProgram2D::loadProgram()
{
	//Generate program
	mProgramID = glCreateProgram();

	//Load vertex shader
	GLuint vertexShader = loadShaderFromFile( "34_glsl_texturing/LTexturedPolygonProgram2D.glvs", GL_VERTEX_SHADER );

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
	GLuint fragmentShader = loadShaderFromFile( "34_glsl_texturing/LTexturedPolygonProgram2D.glfs", GL_FRAGMENT_SHADER );

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

	mTexCoordLocation = glGetAttribLocation( mProgramID, "LTexCoord" );
	if( mTexCoordLocation == -1 )
	{
		printf( "%s is not a valid glsl program variable!\n", "LTexCoord" );
	}

	mTextureColorLocation = glGetUniformLocation( mProgramID, "LTextureColor" );
	if( mTextureColorLocation == -1 )
	{
		printf( "%s is not a valid glsl program variable!\n", "LTextureColor" );
	}

	mTextureUnitLocation = glGetUniformLocation( mProgramID, "LTextureUnit" );
	if( mTextureUnitLocation == -1 )
	{
		printf( "%s is not a valid glsl program variable!\n", "LTextureUnit" );
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

void LTexturedPolygonProgram2D::setVertexPointer( GLsizei stride, const GLvoid* data )
{
	glVertexAttribPointer( mVertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, stride, data );
}

void LTexturedPolygonProgram2D::setTexCoordPointer( GLsizei stride, const GLvoid* data )
{
	glVertexAttribPointer( mTexCoordLocation, 2, GL_FLOAT, GL_FALSE, stride, data );
}

void LTexturedPolygonProgram2D::enableVertexPointer()
{
	glEnableVertexAttribArray( mVertexPos2DLocation );
}

void LTexturedPolygonProgram2D::disableVertexPointer()
{
	glDisableVertexAttribArray( mVertexPos2DLocation );
}

void LTexturedPolygonProgram2D::enableTexCoordPointer()
{
	glEnableVertexAttribArray( mTexCoordLocation );
}

void LTexturedPolygonProgram2D::disableTexCoordPointer()
{
	glDisableVertexAttribArray( mTexCoordLocation );
}

void LTexturedPolygonProgram2D::setProjection( glm::mat4 matrix )
{
	mProjectionMatrix = matrix; 
}

void LTexturedPolygonProgram2D::setModelView( glm::mat4 matrix )
{
	mModelViewMatrix = matrix;
}

void LTexturedPolygonProgram2D::leftMultProjection( glm::mat4 matrix )
{
	mProjectionMatrix = matrix * mProjectionMatrix;
}

void LTexturedPolygonProgram2D::leftMultModelView( glm::mat4 matrix )
{
	mModelViewMatrix = matrix * mModelViewMatrix;
}

void LTexturedPolygonProgram2D::updateProjection()
{
	glUniformMatrix4fv(	mProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr( mProjectionMatrix ) );
}

void LTexturedPolygonProgram2D::updateModelView()
{
	glUniformMatrix4fv(	mModelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr( mModelViewMatrix ) );
}

void LTexturedPolygonProgram2D::setTextureColor( LColorRGBA color )
{
	glUniform4f( mTextureColorLocation, color.r, color.g, color.b, color.a );
}

void LTexturedPolygonProgram2D::setTextureUnit( GLuint unit )
{
	glUniform1i( mTextureUnitLocation, unit );
}

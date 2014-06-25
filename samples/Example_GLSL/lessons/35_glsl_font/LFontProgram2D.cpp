/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 001

#include "LFontProgram2D.h"
#include <glm/gtc/type_ptr.hpp> 

LFontProgram2D::LFontProgram2D()
{
	mVertexPos2DLocation = 0;
	mTexCoordLocation = 0;

	mProjectionMatrixLocation = 0;
	mModelViewMatrixLocation = 0;
    mTextColorLocation = 0;
    mTextureUnitLocation = 0;
}

bool LFontProgram2D::loadProgram()
{
	//Generate program
	mProgramID = glCreateProgram();

	//Load vertex shader
	GLuint vertexShader = loadShaderFromFile( "35_glsl_font/LFontProgram2D.glvs", GL_VERTEX_SHADER );

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
	GLuint fragmentShader = loadShaderFromFile( "35_glsl_font/LFontProgram2D.glfs", GL_FRAGMENT_SHADER );

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

	mTextColorLocation = glGetUniformLocation( mProgramID, "LTextColor" );
	if( mTextColorLocation == -1 )
	{
		printf( "%s is not a valid glsl program variable!\n", "LTextColor" );
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

void LFontProgram2D::setVertexPointer( GLsizei stride, const GLvoid* data )
{
	glVertexAttribPointer( mVertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, stride, data );
}

void LFontProgram2D::setTexCoordPointer( GLsizei stride, const GLvoid* data )
{
	glVertexAttribPointer( mTexCoordLocation, 2, GL_FLOAT, GL_FALSE, stride, data );
}

void LFontProgram2D::enableVertexPointer()
{
	glEnableVertexAttribArray( mVertexPos2DLocation );
}

void LFontProgram2D::disableVertexPointer()
{
	glDisableVertexAttribArray( mVertexPos2DLocation );
}

void LFontProgram2D::enableTexCoordPointer()
{
	glEnableVertexAttribArray( mTexCoordLocation );
}

void LFontProgram2D::disableTexCoordPointer()
{
	glDisableVertexAttribArray( mTexCoordLocation );
}

void LFontProgram2D::setProjection( glm::mat4 matrix )
{
	mProjectionMatrix = matrix;
}

void LFontProgram2D::setModelView( glm::mat4 matrix )
{
	mModelViewMatrix = matrix;
}

void LFontProgram2D::leftMultProjection( glm::mat4 matrix )
{
	mProjectionMatrix = mProjectionMatrix * matrix;
}

void LFontProgram2D::leftMultModelView( glm::mat4 matrix )
{
	mModelViewMatrix = mModelViewMatrix * matrix;
}

void LFontProgram2D::updateProjection()
{
	glUniformMatrix4fv(	mProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr( mProjectionMatrix ) );
}

void LFontProgram2D::updateModelView()
{
	glUniformMatrix4fv(	mModelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr( mModelViewMatrix ) );
}

void LFontProgram2D::setTextColor( LColorRGBA color )
{
	glUniform4f( mTextColorLocation, color.r, color.g, color.b, color.a ); 
}

void LFontProgram2D::setTextureUnit( GLuint unit )
{
	glUniform1i( mTextureUnitLocation, unit );
}

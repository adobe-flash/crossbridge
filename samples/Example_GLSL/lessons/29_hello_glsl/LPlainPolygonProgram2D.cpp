/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 001

#include "LPlainPolygonProgram2D.h"

bool LPlainPolygonProgram2D::loadProgram()
{
	//Success flag
	GLint programSuccess = GL_TRUE;

	//Generate program
	mProgramID = glCreateProgram();

	//Create vertex shader
	GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );

	//Get vertex source
	const GLchar* vertexShaderSource[] =
	{
		"void main() { gl_Position = gl_Vertex; }"
	};

	//Set vertex source
	glShaderSource( vertexShader, 1, vertexShaderSource, NULL );

	//Compile vertex source
	glCompileShader( vertexShader );

	//Check vertex shader for errors
	GLint vShaderCompiled = GL_FALSE;
	glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &vShaderCompiled );
	if( vShaderCompiled != GL_TRUE )
	{
		printf( "Unable to compile vertex shader %d!\n", vertexShader );
		printShaderLog( vertexShader );
        return false;
	}

	//Attach vertex shader to program
	glAttachShader( mProgramID, vertexShader );


	//Create fragment shader
	GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

	//Get fragment source
	const GLchar* fragmentShaderSource[] =
	{
		"void main() { gl_FragColor = vec4( 1.0, 0.0, 0.0, 1.0 ); }"
	};

	//Set fragment source
	glShaderSource( fragmentShader, 1, fragmentShaderSource, NULL );

	//Compile fragment source
	glCompileShader( fragmentShader );

	//Check fragment shader for errors
	GLint fShaderCompiled = GL_FALSE;
	glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled );
	if( fShaderCompiled != GL_TRUE )
	{
		printf( "Unable to compile fragment shader %d!\n", fragmentShader );
		printShaderLog( fragmentShader );
        return false;
	}

	//Attach fragment shader to program
	glAttachShader( mProgramID, fragmentShader );


	//Link program
    glLinkProgram( mProgramID );

	//Check for errors
	glGetProgramiv( mProgramID, GL_LINK_STATUS, &programSuccess );
	if( programSuccess != GL_TRUE )
    {
		printf( "Error linking program %d!\n", mProgramID );
		printProgramLog( mProgramID );
        return false;
    }

	return true;
}

/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 001

#ifndef LSHADER_PROGRAM_H
#define LSHADER_PROGRAM_H

#include "LOpenGL.h"
#include <stdio.h>

class LShaderProgram
{
	public:
		LShaderProgram();
        /*
        Pre Condition:
         -None
        Post Condition:
         -Initializes variables
        Side Effects:
         -None
        */

        virtual ~LShaderProgram();
        /*
        Pre Condition:
         -None
        Post Condition:
         -Frees shader program
        Side Effects:
         -None
        */

		virtual bool loadProgram() = 0;
        /*
        Pre Condition:
         -A valid OpenGL context
        Post Condition:
         -Loads shader program
        Side Effects:
         -None
        */

		virtual void freeProgram();
        /*
        Pre Condition:
         -None
        Post Condition:
         -Frees shader program if it exists
        Side Effects:
         -None
        */

		bool bind();
        /*
        Pre Condition:
         -A loaded shader program
        Post Condition:
         -Sets this program as the current shader program
         -Reports to console if there was an error
        Side Effects:
         -None
        */

		void unbind();
        /*
        Pre Condition:
         -None
        Post Condition:
         -Sets default shader program as current program
        Side Effects:
         -None
        */

		GLuint getProgramID();
        /*
        Pre Condition:
         -None
        Post Condition:
         -Returns program ID
        Side Effects:
         -None
        */

	protected:
		void printProgramLog( GLuint program );
        /*
        Pre Condition:
         -None
        Post Condition:
         -Prints program log
         -Reports error is GLuint ID is not a shader program
        Side Effects:
         -None
        */

		void printShaderLog( GLuint shader );
        /*
        Pre Condition:
         -None
        Post Condition:
         -Prints shader log
         -Reports error is GLuint ID is not a shader
        Side Effects:
         -None
        */

		//Program ID
		GLuint mProgramID;
};

#endif

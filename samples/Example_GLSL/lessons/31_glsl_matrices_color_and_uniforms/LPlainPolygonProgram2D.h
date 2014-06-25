/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 002

#ifndef LPLAIN_POLYGON_PROGRAM_2D_H
#define LPLAIN_POLYGON_PROGRAM_2D_H

#include "LShaderProgram.h"

class LPlainPolygonProgram2D : public LShaderProgram
{
	public:
		LPlainPolygonProgram2D();
        /*
        Pre Condition:
         -None
        Post Condition:
         -Initializes variables
        Side Effects:
         -None
        */

		bool loadProgram();
        /*
        Pre Condition:
         -A valid OpenGL context
        Post Condition:
         -Loads plain polygon program
        Side Effects:
         -None
        */

		void setColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.f );
        /*
        Pre Condition:
         -Bound LPlainPolygonProgram2D
        Post Condition:
         -Update uniform color used for rendering
        Side Effects:
         -None
        */

	private:
		//Color uniform location
		GLint mPolygonColorLocation;
};

#endif

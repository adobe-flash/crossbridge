/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redestributed without written permission.*/
//Version: 001

#ifndef LDOUBLE_MULTI_COLOR_POLYGON_PROGRAM_2D_H
#define LDOUBLE_MULTI_COLOR_POLYGON_PROGRAM_2D_H

#include "LShaderProgram.h"
#include <glm/glm.hpp>

class LDoubleMultiColorPolygonProgram2D : public LShaderProgram
{
	public:
		LDoubleMultiColorPolygonProgram2D();
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
         -Loads double multicolor polygon program
        Side Effects:
         -None
        */

		void setVertexPointer( GLsizei stride, const GLvoid* data );
        /*
        Pre Condition:
         -Bound LMultiColorPolygonProgram2D
        Post Condition:
         -Sets vertex position attribute pointer
        Side Effects:
         -None
        */

		void setColor1Pointer( GLsizei stride, const GLvoid* data );
        /*
        Pre Condition:
         -Bound LMultiColorPolygonProgram2D
        Post Condition:
         -Sets vertex color 1 attribute pointer
        Side Effects:
         -None
        */

		void setColor2Pointer( GLsizei stride, const GLvoid* data );
        /*
        Pre Condition:
         -Bound LMultiColorPolygonProgram2D
        Post Condition:
         -Sets vertex color 2 attribute pointer
        Side Effects:
         -None
        */

		void enableDataPointers();
		/*
        Pre Condition:
         -Bound LDoubleMultiColorPolygonProgram2D
        Post Condition:
         -Enables all attributes
        Side Effects:
         -None
        */

		void disableDataPointers();
		/*
        Pre Condition:
         -Bound LDoubleMultiColorPolygonProgram2D
        Post Condition:
         -Disables all attributes
        Side Effects:
         -None
        */

		void setProjection( glm::mat4 matrix );
        /*
        Pre Condition:
         -None
        Post Condition:
         -Sets member projection matrix
        Side Effects:
         -None
        */

		void setModelView( glm::mat4 matrix );
		/*
        Pre Condition:
         -None
        Post Condition:
         -Sets member modelview matrix
        Side Effects:
         -None
        */

		void leftMultProjection( glm::mat4 matrix );
		/*
        Pre Condition:
         -None
        Post Condition:
         -Left multiplies member projection matrix
        Side Effects:
         -None
        */

		void leftMultModelView( glm::mat4 matrix );
		/*
        Pre Condition:
         -None
        Post Condition:
         -Left multiplies member modelview matrix
        Side Effects:
         -None
        */

		void updateProjection();
		/*
        Pre Condition:
         -Bound LDoubleMultiColorPolygonProgram2D
        Post Condition:
         -Updates shader program projection matrix with member projection matrix
        Side Effects:
         -None
        */

		void updateModelView();
		/*
        Pre Condition:
         -Bound LDoubleMultiColorPolygonProgram2D
        Post Condition:
         -Updates shader program modelview matrix with member modelview matrix
        Side Effects:
         -None
        */

	private:
		//Attribute locations
		GLint mVertexPos2DLocation;
		GLint mMultiColor1Location;
		GLint mMultiColor2Location;

		//Projection matrix
		glm::mat4 mProjectionMatrix;
		GLint mProjectionMatrixLocation;

		//Modelview matrix
		glm::mat4 mModelViewMatrix;
		GLint mModelViewMatrixLocation;
};

#endif

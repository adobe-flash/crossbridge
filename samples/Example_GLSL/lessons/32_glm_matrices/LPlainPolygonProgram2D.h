/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 003

#ifndef LPLAIN_POLYGON_PROGRAM_2D_H
#define LPLAIN_POLYGON_PROGRAM_2D_H

#include "LShaderProgram.h"
#include <glm/glm.hpp>

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
         -Updates uniform color used for rendering
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
         -Bound LPlainPolygonProgram2D
        Post Condition:
         -Updates shader program projection matrix with member projection matrix
        Side Effects:
         -None
        */

		void updateModelView();
		/*
        Pre Condition:
         -Bound LPlainPolygonProgram2D
        Post Condition:
         -Updates shader program modelview matrix with member modelview matrix
        Side Effects:
         -None
        */

	private:
		//Color uniform location
		GLint mPolygonColorLocation;

		//Projection matrix
		glm::mat4 mProjectionMatrix;
		GLint mProjectionMatrixLocation;

		//Modelview matrix
		glm::mat4 mModelViewMatrix;
		GLint mModelViewMatrixLocation;
};

#endif

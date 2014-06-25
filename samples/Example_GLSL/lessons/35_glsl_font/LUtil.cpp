/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 035

#include "LUtil.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include "LTexturedPolygonProgram2D.h"
#include "LFont.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 

//Screen dimensions
LFRect gScreenArea = { 0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT };

//Textured polygon shader
LTexturedPolygonProgram2D gTexturedPolygonProgram2D;

//Loaded texture
LTexture gOpenGLTexture;
LColorRGBA gImgColor = { 0.5f, 0.5f, 0.5f, 1.f };

//Font shader
LFontProgram2D gFontProgram2D;

//Loaded font
LFont gFont;
LColorRGBA gTextColor = { 1.f, 0.5f, 1.f, 1.f };

bool initGL()
{
    //Initialize GLEW
    GLenum glewError = glewInit();
    if( glewError != GLEW_OK )
    {
        printf( "Error initializing GLEW! %s\n", glewGetErrorString( glewError ) );
        return false;
    }

    //Make sure OpenGL 2.1 is supported
    if( !GLEW_VERSION_2_1 )
    {
        printf( "OpenGL 2.1 not supported!\n" );
        return false;
    }

    //Set the viewport
    glViewport( 0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT );

    //Initialize clear color
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    //Enable texturing
    glEnable( GL_TEXTURE_2D );

    //Set blending
    glEnable( GL_BLEND );
    glDisable( GL_DEPTH_TEST );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    //Check for error
    GLenum error = glGetError();
    if( error != GL_NO_ERROR )
    {
        printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
        return false;
    }

    //Initialize DevIL and DevILU
    ilInit();
    iluInit();
    ilClearColour( 255, 255, 255, 000 );

    //Check for error
    ILenum ilError = ilGetError();
    if( ilError != IL_NO_ERROR )
    {
        printf( "Error initializing DevIL! %s\n", iluErrorString( ilError ) );
        return false;
    }

    return true;
}

bool loadGP()
{
	//Load textured shader program
	if( !gTexturedPolygonProgram2D.loadProgram() )
	{
		printf( "Unable to load textured polygon program!\n" );
		return false;
	}

	//Bind textured shader program
	gTexturedPolygonProgram2D.bind();

	//Initialize projection
	gTexturedPolygonProgram2D.setProjection( glm::ortho<GLfloat>( 0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 1.0, -1.0 ) );
	gTexturedPolygonProgram2D.updateProjection();

	//Initialize modelview
	gTexturedPolygonProgram2D.setModelView( glm::mat4() );
	gTexturedPolygonProgram2D.updateModelView();

    //Set texture unit
    gTexturedPolygonProgram2D.setTextureUnit( 0 );

	//Set program for texture
	LTexture::setTexturedPolygonProgram2D( &gTexturedPolygonProgram2D );

	//Load font shader program
	if( !gFontProgram2D.loadProgram() )
	{
		printf( "Unable to load font rendering program!\n" );
		return false;
	}

	//Bind font shader program
	gFontProgram2D.bind();

	//Initialize projection
	gFontProgram2D.setProjection( glm::ortho<GLfloat>( 0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 1.0, -1.0 ) );
	gFontProgram2D.updateProjection();

	//Initialize modelview
	gFontProgram2D.setModelView( glm::mat4() );
	gFontProgram2D.updateModelView();

    //Set texture unit
    gFontProgram2D.setTextureUnit( 0 );

	//Set program for font rendering
	LFont::setFontProgram2D( &gFontProgram2D );

	return true;
}

bool loadMedia()
{
	//Load image
	if( !gOpenGLTexture.loadTextureFromFile32( "35_glsl_font/opengl.png" ) )
	{
	    printf( "Unable to load opengl texture!\n" );
		return false;
	}

	//Load font
	if( !gFont.loadFreeType( "35_glsl_font/lazy.ttf", 60 ) )
	{
	    printf( "Unable to load font!\n" );
		return false;
	}

    return true;
}

void update()
{

}

void render()
{
    //Clear color buffer
    glClear( GL_COLOR_BUFFER_BIT );

	//Bind texture rendering program
	gTexturedPolygonProgram2D.bind();

	//Reset transformations
	gTexturedPolygonProgram2D.setModelView( glm::mat4() );

	//Render texture centered
	gTexturedPolygonProgram2D.setTextureColor( gImgColor );
	gOpenGLTexture.render( ( SCREEN_WIDTH - gOpenGLTexture.imageWidth() ) / 2.f, ( SCREEN_HEIGHT - gOpenGLTexture.imageHeight() ) / 2.f );

	//Bind font rendering program
	gFontProgram2D.bind();

	//Reset transformations
	gFontProgram2D.setModelView( glm::mat4() );

	//Render text centered
	gFontProgram2D.setTextColor( gTextColor );
	gFont.renderText( 0.f, 0.f, "GLSL Text Rendering!", &gScreenArea, LFONT_TEXT_ALIGN_CENTERED_H | LFONT_TEXT_ALIGN_CENTERED_V );

    //Update screen
    glutSwapBuffers();
}

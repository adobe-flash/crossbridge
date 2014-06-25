/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 019

#include "LUtil.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include "LSpriteSheet.h"

//Sprite sheet
LSpriteSheet gArrowSprites;

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

    //Initialize Projection Matrix
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 1.0, -1.0 );

    //Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

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

bool loadMedia()
{
    //Load texture
	if( !gArrowSprites.loadTextureFromFile( "19_sprite_sheets/arrows.png" ) )
	{
	    printf( "Unable to load sprite sheet!\n" );
		return false;
	}

	//Set clips
	LFRect clip = { 0.f, 0.f, 128.f, 128.f };

	//Top left
	clip.x = 0.f;
	clip.y = 0.f;
	gArrowSprites.addClipSprite( clip );

	//Top right
	clip.x = 128.f;
	clip.y = 0.f;
	gArrowSprites.addClipSprite( clip );

	//Bottom left
	clip.x = 0.f;
	clip.y = 128.f;
	gArrowSprites.addClipSprite( clip );

	//Bottom right
	clip.x = 128.f;
	clip.y = 128.f;
	gArrowSprites.addClipSprite( clip );

    //Generate VBO
	if( !gArrowSprites.generateDataBuffer() )
	{
	    printf( "Unable to clip sprite sheet!\n" );
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

	//Render top left arrow
	glLoadIdentity();
	glTranslatef( 64.f, 64.f, 0.f );
	gArrowSprites.renderSprite( 0 );

	//Render top right arrow
	glLoadIdentity();
	glTranslatef( SCREEN_WIDTH - 64.f, 64.f, 0.f );
	gArrowSprites.renderSprite( 1 );

	//Render bottom left arrow
	glLoadIdentity();
	glTranslatef( 64.f, SCREEN_HEIGHT - 64.f, 0.f );
	gArrowSprites.renderSprite( 2 );

	//Render bottom right arrow
	glLoadIdentity();
	glTranslatef( SCREEN_WIDTH - 64.f, SCREEN_HEIGHT - 64.f, 0.f );
	gArrowSprites.renderSprite( 3 );

    //Update screen
    glutSwapBuffers();
}

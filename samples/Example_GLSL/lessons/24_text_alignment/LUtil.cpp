/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 025

#include "LUtil.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include "LFont.h"

//Text renderer
LFont gFont;

//Alignment variables
LFontTextAlignment gAlignH = LFONT_TEXT_ALIGN_LEFT;
LFontTextAlignment gAlignV = LFONT_TEXT_ALIGN_TOP;
int gAlign = gAlignH | gAlignV;

//Screen area
LFRect gScreenArea = { 0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT };

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

    //Init FreeType
    if( !LFont::initFreeType() )
    {
        printf( "Unable to initialize FreeType!\n" );
    }

    return true;
}

bool loadMedia()
{
    //Load font
	if( !gFont.loadFreeType( "24_text_alignment/lazy.ttf", 60 ) )
	{
	    printf( "Unable to load ttf font!\n" );
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
    glLoadIdentity();

    //Render yellow text
    glColor3f( 1.f, 1.f, 0.f );
    gFont.renderText( 0.f, SCREEN_HEIGHT / 2.f, "Testing...\nAlignment...", &gScreenArea, gAlign );

    //Update screen
    glutSwapBuffers();
}

void handleKeys( unsigned char key, int x, int y )
{
    if( key == 'a' )
    {
        switch( gAlignH )
        {
            case LFONT_TEXT_ALIGN_LEFT:
                gAlignH = LFONT_TEXT_ALIGN_RIGHT;
                break;
            case LFONT_TEXT_ALIGN_CENTERED_H:
                gAlignH = LFONT_TEXT_ALIGN_LEFT;
                break;
            case LFONT_TEXT_ALIGN_RIGHT:
                gAlignH = LFONT_TEXT_ALIGN_CENTERED_H;
                break;
        }
    }
    else if( key == 'd' )
    {
        switch( gAlignH )
        {
            case LFONT_TEXT_ALIGN_LEFT:
                gAlignH = LFONT_TEXT_ALIGN_CENTERED_H;
                break;
            case LFONT_TEXT_ALIGN_CENTERED_H:
                gAlignH = LFONT_TEXT_ALIGN_RIGHT;
                break;
            case LFONT_TEXT_ALIGN_RIGHT:
                gAlignH = LFONT_TEXT_ALIGN_LEFT;
                break;
        }
    }
    else if( key == 'w' )
    {
        switch( gAlignV )
        {
            case LFONT_TEXT_ALIGN_TOP:
                gAlignV = LFONT_TEXT_ALIGN_BOTTOM;
                break;
            case LFONT_TEXT_ALIGN_CENTERED_V:
                gAlignV = LFONT_TEXT_ALIGN_TOP;
                break;
            case LFONT_TEXT_ALIGN_BOTTOM:
                gAlignV = LFONT_TEXT_ALIGN_CENTERED_V;
                break;
        }
    }
    else if( key == 's' )
    {
        switch( gAlignV )
        {
            case LFONT_TEXT_ALIGN_TOP:
                gAlignV = LFONT_TEXT_ALIGN_CENTERED_V;
                break;
            case LFONT_TEXT_ALIGN_CENTERED_V:
                gAlignV = LFONT_TEXT_ALIGN_BOTTOM;
                break;
            case LFONT_TEXT_ALIGN_BOTTOM:
                gAlignV = LFONT_TEXT_ALIGN_TOP;
                break;
        }
    }

    //Set alignment
    gAlign = gAlignH | gAlignV;
}

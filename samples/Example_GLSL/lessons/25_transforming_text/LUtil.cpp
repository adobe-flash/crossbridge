/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 025

#include "LUtil.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include "LFont.h"

//Font
LFont gFont;

//Text areas
LFRect gScaledArea = { 0.f, 0.f, 0.f, 0.f };
LFRect gPivotArea = { 0.f, 0.f, 0.f, 0.f };
LFRect gCirclingArea = { 0.f, 0.f, 0.f, 0.f };

//Transformation variables
GLfloat gBigTextScale = 3.f;
GLfloat gPivotAngle = 0.f;
GLfloat gCirclingAngle = 0.f;

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
    //Load text
	if( !gFont.loadFreeType( "25_transforming_text/lazy.ttf", 60 ) )
	{
	    printf( "Unable to load ttf font!\n" );
		return false;
	}

    //Calculate rendering areas
    gScaledArea = gFont.getStringArea( "Big Text!" );
    gPivotArea = gFont.getStringArea( "Pivot" );
    gCirclingArea = gFont.getStringArea( "Wheee!" );

    return true;
}

void update()
{
    //Update angles
    gPivotAngle += -1.f;
    gCirclingAngle += +2.f;

    //Scale
    gBigTextScale += 0.1f;
    if( gBigTextScale >= 3.f )
    {
        gBigTextScale = 0.f;
    }
}

void render()
{
    //Clear color buffer
    glClear( GL_COLOR_BUFFER_BIT );


    //Big upper middle text
    glLoadIdentity();
    glColor3f( 1.f, 0.f, 0.f );

    //Move to render point
    glTranslatef( ( SCREEN_WIDTH - gScaledArea.w * gBigTextScale ) / 2.f, ( SCREEN_HEIGHT - gScaledArea.h * gBigTextScale ) / 4.f, 0.f );

    //Scale and render
    glScalef( gBigTextScale, gBigTextScale, gBigTextScale );
    gFont.renderText( 0.f, 0.f, "Big Text!" , &gScaledArea, LFONT_TEXT_ALIGN_CENTERED_H );


    //Lower pivoting text
    glLoadIdentity();
    glColor3f( 0.f, 1.f, 0.f );

    //Move to render point
    glTranslatef( ( SCREEN_WIDTH - gPivotArea.w * 1.5f ) / 2.f, ( SCREEN_HEIGHT - gPivotArea.h * 1.5f ) * 3.f / 4.f, 0.f );

    //Scale and move to pivot point
    glScalef( 1.5f, 1.5f, 1.5f );
    glTranslatef( gPivotArea.w / 2.f, gPivotArea.h / 2.f, 0.f );

    //Rotate around pivot
    glRotatef( gPivotAngle, 0.f, 0.f, 1.f );

    //Move back to render point and render
    glTranslatef( -gPivotArea.w / 2.f, -gPivotArea.h / 2.f, 0.f );
    gFont.renderText( 0.f, 0.f, "Pivot", &gPivotArea, LFONT_TEXT_ALIGN_CENTERED_H );


    //Circling text
    glLoadIdentity();
    glColor3f( 0.f, 0.f, 1.f );

    //Move to center of screen
    glTranslatef( SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f, 0.f );

    //Rotate around center
    glRotatef( gCirclingAngle, 0.f, 0.f, 1.f );

    //Move to arm position
    glTranslatef( 0.f, -SCREEN_HEIGHT / 2.f, 0.f );

    //Center on arm
    glTranslatef( -gCirclingArea.w / 2.f, 0.f, 0.f );

    //Render
    gFont.renderText( 0.f, 0.f, "Wheee!", &gCirclingArea, LFONT_TEXT_ALIGN_CENTERED_H );


    //Update screen
    glutSwapBuffers();
}

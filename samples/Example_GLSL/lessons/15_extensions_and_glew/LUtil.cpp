/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 015

#include "LUtil.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include "LTexture.h"

//Repeating image
LTexture gRepeatingTexture;

//Texture offset
GLfloat gTexX = 0.f, gTexY = 0.f;

//Texture wrap type
int gTextureWrapType = 0;

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
        printf( "Error initializing DevIL! %s", iluErrorString( ilError ) );
        return false;
    }

    return true;
}

bool loadMedia()
{
    //Load texture
    if( !gRepeatingTexture.loadTextureFromFile( "15_extensions_and_glew/texture.png" ) )
    {
        printf( "Unable to load repeating texture!\n" );
        return false;
    }

    return true;
}

void update()
{
    //Scroll texture
    gTexX++;
    gTexY++;

    //Cap scrolling
    if( gTexX >= gRepeatingTexture.textureWidth() )
    {
        gTexX = 0.f;
    }
    if( gTexY >= gRepeatingTexture.textureHeight() )
    {
        gTexY = 0.f;
    }
}

void render()
{
    //Clear color buffer
    glClear( GL_COLOR_BUFFER_BIT );

    //Calculate texture maxima
    GLfloat textureRight = (GLfloat)SCREEN_WIDTH / (GLfloat)gRepeatingTexture.textureWidth();
    GLfloat textureBottom = (GLfloat)SCREEN_HEIGHT / (GLfloat)gRepeatingTexture.textureHeight();

    //Use repeating texture
    glBindTexture( GL_TEXTURE_2D, gRepeatingTexture.getTextureID() );

    //Switch to texture matrix
    glMatrixMode( GL_TEXTURE );

    //Reset transformation
    glLoadIdentity();

    //Scroll texture
    glTranslatef( gTexX / gRepeatingTexture.textureWidth(), gTexY / gRepeatingTexture.textureHeight(), 0.f );

    //Render
    glBegin( GL_QUADS );
        glTexCoord2f(          0.f,           0.f ); glVertex2f(          0.f,           0.f );
        glTexCoord2f( textureRight,           0.f ); glVertex2f( SCREEN_WIDTH,           0.f );
        glTexCoord2f( textureRight, textureBottom ); glVertex2f( SCREEN_WIDTH, SCREEN_HEIGHT );
        glTexCoord2f(          0.f, textureBottom ); glVertex2f(          0.f, SCREEN_HEIGHT );
    glEnd();

    //Update screen
    glutSwapBuffers();
}

void handleKeys( unsigned char key, int x, int y )
{
    //If q is pressed
    if( key == 'q' )
    {
        //Cycle through texture wraps
        gTextureWrapType++;
        if( gTextureWrapType >= 5 )
        {
            gTextureWrapType = 0;
        }

        //Set texture repetition
        glBindTexture( GL_TEXTURE_2D, gRepeatingTexture.getTextureID() );
        switch( gTextureWrapType )
        {
            case 0:
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
                printf( "%d: GL_REPEAT\n", gTextureWrapType );
            break;

            case 1:
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
                printf( "%d: GL_CLAMP\n", gTextureWrapType );
            break;

            case 2:
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
                printf( "%d: GL_CLAMP_TO_BORDER\n", gTextureWrapType );
            break;

            case 3:
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
                printf( "%d: GL_CLAMP_TO_EDGE\n", gTextureWrapType );
            break;

            case 4:
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT );
                printf( "%d: GL_MIRRORED_REPEAT\n", gTextureWrapType );
            break;
        }
    }
}

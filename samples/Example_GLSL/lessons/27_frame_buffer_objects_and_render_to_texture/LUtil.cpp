/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redestributed without written permission.*/
//Version: 027

#include "LUtil.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include "LTexture.h"

//Rotating texture
LTexture gOpenGLTexture;
GLfloat gAngle = 0.f;

//Framebuffer
GLuint gFBO = NULL;
LTexture gFBOTexture;

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

    //Generate framebuffer name
    glGenFramebuffers( 1, &gFBO );

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
	if( !gOpenGLTexture.loadTextureFromFile32( "27_frame_buffer_objects_and_render_to_texture/opengl.png" ) )
	{
	    printf( "Unable to load texture!\n" );
		return false;
	}

    return true;
}

void update()
{
    //Update rotation
    gAngle += 6.f;
}

void render()
{
    //Clear color and buffer
    glClear( GL_COLOR_BUFFER_BIT );

    //Render FBO texture
    if( gFBOTexture.getTextureID() != 0 )
    {
        glLoadIdentity();
        glTranslatef( SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f, 0.f );
        glRotatef( -gAngle, 0.f, 0.f, 1.f );
        glTranslatef( gFBOTexture.imageWidth() / -2.f, gFBOTexture.imageHeight() / -2.f, 0.f );
        glColor3f( 1.f, 1.f, 1.f );
        gFBOTexture.render( 0.f, 0.f );
    }

    //Render scene
    renderScene();

    //Update screen
    glutSwapBuffers();
}

void handleKeys( unsigned char key, int x, int y )
{
    //If the user presses q
    if( key == 'q' )
    {
        //Bind framebuffer for use
        glBindFramebuffer( GL_FRAMEBUFFER, gFBO );

        //If FBO texture doesn't exist
        if( gFBOTexture.getTextureID() == 0 )
        {
            //Create it
            gFBOTexture.createPixels32( SCREEN_WIDTH, SCREEN_HEIGHT );
            gFBOTexture.padPixels32();
            gFBOTexture.loadTextureFromPixels32();
        }

        //Bind texture
        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gFBOTexture.getTextureID(), 0 );

        //Clear framebuffer
        glClear( GL_COLOR_BUFFER_BIT );

        //Render scene to framebuffer
        renderScene();

        //Unbind framebuffer
        glBindFramebuffer( GL_FRAMEBUFFER, NULL );
    }
}

void renderScene()
{
    //Render texture
    glLoadIdentity();
    glTranslatef( SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f, 0.f );
    glRotatef( gAngle, 0.f, 0.f, 1.f );
    glTranslatef( gOpenGLTexture.imageWidth() / -2.f, gOpenGLTexture.imageHeight() / -2.f, 0.f );
    glColor3f( 1.f, 1.f, 1.f );
    gOpenGLTexture.render( 0.f, 0.f );

    //Unbind texture for geometry
    glBindTexture( GL_TEXTURE_2D, NULL );

    //Red quad
    glLoadIdentity();
    glTranslatef( SCREEN_WIDTH * 1.f / 4.f, SCREEN_HEIGHT * 1.f / 4.f, 0.f );
    glRotatef( gAngle, 0.f, 0.f, 1.f );
    glBegin( GL_QUADS );
        glColor3f( 1.f, 0.f, 0.f );
        glVertex2f( -SCREEN_WIDTH / 16.f, -SCREEN_HEIGHT / 16.f );
        glVertex2f(  SCREEN_WIDTH / 16.f, -SCREEN_HEIGHT / 16.f );
        glVertex2f(  SCREEN_WIDTH / 16.f,  SCREEN_HEIGHT / 16.f );
        glVertex2f( -SCREEN_WIDTH / 16.f,  SCREEN_HEIGHT / 16.f );
    glEnd();

    //Green quad
    glLoadIdentity();
    glTranslatef( SCREEN_WIDTH * 3.f / 4.f, SCREEN_HEIGHT * 1.f / 4.f, 0.f );
    glRotatef( gAngle, 0.f, 0.f, 1.f );
    glBegin( GL_QUADS );
        glColor3f( 0.f, 1.f, 0.f );
        glVertex2f( -SCREEN_WIDTH / 16.f, -SCREEN_HEIGHT / 16.f );
        glVertex2f(  SCREEN_WIDTH / 16.f, -SCREEN_HEIGHT / 16.f );
        glVertex2f(  SCREEN_WIDTH / 16.f,  SCREEN_HEIGHT / 16.f );
        glVertex2f( -SCREEN_WIDTH / 16.f,  SCREEN_HEIGHT / 16.f );
    glEnd();

    //Blue quad
    glLoadIdentity();
    glTranslatef( SCREEN_WIDTH * 1.f / 4.f, SCREEN_HEIGHT * 3.f / 4.f, 0.f );
    glRotatef( gAngle, 0.f, 0.f, 1.f );
    glBegin( GL_QUADS );
        glColor3f( 0.f, 0.f, 1.f );
        glVertex2f( -SCREEN_WIDTH / 16.f, -SCREEN_HEIGHT / 16.f );
        glVertex2f(  SCREEN_WIDTH / 16.f, -SCREEN_HEIGHT / 16.f );
        glVertex2f(  SCREEN_WIDTH / 16.f,  SCREEN_HEIGHT / 16.f );
        glVertex2f( -SCREEN_WIDTH / 16.f,  SCREEN_HEIGHT / 16.f );
    glEnd();

    //Yellow quad
    glLoadIdentity();
    glTranslatef( SCREEN_WIDTH * 3.f / 4.f, SCREEN_HEIGHT * 3.f / 4.f, 0.f );
    glRotatef( gAngle, 0.f, 0.f, 1.f );
    glBegin( GL_QUADS );
        glColor3f( 1.f, 1.f, 0.f );
        glVertex2f( -SCREEN_WIDTH / 16.f, -SCREEN_HEIGHT / 16.f );
        glVertex2f(  SCREEN_WIDTH / 16.f, -SCREEN_HEIGHT / 16.f );
        glVertex2f(  SCREEN_WIDTH / 16.f,  SCREEN_HEIGHT / 16.f );
        glVertex2f( -SCREEN_WIDTH / 16.f,  SCREEN_HEIGHT / 16.f );
    glEnd();
}

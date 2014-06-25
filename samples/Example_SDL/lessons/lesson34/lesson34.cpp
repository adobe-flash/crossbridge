/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/

/*
WARNING: In this program we have video functions running in seperate threads.
This is for demonstrative purposes only. You should never in a real application
have video functions running in seperate threads.
*/

//The headers
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_thread.h"
#include <string>

//Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//The surfaces
SDL_Surface *background = NULL;
SDL_Surface *screen = NULL;
SDL_Surface *text[ 5 ] = { NULL, NULL, NULL, NULL, NULL };

//The event structure
SDL_Event event;

//The font
TTF_Font *font = NULL;

//The color of the font
SDL_Color textColor = { 0, 0, 0 };

//The threads that will be used
SDL_Thread *threadA = NULL;
SDL_Thread *threadB = NULL;

//The protective semaphore
SDL_sem *videoLock = NULL;

SDL_Surface *load_image( std::string filename )
{
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized surface that will be used
    SDL_Surface* optimizedImage = NULL;

    //Load the image
    loadedImage = IMG_Load( filename.c_str() );

    //If the image loaded
    if( loadedImage != NULL )
    {
        //Create an optimized surface
        optimizedImage = SDL_DisplayFormat( loadedImage );

        //Free the old surface
        SDL_FreeSurface( loadedImage );

        //If the surface was optimized
        if( optimizedImage != NULL )
        {
            //Color key surface
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF ) );
        }
    }

    //Return the optimized surface
    return optimizedImage;
}

void show_surface( int x, int y, SDL_Surface* source )
{
    //Lock
    SDL_SemWait( videoLock );

    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit
    SDL_BlitSurface( source, NULL, screen, &offset );

    //Update the screen
    SDL_Flip( screen );

    //Unlock
    SDL_SemPost( videoLock );
}

int blitter_a( void *data )
{
    //Y offset
    int y = 10;

    //Go through the surface
    for( int b = 0; b < 5; b++ )
    {
        //Wait
        SDL_Delay( 200 );

        //Show surface
        show_surface( ( ( SCREEN_WIDTH / 2 ) - text[ b ]->w ) / 2, y, text[ b ] );

        //Move down
        y += 100;
    }

    return 0;
}

int blitter_b( void *data )
{
    //Y offset
    int y = 10;

    //Go through the surface
    for( int b = 0; b < 5; b++ )
    {
        //Wait
        SDL_Delay( 200 );

        //Show surface
        show_surface( ( SCREEN_WIDTH / 2 ) + ( ( ( SCREEN_WIDTH / 2 ) - text[ b ]->w ) / 2 ), y, text[ b ] );

        //Move down
        y += 100;
    }

    return 0;
}

bool init()
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;
    }

    //Initialize SDL_ttf
    if( TTF_Init() == -1 )
    {
        return false;
    }

    //Create the semaphore
    videoLock = SDL_CreateSemaphore( 1 );

    //Set the window caption
    SDL_WM_SetCaption( "Testing Threads", NULL );

    //If everything initialized fine
    return true;
}

bool load_files()
{
    //Load the background image
    background = load_image( "background.png" );

    //Open the font
    font = TTF_OpenFont( "lazy.ttf", 72 );

    //If there was a problem in loading the background
    if( background == NULL )
    {
        return false;
    }

    //If there was an error in loading the font
    if( font == NULL )
    {
        return false;
    }

    //Render text
    text[ 0 ] = TTF_RenderText_Solid( font, "One", textColor );
    text[ 1 ] = TTF_RenderText_Solid( font, "Two", textColor );
    text[ 2 ] = TTF_RenderText_Solid( font, "Three", textColor );
    text[ 3 ] = TTF_RenderText_Solid( font, "Four", textColor );
    text[ 4 ] = TTF_RenderText_Solid( font, "Five", textColor );

    //If everything loaded fine
    return true;
}

void clean_up()
{
    //Destroy semaphore
    SDL_DestroySemaphore( videoLock );

    //Free the surfaces
    SDL_FreeSurface( background );

    //Free text
    for( int t = 0; t < 5; t++ )
    {
        SDL_FreeSurface( text[ t ] );
    }

    //Close the font
    TTF_CloseFont( font );

    //Quit SDL_ttf
    TTF_Quit();

    //Quit SDL
    SDL_Quit();
}

int main( int argc, char* args[] )
{
    //Quit flag
    bool quit = false;

    //Initialize
    if( init() == false )
    {
        return 1;
    }

    //Load the files
    if( load_files() == false )
    {
        return 1;
    }

    //Show the background
    show_surface( 0, 0, background );

    //Create and run the threads
    threadA = SDL_CreateThread( blitter_a, NULL );
    threadB = SDL_CreateThread( blitter_b, NULL );

    //Wait for the threads to finish
    SDL_WaitThread( threadA, NULL );
    SDL_WaitThread( threadB, NULL );

    //While the user hasn't quit
    while( quit == false )
    {
        //If there's an event to handle
        if( SDL_PollEvent( &event ) )
        {
            //If the user has Xed out the window
            if( event.type == SDL_QUIT )
            {
                //Quit the program
                quit = true;
            }
        }
    }

    //Clean up
    clean_up();

    return 0;
}

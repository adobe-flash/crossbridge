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
#include "SDL/SDL_thread.h"
#include <string>

//Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//The surfaces
SDL_Surface *screen = NULL;
SDL_Surface *images[ 5 ] = { NULL, NULL, NULL, NULL, NULL };
SDL_Surface *buffer = NULL;

//The event structure
SDL_Event event;

//The threads that will be used
SDL_Thread *producerThread = NULL;
SDL_Thread *consumerThread = NULL;

//The protective mutex
SDL_mutex *bufferLock = NULL;

//The conditions
SDL_cond *canProduce = NULL;
SDL_cond *canConsume = NULL;

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

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit
    SDL_BlitSurface( source, clip, destination, &offset );
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

    //Create the mutex
    bufferLock = SDL_CreateMutex();

    //Create Conditions
    canProduce = SDL_CreateCond();
    canConsume = SDL_CreateCond();

    //Set the window caption
    SDL_WM_SetCaption( "Producer / Consumer Test", NULL );

    //If everything initialized fine
    return true;
}

bool load_files()
{
    //Load image
    images[ 0 ] = load_image( "1.png" );
    images[ 1 ] = load_image( "2.png" );
    images[ 2 ] = load_image( "3.png" );
    images[ 3 ] = load_image( "4.png" );
    images[ 4 ] = load_image( "5.png" );

    //If there was a problem in loading the images
    if( ( images[ 0 ] == NULL ) || ( images[ 1 ] == NULL ) || ( images[ 2 ] == NULL ) || ( images[ 3 ] == NULL ) || ( images[ 4 ] == NULL ) )
    {
        return false;
    }

    //If everything loaded fine
    return true;
}

void clean_up()
{
    //Destroy mutex
    SDL_DestroyMutex( bufferLock );

    //Destroy condition
    SDL_DestroyCond( canProduce );
    SDL_DestroyCond( canConsume );

    //Free the surfaces
    for( int i = 0; i < 5; i++ )
    {
        SDL_FreeSurface( images[ i ] );
    }

    //Quit SDL
    SDL_Quit();
}

void produce( int x, int y )
{
    //Lock
    SDL_mutexP( bufferLock );

    //If the buffer is full
    if( buffer != NULL )
    {
        //Wait for buffer to be cleared
        SDL_CondWait( canProduce, bufferLock );
    }

    //Fill and show buffer
    buffer = images[ rand() % 5 ];
    apply_surface( x, y, buffer, screen );

    //Update the screen
    SDL_Flip( screen );

    //Unlock
    SDL_mutexV( bufferLock );

    //Signal consumer
    SDL_CondSignal( canConsume );
}

void consume( int x, int y )
{
    //Lock
    SDL_mutexP( bufferLock );

    //If the buffer is empty
    if( buffer == NULL )
    {
        //Wait for buffer to be filled
        SDL_CondWait( canConsume, bufferLock );
    }

    //Show and empty buffer
    apply_surface( x, y, buffer, screen );
    buffer = NULL;

    //Update the screen
    SDL_Flip( screen );

    //Unlock
    SDL_mutexV( bufferLock );

    //Signal producer
    SDL_CondSignal( canProduce );
}

int producer( void *data )
{
    //The offset of the blit.
    int y = 10;

    //Seed random
    srand( SDL_GetTicks() );

    //Produce
    for( int p = 0; p < 5; p++ )
    {
        //Wait
        SDL_Delay( rand() % 1000 );

        //Produce
        produce( 10, y );

        //Move down
        y += 90;
    }

    return 0;
}

int consumer( void *data )
{
    //The offset of the blit.
    int y = 10;

    for( int p = 0; p < 5; p++ )
    {
        //Wait
        SDL_Delay( rand() % 1000 );

        //Consume
        consume( 330, y );

        //Move down
        y += 90;
    }

    return 0;
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

    //Fill screen
    SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0x80, 0x80, 0x80 ) );

    //Update the screen
    if( SDL_Flip( screen ) == -1 )
    {
        return 1;
    }

    //Create and run the threads
    producerThread = SDL_CreateThread( producer, NULL );
    consumerThread = SDL_CreateThread( consumer, NULL );

    //Wait for the threads to finish
    SDL_WaitThread( producerThread, NULL );
    SDL_WaitThread( consumerThread, NULL );

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

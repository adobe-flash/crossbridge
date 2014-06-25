/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/

//The headers
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <vector>

//Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//The frame rate
const int FRAMES_PER_SECOND = 20;

//The dot attributes
const int DOT_WIDTH = 20;
const int DOT_HEIGHT = 20;

//The surfaces
SDL_Surface *dot = NULL;
SDL_Surface *screen = NULL;

//The event structure
SDL_Event event;

//The dot
class Dot
{
    private:
    //The offsets of the dot
    int x, y;

    //The collision boxes of the dot
    std::vector<SDL_Rect> box;

    //The velocity of the dot
    int xVel, yVel;

    //Moves the collision boxes relative to the dot's offset
    void shift_boxes();

    public:
    //Initializes the variables
    Dot( int X, int Y );

    //Takes key presses and adjusts the dot's velocity
    void handle_input();

    //Moves the dot
    void move( std::vector<SDL_Rect> &rects );

    //Shows the dot on the screen
    void show();

    //Gets the collision boxes
    std::vector<SDL_Rect> &get_rects();
};

//The timer
class Timer
{
    private:
    //The clock time when the timer started
    int startTicks;

    //The ticks stored when the timer was paused
    int pausedTicks;

    //The timer status
    bool paused;
    bool started;

    public:
    //Initializes variables
    Timer();

    //The various clock actions
    void start();
    void stop();
    void pause();
    void unpause();

    //Gets the timer's time
    int get_ticks();

    //Checks the status of the timer
    bool is_started();
    bool is_paused();
};

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

bool check_collision( std::vector<SDL_Rect> &A, std::vector<SDL_Rect> &B )
{
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Go through the A boxes
    for( int Abox = 0; Abox < A.size(); Abox++ )
    {
        //Calculate the sides of rect A
        leftA = A[ Abox ].x;
        rightA = A[ Abox ].x + A[ Abox ].w;
        topA = A[ Abox ].y;
        bottomA = A[ Abox ].y + A[ Abox ].h;

        //Go through the B boxes
        for( int Bbox = 0; Bbox < B.size(); Bbox++ )
        {
            //Calculate the sides of rect B
            leftB = B[ Bbox ].x;
            rightB = B[ Bbox ].x + B[ Bbox ].w;
            topB = B[ Bbox ].y;
            bottomB = B[ Bbox ].y + B[ Bbox ].h;

            //If no sides from A are outside of B
            if( ( ( bottomA <= topB ) || ( topA >= bottomB ) || ( rightA <= leftB ) || ( leftA >= rightB ) ) == false )
            {
                //A collision is detected
                return true;
            }
        }
    }

    //If neither set of collision boxes touched
    return false;
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

    //Set the window caption
    SDL_WM_SetCaption( "Move the Dot", NULL );

    //If everything initialized fine
    return true;
}

bool load_files()
{
    //Load the dot image
    dot = load_image( "dot.bmp" );

    //If there was a problem in loading the dot
    if( dot == NULL )
    {
        return false;
    }

    //If everything loaded fine
    return true;
}

void clean_up()
{
    //Free the surface
    SDL_FreeSurface( dot );

    //Quit SDL
    SDL_Quit();
}

Dot::Dot( int X, int Y )
{
    //Initialize the offsets
    x = X;
    y = Y;

    //Initialize the velocity
    xVel = 0;
    yVel = 0;

    //Create the necessary SDL_Rects
    box.resize( 11 );

    //Initialize the collision boxes' width and height
    box[ 0 ].w = 6;
    box[ 0 ].h = 1;

    box[ 1 ].w = 10;
    box[ 1 ].h = 1;

    box[ 2 ].w = 14;
    box[ 2 ].h = 1;

    box[ 3 ].w = 16;
    box[ 3 ].h = 2;

    box[ 4 ].w = 18;
    box[ 4 ].h = 2;

    box[ 5 ].w = 20;
    box[ 5 ].h = 6;

    box[ 6 ].w = 18;
    box[ 6 ].h = 2;

    box[ 7 ].w = 16;
    box[ 7 ].h = 2;

    box[ 8 ].w = 14;
    box[ 8 ].h = 1;

    box[ 9 ].w = 10;
    box[ 9 ].h = 1;

    box[ 10 ].w = 6;
    box[ 10 ].h = 1;

    //Move the collision boxes to their proper spot
    shift_boxes();
}

void Dot::shift_boxes()
{
    //The row offset
    int r = 0;

    //Go through the dot's collision boxes
    for( int set = 0; set < box.size(); set++ )
    {
        //Center the collision box
        box[ set ].x = x + ( DOT_WIDTH - box[ set ].w ) / 2;

        //Set the collision box at its row offset
        box[ set ].y = y + r;

        //Move the row offset down the height of the collision box
        r += box[ set ].h;
    }
}

void Dot::handle_input()
{
    //If a key was pressed
    if( event.type == SDL_KEYDOWN )
    {
        //Adjust the velocity
        switch( event.key.keysym.sym )
        {
            case SDLK_UP: yVel -= 1; break;
            case SDLK_DOWN: yVel += 1; break;
            case SDLK_LEFT: xVel -= 1; break;
            case SDLK_RIGHT: xVel += 1; break;
        }
    }
    //If a key was released
    else if( event.type == SDL_KEYUP )
    {
        //Adjust the velocity
        switch( event.key.keysym.sym )
        {
            case SDLK_UP: yVel += 1; break;
            case SDLK_DOWN: yVel -= 1; break;
            case SDLK_LEFT: xVel += 1; break;
            case SDLK_RIGHT: xVel -= 1; break;
        }
    }
}

void Dot::move( std::vector<SDL_Rect> &rects )
{
    //Move the dot left or right
    x += xVel;

    //Move the collision boxes
    shift_boxes();

    //If the dot went too far to the left or right or has collided with the other dot
    if( ( x < 0 ) || ( x + DOT_WIDTH > SCREEN_WIDTH ) || ( check_collision( box, rects ) ) )
    {
        //Move back
        x -= xVel;
        shift_boxes();
    }

    //Move the dot up or down
    y += yVel;

    //Move the collision boxes
    shift_boxes();

    //If the dot went too far up or down or has collided with the other dot
    if( ( y < 0 ) || ( y + DOT_HEIGHT > SCREEN_HEIGHT ) || ( check_collision( box, rects ) ) )
    {
        //Move back
        y -= yVel;
        shift_boxes();
    }
}

void Dot::show()
{
    //Show the dot
    apply_surface( x, y, dot, screen );
}

std::vector<SDL_Rect> &Dot::get_rects()
{
    //Retrieve the collision boxes
    return box;
}

Timer::Timer()
{
    //Initialize the variables
    startTicks = 0;
    pausedTicks = 0;
    paused = false;
    started = false;
}

void Timer::start()
{
    //Start the timer
    started = true;

    //Unpause the timer
    paused = false;

    //Get the current clock time
    startTicks = SDL_GetTicks();
}

void Timer::stop()
{
    //Stop the timer
    started = false;

    //Unpause the timer
    paused = false;
}

void Timer::pause()
{
    //If the timer is running and isn't already paused
    if( ( started == true ) && ( paused == false ) )
    {
        //Pause the timer
        paused = true;

        //Calculate the paused ticks
        pausedTicks = SDL_GetTicks() - startTicks;
    }
}

void Timer::unpause()
{
    //If the timer is paused
    if( paused == true )
    {
        //Unpause the timer
        paused = false;

        //Reset the starting ticks
        startTicks = SDL_GetTicks() - pausedTicks;

        //Reset the paused ticks
        pausedTicks = 0;
    }
}

int Timer::get_ticks()
{
    //If the timer is running
    if( started == true )
    {
        //If the timer is paused
        if( paused == true )
        {
            //Return the number of ticks when the timer was paused
            return pausedTicks;
        }
        else
        {
            //Return the current time minus the start time
            return SDL_GetTicks() - startTicks;
        }
    }

    //If the timer isn't running
    return 0;
}

bool Timer::is_started()
{
    return started;
}

bool Timer::is_paused()
{
    return paused;
}

int main( int argc, char* args[] )
{
    //Quit flag
    bool quit = false;

    //Make the dots
    Dot myDot( 0, 0 ), otherDot( 20, 20 );

    //The frame rate regulator
    Timer fps;

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

    //While the user hasn't quit
    while( quit == false )
    {
        //Start the frame timer
        fps.start();

        //While there's events to handle
        while( SDL_PollEvent( &event ) )
        {
            //Handle events for the dot
            myDot.handle_input();

            //If the user has Xed out the window
            if( event.type == SDL_QUIT )
            {
                //Quit the program
                quit = true;
            }
        }

        //Move the dot
        myDot.move( otherDot.get_rects() );

        //Fill the screen white
        SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF ) );

        //Show the dots on the screen
        otherDot.show();
        myDot.show();

        //Update the screen
        if( SDL_Flip( screen ) == -1 )
        {
            return 1;
        }

        //Cap the frame rate
        if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
        {
            SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
        }
    }

    //Clean up
    clean_up();

    return 0;
}

/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/

//The headers
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <vector>
#include <cmath>

//Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//The frame rate
const int FRAMES_PER_SECOND = 20;

//The dot attributes
const int DOT_WIDTH = 20;

//The surfaces
SDL_Surface *dot = NULL;
SDL_Surface *screen = NULL;

//The event structure
SDL_Event event;

//A circle structure
struct Circle
{
    int x, y;
    int r;
};

//The dot
class Dot
{
    private:
    //The area of the dot
    Circle c;

    //The velocity of the dot
    int xVel, yVel;

    public:
    //Initializes the variables
    Dot();

    //Takes key presses and adjusts the dot's velocity
    void handle_input();

    //Moves the dot
    void move( std::vector<SDL_Rect> &rects, Circle &circle );

    //Shows the dot on the screen
    void show();
};

//The timer class
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

double distance( int x1, int y1, int x2, int y2 )
{
    //Return the distance between the two points
    return sqrt( pow( x2 - x1, 2 ) + pow( y2 - y1, 2 ) );
}

bool check_collision( Circle &A, Circle &B )
{
    //If the distance between the centers of the circles is less than the sum of their radii
    if( distance( A.x, A.y, B.x, B.y ) < ( A.r + B.r ) )
    {
        //The circles have collided
        return true;
    }

    //If not
    return false;
}

bool check_collision( Circle &A, std::vector<SDL_Rect> &B )
{
    //Closest point on collision box
    int cX, cY;

    //Go through the B boxes
    for( int Bbox = 0; Bbox < B.size(); Bbox++ )
    {
        //Find closest x offset
        if( A.x < B[ Bbox ].x )
        {
            cX = B[ Bbox ].x;
        }
        else if( A.x > B[ Bbox ].x + B[ Bbox ].w )
        {
            cX = B[ Bbox ].x + B[ Bbox ].w;
        }
        else
        {
            cX = A.x;
        }

        //Find closest y offset
        if( A.y < B[ Bbox ].y )
        {
            cY = B[ Bbox ].y;
        }
        else if( A.y > B[ Bbox ].y + B[ Bbox ].h )
        {
            cY = B[ Bbox ].y + B[ Bbox ].h;
        }
        else
        {
            cY = A.y;
        }

        //If the closest point is inside the circle
        if( distance( A.x, A.y, cX, cY ) < A.r )
        {
            //This box and the circle have collided
            return true;
        }
    }

    //If the shapes have not collided
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

Dot::Dot()
{
    //Initialize the offsets and dimentions
    c.x = DOT_WIDTH / 2;
    c.y = DOT_WIDTH / 2;
    c.r = DOT_WIDTH / 2;

    //Initialize the velocity
    xVel = 0;
    yVel = 0;
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

void Dot::move( std::vector<SDL_Rect> &rects, Circle &circle )
{
    //Move the dot left or right
    c.x += xVel;

    //If the dot went too far to the left or right or has collided with the other shapes
    if( ( c.x - DOT_WIDTH / 2 < 0 ) || ( c.x + DOT_WIDTH / 2 > SCREEN_WIDTH ) || ( check_collision( c, rects ) ) || ( check_collision( c, circle ) ) )
    {
        //Move back
        c.x -= xVel;
    }

    //Move the dot up or down
    c.y += yVel;

    //If the dot went too far up or down or has collided with the other shapes
    if( ( c.y - DOT_WIDTH / 2 < 0 ) || ( c.y + DOT_WIDTH / 2 > SCREEN_HEIGHT ) || ( check_collision( c, rects ) ) || ( check_collision( c, circle ) ) )
    {
        //Move back
        c.y -= yVel;
    }
}

void Dot::show()
{
    //Show the dot
    apply_surface( c.x - c.r, c.y - c.r, dot, screen );
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

    //Make the dot
    Dot myDot;

    //Make the shapes
    std::vector<SDL_Rect> box( 1 );
    Circle otherDot;

    //Set the shapes' attributes
    box[ 0 ].x = 60;
    box[ 0 ].y = 60;
    box[ 0 ].w = 40;
    box[ 0 ].h = 40;

    otherDot.x = 30;
    otherDot.y = 30;
    otherDot.r = DOT_WIDTH / 2;

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
        myDot.move( box, otherDot );

        //Fill the screen white
        SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF ) );

        //Show the box
        SDL_FillRect( screen, &box[ 0 ], SDL_MapRGB( screen->format, 0x00, 0x00, 0x00 ) );

        //Show the other dot
        apply_surface( otherDot.x - otherDot.r, otherDot.y - otherDot.r, dot, screen );

        //Show our dot
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

/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/

//The headers
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <fstream>

//Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//The frame rate
const int FRAMES_PER_SECOND = 20;

//The dot dimensions
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
    //The X and Y offsets of the dot
    int x, y;

    //The velocity of the dot
    int xVel, yVel;

    public:
    //Initializes the variables
    Dot();

    //Takes key presses and adjusts the dot's velocity
    void handle_input();

    //Moves the dot
    void move();

    //Shows the dot on the screen
    void show();

    //Set the dot's x/y offsets
    void set_x( int X );
    void set_y( int Y );

    //Get the dot's x/y offsets
    int get_x();
    int get_y();
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

bool load_files( Dot &thisDot, Uint32 &bg )
{
    //Load the dot image
    dot = load_image( "dot.png" );

    //If there was a problem in loading the dot
    if( dot == NULL )
    {
        return false;
    }

    //Open a file for reading
    std::ifstream load( "game_save" );

    //If the file loaded
    if( load != NULL )
    {
        //The offset
        int offset;

        //The level name
        std::string level;

        //Set the x offset
        load >> offset;
        thisDot.set_x( offset );

        //Set the y offset
        load >> offset;
        thisDot.set_y( offset );

        //If the x offset is invalid
        if( ( thisDot.get_x() < 0 ) || ( thisDot.get_x() > SCREEN_WIDTH - DOT_WIDTH ) )
        {
            return false;
        }

        //If the y offset is invalid
        if( ( thisDot.get_y() < 0 ) || ( thisDot.get_y() > SCREEN_HEIGHT - DOT_HEIGHT ) )
        {
            return false;
        }

        //Skip past the end of the line
        load.ignore();

        //Get the next line
        getline( load, level );

        //If an error occurred while trying to read the data
        if( load.fail() == true )
        {
            return false;
        }

        //If the level was white
        if( level == "White Level" )
        {
            //Set the background color
            bg = SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF );
        }
        //If the level was red
        else if( level == "Red Level" )
        {
            //Set the background color
            bg = SDL_MapRGB( screen->format, 0xFF, 0x00, 0x00 );
        }
        //If the level was green
        else if( level == "Green Level" )
        {
            //Set the background color
            bg = SDL_MapRGB( screen->format, 0x00, 0xFF, 0x00 );
        }
        //If the level was blue
        else if( level == "Blue Level" )
        {
            //Set the background color
            bg = SDL_MapRGB( screen->format, 0x00, 0x00, 0xFF );
        }

        //Close the file
        load.close();
    }

    //If everything loaded fine
    return true;
}

void clean_up( Dot &thisDot, Uint32 &bg )
{
    //Free the surface
    SDL_FreeSurface( dot );

    //Open a file for writing
    std::ofstream save( "game_save" );

    //Write offsets to the file
    save << thisDot.get_x();
    save << " ";
    save << thisDot.get_y();
    save << "\n";

    //The RGB values from the background
    Uint8 r, g, b;

    //Get RGB values from the background color
    SDL_GetRGB( bg, screen->format, &r, &g, &b );

    //If the background was white
    if( ( r == 0xFF ) && ( g == 0xFF ) && ( b == 0xFF ) )
    {
        //Write level type to the file
        save << "White Level";
    }
    //If the background was red
    else if( r == 0xFF )
    {
        //Write level type to the file
        save << "Red Level";
    }
    //If the background was green
    else if( g == 0xFF )
    {
        //Write level type to the file
        save << "Green Level";
    }
    //If the background was blue
    else if( b == 0xFF )
    {
        //Write level type to the file
        save << "Blue Level";
    }

    //Close the file
    save.close();

    //Quit SDL
    SDL_Quit();
}

Dot::Dot()
{
    //Initialize the offsets
    x = 0;
    y = 0;

    //Initialize the velocity
    xVel = 0;
    yVel = 0;
}

void Dot::set_x( int X )
{
    x = X;
}

void Dot::set_y( int Y )
{
    y = Y;
}

int Dot::get_x()
{
    return x;
}

int Dot::get_y()
{
    return y;
}

void Dot::handle_input()
{
    //If a key was pressed
    if( event.type == SDL_KEYDOWN )
    {
        //Adjust the velocity
        switch( event.key.keysym.sym )
        {
            case SDLK_UP: yVel -= DOT_HEIGHT / 2; break;
            case SDLK_DOWN: yVel += DOT_HEIGHT / 2; break;
            case SDLK_LEFT: xVel -= DOT_WIDTH / 2; break;
            case SDLK_RIGHT: xVel += DOT_WIDTH / 2; break;
        }
    }
    //If a key was released
    else if( event.type == SDL_KEYUP )
    {
        //Adjust the velocity
        switch( event.key.keysym.sym )
        {
            case SDLK_UP: yVel += DOT_HEIGHT / 2; break;
            case SDLK_DOWN: yVel -= DOT_HEIGHT / 2; break;
            case SDLK_LEFT: xVel += DOT_WIDTH / 2; break;
            case SDLK_RIGHT: xVel -= DOT_WIDTH / 2; break;
        }
    }
}

void Dot::move()
{
    //Move the dot left or right
    x += xVel;

    //If the dot went too far to the left or right
    if( ( x < 0 ) || ( x + DOT_WIDTH > SCREEN_WIDTH ) )
    {
        //Move back
        x -= xVel;
    }

    //Move the dot up or down
    y += yVel;

    //If the dot went too far up or down
    if( ( y < 0 ) || ( y + DOT_HEIGHT > SCREEN_HEIGHT ) )
    {
        //Move back
        y -= yVel;
    }
}

void Dot::show()
{
    //Show the dot
    apply_surface( x, y, dot, screen );
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

    //Initialize
    if( init() == false )
    {
        return 1;
    }

    //The dot
    Dot myDot;

    //The background color
    Uint32 background = SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF );

    //The frame rate regulator
    Timer fps;

    //Load the files
    if( load_files( myDot, background ) == false )
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

            //If the user has pressed at key
            if( event.type == SDL_KEYDOWN )
            {
                //Change background according to key press
                switch( event.key.keysym.sym )
                {
                    case SDLK_1: background = SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF ); break;
                    case SDLK_2: background = SDL_MapRGB( screen->format, 0xFF, 0x00, 0x00 ); break;
                    case SDLK_3: background = SDL_MapRGB( screen->format, 0x00, 0xFF, 0x00 ); break;
                    case SDLK_4: background = SDL_MapRGB( screen->format, 0x00, 0x00, 0xFF ); break;
                }
            }

            //If the user has Xed out the window
            if( event.type == SDL_QUIT )
            {
                //Quit the program
                quit = true;
            }
        }

        //Move the dot
        myDot.move();

        //Fill the background
        SDL_FillRect( screen, &screen->clip_rect, background );

        //Show the dot on the screen
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

    //Clean up and save
    clean_up( myDot, background );

    return 0;
}

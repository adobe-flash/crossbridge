/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/

//The headers
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include <string>

//Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//The surfaces
SDL_Surface *background = NULL;
SDL_Surface *message = NULL;
SDL_Surface *screen = NULL;

//The event structure
SDL_Event event;

//The font
TTF_Font *font = NULL;

//The color of the font
SDL_Color textColor = { 0, 0, 0 };

//The music that will be played
Mix_Music *music = NULL;

//The sound effects that will be used
Mix_Chunk *scratch = NULL;
Mix_Chunk *high = NULL;
Mix_Chunk *med = NULL;
Mix_Chunk *low = NULL;

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

    //Initialize SDL_ttf
    if( TTF_Init() == -1 )
    {
        return false;
    }

    //Initialize SDL_mixer
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
    {
        return false;
    }

    //Set the window caption
    SDL_WM_SetCaption( "Monitor Music", NULL );

    //If everything initialized fine
    return true;
}

bool load_files()
{
    //Load the background image
    background = load_image( "background.png" );

    //Open the font
    font = TTF_OpenFont( "lazy.ttf", 30 );

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

    //Load the music
    music = Mix_LoadMUS( "beat.wav" );

    //If there was a problem loading the music
    if( music == NULL )
    {
        return false;
    }

    //Load the sound effects
    scratch = Mix_LoadWAV( "scratch.wav" );
    high = Mix_LoadWAV( "high.wav" );
    med = Mix_LoadWAV( "medium.wav" );
    low = Mix_LoadWAV( "low.wav" );

    //If there was a problem loading the sound effects
    if( ( scratch == NULL ) || ( high == NULL ) || ( med == NULL ) || ( low == NULL ) )
    {
        return false;
    }

    //If everything loaded fine
    return true;
}

void clean_up()
{
    //Free the surfaces
    SDL_FreeSurface( background );

    //Free the sound effects
    Mix_FreeChunk( scratch );
    Mix_FreeChunk( high );
    Mix_FreeChunk( med );
    Mix_FreeChunk( low );

    //Free the music
    Mix_FreeMusic( music );

    //Close the font
    TTF_CloseFont( font );

    //Quit SDL_mixer
    Mix_CloseAudio();

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

    //Apply the background
    apply_surface( 0, 0, background, screen );

    //Render the text
    message = TTF_RenderText_Solid( font, "Press 1, 2, 3, or 4 to play a sound effect", textColor );

    //If there was an error in rendering the text
    if( message == NULL )
    {
        return 1;
    }

    //Show the message on the screen
    apply_surface( ( SCREEN_WIDTH - message->w ) / 2, 100, message, screen );

    //Free the message
    SDL_FreeSurface( message );

    //Render the text
    message = TTF_RenderText_Solid( font, "Press 9 to play or pause the music", textColor );

    //If there was an error in rendering the text
    if( message == NULL )
    {
        return 1;
    }

    //Show the message on the screen
    apply_surface( ( SCREEN_WIDTH - message->w ) / 2, 200, message, screen );

    //Free the message
    SDL_FreeSurface( message );

    //Render the text
    message = TTF_RenderText_Solid( font, "Press 0 to stop the music", textColor );

    //If there was an error in rendering the text
    if( message == NULL )
    {
        return 1;
    }

    //Show the message on the screen
    apply_surface( ( SCREEN_WIDTH - message->w ) / 2, 300, message, screen );

    //Free the message
    SDL_FreeSurface( message );

    //Update the screen
    if( SDL_Flip( screen ) == -1 )
    {
        return 1;
    }

    //While the user hasn't quit
    while( quit == false )
    {
        //While there's events to handle
        while( SDL_PollEvent( &event ) )
        {
            //If a key was pressed
            if( event.type == SDL_KEYDOWN )
            {
                //If 1 was pressed
                if( event.key.keysym.sym == SDLK_1 )
                {
                    //Play the scratch effect
                    if( Mix_PlayChannel( -1, scratch, 0 ) == -1 )
                    {
                        return 1;
                    }
                }
                //If 2 was pressed
                else if( event.key.keysym.sym == SDLK_2 )
                {
                    //Play the high hit effect
                    if( Mix_PlayChannel( -1, high, 0 ) == -1 )
                    {
                        return 1;
                    }
                }
                //If 3 was pressed
                else if( event.key.keysym.sym == SDLK_3 )
                {
                    //Play the medium hit effect
                    if( Mix_PlayChannel( -1, med, 0 ) == -1 )
                    {
                        return 1;
                    }
                }
                //If 4 was pressed
                else if( event.key.keysym.sym == SDLK_4 )
                {
                    //Play the low hit effect
                    if( Mix_PlayChannel( -1, low, 0 ) == -1 )
                    {
                        return 1;
                    }
                }
                //If 9 was pressed
                else if( event.key.keysym.sym == SDLK_9 )
                {
                    //If there is no music playing
                    if( Mix_PlayingMusic() == 0 )
                    {
                        //Play the music
                        if( Mix_PlayMusic( music, -1 ) == -1 )
                        {
                            return 1;
                        }
                    }
                    //If music is being played
                    else
                    {
                        //If the music is paused
                        if( Mix_PausedMusic() == 1 )
                        {
                            //Resume the music
                            Mix_ResumeMusic();
                        }
                        //If the music is playing
                        else
                        {
                            //Pause the music
                            Mix_PauseMusic();
                        }
                    }
                }
                //If 0 was pressed
                else if( event.key.keysym.sym == SDLK_0 )
                {
                    //Stop the music
                    Mix_HaltMusic();
                }
            }
            //If the user has Xed out the window
            if( event.type == SDL_QUIT )
            {
                //Quit the program
                quit = true;
            }
        }
    }

    //Free surfaces, fonts and sounds
    //then quit SDL_mixer, SDL_ttf and SDL
    clean_up();

    return 0;
}

/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/

//The headers
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>

//Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//The surfaces
SDL_Surface *bitmapFont = NULL;
SDL_Surface *screen = NULL;

//The event structure
SDL_Event event;

//Our bitmap font
class BitmapFont
{
    private:
    //The font surface
    SDL_Surface *bitmap;

    //The individual characters in the surface
    SDL_Rect chars[ 256 ];

    //Spacing Variables
    int newLine, space;

    public:
    //The default constructor
    BitmapFont();

    //Generates the font when the object is made
    BitmapFont( SDL_Surface *surface );

    //Generates the font
    void build_font( SDL_Surface *surface );

    //Shows the text
    void show_text( int x, int y, std::string text, SDL_Surface *surface );
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

Uint32 get_pixel32( int x, int y, SDL_Surface *surface )
{
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32 *)surface->pixels;

    //Get the pixel requested
    return pixels[ ( y * surface->w ) + x ];
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
    SDL_WM_SetCaption( "Bitmap font test", NULL );

    //If everything initialized fine
    return true;
}

bool load_files()
{
    //Load the font
    bitmapFont = load_image( "lazyfont.png" );

    //If there was an error in loading the font
    if( bitmapFont == NULL )
    {
        return false;
    }

    //If everything loaded fine
    return true;
}

void clean_up()
{
    //Free the font
    SDL_FreeSurface( bitmapFont );

    //Quit SDL
    SDL_Quit();
}

BitmapFont::BitmapFont()
{
    //Initialize variables
    bitmap = NULL;
    newLine = 0;
    space = 0;
}

BitmapFont::BitmapFont( SDL_Surface *surface )
{
    //Build the font
    build_font( surface );
}

void BitmapFont::build_font( SDL_Surface *surface )
{
    //If surface is NULL
    if( surface == NULL )
    {
        return;
    }

    //Get the bitmap
    bitmap = surface;

    //Set the background color
    Uint32 bgColor = SDL_MapRGB( bitmap->format, 0, 0xFF, 0xFF );

    //Set the cell dimensions
    int cellW = bitmap->w / 16;
    int cellH = bitmap->h / 16;

    //New line variables
    int top = cellH;
    int baseA = cellH;

    //The current character we're setting
    int currentChar = 0;

    //Go through the cell rows
    for( int rows = 0; rows < 16; rows++ )
    {
        //Go through the cell columns
        for( int cols = 0; cols < 16; cols++ )
        {
            //Set the character offset
            chars[ currentChar ].x = cellW * cols;
            chars[ currentChar ].y = cellH * rows;

            //Set the dimensions of the character
            chars[ currentChar ].w = cellW;
            chars[ currentChar ].h = cellH;

            //Find Left Side
            //Go through pixel columns
            for( int pCol = 0; pCol < cellW; pCol++ )
            {
                //Go through pixel rows
                for( int pRow = 0; pRow < cellH; pRow++ )
                {
                    //Get the pixel offsets
                    int pX = ( cellW * cols ) + pCol;
                    int pY = ( cellH * rows ) + pRow;

                    //If a non colorkey pixel is found
                    if( get_pixel32( pX, pY, bitmap ) != bgColor )
                    {
                        //Set the x offset
                        chars[ currentChar ].x = pX;

                        //Break the loops
                        pCol = cellW;
                        pRow = cellH;
                    }
                }
            }

            //Find Right Side
            //Go through pixel columns
            for( int pCol_w = cellW - 1; pCol_w >= 0; pCol_w-- )
            {
                //Go through pixel rows
                for( int pRow_w = 0; pRow_w < cellH; pRow_w++ )
                {
                    //Get the pixel offsets
                    int pX = ( cellW * cols ) + pCol_w;
                    int pY = ( cellH * rows ) + pRow_w;

                    //If a non colorkey pixel is found
                    if( get_pixel32( pX, pY, bitmap ) != bgColor )
                    {
                        //Set the width
                        chars[ currentChar ].w = ( pX - chars[ currentChar ].x ) + 1;

                        //Break the loops
                        pCol_w = -1;
                        pRow_w = cellH;
                    }
                }
            }

            //Find Top
            //Go through pixel rows
            for( int pRow = 0; pRow < cellH; pRow++ )
            {
                //Go through pixel columns
                for( int pCol = 0; pCol < cellW; pCol++ )
                {
                    //Get the pixel offsets
                    int pX = ( cellW * cols ) + pCol;
                    int pY = ( cellH * rows ) + pRow;

                    //If a non colorkey pixel is found
                    if( get_pixel32( pX, pY, bitmap ) != bgColor )
                    {
                        //If new top is found
                        if( pRow < top )
                        {
                            top = pRow;
                        }

                        //Break the loops
                        pCol = cellW;
                        pRow = cellH;
                    }
                }
            }

            //Find Bottom of A
            if( currentChar == 'A' )
            {
                //Go through pixel rows
                for( int pRow = cellH - 1; pRow >= 0; pRow-- )
                {
                    //Go through pixel columns
                    for( int pCol = 0; pCol < cellW; pCol++ )
                    {
                        //Get the pixel offsets
                        int pX = ( cellW * cols ) + pCol;
                        int pY = ( cellH * rows ) + pRow;

                        //If a non colorkey pixel is found
                        if( get_pixel32( pX, pY, bitmap ) != bgColor )
                        {
                            //Bottom of a is found
                            baseA = pRow;

                            //Break the loops
                            pCol = cellW;
                            pRow = -1;
                        }
                    }
                }
            }

            //Go to the next character
            currentChar++;
        }
    }

    //Calculate space
    space = cellW / 2;

    //Calculate new line
    newLine = baseA - top;

    //Lop off excess top pixels
    for( int t = 0; t < 256; t++ )
    {
        chars[ t ].y += top;
        chars[ t ].h -= top;
    }
}

void BitmapFont::show_text( int x, int y, std::string text, SDL_Surface *surface )
{
    //Temp offsets
    int X = x, Y = y;

    //If the font has been built
    if( bitmap != NULL )
    {
        //Go through the text
        for( int show = 0; show < text.length(); show++ )
        {
            //If the current character is a space
            if( text[ show ] == ' ' )
            {
                //Move over
                X += space;
            }
            //If the current character is a newline
            else if( text[ show ] == '\n' )
            {
                //Move down
                Y += newLine;

                //Move back
                X = x;
            }
            else
            {
                //Get the ASCII value of the character
                int ascii = (unsigned char)text[ show ];

                //Show the character
                apply_surface( X, Y, bitmap, surface, &chars[ ascii ] );

                //Move over the width of the character with one pixel of padding
                X += chars[ ascii ].w + 1;
            }
        }
    }
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

    //Create our font
    BitmapFont font( bitmapFont );

    //Fill the screen white
    SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF ) );

    //Show the text
    font.show_text( 100, 100, "Bitmap Font:\nABDCEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789", screen );

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

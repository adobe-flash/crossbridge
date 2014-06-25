/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 003

#include "LFont.h"
#include FT_BITMAP_H

//LFont library
FT_Library LFont::mLibrary;

bool LFont::initFreeType()
{
    //Init FreeType for single threaded applications
    #ifndef __FREEGLUT_H__
        FT_Error error = FT_Init_FreeType( &mLibrary );
        if( error )
        {
            printf( "FreeType error:%X", error );
            return false;
        }
    #endif

    return true;
}

LFont::LFont()
{
    mSpace = 0.f;
    mLineHeight = 0.f;
    mNewLine = 0.f;
}

LFont::~LFont()
{
    //Deallocate font
    freeFont();
}

bool LFont::loadBitmap( std::string path )
{
    //Loading flag
    bool success = true;

    //Background pixel
    const GLubyte BLACK_PIXEL = 0x00;

    //Get rid of the font if it exists
    freeFont();

	//Image pixels loaded
    if( loadPixelsFromFile8( path ) )
    {
		//Get cell dimensions
		GLfloat cellW = imageWidth() / 16.f;
		GLfloat cellH = imageHeight() / 16.f;

		//Get letter top and bottom
		GLuint top = cellH;
		GLuint bottom = 0;
		GLuint aBottom = 0;

        //Current pixel coordinates
        int pX = 0;
        int pY = 0;

        //Base cell offsets
        int bX = 0;
        int bY = 0;

        //Begin parsing bitmap font
		GLuint currentChar = 0;
		LFRect nextClip = { 0.f, 0.f, cellW, cellH };

		//Go through cell rows
		for( unsigned int rows = 0; rows < 16; ++rows )
		{
		    //Go through each cell column in the row
			for( unsigned int cols = 0; cols < 16; ++cols )
			{
			    //Begin cell parsing

				//Set base offsets
				bX = cellW * cols;
				bY = cellH * rows;

				//Initialize clip
				nextClip.x = cellW * cols;
				nextClip.y = cellH * rows;

				nextClip.w = cellW;
				nextClip.h = cellH;

				//Find left side of character
				for( int pCol = 0; pCol < cellW; ++pCol )
				{
					for( int pRow = 0; pRow < cellH; ++pRow )
					{
					    //Set pixel offset
						pX = bX + pCol;
						pY = bY + pRow;

                        //Non-background pixel found
						if( getPixel8( pX, pY ) != BLACK_PIXEL )
						{
						    //Set sprite's x offset
							nextClip.x = pX;

							//Break the loops
							pCol = cellW;
							pRow = cellH;
						}
					}
				}

				//Right side
				for( int pCol_w = cellW - 1; pCol_w >= 0; pCol_w-- )
				{
					for( int pRow_w = 0; pRow_w < cellH; pRow_w++ )
					{
					    //Set pixel offset
						pX = bX + pCol_w;
						pY = bY + pRow_w;

                        //Non-background pixel found
						if( getPixel8( pX, pY ) != BLACK_PIXEL )
						{
						    //Set sprite's width
							nextClip.w = ( pX - nextClip.x ) + 1;

							//Break the loops
							pCol_w = -1;
							pRow_w = cellH;
						}
					}
				}

				//Find Top
				for( int pRow = 0; pRow < cellH; ++pRow )
				{
					for( int pCol = 0; pCol < cellW; ++pCol )
					{
					    //Set pixel offset
						pX = bX + pCol;
						pY = bY + pRow;

						//Non-background pixel found
						if( getPixel8( pX, pY ) != BLACK_PIXEL )
						{
							//New Top Found
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

				//Find Bottom
				for( int pRow_b = cellH - 1; pRow_b >= 0; --pRow_b )
				{
					for( int pCol_b = 0; pCol_b < cellW; ++pCol_b )
					{
					    //Set pixel offset
						pX = bX + pCol_b;
						pY = bY + pRow_b;

                        //Non-background pixel found
						if( getPixel8( pX, pY ) != BLACK_PIXEL )
						{
							//Set BaseLine
							if( currentChar == 'A' )
							{
								aBottom = pRow_b;
							}

							//New bottom Found
							if( pRow_b > bottom )
							{
								bottom = pRow_b;
							}

							//Break the loops
							pCol_b = cellW;
							pRow_b = -1;
						}
					}
				}

				//Go to the next character
				mClips.push_back( nextClip );
				++currentChar;
			}
		}

		//Set Top
		for( int t = 0; t < 256; ++t )
		{
			mClips[ t ].y += top;
			mClips[ t ].h -= top;
		}

        //Create texture from parsed pixels
        if( loadTextureFromPixels8() )
        {
            //Build vertex buffer from sprite sheet data
            if( !generateDataBuffer( LSPRITE_ORIGIN_TOP_LEFT ) )
            {
                printf( "Unable to create vertex buffer for bitmap font!\n" );
                success = false;
            }
        }
        else
        {
            printf( "Unable to create texture from bitmap font pixels!\n" );
            success = false;
        }

        //Set font texture wrap
        glBindTexture( GL_TEXTURE_2D, getTextureID() );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );

        //Set spacing variables
		mSpace = cellW / 2;
		mNewLine = aBottom - top;
		mLineHeight = bottom - top;
    }
    else
    {
		printf( "Could not load bitmap font image: %s!\n", path.c_str() );
        success = false;
    }

    return success;
}

bool LFont::loadFreeType( std::string path, GLuint pixelSize )
{
    //Error flag
    FT_Error error = NULL;

    //Init FreeType for multithread applications
    #ifdef __FREEGLUT_H__
        error = FT_Init_FreeType( &mLibrary );
        if( error )
        {
            printf( "FreeType error:%X", error );
            return false;
        }
    #endif

    //Get cell dimensions
    GLuint cellW = 0;
    GLuint cellH = 0;
    int maxBearing = 0;
    int minHang = 0;

    //Character data
    LTexture bitmaps[ 256 ];
    FT_Glyph_Metrics metrics[ 256 ];

    //Load face
    FT_Face face = NULL;
    error = FT_New_Face( mLibrary, path.c_str(), 0, &face );
    if( !error )
    {
        //Set face size
        error = FT_Set_Pixel_Sizes( face, 0, pixelSize );
        if( !error )
        {
            //Go through extended ASCII to get glyph data
            for( int i = 0; i < 256; ++i )
            {
                //Load and render glyph
                error = FT_Load_Char( face, i, FT_LOAD_RENDER );
                if( !error )
                {
                    //Get metrics
                    metrics[ i ] = face->glyph->metrics;

                    //Copy glyph bitmap
                    bitmaps[ i ].copyPixels8( face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows );

                    //Calculate max bearing
                    if( metrics[ i ].horiBearingY / 64 > maxBearing )
                    {
                        maxBearing = metrics[ i ].horiBearingY / 64;
                    }

                    //Calculate max width
                    if( metrics[ i ].width / 64 > cellW )
                    {
                        cellW = metrics[ i ].width / 64;
                    }

                    //Calculate gylph hang
                    int glyphHang = ( metrics[ i ].horiBearingY - metrics[ i ].height ) / 64;
                    if( glyphHang < minHang )
                    {
                        minHang = glyphHang;
                    }
                }
                else
                {
                    printf( "Unable to load glyph. FreeType error:%X\n", error );
                    error = NULL;
                }
            }



            //Create bitmap font
            cellH = maxBearing - minHang;
            createPixels8( cellW * 16, cellH * 16 );

            //Begin creating bitmap font
            GLuint currentChar = 0;
            LFRect nextClip = { 0.f, 0.f, cellW, cellH };

            //Blitting coordinates
            int bX = 0;
            int bY = 0;

            //Go through cell rows
            for( unsigned int rows = 0; rows < 16; rows++ )
            {
                //Go through each cell column in the row
                for( unsigned int cols = 0; cols < 16; cols++ )
                {
                    //Set base offsets
                    bX = cellW * cols;
                    bY = cellH * rows;

                    //Initialize clip
                    nextClip.x = bX;
                    nextClip.y = bY;
                    nextClip.w = metrics[ currentChar ].width / 64;
                    nextClip.h = cellH;

                    //Blit character
                    bitmaps[ currentChar ].blitPixels8( bX, bY + maxBearing - metrics[ currentChar ].horiBearingY / 64, *this );

                    //Go to the next character
                    mClips.push_back( nextClip );
                    currentChar++;
                }
            }

            //Make texture power of two
            padPixels8();

            //Create texture
            if( loadTextureFromPixels8() )
            {
                //Build vertex buffer from sprite sheet data
                if( !generateDataBuffer( LSPRITE_ORIGIN_TOP_LEFT ) )
                {
                    printf( "Unable to create vertex buffer for bitmap font!" );
                    error = 0xA2;
                }
            }
            else
            {
                printf( "Unable to create texture from generated bitmap font!\n" );
                error = 0xA2;
            }

            //Set font texture wrap
            glBindTexture( GL_TEXTURE_2D, getTextureID() );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );

            //Set spacing variables
            mSpace = cellW / 2;
            mNewLine = maxBearing;
            mLineHeight = cellH;
        }
        else
        {
            printf( "Unable to set font size. FreeType error:%X\n", error );
        }

        //Free face
        FT_Done_Face( face );
    }
    else
    {
        printf( "Unable to load load font face. FreeType error:%X\n", error );
        return false;
    }

    //Close FreeType for multithreaded applications
    #ifdef __FREEGLUT_H__
        FT_Done_FreeType( mLibrary );
    #endif

    return error == NULL;
}

void LFont::freeFont()
{
    //Get rid of sprite sheet
    freeTexture();

    //Reinitialize spacing constants
    mSpace = 0.f;
    mLineHeight = 0.f;
    mNewLine = 0.f;
}

void LFont::renderText( GLfloat x, GLfloat y, std::string text )
{
    //If there is a texture to render from
    if( getTextureID() != 0 )
    {
        //Draw positions
        GLfloat dX = x;
        GLfloat dY = y;

        //Move to draw position
        glTranslatef( x, y, 0.f );

        //Set texture
        glBindTexture( GL_TEXTURE_2D, getTextureID() );

        //Enable vertex and texture coordinate arrays
        glEnableClientState( GL_VERTEX_ARRAY );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );

        //Bind vertex data
        glBindBuffer( GL_ARRAY_BUFFER, mVertexDataBuffer );

        //Set texture coordinate data
        glTexCoordPointer( 2, GL_FLOAT, sizeof(LVertexData2D), (GLvoid*) offsetof( LVertexData2D, texCoord ) );

        //Set vertex data
        glVertexPointer( 2, GL_FLOAT, sizeof(LVertexData2D), (GLvoid*) offsetof( LVertexData2D, position ) );

            //Go through string
            for( int i = 0; i < text.length(); ++i )
            {
                //Space
                if( text[ i ] == ' ' )
                {
                    glTranslatef( mSpace, 0.f, 0.f );
                    dX += mSpace;
                }
                //Newline
                else if( text[ i ] == '\n' )
                {
                    glTranslatef( x - dX, mNewLine, 0.f );
                    dY += mNewLine;
                    dX += x - dX;
                }
                //Character
                else
                {
                    //Get ASCII
                    GLuint ascii = (unsigned char)text[ i ];

                    //Draw quad using vertex data and index data
                    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIndexBuffers[ ascii ] );
                    glDrawElements( GL_QUADS, 4, GL_UNSIGNED_INT, NULL );

                    //Move over
                    glTranslatef( mClips[ ascii ].w, 0.f, 0.f );
                    dX += mClips[ ascii ].w;
                }
            }

        //Disable vertex and texture coordinate arrays
        glDisableClientState( GL_TEXTURE_COORD_ARRAY );
        glDisableClientState( GL_VERTEX_ARRAY );
    }
}

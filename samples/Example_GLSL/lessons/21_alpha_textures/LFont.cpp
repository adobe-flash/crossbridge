/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 002

#include "LFont.h"

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

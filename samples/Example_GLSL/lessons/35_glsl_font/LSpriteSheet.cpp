/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 003

#include "LSpriteSheet.h"

LSpriteSheet::LSpriteSheet()
{
	//Initialize vertex buffer data
	mVertexDataBuffer = NULL;
	mIndexBuffers = NULL;
}

LSpriteSheet::~LSpriteSheet()
{
	//Clear sprite sheet data
	freeSheet();
}

int LSpriteSheet::addClipSprite( LFRect& newClip )
{
	//Add clip and return index
	mClips.push_back( newClip );
	return mClips.size() - 1;
}

LFRect LSpriteSheet::getClip( int index )
{
	return mClips[ index ];
}

bool LSpriteSheet::generateDataBuffer( LSpriteOrigin origin )
{
	//If there is a texture loaded and clips to make vertex data from
	if( getTextureID() != 0 && mClips.size() > 0 )
	{
		//Allocate vertex data
		int totalSprites = mClips.size();
		LTexturedVertex2D* vertexData = new LTexturedVertex2D[ totalSprites * 4 ];
		mIndexBuffers = new GLuint[ totalSprites ];

		//Allocate vertex data buffer name
		glGenBuffers( 1, &mVertexDataBuffer );

		//Allocate index buffers names
		glGenBuffers( totalSprites, mIndexBuffers );

		//Go through clips
		GLfloat tW = textureWidth();
		GLfloat tH = textureHeight();
		GLuint spriteIndices[ 4 ] = { 0, 0, 0, 0 };

        //Origin variables
        GLfloat vTop = 0.f;
        GLfloat vBottom = 0.f;
        GLfloat vLeft = 0.f;
        GLfloat vRight = 0.f;

		for( int i = 0; i < totalSprites; ++i )
		{
			//Initialize indices
			spriteIndices[ 0 ] = i * 4 + 0;
			spriteIndices[ 1 ] = i * 4 + 1;
			spriteIndices[ 2 ] = i * 4 + 2;
			spriteIndices[ 3 ] = i * 4 + 3;

            //Set origin
            switch( origin )
            {
                case LSPRITE_ORIGIN_TOP_LEFT:
                    vTop = 0.f;
                    vBottom = mClips[ i ].h;
                    vLeft = 0.f;
                    vRight = mClips[ i ].w;
                    break;

                case LSPRITE_ORIGIN_TOP_RIGHT:
                    vTop = 0.f;
                    vBottom = mClips[ i ].h;
                    vLeft = -mClips[ i ].w;
                    vRight = 0.f;
                    break;

                case LSPRITE_ORIGIN_BOTTOM_LEFT:
                    vTop = -mClips[ i ].h;
                    vBottom = 0.f;
                    vLeft = 0.f;
                    vRight = mClips[ i ].w;
                    break;

                case LSPRITE_ORIGIN_BOTTOM_RIGHT:
                    vTop = -mClips[ i ].h;
                    vBottom = 0.f;
                    vLeft = -mClips[ i ].w;
                    vRight = 0.f;
                    break;

                //Also for LSPRITE_ORIGIN_CENTER
                default:
                    vTop = -mClips[ i ].h / 2.f;
                    vBottom = mClips[ i ].h / 2.f;
                    vLeft = -mClips[ i ].w / 2.f;
                    vRight = mClips[ i ].w / 2.f;
                    break;
            }

			//Top left
			vertexData[ spriteIndices[ 0 ] ].position.x = vLeft;
			vertexData[ spriteIndices[ 0 ] ].position.y = vTop;

			vertexData[ spriteIndices[ 0 ] ].texCoord.s =  (mClips[ i ].x) / tW;
			vertexData[ spriteIndices[ 0 ] ].texCoord.t =  (mClips[ i ].y) / tH;

			//Top right
			vertexData[ spriteIndices[ 1 ] ].position.x = vRight;
			vertexData[ spriteIndices[ 1 ] ].position.y = vTop;

			vertexData[ spriteIndices[ 1 ] ].texCoord.s =  (mClips[ i ].x + mClips[ i ].w) / tW;
			vertexData[ spriteIndices[ 1 ] ].texCoord.t =  (mClips[ i ].y) / tH;

			//Bottom right
			vertexData[ spriteIndices[ 2 ] ].position.x = vRight;
			vertexData[ spriteIndices[ 2 ] ].position.y = vBottom;

			vertexData[ spriteIndices[ 2 ] ].texCoord.s =  (mClips[ i ].x + mClips[ i ].w) / tW;
			vertexData[ spriteIndices[ 2 ] ].texCoord.t =  (mClips[ i ].y + mClips[ i ].h) / tH;

			//Bottom left
			vertexData[ spriteIndices[ 3 ] ].position.x = vLeft;
			vertexData[ spriteIndices[ 3 ] ].position.y = vBottom;

			vertexData[ spriteIndices[ 3 ] ].texCoord.s =  (mClips[ i ].x) / tW;
			vertexData[ spriteIndices[ 3 ] ].texCoord.t =  (mClips[ i ].y + mClips[ i ].h) / tH;

			//Bind sprite index buffer data
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIndexBuffers[ i ] );
			glBufferData( GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), spriteIndices, GL_STATIC_DRAW );
		}

		//Bind vertex data
		glBindBuffer( GL_ARRAY_BUFFER, mVertexDataBuffer );
		glBufferData( GL_ARRAY_BUFFER, totalSprites * 4 * sizeof(LTexturedVertex2D), vertexData, GL_STATIC_DRAW );

		//Deallocate vertex data
		delete[] vertexData;
	}
	//No clips
	else
	{
	    if( getTextureID() == 0 )
	    {
            printf( "No texture to render with!\n" );
	    }

        if( mClips.size() <= 0 )
	    {
            printf( "No clips to generate vertex data from!\n" );
	    }

		return false;
	}

	return true;
}

void LSpriteSheet::freeSheet()
{
	//Clear vertex buffer
	if( mVertexDataBuffer != NULL )
	{
		glDeleteBuffers( 1, &mVertexDataBuffer );
		mVertexDataBuffer = NULL;
	}

	//Clear index buffer
	if( mIndexBuffers != NULL )
	{
		glDeleteBuffers( mClips.size(), mIndexBuffers );
        delete[] mIndexBuffers;
		mIndexBuffers = NULL;
	}

	//Clear clips
	mClips.clear();
}

void LSpriteSheet::freeTexture()
{
    //Get rid of sprite sheet data
    freeSheet();

    //Free texture
    LTexture::freeTexture();
}

void LSpriteSheet::renderSprite( int index )
{
    //Sprite sheet data exists
    if( mVertexDataBuffer != NULL )
    {
        //Set texture
        glBindTexture( GL_TEXTURE_2D, getTextureID() );

        //Enable vertex and texture coordinate arrays
		mTexturedPolygonProgram2D->enableVertexPointer();
		mTexturedPolygonProgram2D->enableTexCoordPointer();

            //Bind vertex data
            glBindBuffer( GL_ARRAY_BUFFER, mVertexDataBuffer );

            //Set texture coordinate data
			mTexturedPolygonProgram2D->setTexCoordPointer( sizeof(LTexturedVertex2D), (GLvoid*)offsetof( LTexturedVertex2D, texCoord ) );

            //Set vertex data
			mTexturedPolygonProgram2D->setVertexPointer( sizeof(LTexturedVertex2D), (GLvoid*)offsetof( LTexturedVertex2D, position ) );

            //Draw quad using vertex data and index data
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIndexBuffers[ index ] );
            glDrawElements( GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL );

        //Disable vertex and texture coordinate arrays
		mTexturedPolygonProgram2D->disableVertexPointer();
		mTexturedPolygonProgram2D->disableTexCoordPointer();
    }
}

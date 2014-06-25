/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 001

#ifndef LSPRITE_SHEET_H
#define LSPRITE_SHEET_H

#include "LTexture.h"
#include "LVertexData2D.h"
#include <vector>

class LSpriteSheet : public LTexture
{
    public:
		LSpriteSheet();
        /*
        Pre Condition:
         -None
        Post Condition:
         -Initializes buffer ID
        Side Effects:
         -None
        */

		~LSpriteSheet();
        /*
        Pre Condition:
         -None
        Post Condition:
         -Deallocates sprite sheet data
        Side Effects:
         -None
        */

		int addClipSprite( LFRect& newClip );
        /*
        Pre Condition:
         -None
        Post Condition:
         -Adds clipping rectangle to clip array
         -Returns index of clipping rectangle within clip array
        Side Effects:
         -None
        */

		LFRect getClip( int index );
        /*
        Pre Condition:
         -A valid index
        Post Condition:
         -Returns clipping clipping rectangle at given index
        Side Effects:
         -None
        */

		bool generateDataBuffer();
        /*
        Pre Condition:
         -A loaded base LTexture
         -Clipping rectangles in clip array
        Post Condition:
         -Generates VBO and IBO to render sprites with
         -Returns true on success
         -Reports to console is an error occured
        Side Effects:
         -Member buffers are bound
        */

		void freeSheet();
        /*
        Pre Condition:
         -None
        Post Condition:
         -Deallocates member VBO, IBO, and clip array
        Side Effects:
         -None
        */

        void freeTexture();
        /*
        Pre Condition:
         -None
        Post Condition:
         -Frees sprite sheet and base LTexture
        Side Effects:
         -None
        */

		void renderSprite( int index );
        /*
        Pre Condition:
         -Loaded base LTexture
         -Generated VBO
        Post Condition:
         -Renders sprite at given index
        Side Effects:
         -Base LTexture is bound
         -Member buffers are bound
        */

    protected:
        //Sprite clips
		std::vector<LFRect> mClips;

		//VBO data
		GLuint mVertexDataBuffer;
		GLuint* mIndexBuffers;
};

#endif

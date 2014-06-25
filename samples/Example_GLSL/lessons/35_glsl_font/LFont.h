/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 006

#ifndef LFONT_H
#define LFONT_H

#include "LSpriteSheet.h"
#include "LFontProgram2D.h"
#include <ft2build.h>
#include FT_FREETYPE_H

enum LFontTextAlignment
{
    LFONT_TEXT_ALIGN_LEFT = 1,
    LFONT_TEXT_ALIGN_CENTERED_H = 2,
    LFONT_TEXT_ALIGN_RIGHT = 4,
    LFONT_TEXT_ALIGN_TOP = 8,
    LFONT_TEXT_ALIGN_CENTERED_V = 16,
    LFONT_TEXT_ALIGN_BOTTOM = 32
};

class LFont : private LSpriteSheet
{
	public:
		static void setFontProgram2D( LFontProgram2D* fontProgram2D );
		/*
        Pre Condition:
         -None
        Post Condition:
         -Sets class wide rendering program
        Side Effects:
         -None
        */

        static bool initFreeType();
        /*
        Pre Condition:
         -None
        Post Condition:
         -Instantiates FreeType library used for every LFont object
        Side Effects:
         -None
        */

		LFont();
        /*
        Pre Condition:
         -None
        Post Condition:
         -Initializes spacing variables
        Side Effects:
         -None
        */

		~LFont();
        /*
        Pre Condition:
         -None
        Post Condition:
         -Frees font
        Side Effects:
         -None
        */

		bool loadBitmap( std::string path );
        /*
        Pre Condition:
         -None
        Post Condition:
         -Loads bitmap font
         -Returns true on success
         -Reports errors to console
        Side Effects:
         -None
        */

        bool loadFreeType( std::string path, GLuint pixelSize );
        /*
        Pre Condition:
         -None
        Post Condition:
         -Creates internal 8bit texture with given font file
         -Reports freetype error code if freetype error occurs
        Side Effects:
         -None
        */

        void freeFont();
        /*
        Pre Condition:
         -None
        Post Condition:
         -Deallocates texture and sprite sheet data
        Side Effects:
         -None
        */

        void renderText( GLfloat x, GLfloat y, std::string text, LFRect* area = NULL, int align = LFONT_TEXT_ALIGN_LEFT );
        /*
        Pre Condition:
         -A loaded font and bound LFontShader2D
        Post Condition:
         -Renders text
         -If area is given, text is aligned within given area
        Side Effects:
         -Binds member texture and data buffers
        */

        GLfloat getLineHeight();
        /*
        Pre Condition:
         -A loaded font
        Post Condition:
         -Return height for a single line of text
        Side Effects:
         -None
        */

        LFRect getStringArea( std::string text );
        /*
        Pre Condition:
         -A loaded font
        Post Condition:
         -Returns area for given text
        Side Effects:
         -None
        */

	private:
        GLfloat substringWidth( const char* substring );
        /*
        Pre Condition:
        -A loaded font
        Post Condition:
        -Returns the sprite width until it reached a '\n' or '\0'
        Side Effects:
         -None
        */

        GLfloat stringHeight( const char* thisString );
        /*
        Pre Condition:
        -A loaded font
        Post Condition:
        -Returns the pixel height required to render the font
        Side Effects:
         -None
        */

		//Class wide program
		static LFontProgram2D* mFontProgram2D;

        //Font TTF library
        static FT_Library mLibrary;

        //Spacing variables
        GLfloat mSpace;
        GLfloat mLineHeight;
        GLfloat mNewLine;
};

#endif

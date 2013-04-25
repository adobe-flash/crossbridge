// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "AS3/AS3.h"

// A terribly slow implementation of conway's game of life.
// for a better implementation see this: http://tomas.rokicki.com/hlife/

bool *universe_current, *universe_future;
uint32_t xd, yd;

void setCell(uint32_t x, uint32_t y, bool status)
{
    x %= xd;
    y %= yd;
    universe_future[x + (y*xd)] = status;
}

bool isAlive(uint32_t x, uint32_t y)
{
    x %= xd;
    y %= yd;
    return universe_current[x + (y*xd)];
}

extern "C" void updateUniverse()
{
    // Apply the rules to the current universe to generate the next
    for(uint32_t x=0; x<xd; x++) {
        for(uint32_t y=0; y<yd; y++) {
            int livecount = 0;
            livecount += isAlive(x-1,y);
            livecount += isAlive(x-1,y-1);
            livecount += isAlive(x,y-1);
            livecount += isAlive(x+1,y-1);
            livecount += isAlive(x+1,y);
            livecount += isAlive(x+1,y+1);
            livecount += isAlive(x,y+1);
            livecount += isAlive(x-1,y+1);
            
            setCell(x, y, isAlive(x,y));
            
            if(isAlive(x, y)) {
                // Rules 1,2,3:
                // A live cell with fewer than 2 or greater than 3 live
                // neighbours dies.
                if(livecount != 2 && livecount != 3)
                    setCell(x, y, false);
            } else {
                // Rules 4:
                // A dead cell with three live neighbours comes to life.
                if(livecount == 3)
                    setCell(x, y, true);
            }
        }
    }
    
    // flip universes
    bool *tmp = universe_current;
    universe_current = universe_future;
    universe_future = tmp;
    
    // update the bitmap
    for(uint32_t x=0; x<xd; x++) {
        for(uint32_t y=0; y<yd; y++) {
            inline_as3(
                "import com.adobe.flascc.CModule;\n"
                "import flash.display.BitmapData;\n"
                "import flash.display.Graphics;\n"
                "import flash.display.Stage;\n"
                "var x:uint = %0 * 3;\n"
                "var y:uint = %1 * 3;\n"
                "var c:uint = %2 ? 0x0 : 0xFFFFFF;\n"
                "CModule.activeConsole.bmd.setPixel32(x, y, c);\n"
                "CModule.activeConsole.bmd.setPixel32(x+1, y, c);\n"
                "CModule.activeConsole.bmd.setPixel32(x+1, y+1, c);\n"
                "CModule.activeConsole.bmd.setPixel32(x, y+1, c);\n"
                : : "r"(x), "r"(y), "r"(isAlive(x, y))
            );
        }
    }
}

int main()
{
    inline_as3(
        "import com.adobe.flascc.CModule;\n"
        "import flash.display.Bitmap;\n"
        "import flash.display.BitmapData;\n"
        "%0 = CModule.rootSprite.stage.stageWidth/3;\n"
        "%1 = CModule.rootSprite.stage.stageHeight/3;\n"
        : "=r"(xd), "=r"(yd) :
    );

    universe_current = new bool[xd*yd];
    universe_future = new bool[xd*yd];
    
    for(uint32_t x=0; x<xd; x++) {
        for(uint32_t y=0; y<yd; y++) {
            universe_current[x + y*xd] = rand()%2;
            universe_future[x + y*xd] = false;
        }
    }

    AS3_GoAsync();
}

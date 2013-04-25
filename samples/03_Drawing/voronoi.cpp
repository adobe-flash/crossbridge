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

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "VoronoiDiagramGenerator.h"
#include "AS3/AS3.h"

int main(int argc,char **argv) 
{
    int stagewidth, stageheight;
    
    inline_as3(
        "import flash.display.Stage;\n"
        "import flash.display.Graphics;\n"
        "import com.adobe.flascc.CModule;\n"
        "var gfx = CModule.rootSprite.graphics;\n"
        "gfx.lineStyle(1, 0);\n"
        "gfx.beginFill(0, 0.0);\n"
        "%0 = CModule.rootSprite.stage.stageWidth;\n"
        "%1 = CModule.rootSprite.stage.stageHeight;\n"
        : "=r"(stagewidth),"=r"(stageheight) :
    );
        
    const int cellcount = 512;
	float xvals[cellcount], yvals[cellcount];
    for(int i=0; i<cellcount; i++) {
        xvals[i] = stagewidth * ((float)rand()/(float)RAND_MAX);
        yvals[i] = stageheight * ((float)rand()/(float)RAND_MAX);
    }

	VoronoiDiagramGenerator vdg;
	vdg.generateVoronoi(xvals, yvals, cellcount, 0, stagewidth, 0, stageheight, 3);
	vdg.resetIterator();

    float x1,y1,x2,y2;
	while(vdg.getNext(x1,y1,x2,y2))
	{
        inline_as3("gfx.moveTo(%0,%1);\n" : : "r"(x1), "r"(y1));
        inline_as3("gfx.lineTo(%0,%1);\n" : : "r"(x2), "r"(y2));
	}
    inline_as3("gfx.endFill();\n");
}

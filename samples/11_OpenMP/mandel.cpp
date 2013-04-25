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
#include <math.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
#include <Flash++.h>

/*
Because AS3 objects other than ByteArray are not shareable across workers the
Flash++ interop makes a distinction between "ui" object references and "local"
object references. References to objects n the ui worker will block when you
call a method or access a property on them until the CModule.serviceUIRequests()
function is called from the ui worker. By putting the ui worker types into the
"AS3::ui" c++ namespace and local wokrer types into the "AS3::local" c++
namespace they are made incompatible so that you will get a compile error if
you attempt to pass a local reference to a ui method or vice-versa.

The using declaration here imports all of the ui worker types because we want to
perform some logic on the background thread (the mandlebrot calculation) but
then we want to interact with objects (textfields and bitmaps) on the ui worker.
*/
using namespace AS3::ui;

static var decreaseThreads(void *arg, var as3Args) {
    // decrease thread count!
    if(*(volatile int *)arg > 1)
        --*(volatile int *)arg;
    return internal::_undefined;
}

static var increaseThreads(void *arg, var as3Args) {
    // increase thread count!
    ++*(volatile int *)arg;
    return internal::_undefined;
}

int main()
{
    volatile int threadCount = 1;

    flash::display::Stage stage = internal::get_Stage();

    // register an event listener to increase thread count on left mouse down
    stage->addEventListener(flash::events::MouseEvent::MOUSE_DOWN, Function::_new(increaseThreads, (void *)&threadCount));

    // register an event listener to decrease thread count on right mouse down
    stage->addEventListener(flash::events::MouseEvent::RIGHT_MOUSE_DOWN, Function::_new(decreaseThreads, (void *)&threadCount));

    int superSample = 1;
    flash::display::BitmapData bitmapData = flash::display::BitmapData::_new(stage->stageWidth * superSample, stage->stageHeight * superSample);
    flash::display::Bitmap bitmap = flash::display::Bitmap::_new(bitmapData);
    bitmap->width = stage->stageWidth;
    bitmap->height = stage->stageHeight;
    stage->addChild(bitmap);

    flash::text::TextField fpsCounter = flash::text::TextField::_new();
    fpsCounter->defaultTextFormat = flash::text::TextFormat::_new(internal::new_String("Helvetica"),internal::new_Number(24));

    fpsCounter->width = stage->stageWidth;
    fpsCounter->height = 100;
    fpsCounter->multiline = true;
    fpsCounter->x = 0;
    fpsCounter->y = 0;
    stage->addChild(fpsCounter);

    flash::utils::ByteArray ram = internal::get_ram();
    flash::geom::Rectangle bitmapDataRect = bitmapData->rect;

    double magnify = 1.0;                                  /* no magnification             */
    int itermax = 32;                                     /* how many iterations to do    */
    double iterCut = 100.0;                                /* when to stop iterating       */
    int hxres = stage->stageWidth * superSample;           /* horizonal resolution         */
    int hyres = stage->stageHeight * superSample;          /* vertical resolution          */
    int subPixelSampling = 1;                              /* stochastic subpixel samples  */
    unsigned *colors = new unsigned[hxres * hyres];

    /*
    This code will loop forever, but because we're running in a background
    worker this isn't going to cause a script timeout
    */
    for(;;) {
        timeval startTime, endTime;
        gettimeofday(&startTime, NULL);

        /* Make sure OpenMP knows how many threads we want to use. */
        omp_set_num_threads(threadCount);

        /*
        This pragma is the secret sauce that lets OpenMP know that we want this
        particular for loop to be parallelized and run on multiple threads.
        OpenMP will handle the thread creation and job allocation for us.
        */
        #pragma omp parallel for schedule(dynamic, 16)
        for (int hy=0; hy<hyres; hy++)  {
            unsigned *curColor = colors + hy * hxres;
            int hx;
            // let thread # contribute to color
            unsigned threadColor = (256 * omp_get_thread_num()) / threadCount;
            for (int hx=0; hx<hxres; hx++)  {
                double x,xx,y,cx,cy;
                int iteration;
                unsigned val = 0;
                int subiter = subPixelSampling;
                double xoff = 0.0, yoff = 0.0;
                int iterAccum = 0;
                do {
                    cx = (((double)hx + xoff)/((double)hxres)-0.5)/magnify*3.0 -1.74;
                    cy = (((double)hy + yoff)/((double)hyres)-0.5)/magnify*3.0; //+0.0281;
                    x = 0.0;
                    y = 0.0;
                    for (iteration=1; iteration<itermax; iteration++)  {
                        xx = x*x-y*y+cx;
                        y = 2.0*x*y+cy;
                        x = xx;
                        if (x*x+y*y>iterCut) {
                            break;
                        }
                    }
                    iterAccum += iteration;

                    if(subiter > 1) {
                        xoff = 0.75 * (1.0 + (((double)rand() / (double)RAND_MAX) * -2.0));
                        yoff = 0.75 * (1.0 + (((double)rand() / (double)RAND_MAX) * -2.0));
                    }
                }
                while (--subiter > 0);
                
                *curColor = (((iterAccum*32)/subPixelSampling) << 16) | threadColor | 0x8f000000;
                curColor++;
            }
        }
        gettimeofday(&endTime, NULL);
        double startSec = startTime.tv_sec + (startTime.tv_usec / 1000000.0);
        double endSec = endTime.tv_sec + (endTime.tv_usec / 1000000.0);
        double frameTime = 1.0 / (endSec - startSec);

        /*
        Because the bitmapData object belongs to the primordial flash worker
        this method call will block and will get run on the primordial worker
        when it next executes CModule.serviceUIRequests() which should happen
        once every frame.

        Another thing to note is that although the reference to 'ram' is a
        ByteArray in the AS3::ui namespace, and therefore not visible to the
        background worker, the underlying data is shared accross all flascc
        workers so the integer offset 'colors' coming from the current
        background worker is still valid and points to the same data when used
        in this call to setPixels performed on the ui worker.
        */
        bitmapData->setPixels(bitmapDataRect, ram, colors);
        char fpsText[512];
        if(threadCount > 1)
            snprintf(&fpsText[0], 512, "%.1f fps with %d threads (blue shading indicates which thread is rendering each pixel)", frameTime, threadCount);
        else
            snprintf(&fpsText[0], 512, "%.1f fps with %d threads", frameTime, threadCount);
        fpsCounter->text = &fpsText[0];
        magnify *= 1.01;
    }
    return 0;
}


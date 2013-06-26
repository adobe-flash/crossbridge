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

#include <iostream>
#include <vector>
#include <math.h>
#include <AS3/AS3.h>
#include <Flash++.h>

// as3wig.jar was used to create a C++ wrapper for the AS3 code in the
// AGALMiniAssemblerwhich we use to compile AGAL asm into AGAL bytecode that can
// be uploaded to the GPU. Look at the makefile to see how to invoke as3wig.jar
// You can use it to wrap any AS3 code you want to access from C++.
#include "AGAL.h"

// We're going to be using UI worker references in this example (see sample 2
// for more information on the difference between UI and loca worker
// references).
using namespace AS3::ui;

// Some global vars we'll use in various functions.
flash::display::Stage3D s3d;
flash::display::Stage stage;
flash::display3D::Context3D ctx3d;
flash::display3D::IndexBuffer3D i3dbuffer;
flash::display3D::VertexBuffer3D v3dbuffer;

// This function will be attached to the ENTER_FRAME event to drive the
// animation.
static var enterFrame(void *arg, var as3Args)
{
    static int tc = 0;
    try {
        ctx3d->clear((sin(tc++ / 10.0f) * 0.5) + 0.5, 0, 0, 1, 1, 0, 0xffffffff);
        ctx3d->drawTriangles(i3dbuffer, 0, -1);
        ctx3d->present();
    } catch(var e) {
        char *err = internal::utf8_toString(e);
        std::cout << "Exception: " << err << std::endl;
        free(err);
    }
    return internal::_undefined;
}

// If we fail to create the Context3D we display a warning
static var context3DError(void *arg, var as3Args)
{
    flash::text::TextFormat fmt = flash::text::TextFormat::_new();
    fmt->size = internal::new_int(24);
    fmt->align = flash::text::TextFormatAlign::CENTER;

    flash::text::TextField tf = flash::text::TextField::_new();
    tf->defaultTextFormat = fmt;
    tf->width = stage->stageWidth;
    tf->height = stage->stageHeight;
    tf->multiline = true;
    tf->wordWrap = true;
    tf->text = 
        "\nUnable to create a Stage3D context. Usually this means you ran the swf "
        "directly in a web browser, use the HTML wrapper instead so the wmode "
        "gets set correctly to 'direct'.";

    stage->addChild(tf);
}

// After a Context3D is created this function will be called.
static var initContext3D(void *arg, var as3Args)
{
	printf("initContext3D\n");

    // We have a context, we just need to configure the backbuffer to fill the
    // stage.
    ctx3d = s3d->context3D;
    ctx3d->enableErrorChecking = true;
    ctx3d->configureBackBuffer(stage->stageWidth, stage->stageHeight, 2,
                               true, false);

    com::adobe::utils::AGALMiniAssembler vasm =
        com::adobe::utils::AGALMiniAssembler::_new(false);
    vasm->assemble(flash::display3D::Context3DProgramType::VERTEX,
                   //"m44 vt0, va0, vc0\n"
                   "mov op, va0\n");

    com::adobe::utils::AGALMiniAssembler fasm =
        com::adobe::utils::AGALMiniAssembler::_new(false);
    fasm->assemble(flash::display3D::Context3DProgramType::FRAGMENT,
                   "mov oc, fc0\n");

    flash::display3D::Program3D program = ctx3d->createProgram();
    program->upload(vasm->agalcode, fasm->agalcode);
    ctx3d->setProgram(program);

    i3dbuffer = ctx3d->createIndexBuffer(3);
    uint16_t indicies[] = {
    	2,1,0
    };
    i3dbuffer->uploadFromByteArray(internal::get_ram(), (int)&indicies[0], 0, 3, (void*)&indicies[0]);

    v3dbuffer = ctx3d->createVertexBuffer(3, 3);
    float verticies[] = {
    	-0.5, -0.5, 0,
        0, 0.5, 0,
        0.5, -0.5, 0,
    };
    v3dbuffer->uploadFromByteArray(internal::get_ram(), (int)&verticies[0], 0, 3, (void*)&verticies[0]);
    ctx3d->setVertexBufferAt(0, v3dbuffer, 0, flash::display3D::Context3DVertexBufferFormat::FLOAT_3);

    com::adobe::utils::PerspectiveMatrix3D projection =
        com::adobe::utils::PerspectiveMatrix3D::_new(internal::_null);
    projection->perspectiveFieldOfViewLH(45.0*M_PI/180.0, 1.2, 0.1, 512);
    ctx3d->setProgramConstantsFromMatrix(
        flash::display3D::Context3DProgramType::VERTEX, 0, projection, false);
    var vc = internal::new_Vector_Number();
    vc[0] = internal::new_Number(0.5);
    vc[1] = internal::new_Number(0.5);
    vc[2] = internal::new_Number(0.5);
    vc[3] = internal::new_Number(0.5);
    ctx3d->setProgramConstantsFromVector(flash::display3D::Context3DProgramType::FRAGMENT, 0, vc, -1);
    stage->addEventListener(flash::events::Event::ENTER_FRAME, Function::_new(enterFrame, NULL));

    return internal::_undefined;
}

int main()
{
    // Setup the stage
    stage = internal::get_Stage();
    stage->scaleMode = flash::display::StageScaleMode::NO_SCALE;
    stage->align = flash::display::StageAlign::TOP_LEFT;
    stage->frameRate = 60;

    // Ask for a Stage3D context to be created
    s3d = var(var(stage->stage3Ds)[0]);
    s3d->addEventListener(flash::events::Event::CONTEXT3D_CREATE, Function::_new(initContext3D, NULL));
    s3d->addEventListener(flash::events::ErrorEvent::ERROR, Function::_new(context3DError, NULL));
    s3d->requestContext3D(flash::display3D::Context3DRenderMode::AUTO,
                          flash::display3D::Context3DProfile::BASELINE_CONSTRAINED);

    // Suspend main() and return to the Flash runloop
    AS3_GoAsync();
}
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

/*
TODO:

- Full screen
- context loss
- touch input
- device rotation
*/

#include <iostream>
#include <vector>
#include <math.h>
#include <AS3/AS3.h>
#include <Flash++.h>
#include <AGAL.h>
#include <game.h>

using namespace AS3::ui;

flash::display::Stage3D s3d;
flash::display::Stage stage;

SpaceTriangle::Game thegame;

static var enterFrame(void *arg, var as3Args)
{
    try {
        thegame.tick();
        thegame.render(s3d->context3D);
    } catch(var _e) {
        Error e = var(_e);
        char *err = internal::utf8_toString(e);
        std::cout << "Exception: " << err << std::endl << err << std::endl;
        free(err);
        exit(-1);
    }
    return internal::_undefined;
}

static var handleKeyUp(void *arg, var as3Args)
{
    flash::events::KeyboardEvent ke = var(as3Args[0]);
    thegame.handleKeyUp(ke->keyCode);
    ke->stopPropagation();
    return internal::_undefined;
}

static var handleKeyDown(void *arg, var as3Args)
{
    flash::events::KeyboardEvent ke = var(as3Args[0]);
    thegame.handleKeyDown(ke->keyCode);
    ke->stopPropagation();
    return internal::_undefined;
}

static var handleRightClick(void *arg, var as3Args)
{
    // As long as there is a right click handler function
    // registered the default Flash right-click menu will
    // be disabled. but you could also use this event handler
    // for actual input handling.

    return internal::_undefined;
}

static var handleFullScreen(void *arg, var as3Args)
{
    stage->width = stage->fullScreenHeight;
    stage->height = stage->fullScreenHeight;

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

static var initContext3D(void *arg, var as3Args)
{
    flash::display3D::Context3D ctx3d = s3d->context3D;
    String driverInfo = ctx3d->driverInfo;

    if(driverInfo->indexOf("Software") != -1) {
        // For various reasons your 3D context might actually
        // end up using software rendering instead of being GPU
        // accelerated. This would be a a good place to handle
        // that situation if you want to reduce the graphical
        // complexity or simply prevent the game from running.

        printf("Stage3D is running Software mode...");
    }

    thegame.init(stage, s3d);
    stage->addEventListener(flash::events::Event::ENTER_FRAME, Function::_new(enterFrame, NULL));
    stage->addEventListener(flash::events::KeyboardEvent::KEY_DOWN, Function::_new(handleKeyDown, NULL));
    stage->addEventListener(flash::events::KeyboardEvent::KEY_UP, Function::_new(handleKeyUp, NULL));
    stage->addEventListener(flash::events::FullScreenEvent::FULL_SCREEN, Function::_new(handleFullScreen, NULL));

    try {
        stage->addEventListener(flash::events::MouseEvent::RIGHT_CLICK, Function::_new(handleRightClick, NULL));
    } catch(var e) {
        // Old players don't support this event so we catch that here
        // sadly that means old players will still show the default
        // Flash right-click menu.
    }

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
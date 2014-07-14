#include <Flash++.h>
#include <stdio.h>

using namespace AS3::ui;

var mouseDownHandler(void *arg, var as3Args){
    printf("mouseDown!\n");
    return internal::_undefined;
}

var mouseUpHandler(void *arg, var as3Args){
    printf("mouseUp!\n");
    return internal::_undefined;
}

int main()
{
    flash::display::Stage stage = internal::get_Stage();
    stage->scaleMode = flash::display::StageScaleMode::NO_SCALE;
    stage->align = flash::display::StageAlign::TOP_LEFT;

    flash::display::Sprite mySprite = flash::display::Sprite::_new();
    flash::display::Graphics graphics = mySprite->graphics;
    graphics->beginFill(0x0000ff, 1.0);
    graphics->drawCircle(50.0, 50.0, 50.0);
    graphics->endFill();

    stage->addChild(mySprite);

    // assign handlers to the mouseDown and mouseUp events
    mySprite->addEventListener(flash::events::MouseEvent::MOUSE_DOWN,
                               Function::_new(mouseDownHandler, NULL));

    mySprite->addEventListener(flash::events::MouseEvent::MOUSE_UP,
                               Function::_new(mouseUpHandler, NULL));
    
    // Today's Date
    Date myDate1 = Date::_new();
    std::string myDate1Str = AS3::sz2stringAndFree(internal::utf8_toString(myDate1));
    printf("%s\n", myDate1Str.c_str());
    
    int year = myDate1->fullYear;
    printf("Year: %d\n", year);
        
    // as a C string
    char* dstr = internal::utf8_toString(myDate1);
    printf("%s\n", dstr);
    free(dstr);
        
    // Specified Date
    Date myDate2 = Date::_new(internal::new_int(2000),
                              internal::new_int(0),
                              internal::new_int(1));

    std::string myDate2Str = AS3::sz2stringAndFree(internal::utf8_toString(myDate2));
    printf("%s\n", myDate2Str.c_str());
    
    // as a C string
    char* dstr2 = internal::utf8_toString(myDate1);
    printf("%s\n", dstr2);
    free(dstr2);
    
    // Throw an exception so main does not return normally
    // and cause the static destructors to be executed.
    AS3_GoAsync();
}

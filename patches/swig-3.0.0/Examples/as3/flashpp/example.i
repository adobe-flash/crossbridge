/* File : example.i */
%module example

%{
    #include "example.h"
    #include <AS3/AS3++.h>
#include <Flash++.h>
%}

%include "example.h"

%{
    BaseClass::BaseClass() {}
%}

// with inline we both declare class and wrapper at once
%inline %{ 

class SpecialClass : public BaseClass {
public:
    AS3::ui::var drawArea;
    AS3::ui::flash::display::Shape canvas;
	SpecialClass() {
        AS3_Trace("SpecialClass::Created");
        AS3::local::var s = AS3::local::internal::new_String("SpecialClass loaded.");
        AS3::local::internal::trace(s);
        AS3::ui::flash::display::Stage stage = AS3::ui::internal::get_Stage();
        canvas = AS3::ui::flash::display::Shape::_new();
        drawArea = canvas;
    }
	~SpecialClass() {AS3_Trace("SpecialClass::Destroyed");}
    void DrawCircle(float32 radius) {}
    int getInteger() {return 1000;}
	void DrawSegment() {}
    
};

%}

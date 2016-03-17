package SWIGExample {
import com.adobe.flascc.swig.*;
import avmplus.System;

function ASStrFunc():String {
    return "here's a String";
}

function ASOtherStrFunc(a:String, b:int):String {
    Trace("string: " + a);
    Trace("int: " + b);
    return "string from AS";
}

public function ASMain() {
    Trace("here's what the default function returns: " + 
        ExampleModule.char_func());
    ExampleModule.char_func = ASStrFunc;
    Trace("here's what the function pointer returns: " + 
        ExampleModule.char_func());
    
    var strfunc:Function = ExampleModule.str_func_getter();
    Trace("AS got: " + strfunc("test", 5));
    ExampleModule.str_func_getter = function() { return ASOtherStrFunc };
    var strfunc1:Function = ExampleModule.str_func_getter();
    Trace("AS got: " + strfunc1("test", 5));
    ExampleModule.call_getter();

    ExampleModule.str_func_setter = ExampleModule.a_str_func_setter;
    ExampleModule.str_func_setter(ASOtherStrFunc);
    ExampleModule.call_str_func();

    var numptrs:uint = SWIG_AS3WrapperNum();
    SWIG_AS3UnregCCallWrapper(ASStrFunc);
    SWIG_AS3UnregCCallWrapper(ASOtherStrFunc);
    SWIG_AS3UnregASCallWrapper(strfunc);
    var newnumptrs:uint = SWIG_AS3WrapperNum();
    if (newnumptrs + 3 != numptrs) {
        Trace("unexpected number of function pointers: " + numptrs);
    }

    return 0;
}
}

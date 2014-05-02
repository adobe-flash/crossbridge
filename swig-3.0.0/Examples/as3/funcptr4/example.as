
/* Function pointers support */
package SWIGExample {
import flash.utils.Dictionary;
import com.adobe.flascc.CModule;

var _SWIG_AS3Func2Ptr:Dictionary = new Dictionary();
var _SWIG_AS3Ptr2Func:Dictionary = new Dictionary();

function SWIG_AS3RegASCallWrapper(ptr:int, wrapper:Function):void {
    if (_SWIG_AS3Ptr2Func[ptr]) {
        throw("tried to register AS-callable wrapper for existing AS wrapper");
    }
    _SWIG_AS3Func2Ptr[wrapper] = ptr;
    _SWIG_AS3Ptr2Func[ptr] = wrapper;
}

// Returns Function objects callable from AS
function SWIG_AS3GetASCallWrapper(ptr:int):Function {
    if (_SWIG_AS3Ptr2Func[ptr]) {
        return _SWIG_AS3Ptr2Func[ptr];
    } else {
        return null;
    }
}

function SWIG_AS3RegCCallWrapper(func:Function, wrapper:Function):void {
    var ptr:int;
    
    if (_SWIG_AS3Func2Ptr[func]) {
        throw("tried to register C-callable wrapper for native C function");
    }

    ptr = CModule.allocFunPtrs(1, 4);
    CModule.regFun(ptr, wrapper);
    _SWIG_AS3Func2Ptr[func] = ptr;
    _SWIG_AS3Ptr2Func[ptr] = func;

}

// Returns a C function pointer corresponding to an Actionscript-callable 
// wrapper Function. (The C function is callable from C, of course).
function SWIG_AS3GetCCallPtr(func:Function):int {
    if (_SWIG_AS3Func2Ptr[func]) {
        return _SWIG_AS3Func2Ptr[func];
    } else {
        return 0;
    }
}
} /* package */



package {
import SWIGExample.*;
import flash.utils.ByteArray;


public class example {
	public static function do_op(a:int, b:int, op:Function):int {
		return _wrap_do_op(a, b, op);
	}

	public static function get add():Function{
		return _wrap_add();
	}

	public static function add(arg1:int, arg2:int):int {
		return _wrap_add(arg1, arg2);
	}

	public static function get sub():Function{
		return _wrap_sub();
	}

	public static function sub(arg1:int, arg2:int):int {
		return _wrap_sub(arg1, arg2);
	}

	public static function get mul():Function{
		return _wrap_mul();
	}

	public static function mul(arg1:int, arg2:int):int {
		return _wrap_mul(arg1, arg2);
	}

	public static function set funcvar(funcvar:Function):void {
		_wrap_funcvar_set(funcvar);
	}

	public static function get funcvar():Function {
		return _wrap_funcvar_get();
	}

}
} /* package */

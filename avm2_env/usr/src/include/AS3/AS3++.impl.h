/*
** ADOBE SYSTEMS INCORPORATED
** Copyright 2012 Adobe Systems Incorporated. All Rights Reserved.
**
** NOTICE:  Adobe permits you to use, modify, and distribute this file in
** accordance with the terms of the Adobe license agreement accompanying it.
** If you have received this file from a source other than Adobe, then your use,
** modification, or distribution of it requires the prior written permission of Adobe.
*/

#ifdef AS3_UI
#define MAYBEDELEGATEATTR __attribute__((noinline))
#else
#define MAYBEDELEGATEATTR /**/
#endif

class var;

namespace internal
{

/** Create an AS3 int object.
*/
var new_int(int n);

/** Create an AS3 uint object.
*/
var new_uint(unsigned n);

/** Create an AS3 Number object.
*/
var new_Number(double n);

/** Create an AS3 Boolean object.
*/
var new_Boolean(bool b);

/** Create an AS3 String object. If len is set to -1 then strlen 
 *  is called on the string to calculate its length. Explicitly 
 *  providing len means the string does not need to be 
 *  null-terminated and avoids the call to strlen.
*/
var new_String(const char *s, int len = -1);

/** Create an AS3 Namespace object. If len is set to -1 then strlen 
 *  is called on the string to calculate its length. Explicitly 
 *  providing len means the string does not need to be 
 *  null-terminated and avoids the call to strlen.
*/
var new_Namespace(const char *s, int len = -1);

/** Create an AS3 Array containing references to the specified AS3 vars.
*/
var new_Array(int count, var *elems);

/** Create an AS3 Function object of a C function pointer.	 
 *  The C function must declare two parameters: a void* that 
 *  represents the passed in C context, and a var that represents the
 *  AS3 parameters that were passed to this Function when it is called.
*/
var new_Function(var (*fun)(void *data, var args), void *data);

/** Create an AS3 Vector.<cc>, where cc is a class closure.
*/
var new_Vector(var cc);

/** Create an AS3 Vector.<int>
*/
var new_Vector_int();

/** Create an AS3 Vector.<uint>
*/
var new_Vector_uint();

/** Create an AS3 Vector.<Boolean>
*/
var new_Vector_Boolean();

/** Create an AS3 Vector.<String>
*/
var new_Vector_String();

/** Create an AS3 Vector.<Number>
*/
var new_Vector_Number();

/** Create an AS3 undefined object
*/
var new_undefined();

/** Create an AS3 null object
*/
var new_null();

#ifdef AS3_UI
/** Get a reference to the flash Stage object
*/
var get_Stage();
#endif

/** Get a reference to the Worker object for a given thread id
** or -1 indicating the currently running thread
*/
var get_Worker(long tid = -1);

/** Get a reference to the currently assigned domainMemory ByteArray.
*/
var get_ram();

/** The AS3 undefined var
*/
extern const var _undefined;

/** The AS3 null var
*/
extern const var _null;

/** The AS3 true var
*/
extern const var _true;

/** The AS3 false var
*/
extern const var _false;

/** Equivalent to the AS3 typeof operator
* http://help.adobe.com/en_US/FlashPlatform/reference/actionscript/3/operators.html#typeof
*/
var _typeof(var val);

/** Equivalent to the AS3 equality operator "=="
* http://help.adobe.com/en_US/FlashPlatform/reference/actionscript/3/operators.html#equality
*/
bool equals(var a, var b);
    
/** Equivalent to the AS3 strict equality operator "==="
* http://help.adobe.com/en_US/FlashPlatform/reference/actionscript/3/operators.html#strict_equality
*/
bool strictequals(var a, var b);
    
/** Equivalent to the AS3 less than operator "<"
* http://help.adobe.com/en_US/FlashPlatform/reference/actionscript/3/operators.html#less_than
*/
bool lessthan(var a, var b);

/** Equivalent to the AS3 less than or equal to operator "<="
* http://help.adobe.com/en_US/FlashPlatform/reference/actionscript/3/operators.html#less_than_or_equal_to
*/
bool lessequals(var a, var b);

/** Equivalent to the AS3 greater than to operator ">"
* http://help.adobe.com/en_US/FlashPlatform/reference/actionscript/3/operators.html#greater_than
*/
bool greaterthan(var a, var b);
    
/** Equivalent to the AS3 greater than or equal to operator ">="
* http://help.adobe.com/en_US/FlashPlatform/reference/actionscript/3/operators.html#greater_than_or_equal_to
*/
bool greaterequals(var a, var b);

/** Uses Function.apply:
* http://help.adobe.com/en_US/FlashPlatform/reference/actionscript/3/Function.html#apply()
* To call a Function object with an Array of arguments.
*/
var call_v(var fun, var rcv, var vargs, void *ramPos = (void *)-1);

/** Uses Function.apply:
* http://help.adobe.com/en_US/FlashPlatform/reference/actionscript/3/Function.html#apply()
* To call a Function object. The arguments array will be constructed automatically.
*/
var call(var fun, var rcv, int argCount, var *args, void *ramPos = (void *)-1);

/** Gets a property from an object referenced via the specified name and namespace.
*/
var getproperty(var obj, var ns, var name);

/** Gets a property from an object referenced via the specified name in the public namespace.
*/
var getproperty(var obj, var name);

/** Sets a property on an object referenced via the specified name and namespace.
*/
void setproperty(var obj, var ns, var name, var val);

/** Sets a property on an object referenced via the specified name in the public namespace.
*/
void setproperty(var obj, var name, var val);

/** Gets a property from the current global object.
*/
var getlex(var name);

/** Gets a property from the current global object in the specified namespace.
*/
var getlex(var ns, var name);

/** Sets a property on the current global object.
*/
void setlex(var name, var val);

/** Sets a property on the current global object in the specified namespace.
*/
void setlex(var ns, var name, var val);

/** Construct an object. The "cc" parameter specifies the class to be
* constructed
* @return An object of type "cc"
*/
var construct(var cc);

/** Construct an object. The "cc" parameter specifies the class to be
* constructed, the other parameters are passed to the constructor function
* @return An object of type "cc"
*/
var construct(var cc, var a1);

/** Construct an object. The "cc" parameter specifies the class to be
* constructed, the other parameters are passed to the constructor function
* @return An object of type "cc"
*/
var construct(var cc, var a1, var a2);

/** Construct an object. The "cc" parameter specifies the class to be
* constructed, the other parameters are passed to the constructor function
* @return An object of type "cc"
*/
var construct(var cc, var a1, var a2, var a3);

/** Construct an object. The "cc" parameter specifies the class to be
* constructed, the other parameters are passed to the constructor function
* @return An object of type "cc"
*/
var construct(var cc, var a1, var a2, var a3, var a4);

/** Construct an object. The "cc" parameter specifies the class to be
* constructed, the other parameters are passed to the constructor function
* @return An object of type "cc"
*/
var construct(var cc, var a1, var a2, var a3, var a4, var a5);

/** Coerce an object v to type cc. This differs slightly from the as 
* function because it throws an error when v cannot be coerced into 
* cc whereas the as function does not throw an error and instead 
* returns _null.
*/
var coerce(var cc, var v);

/** Equivalent to the AS3 delete operator
* http://help.adobe.com/en_US/FlashPlatform/reference/actionscript/3/operators.html#delete
* Returns true if the deletion succeeded.
*/
bool _delete(var val, var prop);

/** Throws the var as an AS3 exception.
*/
void _throw(var val);

/** Calls toString() on the AS3 object and copies the string into malloc'ed
* buffer. You are responsible for freeing the returned string.
*/
char *utf8_toString(var val);

/** Cast the var to a boolean.
*/
bool bool_valueOf(var val);

/** Cast the var to an int.
*/
int int_valueOf(var val);

/** Cast the var to an unsigned int.
*/
unsigned uns_valueOf(var val);

/** Cast the var to a double.
*/
double double_valueOf(var val);

/** Equivalent to the AS3 is operator
* http://help.adobe.com/en_US/FlashPlatform/reference/actionscript/3/operators.html#is
*/
bool is(var a, var b);

/** Equivalent to the AS3 as operator
* http://help.adobe.com/en_US/FlashPlatform/reference/actionscript/3/operators.html#as
*/
var as(var a, var b);

/** Equivalent to the AS3 instanceof operator
* http://help.adobe.com/en_US/FlashPlatform/reference/actionscript/3/operators.html#instanceof
*/
bool instanceof(var a, var b);

/** Equivalent to the AS3 in operator
* http://help.adobe.com/en_US/FlashPlatform/reference/actionscript/3/operators.html#in
*/
bool in(var a, var b);

/** Trace a var to the Flash log file, if enabled:
* http://livedocs.adobe.com/flex/3/html/help.html?content=logging_04.html
*/
void trace(var val);

/** \private
*/
class id_holder
{
public:
  /** \private
  */
  unsigned _id;

  id_holder() : _id(-1) {}
};

}

namespace internal
{
  class propref;
}

/** A reference to an Actionscript object
* This class should be used to maintain a strong reference to an Actionscript
* object. Internally it maintains a reference count so that the AS3 object
* reference will be release once the last copy of a given var instance is
* destroyed.
*/
class var : protected AS3_NS::internal::id_holder
{
    friend class AS3_NS::internal::propref;

    friend var AS3_NS::internal::new_int(int i);
    friend var AS3_NS::internal::new_uint(unsigned u);
    friend var AS3_NS::internal::new_Number(double d);
    friend var AS3_NS::internal::new_String(const char *s, int len);
    friend var AS3_NS::internal::new_Namespace(const char *s, int len);
    friend var AS3_NS::internal::new_Boolean(bool b);
    friend var AS3_NS::internal::new_Function(var (*fun)(void *data, var args), void *data);
    friend var AS3_NS::internal::new_Array(int count, var *elems);
    friend var AS3_NS::internal::new_Vector(var cc);
    friend var AS3_NS::internal::new_Vector_int();
    friend var AS3_NS::internal::new_Vector_uint();
    friend var AS3_NS::internal::new_Vector_Boolean();
    friend var AS3_NS::internal::new_Vector_String();
    friend var AS3_NS::internal::new_Vector_Number();
    friend var AS3_NS::internal::new_undefined();
    friend var AS3_NS::internal::new_null();

#ifdef AS3_UI
    friend var AS3_NS::internal::get_Stage();
#endif
    friend var AS3_NS::internal::get_Worker(long);
    friend var AS3_NS::internal::get_ram();

    friend var AS3_NS::internal::_typeof(var val);
    friend bool AS3_NS::internal::equals(var a, var b);
    friend bool AS3_NS::internal::strictequals(var a, var b);
    friend bool AS3_NS::internal::lessthan(var a, var b);
    friend bool AS3_NS::internal::lessequals(var a, var b);
    friend bool AS3_NS::internal::greaterthan(var a, var b);
    friend bool AS3_NS::internal::greaterequals(var a, var b);
    friend var AS3_NS::internal::call_v(var fun, var rcv, var vargs, void *ramPos);
    friend var AS3_NS::internal::call(var fun, var rcv, int argCount, var *args, void *ramPos);
    friend var AS3_NS::internal::getproperty(var obj, var name);
    friend var AS3_NS::internal::getproperty(var obj, var ns, var name);
    friend void AS3_NS::internal::setproperty(var obj, var name, var val);
    friend void AS3_NS::internal::setproperty(var obj, var ns, var name, var val);
    friend var AS3_NS::internal::getlex(var name);
    friend var AS3_NS::internal::getlex(var ns, var name);
    friend void AS3_NS::internal::setlex(var name, var val);
    friend void AS3_NS::internal::setlex(var ns, var name, var val);
    friend var AS3_NS::internal::construct(var cc);
    friend var AS3_NS::internal::construct(var cc, var a1);
    friend var AS3_NS::internal::construct(var cc, var a1, var a2);
    friend var AS3_NS::internal::construct(var cc, var a1, var a2, var a3);
    friend var AS3_NS::internal::construct(var cc, var a1, var a2, var a3, var a4);
    friend var AS3_NS::internal::construct(var cc, var a1, var a2, var a3, var a4, var a5);
    friend var AS3_NS::internal::coerce(var cc, var v);
    friend bool AS3_NS::internal::_delete(var obj, var prop);
    friend void AS3_NS::internal::_throw(var val);

    friend char *AS3_NS::internal::utf8_toString(var val);
    friend bool AS3_NS::internal::bool_valueOf(var val);
    friend int AS3_NS::internal::int_valueOf(var val);
    friend unsigned AS3_NS::internal::uns_valueOf(var val);
    friend double AS3_NS::internal::double_valueOf(var val);

    friend bool AS3_NS::internal::is(var a, var b);
    friend var AS3_NS::internal::as(var a, var b);
    friend bool AS3_NS::internal::instanceof(var a, var b);
    friend bool AS3_NS::internal::in(var a, var b);

    friend void AS3_NS::internal::trace(var val);

protected:
    /** \private
    */
    explicit var(unsigned id) { _id = id; }

    /** \private
    */
    class internal
    {
    public:
      /** \private
      */
      class instance : public id_holder {};

      /** \private
      */
      enum {
          kUndefined = 0,
          kNull,
          kTrue,
          kFalse,
          kZero,
          kOne,
          kNaN,
          kLastSpecial = kFalse
      };

      /** \private
      */
      static void acquire(unsigned id) MAYBEDELEGATEATTR;
  
      /** \private
      */
      static void release(unsigned id) MAYBEDELEGATEATTR;
  
      /** \private
      */
      static void *uiThunk(void *data);
  
      /** \private
      */
      static unsigned funThunk(var (*fun)(void *, var), void *data, unsigned id);

      /** \private
      */
      static unsigned detach(var *thiz) {
          unsigned id = thiz->_id;
          thiz->_id = internal::kUndefined;
          return id;
      }
    };

public:

    /** Decrements the internal reference count and releases the AS3 object
    * once the reference count is zero.
    */
    ~var();

    /** Construct the AS3 var "undefined". Since all undefineds are
    * indistinguishable there is no need to maintain this var in the reference
    * map.
    */
    var() { _id = internal::kUndefined; } // undefined

    /** The copy constructor allows you to duplicate the reference to an AS3
    * object without duplicating the actual object itself. Both var objects
    * will point to the same AS3 object. Reference counting ensures that the
    * AS3 object is only unreferenced when the last var object is destroyed.
    */
    var(const var &val);

    /** The copy constructor allows you to duplicate the reference to an AS3
    * object without duplicating the actual object itself. Both var objects
    * will point to the same AS3 object. Reference counting ensures that the
    * AS3 object is only unreferenced when the last var object is destroyed.
    */
    var &operator=(var val);

    inline bool operator==(var val) { return AS3_NS::internal::equals(*this, val); }
    inline bool operator!=(var val) { return !AS3_NS::internal::equals(*this, val); }
    inline bool operator<(var val) { return AS3_NS::internal::lessthan(*this, val); }
    inline bool operator<=(var val) { return AS3_NS::internal::lessequals(*this, val); }
    inline bool operator>(var val) { return AS3_NS::internal::greaterthan(*this, val); }
    inline bool operator>=(var val) { return AS3_NS::internal::greaterequals(*this, val); }

    /** Index into this object with the specified property, the result is a
    * propref.
    */
    AS3_NS::internal::propref operator[](var prop); // this[prop]

    /** Index into this object with an integer, the result is a propref.
    */
    AS3_NS::internal::propref operator[](int index);

    /** Index into this object with a C string, the result is a propref.
    */
    AS3_NS::internal::propref operator[](const char *prop);
};

namespace internal {
/** Holds a reference to both an AS3 object and the name of a property
* on that object. The propref object maintains a refcount on the
* object that it references. These are not directly constructable, use the
* various operators on the var class to index a var and you will get a
* propref instance.
*/
class propref
{
    friend class AS3_NS::var;

    var _obj, _prop;

    propref();
    propref(const propref &pr);
    propref(var obj, var prop) : _obj(obj), _prop(prop) {}

public:

    /** returns the current var of property on the object referenced by this
    * propref object
    */
    operator var() { 
        return AS3_NS::internal::getproperty(_obj, _prop);
    }

    /** Sets the current var of the property on the object referenced by this
    * propref object
    */
    propref &operator=(var val) { // set
        AS3_NS::internal::setproperty(_obj, _prop, val);
    }

    propref &operator=(propref &pr)
    {
      AS3_NS::internal::setproperty(_obj, _prop, (var)pr);
    }
};

}

namespace internal {
  /** Create an AS3 null object of a given type
   */
  template <typename T> T new_null()
  {
    var v;
    return v;
  }
}

/** Crete a new propref that references a different property on the same object
*/
inline AS3_NS::internal::propref var::operator[](var prop)
{
    return AS3_NS::internal::propref(*this, prop);
}

/** Crete a new propref that indexes the same object via an integer
*/
inline AS3_NS::internal::propref var::operator[](int index)
{
    return operator[](AS3_NS::internal::new_int(index));
}

/** Crete a new propref that references a different property on the same object
* via the string name of the property.
*/
inline AS3_NS::internal::propref var::operator[](const char *prop)
{
    return operator[](AS3_NS::internal::new_String(prop));
}

#undef MAYBEDELEGATEATTR


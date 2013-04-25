// APPLE LOCAL begin radar 4407151
// Test that objective-c++ does not confuse a template parameter named 'Object'
// with an interface of the same name.
// Author: Fariborz Jahanian <fjahanian@apple.com>
// { dg-do compile }
// { dg-options "" }
typedef struct objc_class *Class;

@interface Object
{
 Class isa;
}
@end

template <class Object>
struct pyobject_type
{ 
    static Object* checked_downcast(Object* x)
    {
        return x;
    }
};
// APPLE LOCAL end radar 4407151

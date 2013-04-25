// APPLE LOCAL file
// Radar 3988061
// { dg-do compile { target powerpc*-*-darwin* } }
// { dg-options "-Os -static -fapple-kext" }
// the virtual call to IOHIDElement::free() is normally resolved
// at compile time, but with -fapple-kext we don't want this.
class IOHIDElement
{
protected:
    virtual ~IOHIDElement();
    virtual void free();
};
class IOHIDElementPrivate: public IOHIDElement
{
protected:
    virtual void free();
};
IOHIDElement::~IOHIDElement() {}
void IOHIDElementPrivate::free()
{
    IOHIDElement::free();
}
// { dg-final { scan-assembler-times "IOHIDElement4free" 1} }

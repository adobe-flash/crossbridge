/* APPLE LOCAL file radar 5802025 */
/* This test case used to ICE because we did not generate a getter call from
   OBJC_PROPERTY_REFERENCE_EXPR in time to access the property getter value when
   getter is retuning a class.
*/
/* { dg-options "-mmacosx-version-min=10.5 -framework Foundation" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-framework Foundation" { target arm*-*-darwin* } } */
/* { dg-do run { target *-*-darwin* } } */

#import <Foundation/Foundation.h>
#import <objc/objc-runtime.h>

class Vector3D
{
        public:
                float x, y, z;
        
        public:
                Vector3D();
                Vector3D(const Vector3D &inVector);
                Vector3D(float initX, float initY, float initZ);
                Vector3D &operator=(const Vector3D & rhs);
                
                NSString *description();
};

Vector3D::Vector3D()
{
        x = y = z = 0;
}

Vector3D::Vector3D(const Vector3D &inVector)
{
        x = inVector.x;
        y = inVector.y;
        z = inVector.z;
}

Vector3D::Vector3D(float initX, float initY, float initZ)
{
        x = initX;
        y = initY;
        z = initZ;
}

Vector3D &Vector3D::operator=(const Vector3D & rhs)
{
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        
        return *this;
}

NSString *Vector3D::description()
{
        return [NSString stringWithFormat: @"(%f, %f, %f)", x, y, z];
}

@interface Object3D : NSObject
{
        Vector3D position;
}

@property (assign) Vector3D position;
@end

static int count;

@implementation Object3D

@synthesize position;

- (id) init
{
        self = [super init];
        if(self)
        {
                position = Vector3D(0, 0, 0);
        }
        
        return self;
}

- (void) setPosition: (Vector3D) inPosition
{
        position = inPosition;
        NSLog(@"setPosition was called with the vector %@", inPosition.description());
        ++count;
}
@end

int main (int argc, const char * argv[]) 
{
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

        Object3D *myObject = [[Object3D alloc] init];
        
        // Each of these looks like it should call the setPosition method.  However,
        // if the Vector3D copy constructor is defined, this one won't.
        // APPLE LOCAL begin radar 7591784
        // Workaround for bug in radar 7591784
        Vector3D V3D(1.0f, 1.0f, 1.0f);
        myObject.position = V3D;
        // APPLE LOCAL end radar 7591784
        NSLog(@"After assignment the position is: %@", myObject.position.description());

        [myObject setPosition: Vector3D(2.0f, 2.0f, 2.0f)];
        NSLog(@"After setPosition: the position is: %@", myObject.position.description());

        typedef void (*SetPositionIMP)(id self, SEL _cmd, Vector3D position);
        ((SetPositionIMP)objc_msgSend)(myObject, @selector(setPosition:), Vector3D(3.0f, 3.0f, 3.0f));
        NSLog(@"After objc_msgSend the position is: %@", myObject.position.description());

        [pool release];
        if (count != 3)
         abort();
        return 0;
}

/* APPLE LOCAL begin radar 4894756 */
#ifndef _OBJC_OBJECT2_H_
#define _OBJC_OBJECT2_H_
#undef OBJC_API_VERSION
#if __OBJC2__
#define OBJC_API_VERSION 2
#else
#define OBJC_API_VERSION 0
#endif
#if OBJC_API_VERSION < 2
#include <objc/Object.h>
#else
#ifndef _OBJC_OBJECT_H_
#define _OBJC_OBJECT_H_
#include <objc/runtime.h>

@interface Object {
@public
     Class isa;
}
+initialize;
+alloc;
+new;
+free;
-free;
+(Class)class;
-(Class)class;
-init;
-superclass;
-(const char *)name;
-(BOOL)conformsTo:(Protocol *)protocol;
@end

@implementation Object

+initialize {
     return self;
}

+alloc {
     return class_createInstance(self, 0);
}

+new {
     return [[self alloc] init];
}

+free {
     return nil;
}

-free {
     return object_dispose(self);
}

+(Class)class {
     return (Class)self;
}

-(Class)class {
     return (Class)isa;
}

-init {
     return self;
}

-superclass {
     return class_getSuperclass([self class]);
}

-(const char *)name {
     return class_getName([self class]);
}

-(BOOL)conformsTo:(Protocol *)protocol {
     Class cls;
     for (cls = [self class]; cls; cls = [cls superclass]) {
         if (class_conformsToProtocol(cls, protocol)) return YES;
     }
     return NO;
}

@end
#endif /* _OBJC_OBJECT_H_ */
#endif
#endif /* _OBJC_OBJECT2_H_ */
/* APPLE LOCAL end radar 4894756 */

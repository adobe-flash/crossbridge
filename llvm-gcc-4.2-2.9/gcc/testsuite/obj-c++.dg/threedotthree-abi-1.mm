/* APPLE LOCAL file radar 4301047 - radar 5245946*/
#include <stdio.h>
#include <string.h>
#include <objc/Protocol.h>
#ifndef __NEXT_RUNTIME__
#include <objc/objc-api.h>
#endif

extern "C" void abort();


@protocol CommonProtocol

-(oneway void)methodCall_On:(in bycopy id)someValue_On;
-(oneway void)methodCall_nO:(bycopy in id)someValue_nO;

-(oneway void)methodCall_Oo:(out bycopy id)someValue_Oo;
-(oneway void)methodCall_oO:(bycopy out id)someValue_oO;

-(oneway void)methodCall_rn:(in const id)someValue_rn;

-(oneway void)methodCall_oOn:(in bycopy out id)someValue_oOn;

@end

@interface ObjCClass <CommonProtocol>
{

}

@end

@implementation ObjCClass
-(oneway void)methodCall_On:(in bycopy id)someValue_On { }
-(oneway void)methodCall_nO:(bycopy in id)someValue_nO { }

-(oneway void)methodCall_Oo:(out bycopy id)someValue_Oo { }
-(oneway void)methodCall_oO:(bycopy out id)someValue_oO { }

-(oneway void)methodCall_rn:(in const id)someValue_rn { }
-(oneway void)methodCall_oOn:(in bycopy out id)someValue_oOn { }
@end

/* APPLE LOCAL radar 4894756 */
/* declaration moved */
struct objc_method_description *meth;
#   if (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5)
struct objc_method_description meth_object;
#endif

int main()
{
	/* APPLE LOCAL radar 4894756 */
        Protocol *proto = @protocol(CommonProtocol);
#   if (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5)
	meth_object = protocol_getMethodDescription (proto,
			@selector(methodCall_On:), YES, YES);
	meth = &meth_object;
#else
        meth = [proto descriptionForInstanceMethod: @selector(methodCall_On:)];
#endif
	if (strcmp (meth->types, "Vv12@0:4On@8"))
	  abort();
#   if (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5)
	meth_object = protocol_getMethodDescription (proto,
		 	@selector(methodCall_nO:), YES, YES);
	meth = &meth_object;
#else
        meth = [proto descriptionForInstanceMethod: @selector(methodCall_nO:)];
#endif
	if (strcmp (meth->types, "Vv12@0:4nO@8"))
	  abort();
#   if (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5)
	meth_object = protocol_getMethodDescription (proto,
			@selector(methodCall_Oo:), YES, YES);
	meth = &meth_object;
#else
        meth = [proto descriptionForInstanceMethod: @selector(methodCall_Oo:)];
#endif
	if (strcmp (meth->types, "Vv12@0:4Oo@8"))
	  abort();
#   if (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5)
	meth_object = protocol_getMethodDescription (proto,
			@selector(methodCall_oO:), YES, YES);
	meth = &meth_object;
#else
        meth = [proto descriptionForInstanceMethod: @selector(methodCall_oO:)];
#endif
	if (strcmp (meth->types, "Vv12@0:4oO@8"))
	  abort();
#   if (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5)
	meth_object = protocol_getMethodDescription (proto,
			@selector(methodCall_rn:), YES, YES);
	meth = &meth_object;
#else
        meth = [proto descriptionForInstanceMethod: @selector(methodCall_rn:)];
#endif
	if (strcmp (meth->types, "Vv12@0:4rn@8"))
	  abort();
#   if (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5)
	meth_object = protocol_getMethodDescription (proto,
			@selector(methodCall_oOn:), YES, YES);	
	meth = &meth_object;
#else
        meth = [proto descriptionForInstanceMethod: @selector(methodCall_oOn:)];
#endif
	if (strcmp (meth->types, "Vv12@0:4oOn@8"))
	  abort();
	return 0;
}

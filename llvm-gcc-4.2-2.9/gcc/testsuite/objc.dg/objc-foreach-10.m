/* APPLE LOCAL file radar 4623423 */
/* Test type 'Class' is recognized as type of a for-each's selector. */
#include <Foundation/Foundation.h>
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile } */

@interface PBXLSDebuggerAdaptor : NSObject {
};
@end


@interface Foo : NSObject {
};
+ (NSString*) _name;
+ (NSMutableDictionary*) _adaptorClassesDictionary;
+ (NSMutableSet*) _debuggerAdaptorClassesSet;
+ (void) registerAdaptorClasses:(NSArray *)adaptorClasses;
+ (void) registerAdaptorClasses1:(NSArray *)adaptorClasses;
@end

@implementation Foo 
+ (NSString*) _name { return nil;}
+ (NSMutableDictionary*) _adaptorClassesDictionary { return nil;}
+ (NSMutableSet*) _debuggerAdaptorClassesSet { return nil;}

+ (void) registerAdaptorClasses:(NSArray *)adaptorClasses {
  int i;
  for (i=0;i<[adaptorClasses count];i++) {
        Class adaptorClass = [adaptorClasses objectAtIndex: i];
        [[self _adaptorClassesDictionary] setObject:adaptorClass forKey:[adaptorClass _name]];
        if ([adaptorClass isSubclassOfClass:[PBXLSDebuggerAdaptor class]]) {
            [[self _debuggerAdaptorClassesSet] addObject:adaptorClass];
        }
    }
}

+ (void) registerAdaptorClasses1:(NSArray *)adaptorClasses
{
    for (Class  adaptorClass in adaptorClasses) {
        [[self _adaptorClassesDictionary] setObject:adaptorClass forKey:[adaptorClass _name]];
        if ([adaptorClass isSubclassOfClass:[PBXLSDebuggerAdaptor class]]) {
            [[self _debuggerAdaptorClassesSet] addObject:adaptorClass];
        }
    }
}
@end

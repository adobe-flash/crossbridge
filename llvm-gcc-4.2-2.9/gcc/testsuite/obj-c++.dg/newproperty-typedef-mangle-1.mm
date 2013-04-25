/* APPLE LOCAL file radar 5002848 */
/* This program tests that for mangling a property type which uses a typedef name,
   the underlying type is used for name mangling. This must compile and run with 
   no error. */
#include <Foundation/Foundation.h>
#include <CoreData/CoreData.h>
#include <objc/runtime.h>
/* { dg-options "-mmacosx-version-min=10.5 -framework Foundation -framework CoreData -fobjc-new-property " } */
/* { dg-do run { target powerpc*-*-darwin* i?86*-*-darwin* } } */


typedef NSString StoreVersionID ;

@interface Parent : NSManagedObject
@property(retain) StoreVersionID* foo;
@end

@implementation Parent
@dynamic foo;
@end

int main (int argc, const char * argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    NSEntityDescription *fooEntity;
    NSAttributeDescription* fooAttribute;
    Parent* testObject;
    
    // Parent works fine
    fooEntity = [[NSEntityDescription alloc] init];
    
    [fooEntity setName:@"FooEntity_TestingProperties"];
    [fooEntity setManagedObjectClassName:@"Parent"];
    fooAttribute = [[[NSAttributeDescription alloc] init] autorelease];
    [fooAttribute setName:@"foo"];
    [fooAttribute setAttributeType:NSStringAttributeType];
    [fooEntity setProperties:[NSArray arrayWithObject:fooAttribute]];
    
    testObject = [[NSManagedObject alloc] initWithEntity:fooEntity insertIntoManagedObjectContext:nil];
    
    objc_property_t propMetaData = class_getProperty([Parent class], "foo");

    const char* propMetaAttributes = property_getAttributes(propMetaData);
    char buffer[1 + strlen(propMetaAttributes)];
    strcpy(buffer, propMetaAttributes);
    char *state = buffer, *attribute;
    while ((attribute = strsep(&state, ",")) != NULL) {
        switch (attribute[0]) {
        case 'T':
            if (attribute[1] != '@') {
                char* end = strchr(attribute, ',');
                if (end) {
                    end[0] = '\0';
                }
                NSLog(@"Excepted property type encoding of either @\"NSString\" or @\"StoreVersionID\" but instead got %s", &attribute[1]);
                break;
            }
            break;
        default: break;
        }
    }    
    
    [testObject release];
    [fooEntity release];
    
    

    [pool release];
    return 0;
}

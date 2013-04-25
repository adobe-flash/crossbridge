/* APPLE LOCAL file 4550582 */
/* Execution check for foreach. Make sure that whole tree is generated for function with
   this particular brand of foreach. */
/* { dg-options "-mmacosx-version-min=10.5 -framework Foundation" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-framework Foundation" { target arm*-*-darwin* } } */
/* { dg-do run } */

#include <Foundation/Foundation.h>

/* foreach tester */

int Verbosity = 0;
int Errors = 0;

bool testHandwritten(char *style, char *test, char *message, id collection, NSSet *reference) {
    int counter = 0;
    bool result = true;
    if (Verbosity) {
        printf("testing: %s %s %s\n", style, test, message);
    }
/*
    for (id elem in collection)
        if ([reference member:elem]) ++counter;
 */
   NSFastEnumerationState state = { 0 };
   id buffer[4];
   unsigned int limit = [collection countByEnumeratingWithState:&state objects:buffer count:4];
   if (limit != 0) {
        unsigned int mutationsPtr = *state.mutationsPtr;
        do {
            unsigned int innerCounter = 0;
            do {
                id elem = state.itemsPtr[innerCounter++];
                
                if ([reference member:elem]) ++counter;
                
            } while (innerCounter < limit);
        } while (limit = [collection countByEnumeratingWithState:&state objects:buffer count:4]);
    }
            
 
 
    if (counter == [reference count]) {
        if (Verbosity) {
            printf("success: %s %s %s\n", style, test, message);
        }
    }
    else {
        result = false;
	if (Verbosity)
          printf("** failed: %s %s %s\n", style, test, message);
        ++Errors;
    }
}

bool testCompiler(char *style, char *test, char *message, id collection, NSSet *reference) {
    int counter = 0;
    bool result = true;
    if (Verbosity) {
        printf("testing: %s %s %s\n", style, test, message);
    }
    for (id elem in collection)
        if ([reference member:elem]) ++counter;
    if (counter == [reference count]) {
        if (Verbosity) {
            printf("success: %s %s %s\n", style, test, message);
        }
    }
    else {
        result = false;
	if (Verbosity)
          printf("** failed: %s %s %s\n", style, test, message);
        ++Errors;
    }
}

bool testCompleteness(char *test, char *message, id collection, NSSet *reference) {
    testHandwritten("handwritten", test, message, collection, reference);
    testCompiler("compiler", test, message, collection, reference);
}
    
NSMutableSet *ReferenceSet = nil;
NSMutableArray *ReferenceArray = nil;

void makeReferences(int n) {
    if (!ReferenceSet) {
        int i;
        ReferenceSet = [[NSMutableSet alloc] init];
        ReferenceArray = [[NSMutableArray alloc] init];
        for (i = 0; i < n; ++i) {
            NSNumber *number = [[NSNumber alloc] initWithInt:i];
            [ReferenceSet addObject:number];
            [ReferenceArray addObject:number];
            [number release];
        }
    }
}
    
void testCollections(char *test, NSArray *array, NSSet *set) {
    NSAutoreleasePool *pool = [NSAutoreleasePool new];
    id collection;
    collection = [NSMutableArray arrayWithArray:array];
    testCompleteness(test, "mutable array", collection, set);
    //testCompleteness(test, "mutable array enumerator", [collection objectEnumerator], set);
    collection = [NSArray arrayWithArray:array];
    testCompleteness(test, "immutable array", collection, set);
    //testCompleteness(test, "immutable array enumerator", [collection objectEnumerator], set);
    collection = set;
    testCompleteness(test, "immutable set", collection, set);
    //testCompleteness(test, "immutable set enumerator", [collection objectEnumerator], set);
    collection = [NSMutableSet setWithArray:array];
    testCompleteness(test, "mutable set", collection, set);
    //testCompleteness(test, "mutable set enumerator", [collection objectEnumerator], set);
    [pool drain];
}

void testInnerDecl(char *test, char *message, id collection) {
    int counter = 0;
    for (id x in collection)
        ++counter;
    if (counter != [collection count]) {
        printf("** failed: %s %s\n", test, message);
        ++Errors;
    }
}

// use 0 to show that the functions are emitted when no foreach loop is present
// use 1 to show that the functions disappear

void testOuterDecl(char *test, char *message, id collection) {
    int counter = 0;
    id x;
    for (x in collection)
        ++counter;
    if (counter != [collection count]) {
        printf("** failed: %s %s\n", test, message);
        ++Errors;
    }
}
void testInnerExpression(char *test, char *message, id collection) {
    int counter = 0;
    for (id x in [collection self])
        ++counter;
    if (counter != [collection count]) {
        printf("** failed: %s %s\n", test, message);
        ++Errors;
    }
}
void testOuterExpression(char *test, char *message, id collection) {
    int counter = 0;
    id x;
    for (x in [collection self])
        ++counter;
    if (counter != [collection count]) {
        printf("** failed: %s %s\n", test, message);
        ++Errors;
    }
}

void testExpressions(char *message, id collection) {
    testInnerDecl("inner", message, collection);
    testOuterDecl("outer", message, collection);
    testInnerExpression("outer expression", message, collection);
    testOuterExpression("outer expression", message, collection);
}
    

int main(int argc, char *argv) {
/**
    ++Verbosity;
*/
    NSAutoreleasePool *pool = [NSAutoreleasePool new];
    testCollections("nil", nil, nil);
    testCollections("empty", [NSArray array], [NSSet set]);
    makeReferences(100);
    testCollections("100 item", ReferenceArray, ReferenceSet);
    testExpressions("array", ReferenceArray);
    [pool drain];
    if (Errors)
	abort ();
    exit(Errors);
}

/* APPLE LOCAL file radar 4667060 */
/* This tests the new spec. for foreach-statement. On exit from foreach loop
   with no match, value of 'elem' is set to nil! */
#include <Foundation/Foundation.h>
/* { dg-options "-mmacosx-version-min=10.5 -framework Foundation -fobjc-exceptions" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-framework Foundation -fobjc-exceptions" { target arm*-*-darwin* } } */
/* { dg-do run { target *-*-darwin* } } */

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
        printf("** failed: %s %s %s (%d vs %ld)\n", style, test, message, counter, (long)[reference count]);
        ++Errors;
    }
    return result;
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
        printf("** failed: %s %s %s (%d vs %ld)\n", style, test, message, counter, (long)[reference count]);
        ++Errors;
    }
    return result;
}

// array is filled with NSNumbers, in order, from 0 - N
bool testBreak(unsigned int where, NSArray *array) {
    int counter = 0;
    NSAutoreleasePool *pool = [NSAutoreleasePool new];
    id enumerator = [array objectEnumerator];
    for (id elem in enumerator) {
        if (++counter == where)
            break;
    }
    if (counter != where) {
        ++Errors;
        printf("*** break at %d didn't work (actual was %d)\n", where, counter);
        return false;
    }
    for (id elem in enumerator)
        ++counter;
    if (counter != [array count]) {
        ++Errors;
        printf("*** break at %d didn't finish (actual was %d)\n", where, counter);
        return false;
    }
    [pool drain];
    return true;
}

bool testFallthrough(NSArray *array) {
    id elem;
    int counter = 0;
    for (elem in array) {
        ++counter;
    }
    if (counter > 0) {
        if (elem != nil) {
            ++Errors;
            printf("*** fall through didn't nil iterator element\n");
        }
        return elem == nil;
    }
    return true;
}

bool testBreakOnValue(NSArray *array) {
    id elem;
    id value;
    if ([array count] < 3) return true;
    value = [array objectAtIndex:2];
    for (elem in array) {
        if (value == elem) break;
    }
    if (elem != value) {
        ++Errors;
        printf("*** break on value didn't work\n");
        return false;
    }
    return true;
}
    
bool testBreaks(NSArray *array) {
    int counter = 0;
    for (counter = 1; counter < [array count]; ++counter) {
        testBreak(counter, array);
    }
}
        
bool testCompleteness(char *test, char *message, id collection, NSSet *reference) {
    testHandwritten("handwritten", test, message, collection, reference);
    testCompiler("compiler", test, message, collection, reference);
}

bool testEnumerator(char *test, char *message, id collection, NSSet *reference) {
    testHandwritten("handwritten", test, message, [collection objectEnumerator], reference);
    testCompiler("compiler", test, message, [collection objectEnumerator], reference);
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
    testEnumerator(test, "mutable array enumerator", collection, set);
    collection = [NSArray arrayWithArray:array];
    testCompleteness(test, "immutable array", collection, set);
    testEnumerator(test, "immutable array enumerator", collection, set);
    collection = set;
    testCompleteness(test, "immutable set", collection, set);
    testEnumerator(test, "immutable set enumerator", collection, set);
    collection = [NSMutableSet setWithArray:array];
    testCompleteness(test, "mutable set", collection, set);
    testEnumerator(test, "mutable set enumerator", collection, set);
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
#define Radar4550582 1

void testOuterDecl(char *test, char *message, id collection) {
#if Radar4550582
    int counter = 0;
    id x;
    for (x in collection)
        ++counter;
    if (counter != [collection count]) {
        printf("** failed: %s %s\n", test, message);
        ++Errors;
    }
#endif
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
#if Radar4550582
    int counter = 0;
    id x;
    for (x in [collection self])
        ++counter;
    if (counter != [collection count]) {
        printf("** failed: %s %s\n", test, message);
        ++Errors;
    }
#endif
}

void testExpressions(char *message, id collection) {
    testInnerDecl("inner", message, collection);
    testOuterDecl("outer", message, collection);
    testInnerExpression("outer expression", message, collection);
    testOuterExpression("outer expression", message, collection);
}
    

int main(int argc, char *argv) {
/*
    ++Verbosity;
*/
    NSAutoreleasePool *pool = [NSAutoreleasePool new];
    testCollections("nil", nil, nil);
    testCollections("empty", [NSArray array], [NSSet set]);
    makeReferences(100);
    testCollections("100 item", ReferenceArray, ReferenceSet);
    testExpressions("array", ReferenceArray);
    testBreaks(ReferenceArray);
    testFallthrough(ReferenceArray);
    testBreakOnValue(ReferenceArray);
    [pool drain];
    exit(Errors);
}

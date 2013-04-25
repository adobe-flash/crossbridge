/* APPLE LOCAL file radar 4591756 - radar 5276085 */
/* A run-time test for insertion of read barriers for __weak objects. */
/* Test enhanced for radar 5276085 to test weak array of ivars. */
/* { dg-do run { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-framework Foundation -fobjc-gc" } */
/* { dg-require-effective-target objc_gc } */

// Weak Write Barriers

#define objc_assign_weak _X_objc_assign_weak
#define objc_read_weak _X_objc_read_weak

#import <Foundation/Foundation.h>

#undef objc_assign_weak
#undef objc_read_weak

struct S {
    int expected;
    int encountered;
}
    Reads, Writes;

static
id objc_assign_weak(id value, id *location) {
    Writes.encountered++;
    return value;
}

static
id objc_read_weak(id *location) {
    Reads.encountered++;
    return *location;
}


@interface TestObject : NSObject {
    __weak id ivar;
    __weak id arr_ivar[3];
    __weak id another_arr_ivar[3][2];
}
- (int)doTest;  // error count
@end

__weak id global;
__weak id *pweak;

@implementation TestObject
- (int)writeIvar {
    int i,j;
    ++Writes.expected;
    ivar = self;
    if (Writes.expected != Writes.encountered) {
        printf("writeIvar failed\n");
        return 1;
    }
    Writes.expected += 3;
    for (i=0; i < 3; i++)
      arr_ivar[i] = self;
    if (Writes.expected != Writes.encountered) {
        printf("writeIvar failed\n");
        return 1;
    }

    Writes.expected += 6;
    for (i=0; i < 3; i++)
      for (j=0; j < 2; j++)
        another_arr_ivar[i][j] = self;
    if (Writes.expected != Writes.encountered) {
        printf("writeIvar failed\n");
        return 1;
    }

    return 0;
}

- (int)writeGlobal {
    ++Writes.expected;
    global = self;
    if (Writes.expected != Writes.encountered) {
        printf("writeGlobal failed\n");
        return 1;
    }
    return 0;
}

- (int)writeStatic {
    ++Writes.expected;
    __weak static id local;
    local = self;
    if (Writes.expected != Writes.encountered) {
        printf("writeStatic failed\n");
        return 1;
    }
    return 0;
}

- (int)readIvar {
    ++Reads.expected;
    [ivar self];
    if (Reads.expected != Reads.encountered) {
        printf("readIvar failed\n");
        return 1;
    }
    return 0;
}
- (int)readGlobal {
    ++Reads.expected;
    [global self];
    if (Reads.expected != Reads.encountered) {
        printf("readGlobal failed\n");
        return 1;
    }
    return 0;
}
- (int)readStatic {
    ++Reads.expected;
    __weak static id local;
    [global self];
    if (Reads.expected != Reads.encountered) {
        printf("readGlobal failed\n");
        return 1;
    }
    return 0;
}

- (int) writePtrWeak:(__weak id *)ptr2weakId {
    *ptr2weakId = self;
    ++Writes.expected;
    if (Writes.expected != Writes.encountered) {
        printf("writePtrWeak failed\n");
        return 1;
    }
    pweak = ptr2weakId;
    if (Writes.expected != Writes.encountered) {
        printf("writePtrWeak 2 failed\n");
        return 1;
    }
    if (Reads.expected != Reads.encountered) {
        printf("writePtrWeak 3 failed\n");
        return 1;
    }
    return 0;
}

#define rdar4591756fixed 1

- (int)doTest {
    
    int errors = 0;
//    errors += [self writePtrWeak:&global];
    errors += [self writeIvar];
    errors += [self writeGlobal];
    errors += [self writeStatic];
#if rdar4591756fixed
    errors += [self readIvar];
    errors += [self readGlobal];
    errors += [self readStatic];
#else
#warning skipping read weak tests until rdar4591756fixed
#endif
    
    return errors;
    
}
@end


int main(int argc, char *argv[]) {
    NSAutoreleasePool *pool = [NSAutoreleasePool new];
    
    // basic test is for the write barriers
    // we assume zeroing works
    
    TestObject *to = [TestObject new];
    int errors = [to doTest];
    
    if (errors == 0) {
        printf("wwb: SUCCESS\n");
    }
    return errors;
}
    
    



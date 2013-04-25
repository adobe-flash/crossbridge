/* APPLE LOCAL file radar 5882266 */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-mmacosx-version-min=10.5 -ObjC++ -fobjc-gc -framework Foundation" { target *-*-darwin* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#import <Foundation/Foundation.h>
void * _NSConcreteStackBlock[32];
void _Block_byref_assign_copy(void * dst, void *src){}
void _Block_byref_release(void*src){}


int GlobalInt = 0;
int GlobalInt2 = 0;

id objc_assign_weak(id value, id *location) {
    GlobalInt = 1;
    *location = value;
    return value;
}

id objc_read_weak(id *location) {
    GlobalInt2 = 1;
    return *location;
}


void (^__weak Henry)(void);

int main(char *argc, char *argv[]) {
  // an object should not be retained within a stack Block
  void (^local)(void);
  __block int i = 10;
  Henry = ^ { ++i; };
  local = Henry;
  if (GlobalInt2 == 1) {
    printf("%s: success\n", argv[0]);
    exit(0);
  } else {
    printf("%s: problem with weak write-barrier assignment of stack block\n", argv[0]);
  }
  exit(1);
}

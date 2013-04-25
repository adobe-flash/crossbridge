/* APPLE LOCAL file mainline */
/* All calls must be properly stubified.  Complain about any "call
   _objc_msgSend<end-of-line>" without the $stub suffix.  */
/* APPLE LOCAL radar 4492976 */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* APPLE LOCAL axe stubs 5571540 */
/* { dg-options "-Os -mdynamic-no-pic -fno-exceptions -mmacosx-version-min=10.4" } */
/* APPLE LOCAL radar 5061001 */
/* { dg-require-effective-target ilp32 } */

typedef struct objc_object { } *id ;
int x = 41 ;

extern "C" {
  extern id objc_msgSend(id self, char * op, ...);
  extern int bogonic (int, int, int);
}

@interface Document {}
- (Document *) window;
- (Document *) class;
- (Document *) close;
@end
@implementation Document
- (Document *) class { }
- (Document *) close { }
- (Document *) window { }
- (void)willEndCloseSheet:(void *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo {
        [[self window] close];
        ((void (*)(id, char *, int))objc_msgSend)([self class], (char *)contextInfo, 1);
        ((void (*)(id, char *, int))bogonic)([self class], (char *)contextInfo, 1);
	bogonic (3, 4, 5);
	x++;
}
@end

/* { dg-final { scan-assembler-not "\(bl|call\)\[ \t\]+_objc_msgSend\n" } } */
/* { dg-final { scan-assembler     "\(bl|call\)\[ \t\]+L_objc_msgSend\\\$stub\n" } } */
/* { dg-final { scan-assembler-not "\(bl|call\)\[ \t\]+_bogonic\n" } } */
/* { dg-final { scan-assembler     "\(bl|call\)\[ \t\]+L_bogonic\\\$stub\n" } } */
/* { dg-final { scan-assembler-not "\\\$non_lazy_ptr" } } */

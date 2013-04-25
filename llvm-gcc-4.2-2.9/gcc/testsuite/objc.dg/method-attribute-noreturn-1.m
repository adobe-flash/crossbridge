/* APPLE LOCAL file radar 4727659 */
/* Check for use of attribute 'noreturn' on methods. */

extern void exit (int);

@interface INTF
- (void) noret __attribute__ ((noreturn));
- (void) noretok __attribute__ ((noreturn));
+ (void) c_noret __attribute__ ((noreturn));
+ (void) c_noretok __attribute__ ((noreturn));
@end

@implementation INTF
- (void) noret {}   /* { dg-warning "\'noreturn\' function does return" } */
+ (void) c_noret {} /* { dg-warning "\'noreturn\' function does return" } */
+ (void) c_noretok { exit(0); } // ok
- (void) noretok { exit (0); } // ok
@end

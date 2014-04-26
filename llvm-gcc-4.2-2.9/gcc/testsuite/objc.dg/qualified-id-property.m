/* APPLE LOCAL file 8290584 */
/* { dg-do compile } */

@protocol NetworkLoading
@property (assign, getter=isLoading) int loading;
@end

extern id <NetworkLoading> GMMApp;

@interface I
- (void) Meth;
@end

@implementation I
- (void) Meth {
    GMMApp.loading = 0;
}

@end

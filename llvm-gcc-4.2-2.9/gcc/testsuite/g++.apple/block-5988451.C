/* APPLE LOCAL file radar 5988451 */
/* { dg-options "-mmacosx-version-min=10.5 -ObjC++ -fblocks" { target *-*-darwin* } } */
/* { dg-do compile } */

#import <Cocoa/Cocoa.h>
typedef struct dispatch_queue_s *dispatch_queue_t;
typedef struct dispatch_item_s *dispatch_item_t;

dispatch_item_t dispatch_call(dispatch_queue_t queue,
                              dispatch_legacy_block_t work,
                              dispatch_legacy_block_t completion);

void dispatch_apply_wait(dispatch_legacy_block_t work,
                         unsigned iterations,
                         void *context);



@interface TestController : NSObject {

}

-(IBAction) test:(id)sender;

@end

@implementation TestController

-(IBAction)test:(id)sender
{
  dispatch_queue_t queue;

  dispatch_call(queue, /* { dg-warning "deprecated" } */
    ^(dispatch_item_t item) {
      dispatch_apply_wait(^(dispatch_item_t item) {
                            [NSValue valueWithPointer:self];
                          },
                          10,NULL);
    },
    ^(dispatch_item_t item) {NSLog(@"test");});
}


@end

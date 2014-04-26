/* APPLE LOCAL file radar 5992047 - radar 6175959 */
/* Check that with a previous declaration of _Block_destroy, test case
   compiles with no error or ICE. */
/* { dg-options "-fblocks" } */
/* { dg-do compile { target *-*-darwin* } } */

#define Block_destroy(xxx) _Block_destroy((const void *)(xxx))
void _Block_destroy(const void *aBlock, int isWeak);

typedef struct dispatch_item_s *dispatch_item_t;
typedef void (^completion_block_t)(void);

int main()
{
  completion_block_t X;

  completion_block_t (^block)(dispatch_item_t) = ^(dispatch_item_t item) {
    return X;
  };

  block(0);
}

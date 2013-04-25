/* Allow for an optional semicolon following the ivar block.  */
/* Contributed by: Ziemowit Laski <zlaski@apple.com>.  */

/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

@interface Tink : Object {
@private
 unsigned long mCode[4];
};
- (id)initWithProc:(void *)inProc;
- (void *)getUniqueProc;
@end

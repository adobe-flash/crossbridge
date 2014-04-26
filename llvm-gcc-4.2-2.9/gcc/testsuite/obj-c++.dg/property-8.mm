/* APPLE LOCAL file radar 4523837 */
/* Test for correct property setter lookup when a similarl method name
   exists in an unrelated class. Test should compile with no error.
*/
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

typedef struct {int x, y, w, h;} st1;
typedef struct {int x, y, w, h;} st2;

@interface bar
- (void)setFrame:(st1)frameRect;
@end

@interface bar1
- (void)setFrame:(int)frameRect;
@end

@interface foo
@property st2 frame;
@end

static   st2 r = {1,2,3,4};
st2 test (void)
{
    foo *obj;
    id objid;

    obj.frame = r;

    ((foo*)objid).frame = obj.frame;

    return ((foo*)objid).frame;
}

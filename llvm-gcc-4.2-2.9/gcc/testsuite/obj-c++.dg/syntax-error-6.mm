/* APPLE LOCAL begin radar 4294425 */
/* { dg-do compile } */

@interface NSButton
- (int) state;
@end

void FOO()
{
  NSButton * mCopyAcrobatCB; 
	
  [ [ mCopyAcrobatCB state ] == 0 ] != 1;  /* { dg-error "objective\\-c\\+\\+" } */
}
/* APPLE LOCAL end radar 4294425 */

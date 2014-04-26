/* APPLE LOCAL file radar 4649701 */
/* Test that test compiles with no error when setter is declared in protocol, 
   not declared in its adopting interface and is defined in @implementation. */
/* APPLE LOCAL radar 4899595 */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

@class GroupItem;

@protocol GroupEditorProtocol
  - (void)setGroupItem:(GroupItem *)aGroupItem;
@end


@interface GroupEditor  <GroupEditorProtocol>
{
  GroupItem *groupItem;
}
@property (copy, setter=setGroupItem:) GroupItem *groupItem_property;

@end

@implementation GroupEditor
- (void)setGroupItem:(GroupItem *)aGroupItem
{
}
@synthesize groupItem_property = groupItem;
@end


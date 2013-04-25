/* APPLE LOCAL file radar 4725660 */
/* Check that a bad use of property assignment does not cause an internal error. */
/* APPLE LOCAL radar 4899595 */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */

@class QZFile;

@interface QZImage
{
}
@property (copy) QZFile *imageFile;

- (void) addFile :(QZImage *)qzimage;
@end

@implementation QZImage
@dynamic imageFile;

- (void) addFile : (QZImage *)qzimage
  {
	qzimage.imageFile.data = 0; /* { dg-error "accessing unknown \\'data\\' component of a property" } */
  }
@end

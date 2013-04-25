/* APPLE LOCAL file radar 5512183 */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile } */

@class SSyncSet;
@class SStore;
@class SSyncJob;
@class SNode;

SSyncSet *slocal;

@interface SPeer
{
}
@property(nonatomic,readonly,retain) SSyncSet* syncSet;
@end

@class SSyncSet_iDisk;

@interface SPeer_iDisk_remote1 : SPeer
{
}
- (SSyncSet_iDisk*) syncSet;
@end

@interface SPeer_iDisk_local
{
}
- (SSyncSet_iDisk*) syncSet;
@end

@interface SSyncSet
{
}
@end

@interface SSyncSet_iDisk
{
}

@property(nonatomic,readonly,retain) SPeer_iDisk_local* localPeer;
@end

@interface SPeer_iDisk_remote1 (protected)
@end

@implementation SPeer_iDisk_remote1 (protected)
- (void) preferredSource1
{
  self.syncSet.localPeer;	/* { dg-error "type of accessor does not match the type of property" } */
}
@end

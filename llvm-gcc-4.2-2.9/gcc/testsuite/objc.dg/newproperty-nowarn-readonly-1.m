/* APPLE LOCAL file radar 4841013 */
/* Test that no warning of any kind about 'assign' property attribute
   is issued when property is 'readonly'. */
/* { dg-options "-fobjc-new-property -mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property" { target arm*-*-darwin* } } */
/* { dg-do compile } */

@class SFSyncSet;
@class SFPeer;
@class SFPHDSyncSet;

@protocol SFFileSyncManager
@property(readonly)    SFSyncSet*            iDiskSyncSet;
@property(readonly)    SFSyncSet*            phdSyncSet;
@property(readonly)    id                syncSetsList;
@end

@interface SFFileSyncManager <SFFileSyncManager>
@end

@implementation SFFileSyncManager
@dynamic iDiskSyncSet;
@dynamic phdSyncSet;
@dynamic syncSetsList;
@end

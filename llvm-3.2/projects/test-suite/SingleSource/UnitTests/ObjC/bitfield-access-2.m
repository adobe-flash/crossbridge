#include <stdlib.h>
#include <stdio.h>
#import <Foundation/NSObject.h>
@interface class_o_bitfields: NSObject {
    unsigned _wasSearchActive:1;
    unsigned _wasSearchBarFirstResponder:1;
    unsigned _isOutgoingMailbox:1;
    unsigned _isDraftsMailbox:1;
    unsigned _isEditableMailbox:1;
    unsigned _isOutbox:1;
    unsigned _inEditMode:1;
    unsigned _messageLayerShouldPurge:1;
    unsigned _isShowingSwipeDeleteConfirmation:1;
    unsigned _settingSearchText:1;
    unsigned _isDismissing:1;
    unsigned _shouldSelectInitialMessage:1;
    unsigned _shouldKeepSelectionVisible:2;
    unsigned _searchResultsTableViewIsLoaded:1;
    unsigned _currentMessageOutsideSearchResults:1;
    unsigned _savedContentOffsetIsValid:1;
    unsigned _userChoseSearchType:1;
    unsigned _isDeletingMessages:1;
    unsigned _anySourceSupportsSearch:1;
    unsigned _isMovingAllMessages:1;
    unsigned _isMarkingMessages:1;
    unsigned _isShowingMainLoadMoreButton:1;
    unsigned _isShowingSearchLoadMoreButton:1;
    unsigned _isThreadedMCVC:1;
    unsigned _willPop:1;
    unsigned _shouldFadeOutSelectionOnDisappear:1;
    unsigned _didFadeOutSelectionOnDisappear:1;
    unsigned _preventNextScrollbarFlash:1;
    unsigned _preventScrollOnNextAppear:1;
    unsigned _preventMessageSelection:1;
    unsigned _ignoringUserInteractionForDelete:1;
    unsigned _ignoringUserInteractionForDraftRefresh:1;
    unsigned _useSpecialDeleteAnimation:1;
@public
    unsigned _deselectingDeletedRow:1;
    unsigned _isDismissingActionSheet:1;
    unsigned _viewIsVisible:1;
}
@end
int foo_was_called = 0;
void __attribute__ ((__noinline__)) foo() {
  foo_was_called = 1;
}
@implementation class_o_bitfields
- (void)test_a_bitfield {
  if (_useSpecialDeleteAnimation)       // This should always be false.
    return;
  @try {
    foo();
  }
  @catch (class_o_bitfields *x) {
    foo();
  }
}
@end


int
main() {
  class_o_bitfields *mcvc = [class_o_bitfields alloc];
  mcvc->_deselectingDeletedRow = 1;
  [mcvc test_a_bitfield];
  if (!foo_was_called) {
    printf("bitfield read error!\n");
    abort();
  }
  return 0;
}

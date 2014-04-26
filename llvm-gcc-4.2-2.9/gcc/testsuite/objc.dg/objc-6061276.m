/* APPLE LOCAL file radar 6061276 */
/* Corner protocol case should not result in any warning on line 32 */
/* { dg-do compile } */

#import <Foundation/Foundation.h>


@class ASTNode;

@protocol ScopeLookup 
- (ASTNode<ScopeLookup>*) containingScope;
@end

@interface ASTNode : NSObject {}
@end

@implementation ASTNode
@end

@interface Expression : ASTNode {}
@end

@implementation Expression
- (ASTNode<ScopeLookup>*) containingScope { return nil; }
@end

@interface DeclarationContainingStatement : Expression <ScopeLookup> {}
@end

@implementation DeclarationContainingStatement 
- (void)dummy {
    ASTNode<ScopeLookup> *containingScope = [self containingScope];
    if (containingScope) {}
}
@end

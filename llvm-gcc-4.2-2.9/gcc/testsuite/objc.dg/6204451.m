/* APPLE LOCAL file 6204451 */
/* { dg-options "-O1 -framework Foundation" } */
/* { dg-do run } */

#import <Foundation/NSObject.h>
#include <stdlib.h>

@interface SimpleObject : NSObject {
}

@end

int globalCount = 0;

@implementation SimpleObject

typedef struct _Node
{
 struct _Node  *nextNode;
} Node;

- (void)testMethod
{
  ;
}

- (void)doLinkedListTest
{
  Node    *rootNode, *newNode, *prevNode, *node;
  int    i;
  
  // construct the linked list
  rootNode = nil;
  for (i = 0; i < 5; i++)
  {
    newNode = (Node *)malloc(sizeof(Node));
    newNode->nextNode = nil;
    if (rootNode == nil)
      rootNode = newNode;
    else
      prevNode->nextNode = newNode;
    prevNode = newNode;
    [self testMethod];
  }
  
  // verify the linked list
  node = rootNode;
  while (node != nil)
  {
    globalCount++;
    node = node->nextNode;
  }
}

@end

int main (void)
{
  SimpleObject *myObject = [SimpleObject alloc];
  [myObject doLinkedListTest];
  if (globalCount != 5)
    abort();
  return 0;
}


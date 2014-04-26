/* APPLE LOCAL file constant cfstrings */
/* { dg-do compile } */
/* { dg-options "-fobjc-gc -Os -mmacosx-version-min=10.5.0 -fno-tree-sra" } */
/* { dg-require-effective-target objc_gc } */
/* Radar 5494472 */

@class NSString1, Message1;
typedef signed long CFIndex1;
typedef CFIndex1 CFComparisonResult1;
typedef CFComparisonResult1 (*CFComparatorFunction1)(const void *val1,
						     const void *val2,
						     void *context);
enum _NSComparisonResult1 {NSOrderedAscending = -1, NSOrderedSame1};
typedef int NSInteger1;
typedef NSInteger1 NSComparisonResult1;
static NSComparisonResult1 _compareMsgsByReadStatus(Message1 *msg1,
						    Message1 *msg2,
						    void *context);
typedef struct {
  NSString1 *name;
  CFComparatorFunction1 func;
  int i;
} ComparatorPair;

static const ComparatorPair comparators[] = {
  { @"readstatus", (CFComparatorFunction1)&_compareMsgsByReadStatus, 0 },
  { ((void *)0), ((void *)0), 0 }
};

int sync(int);

void foo(ComparatorPair p);

void MFComparatorFunctionForSortOrder() {
    unsigned index = 0;
    ComparatorPair comp = comparators[0];
    foo(comp);
}

static NSComparisonResult1
_compareMsgsByReadStatus(Message1 *msg1,
			 Message1 *msg2, void *context) {
  NSComparisonResult1 result = NSOrderedSame1;

    return result;
}

/* APPLE LOCAL file import valid for objc 4588440 */
/* { dg-options "-pedantic" } */

#import <stdio.h>
#assert Foo(1)		/* { dg-warning "#assert is a GCC extension" } */

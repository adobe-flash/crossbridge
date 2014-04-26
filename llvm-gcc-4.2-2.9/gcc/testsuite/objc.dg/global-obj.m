/* APPLE LOCAL file 6393374 */

#include "../objc/execute/Object2.h"

Object s[1][1];	/* { dg-error "statically allocated instance of Objective-C class" } */

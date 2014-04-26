/* APPLE LOCAL file #import not deprecated */
/* { dg-do compile } */
/* { dg-options "" } */

#import "importee.h"

#import "importee.h"

void foo (bar x) {}

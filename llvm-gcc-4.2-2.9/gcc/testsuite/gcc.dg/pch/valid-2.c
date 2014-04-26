/* { dg-options "-I. -Winvalid-pch -fexceptions" } */

/* APPLE LOCAL 64-bit default for objc 6348519 */
/* { dg-options "-I. -Winvalid-pch -fexceptions -m32" { target { i?86-*-darwin* x86_64-*-darwin* } } } */
#include "valid-2.h"/* { dg-error "settings for -fexceptions do not match|No such file|they were invalid" } */

int x;

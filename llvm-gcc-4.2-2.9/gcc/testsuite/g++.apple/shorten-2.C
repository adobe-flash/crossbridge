/* APPLE LOCAL file 64bit shorten warning 5429810 */
/* { dg-do compile } */
/* { dg-options "-Wshorten-64-to-32" } */
/* Radar 5429810 */

typedef enum {
  kConst1 = 1ull,
  kConst2 = 2
} MyEnum;

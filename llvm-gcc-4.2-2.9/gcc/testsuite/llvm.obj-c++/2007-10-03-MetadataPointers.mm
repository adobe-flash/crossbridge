/* { dg-do compile } */

@class NSImage;
void bork() {
  NSImage *nsimage;
  [nsimage release];
}

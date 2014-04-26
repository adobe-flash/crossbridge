/* APPLE LOCAL file 64bit shorten warning 6183168 */
/* { dg-do compile } */
/* { dg-options "-Wshorten-64-to-32" } */
/* Radar 6183168 */

int ioctl(unsigned long);

void
socket_nonblock() {
  ioctl(8UL & 1);
}

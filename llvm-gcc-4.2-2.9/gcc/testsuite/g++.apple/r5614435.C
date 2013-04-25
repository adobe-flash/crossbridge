/* APPLE LOCAL file 5614435 */
/* { dg-options "-O3" } */
/* { dg-compile } */
/* Radar 5614435 */

typedef const struct __CFDictionary * CFDictionaryRef;
typedef struct __CFDictionary * CFMutableDictionaryRef;
void QEQueryElementSetArgumentsArray(CFMutableDictionaryRef element);
void QEQueryElementGetArguments(CFDictionaryRef element)
{
  QEQueryElementSetArgumentsArray((CFMutableDictionaryRef)element);
}

void
QEQueryElementSetArgumentsArray(CFMutableDictionaryRef element)
{
  return;
}

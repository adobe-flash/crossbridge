/* APPLE LOCAL file radar 6231433 */
/* { dg-do compile } */

@interface NSKey @end
@interface UpdatesList @end

void foo (int i, NSKey *NSKeyValueCoding_NullValue, UpdatesList *nukedUpdatesList)
{
  i ? NSKeyValueCoding_NullValue : nukedUpdatesList; /* { dg-warning "conditional expression" } */
}

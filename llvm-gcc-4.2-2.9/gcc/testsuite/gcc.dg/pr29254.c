/* { dg-do compile } */
/* { dg-options "-O3 -Werror" } */
  /* APPLE LOCAL begin mainline */
  /* APPLE LOCAL end mainline */

list_compare (int * list1)
{
  if (list1)
    value_compare ();
}

func1 (int * f){}

value_compare (int * a)
{
    if (a)
        list_compare (a);
}

func2 (const int * fb)
{
  /* APPLE LOCAL mainline */
  func1 ((int *) fb); /* { dg-bogus "discards qualifiers" } */
}

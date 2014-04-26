/* APPLE LOCAL file 5920116 */
/* Test proper generation of non-weak virtual function thunks.  */
/* { dg-do run  } */
/* { dg-options "-mdynamic-no-pic" } */

int result = 101;

struct Interface1
{
  int if1_var;
  virtual void Interface1Func(void) = 0;
};

struct Interface2
{
  virtual void Interface2Func(void) = 0;
};

class Implementation : public Interface1, public Interface2
{
  void Interface1Func(void);
  void Interface2Func(void);
};

void Implementation::Interface1Func(void)
{
  result -= 1;
}

void Implementation::Interface2Func(void)
{
  result -= 100;
}

int main (void)
{
  Implementation *o2 = new Implementation;
  ((Interface1 *) o2)->Interface1Func();
  ((Interface2 *) o2)->Interface2Func();
  delete o2;
  return result;
}

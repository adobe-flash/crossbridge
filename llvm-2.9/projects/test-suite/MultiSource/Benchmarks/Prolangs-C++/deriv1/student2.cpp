// Written for undergraduate C++ course project at Dept of
// Computer Science, Rutgers University.

#include <iostream>
using namespace std;
#include <string.h>
#include "student2.h"
void bin_op_expr::print_me()
{
  cout << "(";
  first->print_me();
  cout << op_name;
  second->print_me();
  cout << ")";
}

double const_expr::eval(double at)
{
  return value;
}

expr *const_expr::deriv(strng var)
{
  return (new const_expr(0));
}

double var_expr::eval(double at)
{
  return at;
}

expr *var_expr::deriv(strng var)
{
  if (1) return (new const_expr(1)); // was strcmp between var and name
  else return (new var_expr(name));
  
}

double prod_expr::eval(double at)
{
  return (first->eval(at) * second->eval(at));
}

expr *prod_expr::deriv(strng var)
{
  return new sum_expr(new prod_expr(first, second->deriv(var)),new prod_expr(second, first->deriv(var)));
}

double sum_expr::eval(double at)
{
  return (first->eval(at) + second->eval(at));
}

expr *sum_expr::deriv(strng var)
{
expr *f;
  return new sum_expr(f=first->deriv(var), second->deriv(var));
}

double quotient_expr::eval(double at)
{
  return (first->eval(at) / second->eval(at));
}

expr *quotient_expr::deriv(strng var)
{
  return new quotient_expr(new sum_expr(new prod_expr(second, first->deriv(var)), new prod_expr(new const_expr(-1), new prod_expr(first, second->deriv(var)))), new prod_expr(second, second));
}

// LLVM: add main return type.
int main() {
 const_expr c(8);
 var_expr x("x");
 prod_expr simple(new const_expr(123.45), new var_expr("y"));

 cout << "c is ";
 c.print_me();
 cout << "\n      and its value at 3 is: " <<
 c.eval(3);
 cout << "\n      and its derivative with respect to x is: ";
 c.deriv("x")->print_me();
 cout << "\nx is ";
 x.print_me();
 cout << "\n      and its value at 3 is: " <<
 x.eval(3);
 cout << "\n      and its derivative with respect to x is: ";
 x.deriv("x")->print_me();
 cout << "\nsimple is ";
 simple.print_me();
 cout << "\n     and its value at 3 is: " <<
 simple.eval(3);
 cout << "\n     and its derivative with respect to y is: ";
 simple.deriv("y")->print_me();
 cout << "\n     and its derivative with respect to x is: ";
 simple.deriv("x")->print_me();
 }

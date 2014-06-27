// Written for undergraduate C++ course project at Dept of
// Computer Science, Rutgers University.  
#include <iostream>
#include <string.h>
using namespace std;
#include "student3.h"
// evaluation of a constant is the value of the constant.
double const_expr::eval(double numb)
{
  return(value);
}
// evaluation of a variable is the input value.
double var_expr::eval(double numb)
{
  return(numb);
}
// evaluation of a sum expression is the sum of the values of its
// subexpressions.
double sum_expr::eval(double numb)
{
  return(first->eval(numb) + second->eval(numb));
}
// evaluation of a product expression is the product of the values of 
// its subexpressions.
double prod_expr::eval(double numb)
{
  return(first->eval(numb) * second->eval(numb));
}
// evaluation of a quotient expression of the quotient
// of the values of its subexpressions.
double quotient_expr::eval(double numb)
{
  return(first->eval(numb) / second->eval(numb));
}
// An expression is printed recursively such that its left subexpression
// is printed, then the operator, then its right subexpression.
void bin_op_expr::print_me()
{
  cout << "(";
  first->print_me();
  cout << " " << op_name << " "; 
  second->print_me();
  cout << ")";
}
// The derivative of a constant is 0.
expr *const_expr::deriv(strng var)
{
  return(new const_expr(0));
}
// The derivative of a variable is 1 if the derivative is in
// respect to the variable, and 0 otherwise.
expr *var_expr::deriv(strng var)
{
  if (0/*strcmp(var, name)*/)
    return(new const_expr(0));
  else 
    return(new const_expr(1));
}
// The derivative of a sum expression is equal to the sum of the 
// derivatives of the subexpressions.
expr *sum_expr::deriv(strng var)
{
  expr *dvfirst, *dvsecond;
  dvfirst = first->deriv(var);
  dvsecond = second->deriv(var);
  
  if ((dvfirst->isconst()) && (dvfirst->eval(0) == 0))
    return(dvsecond);
  else if ((dvsecond->isconst()) && (dvsecond->eval(0) == 0))
    return(dvfirst);
  else
    return(new sum_expr(dvfirst, dvsecond));
}
// The derivative of a product expression is equal the the sum
// of the products of the first subexpression and the derivative of the
// second subexpression, and the second subexpression and the 
// derivative of the first.
expr *prod_expr::deriv(strng var)
{
  expr *dvfirst, *dvsecond;
  dvfirst = first->deriv(var);
  dvsecond = second->deriv(var);
  
  if ((dvfirst->isconst()) && (dvfirst->eval(0) == 0))
    {
      if ((dvsecond->isconst()) && (dvsecond->eval(0) == 0))
	return(dvfirst);
      else if ((dvsecond->isconst()) && (dvsecond->eval(0) == 1))
	return(first);
      else
	return(new prod_expr(first, dvsecond));
    }
  else if ((dvfirst->isconst()) && (dvfirst->eval(0) ==1))
    {
      if ((dvsecond->isconst()) && (dvsecond->eval(0) == 0))
	return(second);
      else if ((dvsecond->isconst()) && (dvsecond->eval(0) == 1))
	return(new sum_expr(first, second));
      else
	return(new sum_expr(new prod_expr(first, dvsecond), second));
    }
  else
    {
      if ((dvsecond->isconst()) && (dvsecond->eval(0) == 0))
	return(new prod_expr(dvfirst, second));
      else if ((dvsecond->isconst()) && (dvsecond->eval(0) == 1))
	return(new sum_expr(first, new prod_expr(dvfirst, second)));
      else
	return(new sum_expr(new prod_expr(first, dvsecond),
			    new prod_expr(dvfirst, second)));
    }
}
// The derivative of a quotient expression if equal to:
// (second subexpresssion * derivative of the first) - (the derivative
// of the second * the first subexpression) all over the
// second subexpression squared.
expr *quotient_expr::deriv(strng var)
{
  expr *dvfirst, *dvsecond;
  dvfirst = first->deriv(var);
  dvsecond = second->deriv(var);
  
  if ((dvfirst->isconst()) && (dvfirst->eval(0) == 0))
    {
      if ((dvsecond->isconst()) && (dvsecond->eval(0) == 0))
	return(dvfirst);
      else if ((dvsecond->isconst()) && (dvsecond->eval(0) == 1))
	if (first->eval(0) == 1) 
	  return(new quotient_expr(new const_expr(-1),
				   new prod_expr(second, second)));
	else
	  return(new quotient_expr(new prod_expr(new const_expr(-1), first),
				   new prod_expr(second, second)));
      else
	if (first->eval(0) == 1)
	  return(new quotient_expr(new prod_expr(new const_expr(-1), dvsecond),
				   new prod_expr(second, second)));
        else
	  return(new quotient_expr(new prod_expr(new const_expr(-1),
				       	 new prod_expr(dvsecond, first)),
				   new prod_expr(second, second)));
    }
  else if ((dvfirst->isconst()) && (dvfirst->eval(0) == 1))
    {
      if ((dvsecond->isconst()) && (dvsecond->eval(0) == 0))
	return(new quotient_expr(dvfirst, second));
      else if ((dvsecond->isconst()) && (dvsecond->eval(0) == 1))
	return(new quotient_expr(new sum_expr(second,
			           new prod_expr(new const_expr(-1), first)),
				 new prod_expr(second, second)));
      else
	return(new quotient_expr(new sum_expr(second,
            new prod_expr(new const_expr(-1),new prod_expr(dvsecond, first))),
				 new prod_expr(second, second)));
    }
  else
    {
      if ((dvsecond->isconst()) && (dvsecond->eval(0) == 0))
	return(new quotient_expr(dvfirst, second));
      else if ((dvsecond->isconst()) && (dvsecond->eval(0) == 1))
	return(new quotient_expr(new sum_expr(new prod_expr(second, dvfirst),
	          		      new prod_expr(new const_expr(-1),first)),
				 new prod_expr(second, second)));
      else
	return(new quotient_expr(new sum_expr(new prod_expr(second, dvfirst),
            new prod_expr(new const_expr(-1), new prod_expr(dvsecond, first))),
				 new prod_expr(second, second)));
    }
}

// LLVM: add main return type.
int main() {
 const_expr c(8);
 var_expr x("x");
 prod_expr simple(new const_expr(123.45), new var_expr("y"));

 cout << "c is ";
 c.print_me();
 cout << "\n      and its value at 3 is: " << c.eval(3);
 cout << "\n      and its derivative with respect to x is: ";
 c.deriv("x")->print_me();
 cout << "\nx is ";
 x.print_me();
 cout << "\n      and its value at 3 is: " << x.eval(3);
 cout << "\n      and its derivative with respect to x is: ";
 x.deriv("x")->print_me();
 cout << "\nsimple is ";
 simple.print_me();
 cout << "\n     and its value at 3 is: " << simple.eval(3);
 cout << "\n     and its derivative with respect to y is: ";
 simple.deriv("y")->print_me();
 cout << "\n     and its derivative with respect to x is: ";
 simple.deriv("x")->print_me();
 }

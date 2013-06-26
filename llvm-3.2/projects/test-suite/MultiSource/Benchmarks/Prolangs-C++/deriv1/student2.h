/* this is file class-defs.h provided to students */

//extern "C" {
//char *strcpy(char *nam, char *str);
//}

typedef char *strng;

/* class expr is the top of the class heirarchy for expressions. Any
   expression object will be an instance of some derived class which has
   class expr as its base class (or as the base class of its base class...)
*/
class expr {
 public:
  expr(){}
  ~expr(){}
  virtual void print_me()     = 0;

     /* This is the function that an expression object uses to print itself.
      The version given here should never be run - the print function should
      be redefined in the derived class */

  virtual expr *deriv(char *var)  = 0;
     /* This is the function that an expression object uses to compute its
      derivative with respect to "var". The version given here should never
      be run - deriv() should be redefined in the derived class */

  virtual double eval(double at)  = 0;
   /* This is the function that an expression object uses to compute its
      value when all variables in it are replaced by the value of "at"
      */

};
     
     
/* an expression which is a numerical constant */
class const_expr : public expr {
  double value;
 public:
  const_expr(double v) {value = v;}
  ~const_expr(){}
  void print_me() {} // {printf(" %d ", value);}
  double eval(double at);
  expr *deriv(strng var);
 };

/* an expression which is a single variable */
class var_expr : public expr {
private:
  char *name;
 public:
  var_expr(strng str) {name = strdup(str);}
			/* Copy the initialization string
                       into the name buffer. To read the documentation for
                       strcpy() and other C string manipulation functions,
                       type the Unix shell command "man string" */
  ~var_expr(){}
  void print_me()  {} // {printf(" %s ", name);}
  double eval(double at);
  expr *deriv(strng var);
 };

/* expression resulting from applying a binary operator to two expressions */
class bin_op_expr : public expr {
 public:
  bin_op_expr(expr *e1, expr *e2) {first = e1; second = e2;}
  ~bin_op_expr(){}
  void print_me();
 protected:
  expr *first;
  expr *second;
  char op_name;
 };

/* an expression which is the sum of two expressions */
class sum_expr : public bin_op_expr {
 public:
  sum_expr(expr *e1, expr *e2) : bin_op_expr(e1,e2) {op_name = '+';}
  ~sum_expr(){}
  double eval(double at);
  expr *deriv(strng var);
 };

/* an expression which is the product of two expressions */
class prod_expr : public bin_op_expr {
 public:
  prod_expr(expr *e1, expr *e2) : bin_op_expr(e1,e2) {op_name = '*';}
  ~prod_expr(){}
  double eval(double at);
  expr *deriv(strng var);
 };

/* an expression which is the quotient of two expressions */
class quotient_expr : public bin_op_expr {
 public:
  quotient_expr(expr *e1, expr *e2) : bin_op_expr(e1,e2) {op_name = '/';}
  ~quotient_expr(){}
  double eval(double at);
  expr *deriv(strng var);
 };

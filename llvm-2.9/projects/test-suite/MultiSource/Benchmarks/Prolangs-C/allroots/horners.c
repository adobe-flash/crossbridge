double DERIV_X;     /* Global Var: HORNERS function has the side effect
		       of setting DERIV_X to the derivative of polynomial
		       at X */

double HORNERS(int DEGREE,double COEF[],double X)

/* Algo. 2.6 (pp. 68 - 69) of NUMERICAL ANALYSIS by Richard Burden and
   J. Douglas Faires */
{
  double P_X;        /* Value of the polynomial at X */
  double dP_X;       /* Value of the derivative of the polynomial at X */
  int J;             /* a counter variable */
  
  P_X = dP_X = COEF[DEGREE];

  for (J = DEGREE - 1; J >= 1; J --) {
    P_X = X*P_X + COEF[J];
    dP_X = X*dP_X + P_X;
  }

  P_X = X*P_X + COEF[0];

  DERIV_X = dP_X;
  return P_X;
}

double d_abs(double D)
{
  if (D < 0) D = -1.0*D;
  return D;
}

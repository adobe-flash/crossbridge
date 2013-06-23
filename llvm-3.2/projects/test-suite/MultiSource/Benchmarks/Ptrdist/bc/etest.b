scale = 20

x = 3.454;

  /* Check the sign of x. */
  if (x<0) {
    m = 1
    x = -x
  } 

  /* Precondition x. */
  z = scale;
  scale = 4 + z + .44*x;
  while (x > 1) {
    f += 1;
    x /= 2;
  }

  /* Initialize the variables. */
  v = 1+x;
  a = x;
  d = 1;

  for (i=2; 1; i++) {
    e = (a *= x) / (d *= i)
    if (e == 0) {
      if (f>0) while (f--)  v = v*v;
      scale = z
      if (m) {
	(1/v);
	quit;
      }
      else {
	(v/1);
	quit;
      }
    }
    v += e
  }


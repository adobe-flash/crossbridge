#include <stdio.h>
int main()
{
    // output size
    int rows = 40, cols = 78;

    // number of iterations before we quit
    int maxiter = 255;

#ifdef SMALL_PROBLEM_SIZE
    int slowdown = 20;
#else
    int slowdown = 2000;
#endif

    // size and position of the rect on the imaginary plane
    double fViewRectReal = -2.3, fViewRectImg = -1.0;
    double fMagLevel = 0.05;

    for (int y=0; y < rows; y++)
    {
        // imaginary component of "c"
        double fCImg = fViewRectImg + y * fMagLevel;
        
        for (int x=0; x < cols; x++)
        {
            // real component of "c"
            bool bInside;
            int n;
           
            for (int SLOW = 0; SLOW < slowdown; ++SLOW) {
            double fCReal = fViewRectReal + x * fMagLevel;
            double fZReal = fCReal;
            double fZImg = fCImg;
              bInside = true;
 
              // apply the formula...
              for (n=0; n < maxiter; n++) {
                double fZRealSquared = fZReal * fZReal;
                double fZImgSquared = fZImg * fZImg;

                // have we escaped?
                if (fZRealSquared + fZImgSquared > 4) {
                    bInside = false;
                    break;
                }

                // z = z^2 + c
                fZImg = 2 * fZReal * fZImg + fCImg;
                fZReal = fZRealSquared - fZImgSquared + fCReal;
              }
            }

/* NOTE: This has been disabled, due to fp-precision sensitivity.  This test 
   is effectively now just a performance test, not a correctness test. */
            if (n < 100000)
                putchar('X');
            else
            if (bInside)
                putchar(' ');
            else if (n > 4)
                putchar('.');
            else if (n > 2)
                putchar('+');
            else
                putchar('*');
        }
        putchar('\n');
    }
}

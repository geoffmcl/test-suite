
// from : https://www.mpfr.org/sample.html
/*
    Starting With the GNU MPFR Library
    The following program computes a lower bound on 1+1/1!+1/2!+...+1/100! using a 200-bit precision:

    mpfr_t s, t, u; declares three floating-point variables s, t, u;
    mpfr_init2 (t, 200); initializes the variable t with a 200-bit precision;
    mpfr_set_d (t, 1.0, MPFR_RNDD); sets the value of t to the double-precision number 1.0 rounded toward minus 
    infinity (here no rounding is involved since 1 is represented exactly as a double-precision number and 
    also as a 200-bit MPFR number);
    the statement mpfr_mul_ui (t, t, i, MPFR_RNDU); multiplies t in place by the unsigned integer i, 
    where the result is rounded toward plus infinity;
    mpfr_div (u, u, t, MPFR_RNDD); divides u by t, rounding the result toward minus infinity, and stores it in u;
    the statement mpfr_out_str (stdout, 10, 0, s, MPFR_RNDD); prints the value of s in base 10, rounded toward 
    minus infinity, where the third argument 0 means that the number of printed digits is automatically chosen 
    from the precision of s (note: mpfr_printf could also be used instead of printf, mpfr_out_str and putchar);
    finally the mpfr_clear and mpfr_free_cache calls free the space used by the MPFR variables and caches.
    
    Note: with this program, you need MPFR 3.0 or later.
*/

#include <stdio.h>

#include <gmp.h>
#include <mpfr.h>

#include "sprtf.hxx"
#include "utils.hxx"

int main (int argc, char **argv)
{
  unsigned int i;
  mpfr_t s, t, u;
  char *msg;
  double start = get_seconds();

  mpfr_init2 (t, 200);
  mpfr_set_d (t, 1.0, MPFR_RNDD);
  mpfr_init2 (s, 200);
  mpfr_set_d (s, 1.0, MPFR_RNDD);
  mpfr_init2 (u, 200);
  for (i = 1; i <= 100; i++)
    {
      msg = get_elapsed_stg(start);
      printf("Loop %3u: Elapsed %s secs...\n", i, msg);
      mpfr_mul_ui (t, t, i, MPFR_RNDU);
      mpfr_set_d (u, 1.0, MPFR_RNDD);
      mpfr_div (u, u, t, MPFR_RNDD);
      mpfr_add (s, s, u, MPFR_RNDD);
    }
  msg = get_elapsed_stg(start);
  printf("End  %3u: Elapsed %s secs...\n", i, msg);
  printf ("Sum is ");
  mpfr_out_str (stdout, 10, 0, s, MPFR_RNDD);
  putchar ('\n');
  mpfr_clear (s);
  mpfr_clear (t);
  mpfr_clear (u);
  mpfr_free_cache ();
  return 0;
}

// eof

/*\
 * test-rand.cxx
 *
 * Copyright (c) 2015 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "test-rand.hxx"

static const char *module = "test-rand";


void SimpleRandDemo( int n )
{
   // Print n random numbers.
   int i;
   printf("%s: Print %d random numbers.\n", module, n );
   for( i = 0; i < n; i++ )
      printf( "  %6d\n", rand() );
}

void RangedRandDemo( int range_min, int range_max, int n )
{
   // Generate random numbers in the half-closed interval
   // [range_min, range_max). In other words,
   // range_min <= random number < range_max
   printf("%s: Print %d random numbers in range min %d <= random_number < max %d. (diff=%d)\n", module, n, range_min, range_max,
       range_max - range_min);
   int i;
   for ( i = 0; i < n; i++ )
   {
      int u = (double)rand() / ((double)RAND_MAX + 1) * (range_max - range_min)
            + range_min;
      printf( "  %6d\n", u);
   }
}

void test_rand( void )
{
    time_t tt = time (NULL);
   // Seed the random-number generator with the current time so that
   // the numbers will be different every time we run.
    printf("%s: test_rand - seed %ld\n", module, tt );
   srand( (unsigned)tt);

   SimpleRandDemo( 10 );
   printf("\n");
   RangedRandDemo( -100, 100, 10 );
   printf("\n");
   RangedRandDemo( INT32_MIN / 2, INT32_MAX / 2, 10 ); //  buffer[ i ] = (rand() / (RAND_MAX + 1)) * (INT32_MAX - INT32_MIN);
   printf("\n");
   printf("%s: Done test_rand()\n", module);
}

// eof = test-rand.cxx

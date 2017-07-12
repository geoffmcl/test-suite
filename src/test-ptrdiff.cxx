/*\
 * test-ptrdiff.cxx
 *
 * Copyright (c) 2017 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include "sprtf.hxx"

static const char *module = "test-ptrdiff";

// *********************************************************************
// strange variable to show the difference between two pointers of type
// see : http://en.cppreference.com/w/c/types/ptrdiff_t
// *********************************************************************
int ptrdiff_main(void)
{
    const size_t N = 100;
    int numbers[N];
    int *p1 = &numbers[18];
    int *p2 = &numbers[23];
    ptrdiff_t diff = p2 - p1;

    SPRTF("PTRDIFF_MAX = %llu, sizeof %llu, ", (long long)PTRDIFF_MAX, (long long)sizeof(ptrdiff_t));
    SPRTF("p2-p1=%td (ie %p - %p = %lu bytes)\n", diff, (void *)p2, (void *)p1, (diff * sizeof(void *)));
 
    return 0;
}

// eof = test-ptrdiff.cxx
